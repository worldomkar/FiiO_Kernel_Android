/*
 * f_audio.c -- USB Audio class function driver
  *
 * Copyright (C) 2008 Bryan Wu <cooloney@kernel.org>
 * Copyright (C) 2008 Analog Devices, Inc
 *
 * Enter bugs at http://blackfin.uclinux.org/
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <asm/atomic.h>
#include <linux/kthread.h>
#include <linux/freezer.h>

#include "u_audio.c"
#include "u_audio.h"

#define OUT_EP_MAX_PACKET_SIZE	512//200
static int req_buf_size = OUT_EP_MAX_PACKET_SIZE;
module_param(req_buf_size, int, S_IRUGO);
MODULE_PARM_DESC(req_buf_size, "ISO OUT endpoint request buffer size");

static int req_count = 64;//256;
module_param(req_count, int, S_IRUGO);
MODULE_PARM_DESC(req_count, "ISO OUT endpoint request count");

static int audio_buf_size = 48000;
module_param(audio_buf_size, int, S_IRUGO);
MODULE_PARM_DESC(audio_buf_size, "Audio buffer size");

/* The first usb audio buf to alsa playback */
static int first_audio_buf_size = 65500;
module_param(first_audio_buf_size, int, S_IRUGO);
MODULE_PARM_DESC(first_audio_buf_size, "First Audio buffer size");

static int generic_set_cmd(struct usb_audio_control *con, u8 cmd, int value);
static int generic_get_cmd(struct usb_audio_control *con, u8 cmd);

static struct f_audio *audio_for_usb;

bool dug_uac_flags = false;
int actual_rate = UAC_DEFAULT_RATE;
static bool first_copy_audio_buffer = true;
static struct task_struct *audio_task = NULL;
int audio_thread_wakeup_needed = 0;

/*
 * DESCRIPTORS ... most are static, but strings and full
 * configuration descriptors are built on demand.
 */

/*
 * We have two interfaces- AudioControl and AudioStreaming
 * TODO: only supcard playback currently
 */
#define F_AUDIO_AC_INTERFACE	0
#define F_AUDIO_AS_INTERFACE	1
#define F_AUDIO_NUM_INTERFACES	2

/* B.3.1  Standard AC Interface Descriptor */
static struct usb_interface_descriptor ac_interface_desc /*__initdata*/ = {
	.bLength =		sizeof ac_interface_desc,//USB_DT_INTERFACE_SIZE,
	.bDescriptorType =	USB_DT_INTERFACE,
	.bNumEndpoints =	0,
	.bInterfaceClass =	USB_CLASS_AUDIO,
	.bInterfaceSubClass =	USB_SUBCLASS_AUDIOCONTROL,
};

DECLARE_UAC_AC_HEADER_DESCRIPTOR(2);

#define UAC_DT_AC_HEADER_LENGTH	UAC_DT_AC_HEADER_SIZE(F_AUDIO_NUM_INTERFACES)
/* 1 input terminal, 1 output terminal and 1 feature unit */
#define UAC_DT_TOTAL_LENGTH (UAC_DT_AC_HEADER_LENGTH + UAC_DT_INPUT_TERMINAL_SIZE \
	+ UAC_DT_OUTPUT_TERMINAL_SIZE + UAC_DT_FEATURE_UNIT_SIZE(0))
/* B.3.2  Class-Specific AC Interface Descriptor */
static struct uac1_ac_header_descriptor_2 ac_header_desc = {
	.bLength =		sizeof ac_header_desc,//UAC_DT_AC_HEADER_LENGTH,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubtype =	UAC_HEADER,
	.bcdADC =		__constant_cpu_to_le16(0x0100),
	.wTotalLength =		__constant_cpu_to_le16(UAC_DT_TOTAL_LENGTH),
	.bInCollection =	F_AUDIO_NUM_INTERFACES,
	.baInterfaceNr = {
		[0] =		F_AUDIO_AC_INTERFACE,
		[1] =		F_AUDIO_AS_INTERFACE,
	}
};

#define INPUT_TERMINAL_ID	1
static struct uac_input_terminal_descriptor input_terminal_desc = {
	.bLength =		sizeof input_terminal_desc,//UAC_DT_INPUT_TERMINAL_SIZE,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubtype =	UAC_INPUT_TERMINAL,
	.bTerminalID =		INPUT_TERMINAL_ID,
	.wTerminalType =	UAC_TERMINAL_STREAMING,
	.bAssocTerminal =	0,
	.wChannelConfig =	0x3,//0x3,
};

DECLARE_UAC_FEATURE_UNIT_DESCRIPTOR(0);

#define FEATURE_UNIT_ID		2
static struct uac_feature_unit_descriptor_0 feature_unit_desc = {
	.bLength		= sizeof feature_unit_desc,//UAC_DT_FEATURE_UNIT_SIZE(0),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubtype	= UAC_FEATURE_UNIT,
	.bUnitID		= FEATURE_UNIT_ID,
	.bSourceID		= INPUT_TERMINAL_ID,
	.bControlSize		= 2,
	.bmaControls[0]		= (UAC_FU_MUTE | UAC_FU_VOLUME),
};

static struct usb_audio_control mute_control = {
	.list = LIST_HEAD_INIT(mute_control.list),
	.name = "Mute Control",
	.type = UAC_FU_MUTE,
	/* Todo: add real Mute control code */
	.set = generic_set_cmd,
	.get = generic_get_cmd,
};

static struct usb_audio_control volume_control = {
	.list = LIST_HEAD_INIT(volume_control.list),
	.name = "Volume Control",
	.type = UAC_FU_VOLUME,
	/* Todo: add real Volume control code */
	.set = generic_set_cmd,
	.get = generic_get_cmd,
};

static struct usb_audio_control_selector feature_unit = {
	.list = LIST_HEAD_INIT(feature_unit.list),
	.id = FEATURE_UNIT_ID,
	.name = "Mute & Volume Control",
	.type = UAC_FEATURE_UNIT,
	.desc = (struct usb_descriptor_header *)&feature_unit_desc,
};

#define OUTPUT_TERMINAL_ID	3
static struct uac1_output_terminal_descriptor output_terminal_desc = {
	.bLength		= sizeof output_terminal_desc,//UAC_DT_OUTPUT_TERMINAL_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubtype	= UAC_OUTPUT_TERMINAL,
	.bTerminalID		= OUTPUT_TERMINAL_ID,
	.wTerminalType		= UAC_OUTPUT_TERMINAL_SPEAKER,
	.bAssocTerminal		= FEATURE_UNIT_ID,
	.bSourceID		= FEATURE_UNIT_ID,
};

/* B.4.1  Standard AS Interface Descriptor */
static struct usb_interface_descriptor as_interface_alt_0_desc = {
	.bLength =		sizeof as_interface_alt_0_desc,//USB_DT_INTERFACE_SIZE,
	.bDescriptorType =	USB_DT_INTERFACE,
	.bAlternateSetting =	0,
	.bNumEndpoints =	0,
	.bInterfaceClass =	USB_CLASS_AUDIO,
	.bInterfaceSubClass =	USB_SUBCLASS_AUDIOSTREAMING,
};

static struct usb_interface_descriptor as_interface_alt_1_desc = {
	.bLength =		sizeof as_interface_alt_1_desc,//USB_DT_INTERFACE_SIZE,
	.bDescriptorType =	USB_DT_INTERFACE,
	.bAlternateSetting =	1,
	.bNumEndpoints =	1,
	.bInterfaceClass =	USB_CLASS_AUDIO,
	.bInterfaceSubClass =	USB_SUBCLASS_AUDIOSTREAMING,
};

/* B.4.2  Class-Specific AS Interface Descriptor */
static struct uac1_as_header_descriptor as_header_desc = {
	.bLength =		sizeof as_header_desc,//UAC_DT_AS_HEADER_SIZE,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubtype =	UAC_AS_GENERAL,
	.bTerminalLink =	INPUT_TERMINAL_ID,
	.bDelay =		1,
	.wFormatTag =		UAC_FORMAT_TYPE_I_PCM,
};

DECLARE_UAC_FORMAT_TYPE_I_DISCRETE_DESC(7);

static struct uac_format_type_i_discrete_descriptor_7 as_type_i_desc = {
	.bLength =		sizeof as_type_i_desc,//UAC_FORMAT_TYPE_I_DISCRETE_DESC_SIZE(1),
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubtype =	UAC_FORMAT_TYPE,
	.bFormatType =		UAC_FORMAT_TYPE_I,
	.bSubframeSize =	4,//2,//4,
	.bBitResolution =	24,//16,//24,
	.bSamFreqType =		7,
};

/* Standard ISO OUT Endpoint Descriptor */
static struct usb_endpoint_descriptor as_out_ep_desc  = {
	.bLength =		sizeof as_out_ep_desc,//USB_DT_ENDPOINT_AUDIO_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_SYNC_ADAPTIVE
				| USB_ENDPOINT_XFER_ISOC,
	.wMaxPacketSize =	__constant_cpu_to_le16(OUT_EP_MAX_PACKET_SIZE),
	.bInterval =		2,//4,
};

/* Class-specific AS ISO OUT Endpoint Descriptor */
static struct uac_iso_endpoint_descriptor as_iso_out_desc /*__initdata*/ = {
	.bLength =		sizeof as_iso_out_desc,//UAC_ISO_ENDPOINT_DESC_SIZE,
	.bDescriptorType =	USB_DT_CS_ENDPOINT,
	.bDescriptorSubtype =	UAC_EP_GENERAL,
	.bmAttributes = 	1,
	.bLockDelayUnits =	2,//1,
	.wLockDelay =		__constant_cpu_to_le16(1),
};

static struct usb_descriptor_header *f_audio_desc[] /*__initdata*/ = {
	(struct usb_descriptor_header *)&ac_interface_desc,
	(struct usb_descriptor_header *)&ac_header_desc,

	(struct usb_descriptor_header *)&input_terminal_desc,
	(struct usb_descriptor_header *)&output_terminal_desc,
	(struct usb_descriptor_header *)&feature_unit_desc,

	(struct usb_descriptor_header *)&as_interface_alt_0_desc,
	(struct usb_descriptor_header *)&as_interface_alt_1_desc,
	(struct usb_descriptor_header *)&as_header_desc,

	(struct usb_descriptor_header *)&as_type_i_desc,

	(struct usb_descriptor_header *)&as_out_ep_desc,
	(struct usb_descriptor_header *)&as_iso_out_desc,
	NULL,
};

/* string IDs are assigned dynamically */

#define STRING_MANUFACTURER_IDX		0
#define STRING_PRODUCT_IDX		1

#ifdef CONFIG_LIDA_MACH_X5
#define DRIVER_DESC                     "FiiO X5"
#else
#define DRIVER_DESC                     "FiiO X7"
#endif

static char manufacturer[50];

enum {
	STR_ASSOC,
};

#ifdef CONFIG_LIDA_MACH_X5
static struct usb_string f_strings_fn[] = {
	[STR_ASSOC].s = "FiiO X5",
	{ },
};
#else
static struct usb_string f_strings_fn[] = {
	[STR_ASSOC].s = "FiiO X7",
	{ },
};
#endif

#if 0
static struct usb_string f_strings_dev[] = {
	[STRING_MANUFACTURER_IDX].s = manufacturer,
	[STRING_PRODUCT_IDX].s = DRIVER_DESC,
	{  } /* end of list */
};
#endif

static struct usb_gadget_strings f_stringtab_dev = {
	.language	= 0x0409,	/* en-us */
	.strings	= f_strings_fn,
};

static struct usb_gadget_strings *f_audio_strings[] = {
	&f_stringtab_dev,
	NULL,
};

/*
 * This function is an ALSA sound card following USB Audio Class Spec 1.0.
 */

/*-------------------------------------------------------------------------*/
struct f_audio_buf {
	u8 *buf;
	int actual;
	struct list_head list;
};

static struct f_audio_buf *f_audio_buffer_alloc(int buf_size)
{
	struct f_audio_buf *copy_buf;

	copy_buf = kzalloc(sizeof *copy_buf, GFP_ATOMIC);
	if (!copy_buf)
		return ERR_PTR(-ENOMEM);

	copy_buf->buf = kzalloc(buf_size, GFP_ATOMIC);
	if (!copy_buf->buf) {
		kfree(copy_buf);
		return ERR_PTR(-ENOMEM);
	}

	return copy_buf;
}

static void f_audio_buffer_free(struct f_audio_buf *audio_buf)
{
	kfree(audio_buf->buf);
	kfree(audio_buf);
}
/*-------------------------------------------------------------------------*/

struct f_audio {
	struct gaudio			card;
        atomic_t			online;

	/* endpoints handle full and/or high speeds */
	struct usb_ep			*out_ep;
	struct usb_endpoint_descriptor	*out_desc;

	spinlock_t			lock;
	struct f_audio_buf *copy_buf;
	struct work_struct playback_work;
	struct list_head play_queue;

	/* Control Set command */
	struct list_head cs;
	u8 set_cmd;
	struct usb_audio_control *set_con;

	struct mutex                    uac_mutex;
        struct delayed_work             close_work;
};

static inline struct f_audio *func_to_audio(struct usb_function *f)
{
	return container_of(f, struct f_audio, card.func);
}

/*-------------------------------------------------------------------------*/

static void f_audio_playback_work(struct work_struct *data)
{
	struct f_audio *audio = container_of(data, struct f_audio,
					playback_work);
	struct f_audio_buf *play_buf;
	struct usb_composite_dev *cdev = audio->card.func.config->cdev;

	//INFO(cdev, "f_audio_playback_work: started\n");
	if (!atomic_read(&audio->online)) {
		INFO(cdev, "%s offline\n", __func__);
		return;
	}

	spin_lock_irq(&audio->lock);
	if (list_empty(&audio->play_queue)) {
		spin_unlock_irq(&audio->lock);
		return;
	}
	play_buf = list_first_entry(&audio->play_queue,
			struct f_audio_buf, list);
	list_del(&play_buf->list);
	spin_unlock_irq(&audio->lock);

	//INFO(cdev, "play_buf->actual = %d\n", play_buf->actual);

	u_audio_playback(&audio->card, play_buf->buf, play_buf->actual);
	f_audio_buffer_free(play_buf);
}

static int f_audio_out_ep_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct f_audio *audio = req->context;
	struct usb_composite_dev *cdev = audio->card.func.config->cdev;
	struct f_audio_buf *copy_buf = audio->copy_buf;
	int err;

	if (!copy_buf)
		return -EINVAL;

	/* Copy buffer is full, add it to the play_queue */
        if (!first_copy_audio_buffer) {
	    if (audio_buf_size - copy_buf->actual < req->actual) {
		spin_lock_irq(&audio->lock);
		if (!list_empty(&audio->play_queue)) {
			INFO(cdev, "over-runs, audio write slow.. drop the packet\n");
			f_audio_buffer_free(copy_buf);
		} else {
			list_add_tail(&copy_buf->list, &audio->play_queue);
		}
		spin_unlock_irq(&audio->lock);

		schedule_work(&audio->playback_work);
		copy_buf = f_audio_buffer_alloc(audio_buf_size);
		if (IS_ERR(copy_buf))
			return -ENOMEM;
	    }
        } else {
	    if (first_audio_buf_size - copy_buf->actual < req->actual) {
		spin_lock_irq(&audio->lock);
		if (!list_empty(&audio->play_queue)) {
			INFO(cdev, "over-runs, audio write slow.. drop the packet\n");
			f_audio_buffer_free(copy_buf);
		} else {
			list_add_tail(&copy_buf->list, &audio->play_queue);
		}
		spin_unlock_irq(&audio->lock);

		schedule_work(&audio->playback_work);
		copy_buf = f_audio_buffer_alloc(audio_buf_size);
		if (IS_ERR(copy_buf))
			return -ENOMEM;

                first_copy_audio_buffer = false;
	    }
        }

	//INFO(cdev, "Playback %d bytes\n", req->actual);

	memcpy(copy_buf->buf + copy_buf->actual, req->buf, req->actual);
	copy_buf->actual += req->actual;
	audio->copy_buf = copy_buf;

	err = usb_ep_queue(ep, req, GFP_ATOMIC);
	if (err)
		ERROR(cdev, "%s queue req: %d\n", ep->name, err);

	return 0;

}

static void reset_uac_rate(int freq)
{
    int rate;

    rate = freq;

    switch(rate) {
        case UAC_RATE_VALUE_32000:
            actual_rate = UAC_RATE_32000;//UAC_RATE_32000;
            break;
        case UAC_RATE_VALUE_44100:
            actual_rate = UAC_RATE_44100;
            break;
        case UAC_RATE_VALUE_48000:
            actual_rate = UAC_RATE_48000;
            break;    
        case UAC_RATE_VALUE_88200:
            actual_rate = UAC_RATE_88200;
            break;
        case UAC_RATE_VALUE_96000:
            actual_rate = UAC_RATE_96000;
            break; 
        case UAC_RATE_VALUE_176400:
            actual_rate = UAC_RATE_176400;
            break; 
        case UAC_RATE_VALUE_192000:
            actual_rate = UAC_RATE_192000;
            break;

        default :
            actual_rate = UAC_DEFAULT_RATE;
            break;            
    }
}

static void f_audio_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct f_audio *audio = req->context;
	int status = req->status;
	u32 data = 0;
	struct usb_ep *out_ep = audio->out_ep;
	struct usb_composite_dev *cdev = audio->card.func.config->cdev;
        int actual_value;

        DBG(cdev, "%s: iso_complete status(%d) %d/%d\n",
			__func__, status, req->actual, req->length);

        if (dug_uac_flags) {
            actual_value = req->actual;
            dug_uac_flags = false;
            INFO(cdev, "%s: actual_value is  %d \n", __func__, actual_value);

            reset_uac_rate(actual_value);
        }

	switch (status) {
	case 0:				/* normal completion? */
		if (ep == out_ep)
			f_audio_out_ep_complete(ep, req);
		else if (audio->set_con) {
			memcpy(&data, req->buf, req->length);
			audio->set_con->set(audio->set_con, audio->set_cmd,
					le16_to_cpu(data));
			audio->set_con = NULL;
		}
		break;
	default:
		break;
	}
}

static int audio_set_intf_req(struct usb_function *f,
		const struct usb_ctrlrequest *ctrl)
{
	struct f_audio		*audio = func_to_audio(f);
	struct usb_composite_dev *cdev = f->config->cdev;
	struct usb_request	*req = cdev->req;
	u8			id = ((le16_to_cpu(ctrl->wIndex) >> 8) & 0xFF);
	u16			len = le16_to_cpu(ctrl->wLength);
	u16			w_value = le16_to_cpu(ctrl->wValue);
	u8			con_sel = (w_value >> 8) & 0xFF;
	u8			cmd = (ctrl->bRequest & 0x0F);
	struct usb_audio_control_selector *cs;
	struct usb_audio_control *con;

	DBG(cdev, "1111 bRequest 0x%x, w_value 0x%04x, len %d, entity %d\n",
			ctrl->bRequest, w_value, len, id);

	list_for_each_entry(cs, &audio->cs, list) {
		if (cs->id == id) {
			list_for_each_entry(con, &cs->control, list) {
				if (con->type == con_sel) {
					audio->set_con = con;
					break;
				}
			}
			break;
		}
	}

	audio->set_cmd = cmd;
	req->context = audio;
	req->complete = f_audio_complete;

	return len;
}

static int audio_get_intf_req(struct usb_function *f,
		const struct usb_ctrlrequest *ctrl)
{
	struct f_audio		*audio = func_to_audio(f);
	struct usb_composite_dev *cdev = f->config->cdev;
	struct usb_request	*req = cdev->req;
	int			value = -EOPNOTSUPP;
	u8			id = ((le16_to_cpu(ctrl->wIndex) >> 8) & 0xFF);
	u16			len = le16_to_cpu(ctrl->wLength);
	u16			w_value = le16_to_cpu(ctrl->wValue);
	u8			con_sel = (w_value >> 8) & 0xFF;
	u8			cmd = (ctrl->bRequest & 0x0F);
	struct usb_audio_control_selector *cs;
	struct usb_audio_control *con;

	DBG(cdev, "2222 bRequest 0x%x, w_value 0x%04x, len %d, entity %d\n",
			ctrl->bRequest, w_value, len, id);

	list_for_each_entry(cs, &audio->cs, list) {
		if (cs->id == id) {
			list_for_each_entry(con, &cs->control, list) {
				if (con->type == con_sel && con->get) {
					value = con->get(con, cmd);
					break;
				}
			}
			break;
		}
	}

	req->context = audio;
	req->complete = f_audio_complete;
	memcpy(req->buf, &value, len);

	return len;
}

static int audio_set_endpoint_req(struct usb_function *f,
		const struct usb_ctrlrequest *ctrl)
{
	struct usb_composite_dev *cdev = f->config->cdev;
	int			value = -EOPNOTSUPP;
	u16			ep = le16_to_cpu(ctrl->wIndex);
	u16			len = le16_to_cpu(ctrl->wLength);
	u16			w_value = le16_to_cpu(ctrl->wValue);
        struct usb_ep *debug_ep = cdev->gadget->ep0;

        DBG(cdev, "3333 bRequest 0x%x, w_value 0x%04x, len %d, endpoint %d\n",
                       ctrl->bRequest, w_value, len, ep);

	switch (ctrl->bRequest) {
	case UAC_SET_CUR:
                INFO(cdev, " UAC_SET_CUR !!!\n");
                dug_uac_flags = true;
		value = len;
		break;

	case UAC_SET_MIN:
		break;

	case UAC_SET_MAX:
		break;

	case UAC_SET_RES:
		break;

	case UAC_SET_MEM:
		break;

	default:
		break;
	}

	return value;
}

static int audio_get_endpoint_req(struct usb_function *f,
		const struct usb_ctrlrequest *ctrl)
{
	struct usb_composite_dev *cdev = f->config->cdev;
	int value = -EOPNOTSUPP;
	u8 ep = ((le16_to_cpu(ctrl->wIndex) >> 8) & 0xFF);
	u16 len = le16_to_cpu(ctrl->wLength);
	u16 w_value = le16_to_cpu(ctrl->wValue);

	DBG(cdev, "4444 bRequest 0x%x, w_value 0x%04x, len %d, endpoint %d\n",
			ctrl->bRequest, w_value, len, ep);

	switch (ctrl->bRequest) {
	case UAC_GET_CUR:
	case UAC_GET_MIN:
	case UAC_GET_MAX:
	case UAC_GET_RES:
		value = 3;
		break;
	case UAC_GET_MEM:
		break;
	default:
		break;
	}

	return value;
}

static int
f_audio_setup(struct usb_function *f, const struct usb_ctrlrequest *ctrl)
{
	struct usb_composite_dev *cdev = f->config->cdev;
	struct usb_request	*req = cdev->req;
	int			value = -EOPNOTSUPP;
	u16			w_index = le16_to_cpu(ctrl->wIndex);
	u16			w_value = le16_to_cpu(ctrl->wValue);
	u16			w_length = le16_to_cpu(ctrl->wLength);

	/* composite driver infrastructure handles everything; interface
	 * activation uses set_alt().
	 */
	switch (ctrl->bRequestType) {
	case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
		value = audio_set_intf_req(f, ctrl);
		break;

	case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE:
		value = audio_get_intf_req(f, ctrl);
		break;

	case USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_ENDPOINT:
		value = audio_set_endpoint_req(f, ctrl);
		break;

	case USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_ENDPOINT:
		value = audio_get_endpoint_req(f, ctrl);
		break;

	default:
		ERROR(cdev, "invalid control req%02x.%02x v%04x i%04x l%d\n",
			ctrl->bRequestType, ctrl->bRequest,
			w_value, w_index, w_length);
	}

	/* respond with data transfer or status phase? */
	if (value >= 0) {
		DBG(cdev, "5555 audio req%02x.%02x v%04x i%04x l%d\n",
			ctrl->bRequestType, ctrl->bRequest,
			w_value, w_index, w_length);
		req->zero = 0;
		req->length = value;
		value = usb_ep_queue(cdev->gadget->ep0, req, GFP_ATOMIC);
		if (value < 0)
			ERROR(cdev, "audio response on err %d\n", value);
	}

	/* device either stalls (value < 0) or reports success */
	return value;
}

static int f_audio_set_alt(struct usb_function *f, unsigned intf, unsigned alt)
{
	struct f_audio		*audio = func_to_audio(f);
	struct usb_composite_dev *cdev = f->config->cdev;
	struct usb_ep *out_ep = audio->out_ep;
	struct usb_request *req;
	int i = 0, err = 0;

        DBG(cdev, "intf %d, alt %d\n", intf, alt);

        atomic_set(&audio->online, 1);
	if (intf == 1) {
		if (alt == 1) {
                        audio_thread_wakeup_needed = 1;
                        wake_up_process(audio_task);

			usb_ep_enable(out_ep, audio->out_desc);
			out_ep->driver_data = audio;
			audio->copy_buf = f_audio_buffer_alloc(first_audio_buf_size);
			if (IS_ERR(audio->copy_buf))
				return -ENOMEM;

			/*
			 * allocate a bunch of read buffers
			 * and queue them all at once.
			 */
			for (i = 0; i < req_count && err == 0; i++) {
				req = usb_ep_alloc_request(out_ep, GFP_ATOMIC);
				if (req) {
					req->buf = kzalloc(req_buf_size,
							GFP_ATOMIC);
					if (req->buf) {
						req->length = req_buf_size;
						req->context = audio;
						req->complete =
							f_audio_complete;
						err = usb_ep_queue(out_ep,
							req, GFP_ATOMIC);
						if (err)
							ERROR(cdev,
							"%s queue req: %d\n",
							out_ep->name, err);
					} else
						err = -ENOMEM;
				} else
					err = -ENOMEM;
			}

		} else {
                        audio_thread_wakeup_needed = 0;

			struct f_audio_buf *copy_buf = audio->copy_buf;
			if (copy_buf) {
				list_add_tail(&copy_buf->list,
						&audio->play_queue);
				schedule_work(&audio->playback_work);
			}
                        first_copy_audio_buffer = true;
		}
	}

	return err;
}

static void f_audio_close_work(struct work_struct *data)
{
	struct f_audio *audio =
			container_of(data, struct f_audio, close_work);

	pr_info("close audio files\n");
	mutex_lock(&audio->uac_mutex);
	gaudio_cleanup();
	mutex_unlock(&audio->uac_mutex);
/*
	if (audio_task != NULL)
	{
		kthread_stop(audio_task);
                audio_task = NULL;
	}
*/
        kfree(audio);
}

static void f_audio_disable(struct usb_function *f)
{
        struct f_audio	*audio = func_to_audio(f);

        printk("lipf_debug: %s  enter  !!!\n", __FUNCTION__);
        atomic_set(&audio->online, 0);
	usb_ep_disable(audio->out_ep);

        actual_rate = UAC_RATE_44100;
        schedule_delayed_work(&audio_for_usb->close_work, msecs_to_jiffies(2000));
        audio_thread_wakeup_needed = 0;
}

/*-------------------------------------------------------------------------*/

static void f_audio_build_desc(struct f_audio *audio)
{
	struct gaudio *card = &audio->card;
	u8 *sam_freq;
	int rate;

	/* Set channel numbers */
	input_terminal_desc.bNrChannels = u_audio_get_playback_channels(card);
	as_type_i_desc.bNrChannels = u_audio_get_playback_channels(card);

	/* Set sample rates */
        rate = UAC_RATE_32000;
	sam_freq = as_type_i_desc.tSamFreq[0];
	memcpy(sam_freq, &rate, 3);

	rate = u_audio_get_playback_rate(card);
	sam_freq = as_type_i_desc.tSamFreq[1];
	memcpy(sam_freq, &rate, 3);

        rate = UAC_RATE_48000;
	sam_freq = as_type_i_desc.tSamFreq[2];
	memcpy(sam_freq, &rate, 3);

        rate = UAC_RATE_88200;
	sam_freq = as_type_i_desc.tSamFreq[3];
	memcpy(sam_freq, &rate, 3);

        rate = UAC_RATE_96000;
	sam_freq = as_type_i_desc.tSamFreq[4];
	memcpy(sam_freq, &rate, 3);

        rate = UAC_RATE_176400;
	sam_freq = as_type_i_desc.tSamFreq[5];
	memcpy(sam_freq, &rate, 3);

        rate = UAC_RATE_192000;
	sam_freq = as_type_i_desc.tSamFreq[6];
	memcpy(sam_freq, &rate, 3);

	/* Todo: Set Sample bits and other parameters */

	return;
}

/* audio function driver setup/binding */
static int /*__init*/
f_audio_bind(struct usb_configuration *c, struct usb_function *f)
{
	struct usb_composite_dev *cdev = c->cdev;
	struct f_audio		*audio = func_to_audio(f);
	int			status;
	struct usb_ep		*ep;
        printk("%s:   enter  !!!\n", __FUNCTION__);

	/* set up ASLA audio devices */
	status = gaudio_setup(&audio->card);
	if (status < 0)
	    goto fail;

	f_audio_build_desc(audio);

	/* allocate instance-specific interface IDs, and patch descriptors */
	status = usb_interface_id(c, f);
	if (status < 0)
		goto fail;
	ac_interface_desc.bInterfaceNumber = status;

	status = usb_interface_id(c, f);
	if (status < 0)
		goto fail;
	as_interface_alt_0_desc.bInterfaceNumber = status;
	as_interface_alt_1_desc.bInterfaceNumber = status;

	status = -ENODEV;

	/* allocate instance-specific endpoints */
	ep = usb_ep_autoconfig(cdev->gadget, &as_out_ep_desc);
	if (!ep)
		goto fail;
	audio->out_ep = ep;
	ep->driver_data = cdev;	/* claim */

	status = -ENOMEM;

	/* supcard all relevant hardware speeds... we expect that when
	 * hardware is dual speed, all bulk-capable endpoints work at
	 * both speeds
	 */

	/* copy descriptors, and track endpoint copies */
	if (gadget_is_dualspeed(c->cdev->gadget)) {
		c->highspeed = true;
		f->hs_descriptors = usb_copy_descriptors(f_audio_desc);
	} else
		f->descriptors = usb_copy_descriptors(f_audio_desc);

	return 0;

fail:
	gaudio_cleanup();
	if (ep)
		ep->driver_data = NULL;
	return status;
}

static void
f_audio_unbind(struct usb_configuration *c, struct usb_function *f)
{
	struct f_audio		*audio = func_to_audio(f);
        printk("%s:  enter  !!!\n", __FUNCTION__);
        flush_work(&audio->playback_work);
	usb_free_descriptors(f->descriptors);
	usb_free_descriptors(f->hs_descriptors);
	//kfree(audio);
}

/*-------------------------------------------------------------------------*/

static int generic_set_cmd(struct usb_audio_control *con, u8 cmd, int value)
{
	con->data[cmd] = value;

	return 0;
}

static int generic_get_cmd(struct usb_audio_control *con, u8 cmd)
{
	return con->data[cmd];
}

/* Todo: add more control selecotor dynamically */
int /*__init*/ control_selector_init(struct f_audio *audio)
{
        printk("%s:   enter  !!!\n", __FUNCTION__);
	INIT_LIST_HEAD(&audio->cs);
	list_add(&feature_unit.list, &audio->cs);

	INIT_LIST_HEAD(&feature_unit.control);
	list_add(&mute_control.list, &feature_unit.control);
	list_add(&volume_control.list, &feature_unit.control);

	volume_control.data[UAC__CUR] = 0xffc0;
	volume_control.data[UAC__MIN] = 0xe3a0;
	volume_control.data[UAC__MAX] = 0xfff0;
	volume_control.data[UAC__RES] = 0x0030;

	return 0;
}

static int audio_sleep_thread(void)
{
	int	rc = 0;

	/* Wait until a signal arrives or we are woken up */
	for (;;) {
		try_to_freeze();
		set_current_state(TASK_INTERRUPTIBLE);
		if (signal_pending(current)) {
			rc = -EINTR;
			break;
		}
		if (audio_thread_wakeup_needed)
			break;
		schedule();
	}
	__set_current_state(TASK_RUNNING);
	smp_rmb();	/* ensure the latest bh->state is visible */

	return rc;
}

static int audio_task_thread(void *arg)
{
	printk("%s : Thread Enter\n", __func__);
        int rc;

        do {
                rc = audio_sleep_thread();
		if (rc)
			return rc;
                //usleep_range(5, 5);
        } while (!kthread_should_stop());

	return 0;
}

/**
 * audio_bind_config - add USB audio function to a configuration
 * @c: the configuration to supcard the USB audio function
 * Context: single threaded during gadget setup
 *
 * Returns zero on success, else negative errno.
 */

int /*__init*/ audio_bind_config(struct usb_configuration *c)
{
	int status;
        int gadget_status = get_gadget_connect_flag();

        printk("%s:   enter  !!!\n", __FUNCTION__);

        printk("%s:   gadget_status  is   %d \n", __func__, gadget_status);
        if(!gadget_status) {
            INFO(c->cdev, "  usb  is disconnected,  exit  usb dac mode !!!\n");
            return 0;
        }

	/* allocate and initialize one new instance */
	audio_for_usb = kzalloc(sizeof *audio_for_usb, GFP_KERNEL);
	if (!audio_for_usb)
		return -ENOMEM;

	status = usb_string_ids_tab(c->cdev, f_strings_fn);
	if (status)
		return status;

        //add for window foobar KS -- name
	input_terminal_desc.iTerminal = f_strings_fn[STR_ASSOC].id;

	audio_for_usb->card.func.name = "audio";
	audio_for_usb->card.gadget = c->cdev->gadget;

	INIT_LIST_HEAD(&audio_for_usb->play_queue);
	spin_lock_init(&audio_for_usb->lock);

        mutex_init(&audio_for_usb->uac_mutex);

	/* set up ASLA audio devices */
	//status = gaudio_setup(&audio_for_usb->card);
	//if (status < 0)
	//	goto setup_fail;

	audio_for_usb->card.func.strings = f_audio_strings;
	audio_for_usb->card.func.bind = f_audio_bind;
	audio_for_usb->card.func.unbind = f_audio_unbind;
	audio_for_usb->card.func.set_alt = f_audio_set_alt;
	audio_for_usb->card.func.setup = f_audio_setup;
	audio_for_usb->card.func.disable = f_audio_disable;
	audio_for_usb->out_desc = &as_out_ep_desc;

	control_selector_init(audio_for_usb);

	INIT_WORK(&audio_for_usb->playback_work, f_audio_playback_work);
        INIT_DELAYED_WORK(&audio_for_usb->close_work, f_audio_close_work);

	status = usb_add_function(c, &audio_for_usb->card.func);
	if (status)
		goto add_fail;
        printk("%s:  debug  4444 \n", __FUNCTION__);
	INFO(c->cdev, "audio_buf_size %d, req_buf_size %d, req_count %d\n",
		audio_buf_size, req_buf_size, req_count);

	return status;

add_fail:
	gaudio_cleanup();
setup_fail:
	kfree(audio_for_usb);
	return status;
}

static int f_audio_init(void)
{
        int status;

        printk("%s:  enter  !!!\n", __func__);

	audio_task = kthread_create(audio_task_thread, NULL, "f_audio_task");
	if(IS_ERR(audio_task))
	{
                status = PTR_ERR(audio_task);
                audio_task = NULL;
		printk(KERN_ERR "%s: cread thread failed\n",__FILE__);
                goto init_fail;	
	}

        printk("%s:  end  !!!\n", __func__);
        return 0;

init_fail:
	//kfree(audio_for_usb);
	return status;
}

void audio_cleanup(void)
{
    printk("lipf_debug: %s enter !!!\n", __FUNCTION__);
    //schedule_delayed_work(&audio_for_usb->close_work, msecs_to_jiffies(2000));
    //gaudio_cleanup();
}

void audio_unbind_config(struct usb_configuration *c)
{
        audio_cleanup();
} 

