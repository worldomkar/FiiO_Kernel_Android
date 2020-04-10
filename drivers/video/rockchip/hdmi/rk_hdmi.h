#ifndef __RK_HDMI_H__
#define __RK_HDMI_H__

#include <linux/kernel.h>
#include <linux/fb.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/workqueue.h>
#include <linux/display-sys.h>
#ifdef CONFIG_SWITCH
#include <linux/switch.h>
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <asm/atomic.h>
#include<linux/rk_screen.h>
#include <linux/rk_fb.h>

#define HDMI_VIDEO_NORMAL				(0 << 8)
#define HDMI_VIDEO_EXT					(1 << 8)
#define HDMI_VIDEO_3D					(2 << 8)
#define HDMI_VIDEO_DVI					(3 << 8)
#define HDMI_VIC_MASK					(0xFF)
#define HDMI_TYPE_MASK					(0xFF << 8)
#ifdef HDMI_MAX_ID
	#undef HDMI_MAX_ID
#endif
#define HDMI_MAX_ID	4

// HDMI video information code according CEA-861-E
enum hdmi_video_infomation_code {
	HDMI_640x480p_60HZ = 1,
	HDMI_720x480p_60HZ_4_3,
	HDMI_720x480p_60HZ_16_9,
	HDMI_1280x720p_60HZ,
	HDMI_1920x1080i_60HZ,		//5
	HDMI_720x480i_60HZ_4_3,
	HDMI_720x480i_60HZ_16_9,
	HDMI_720x240p_60HZ_4_3,
	HDMI_720x240p_60HZ_16_9,
	HDMI_2880x480i_60HZ_4_3,	//10
	HDMI_2880x480i_60HZ_16_9,
	HDMI_2880x240p_60HZ_4_3,
	HDMI_2880x240p_60HZ_16_9,
	HDMI_1440x480p_60HZ_4_3,
	HDMI_1440x480p_60HZ_16_9,	//15
	HDMI_1920x1080p_60HZ,
	HDMI_720x576p_50HZ_4_3,
	HDMI_720x576p_50HZ_16_9,
	HDMI_1280x720p_50HZ,
	HDMI_1920x1080i_50HZ,		//20
	HDMI_720x576i_50HZ_4_3,
	HDMI_720x576i_50HZ_16_9,
	HDMI_720x288p_50HZ_4_3,
	HDMI_720x288p_50HZ_16_9,
	HDMI_2880x576i_50HZ_4_3,	//25
	HDMI_2880x576i_50HZ_16_9,
	HDMI_2880x288p_50HZ_4_3,
	HDMI_2880x288p_50HZ_16_9,
	HDMI_1440x576p_50HZ_4_3,
	HDMI_1440x576p_50HZ_16_9,	//30
	HDMI_1920x1080p_50HZ,
	HDMI_1920x1080p_24HZ,
	HDMI_1920x1080p_25HZ,
	HDMI_1920x1080p_30HZ,
	HDMI_2880x480p_60HZ_4_3,	//35
	HDMI_2880x480p_60HZ_16_9,
	HDMI_2880x576p_50HZ_4_3,
	HDMI_2880x576p_50HZ_16_9,
	HDMI_1920x1080i_50HZ_1250,	// V Line 1250 total
	HDMI_1920x1080i_100HZ,		//40
	HDMI_1280x720p_100HZ,
	HDMI_720x576p_100HZ_4_3,
	HDMI_720x576p_100HZ_16_9,
	HDMI_720x576i_100HZ_4_3,
	HDMI_720x576i_100HZ_16_9,	//45
	HDMI_1920x1080i_120HZ,
	HDMI_1280x720p_120HZ,
	HDMI_720x480p_120HZ_4_3,
	HDMI_720x480p_120HZ_16_9,	
	HDMI_720x480i_120HZ_4_3,	//50
	HDMI_720x480i_120HZ_16_9,
	HDMI_720x576p_200HZ_4_3,
	HDMI_720x576p_200HZ_16_9,
	HDMI_720x576i_200HZ_4_3,
	HDMI_720x576i_200HZ_16_9,	//55
	HDMI_720x480p_240HZ_4_3,
	HDMI_720x480p_240HZ_16_9,	
	HDMI_720x480i_240HZ_4_3,
	HDMI_720x480i_240HZ_16_9,
	HDMI_1280x720p_24HZ,		//60
	HDMI_1280x720p_25HZ,
	HDMI_1280x720p_30HZ,
	HDMI_1920x1080p_120HZ,
	HDMI_1920x1080p_100HZ,
};

// HDMI video source
enum {
	HDMI_SOURCE_LCDC0 = 0,
	HDMI_SOURCE_LCDC1 = 1
};

/* If HDMI_ENABLE, system will auto configure output mode according to EDID 
 * If HDMI_DISABLE, system will output mode according to macro HDMI_VIDEO_DEFAULT_MODE
 */
#define HDMI_AUTO_CONFIGURE			HDMI_DISABLE

/* default HDMI output audio mode */
#define HDMI_AUDIO_DEFAULT_CHANNEL		2
#define HDMI_AUDIO_DEFAULT_RATE			HDMI_AUDIO_FS_44100
#define HDMI_AUDIO_DEFAULT_WORD_LENGTH	HDMI_AUDIO_WORD_LENGTH_16bit

enum {
	VIDEO_INPUT_RGB_YCBCR_444 = 0,
	VIDEO_INPUT_YCBCR422,
	VIDEO_INPUT_YCBCR422_EMBEDDED_SYNC,
	VIDEO_INPUT_2X_CLOCK,
	VIDEO_INPUT_2X_CLOCK_EMBEDDED_SYNC,
	VIDEO_INPUT_RGB444_DDR,
	VIDEO_INPUT_YCBCR422_DDR
};

/* HDMI Video Format */
enum {
	HDMI_VIDEO_FORMAT_NORMAL = 0,
	HDMI_VIDEO_FORMAT_4Kx2K,
	HDMI_VIDEO_FORMAT_3D,
};

enum {
	VIDEO_OUTPUT_RGB444 = 0,
	VIDEO_OUTPUT_YCBCR444,
	VIDEO_OUTPUT_YCBCR422
};

/* HDMI 3D type */
enum {
	HDMI_3D_NONE = -1,
	HDMI_3D_FRAME_PACKING = 0,
	HDMI_3D_TOP_BOOTOM = 6,
	HDMI_3D_SIDE_BY_SIDE_HALF = 8,
};

enum {
	VIDEO_INPUT_COLOR_RGB = 0,
	VIDEO_INPUT_COLOR_YCBCR
};
/********************************************************************
**                          �ṹ����                                *
********************************************************************/
/* HDMI video mode code according CEA-861-E*/
enum hdmi_video_mode
{
	HDMI_640x480p_60Hz = 1,
	HDMI_720x480p_60Hz_4_3,
	HDMI_720x480p_60Hz_16_9,
	HDMI_1280x720p_60Hz,
	HDMI_1920x1080i_60Hz,		//5
	HDMI_720x480i_60Hz_4_3,
	HDMI_720x480i_60Hz_16_9,
	HDMI_720x240p_60Hz_4_3,
	HDMI_720x240p_60Hz_16_9,
	HDMI_2880x480i_60Hz_4_3,	//10
	HDMI_2880x480i_60Hz_16_9,
	HDMI_2880x240p_60Hz_4_3,
	HDMI_2880x240p_60Hz_16_9,
	HDMI_1440x480p_60Hz_4_3,
	HDMI_1440x480p_60Hz_16_9,	//15
	HDMI_1920x1080p_60Hz,
	HDMI_720x576p_50Hz_4_3,
	HDMI_720x576p_50Hz_16_9,
	HDMI_1280x720p_50Hz,
	HDMI_1920x1080i_50Hz,		//20
	HDMI_720x576i_50Hz_4_3,
	HDMI_720x576i_50Hz_16_9,
	HDMI_720x288p_50Hz_4_3,
	HDMI_720x288p_50Hz_16_9,
	HDMI_2880x576i_50Hz_4_3,	//25
	HDMI_2880x576i_50Hz_16_9,
	HDMI_2880x288p_50Hz_4_3,
	HDMI_2880x288p_50Hz_16_9,
	HDMI_1440x576p_50Hz_4_3,
	HDMI_1440x576p_50Hz_16_9,	//30
	HDMI_1920x1080p_50Hz,
	HDMI_1920x1080p_24Hz,
	HDMI_1920x1080p_25Hz,
	HDMI_1920x1080p_30Hz,
	HDMI_2880x480p_60Hz_4_3,	//35
	HDMI_2880x480p_60Hz_16_9,
	HDMI_2880x576p_50Hz_4_3,
	HDMI_2880x576p_50Hz_16_9,
	HDMI_1920x1080i_50Hz_2,		// V Line 1250 total
	HDMI_1920x1080i_100Hz,		//40
	HDMI_1280x720p_100Hz,
	HDMI_720x576p_100Hz_4_3,
	HDMI_720x576p_100Hz_16_9,
	HDMI_720x576i_100Hz_4_3,
	HDMI_720x576i_100Hz_16_9,	//45
	HDMI_1920x1080i_120Hz,
	HDMI_1280x720p_120Hz,
	HDMI_720x480p_120Hz_4_3,
	HDMI_720x480p_120Hz_16_9,	
	HDMI_720x480i_120Hz_4_3,	//50
	HDMI_720x480i_120Hz_16_9,
	HDMI_720x576p_200Hz_4_3,
	HDMI_720x576p_200Hz_16_9,
	HDMI_720x576i_200Hz_4_3,
	HDMI_720x576i_200Hz_16_9,	//55
	HDMI_720x480p_240Hz_4_3,
	HDMI_720x480p_240Hz_16_9,	
	HDMI_720x480i_240Hz_4_3,
	HDMI_720x480i_240Hz_16_9,
	HDMI_1280x720p_24Hz,		//60
	HDMI_1280x720p_25Hz,
	HDMI_1280x720p_30Hz,
	HDMI_1920x1080p_120Hz,
	HDMI_1920x1080p_100Hz,
};

/* HDMI Video Data Color Mode */
enum {
	HDMI_COLOR_RGB = 0,
	HDMI_COLOR_YCbCr422,
	HDMI_COLOR_YCbCr444
};

/* HDMI Audio type */
enum hdmi_audio_type
{
	HDMI_AUDIO_LPCM = 1,
	HDMI_AUDIO_AC3,
	HDMI_AUDIO_MPEG1,
	HDMI_AUDIO_MP3,
	HDMI_AUDIO_MPEG2,
	HDMI_AUDIO_AAC_LC,		//AAC
	HDMI_AUDIO_DTS,
	HDMI_AUDIO_ATARC,
	HDMI_AUDIO_DSD,			//One bit Audio
	HDMI_AUDIO_E_AC3,
	HDMI_AUDIO_DTS_HD,
	HDMI_AUDIO_MLP,
	HDMI_AUDIO_DST,
	HDMI_AUDIO_WMA_PRO
};

/* I2S Fs */
enum hdmi_audio_fs {
	HDMI_AUDIO_FS_32000  = 0x1,
	HDMI_AUDIO_FS_44100  = 0x2,
	HDMI_AUDIO_FS_48000  = 0x4,
	HDMI_AUDIO_FS_88200  = 0x8,
	HDMI_AUDIO_FS_96000  = 0x10,
	HDMI_AUDIO_FS_176400 = 0x20,
	HDMI_AUDIO_FS_192000 = 0x40
};

/* Audio Word Length */
enum hdmi_audio_word_length {
	HDMI_AUDIO_WORD_LENGTH_16bit = 0x1,
	HDMI_AUDIO_WORD_LENGTH_20bit = 0x2,
	HDMI_AUDIO_WORD_LENGTH_24bit = 0x4
};

/* EDID block size */
#define HDMI_EDID_BLOCK_SIZE	128

// HDMI state machine
enum hdmi_state{
	HDMI_SLEEP = 0,
	HDMI_INITIAL,
	WAIT_HOTPLUG,
	READ_PARSE_EDID,
	WAIT_HDMI_ENABLE,
	SYSTEM_CONFIG,
	CONFIG_VIDEO,
	CONFIG_AUDIO,
	PLAY_BACK,
};

// HDMI configuration command
enum hdmi_change {
	HDMI_CONFIG_NONE = 0,
	HDMI_CONFIG_VIDEO,
	HDMI_CONFIG_AUDIO,
	HDMI_CONFIG_COLOR,
	HDMI_CONFIG_HDCP,
	HDMI_CONFIG_ENABLE,
	HDMI_CONFIG_DISABLE,
	HDMI_CONFIG_DISPLAY
};

// HDMI Hotplug status
enum {
	HDMI_HPD_REMOVED = 0,
	HDMI_HPD_INSERT,
	HDMI_HPD_ACTIVED
};

enum hdmi_mute_status {
	HDMI_AV_UNMUTE = 0,
	HDMI_VIDEO_MUTE = 0x1,
	HDMI_AUDIO_MUTE = 0x2,
};
/* HDMI STATUS */
#define HDMI_DISABLE	0
#define HDMI_ENABLE		1
#define HDMI_UNKOWN		0xFF

/* HDMI Error Code */
enum hdmi_errorcode
{
	HDMI_ERROR_SUCESS = 0,
	HDMI_ERROR_FALSE,
	HDMI_ERROR_I2C,
	HDMI_ERROR_EDID,
};

// HDMI Video Timing
struct hdmi_video_timing {
	struct fb_videomode mode;	// Video timing
	unsigned int vic;			// Video information code
	unsigned int pixelrepeat;	// Video pixel repeat rate
	unsigned int interface;		// Video input interface
};

// HDMI Video Parameters
struct hdmi_video {
	unsigned int vic;					// Video information code
	unsigned int color_input;			// Input video color mode
	unsigned int color_output;			// Output video color mode
	unsigned int sink_hdmi;				// Output signal is DVI or HDMI
	unsigned int format_3d;				// Output 3D mode
};

/* HDMI audio parameters */
struct hdmi_audio {
	u32 type;							//Audio type
	u32	channel;						//Audio channel number
	u32	rate;							//Audio sampling rate
	u32	word_length;					//Audio data word length
};

struct hdmi_edid {
	unsigned char sink_hdmi;			//HDMI display device flag
	unsigned char ycbcr444;				//Display device support YCbCr444
	unsigned char ycbcr422;				//Display device support YCbCr422
	unsigned char deepcolor;			//bit3:DC_48bit; bit2:DC_36bit; bit1:DC_30bit; bit0:DC_Y444;
	struct fb_monspecs	*specs;			//Device spec
	struct list_head modelist;			//Device supported display mode list
	struct hdmi_audio *audio;			//Device supported audio info
	int	audio_num;						//Device supported audio type number
	int	base_audio_support;				//Device supported base audio
};

struct hdmi;
/* RK HDMI Video Configure Parameters */
struct hdmi_ops {
	int (*enable) (struct hdmi *);
	int (*disable) (struct hdmi *);
	int (*getStatus) (struct hdmi *);
	int (*insert) (struct hdmi*);
	int (*remove) (struct hdmi*);
	int (*getEdid)	(struct hdmi*, int, unsigned char *);
	int (*setVideo) (struct hdmi*, struct hdmi_video *);
	int (*setAudio) (struct hdmi*, struct hdmi_audio *);
	int (*setMute)	(struct hdmi*, int);
	int (*setVSI)	(struct hdmi*, unsigned char, unsigned char);
	int (*setCEC)	(struct hdmi*);
	// call back for hdcp operatoion
	void (*hdcp_cb)(void);
	void (*hdcp_irq_cb)(int);
	int (*hdcp_power_on_cb)(void);
	void (*hdcp_power_off_cb)(void);
};

struct hdmi_property {
	char *name;
	int videosrc;
	int display;
	void *priv;
};

struct hdmi_video_para {
	int vic;
	int input_mode;		//input video data interface
	int input_color;	//input video color mode
	int output_mode;	//output hdmi or dvi
	int output_color;	//output video color mode
};

struct hdmi {
	struct device	*dev;
	struct clk		*hclk;				//HDMI AHP clk
	int             id;					//HDMI id
	int 			regbase;
	int				irq;
	int				regbase_phy;
	int				regsize_phy;
	struct rk_lcdc_device_driver *lcdc;
	
	struct rk_display_device *ddev;		//Registered display device
	#ifdef CONFIG_SWITCH
	struct switch_dev	switch_hdmi;
	#endif
	
	struct hdmi_property *property;
	struct hdmi_ops *ops;
	
	struct mutex lock;
	struct workqueue_struct *workqueue;
	struct delayed_work delay_work;
	
	int autoset;						// if true, auto set hdmi output mode according EDID.
	int mute;							// HDMI display status, 1 means mute display; 0 is unmute 
	spinlock_t	irq_lock;
	struct mutex enable_mutex;
	
	int wait;
	struct completion	complete;
	
	int suspend;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend	early_suspend;
#endif
	
	struct hdmi_edid edid;
	int enable;					// Enable HDMI output or not
	int sleep;							// Sleep flag
	int vic;					// HDMI output video mode code
	int mode_3d;						// HDMI output video 3d mode
	struct hdmi_audio audio;	// HDMI output audio type.
	
	int pwr_mode;				// power mode
	int hotplug;				// hot plug status
	int state;					// hdmi state machine status
	int autoconfig;				// if true, auto config hdmi output mode according to EDID.
	int command;				// HDMI configuration command
	int display;				// HDMI display status
	int xscale;					// x direction scale value
	int yscale;					// y directoon scale value
	int tmdsclk;				// TDMS Clock frequency
	
	int (*insert)(void);
	int (*remove)(void);
	void (*control_output)(int enable);
	int (*config_video)(struct hdmi_video_para *vpara);
	int (*config_audio)(struct hdmi_audio *audio);
	int (*detect_hotplug)(void);
	// call back for edid
	int (*read_edid)(int block, unsigned char *buff);
	int (*set_vif)(rk_screen * screen,bool connect);

	// call back for hdcp operatoion
	void (*hdcp_cb)(void);
	void (*hdcp_irq_cb)(int);
	int (*hdcp_power_on_cb)(void);
	void (*hdcp_power_off_cb)(void);
};

/* Event source */
#define HDMI_SRC_SHIFT		8
#define HDMI_SYSFS_SRC		(0x1 << HDMI_SRC_SHIFT)
#define HDMI_SUSPEND_SRC	(0x2 << HDMI_SRC_SHIFT)
#define HDMI_IRQ_SRC		(0x4 << HDMI_SRC_SHIFT)
#define HDMI_WORKQUEUE_SRC	(0x8 << HDMI_SRC_SHIFT)

/* Event */
#define HDMI_ENABLE_CTL			(HDMI_SYSFS_SRC		| 0)
#define HDMI_DISABLE_CTL		(HDMI_SYSFS_SRC		| 1)
#define HDMI_SUSPEND_CTL		(HDMI_SUSPEND_SRC	| 2)
#define HDMI_RESUME_CTL			(HDMI_SUSPEND_SRC	| 3)
#define HDMI_HPD_CHANGE			(HDMI_IRQ_SRC		| 4)
#define HDMI_SET_VIDEO			(HDMI_SYSFS_SRC		| 5)
#define HDMI_SET_AUDIO			(HDMI_SYSFS_SRC		| 6)
#define HDMI_SET_3D				(HDMI_SYSFS_SRC		| 7)

#define HDMI_DEFAULT_SCALE		100 // Galland
#define HDMI_AUTO_CONFIG		true

// HDMI default vide mode
#define HDMI_VIDEO_DEFAULT_MODE			HDMI_1920x1080p_60Hz  // HDMI_1280x720p_60HZ

// HDMI default audio parameter
#define HDMI_AUDIO_DEFAULT_TYPE 		HDMI_AUDIO_LPCM
#define HDMI_AUDIO_DEFAULT_CHANNEL		2
#define HDMI_AUDIO_DEFAULT_RATE			HDMI_AUDIO_FS_44100
#define HDMI_AUDIO_DEFAULT_WORDLENGTH	HDMI_AUDIO_WORD_LENGTH_16bit

#ifdef HDMI_DEBUG
#define hdmi_dbg(dev, format, arg...)		\
	dev_printk(KERN_INFO , dev , format , ## arg)
#else
#define hdmi_dbg(dev, format, arg...)	
#endif

#define hdmi_err(dev, format, arg...)		\
	dev_printk(KERN_ERR , dev , format , ## arg)

extern struct hdmi *hdmi;
extern struct hdmi *hdmi_register(struct hdmi_property *property, struct hdmi_ops *ops);
extern void hdmi_unregister(struct hdmi *hdmi);
extern struct delayed_work *hdmi_submit_work(struct hdmi *hdmi, int event, int delay, void *data);
extern struct rk_display_device* hdmi_register_display_sysfs(struct hdmi *hdmi, struct device *parent);
extern void hdmi_unregister_display_sysfs(struct hdmi *hdmi);
extern int hdmi_edid_parse_base(unsigned char *buf, int *extend_num, struct hdmi_edid *pedid);
extern int hdmi_edid_parse_extensions(unsigned char *buf, struct hdmi_edid *pedid);

extern void hdmi_init_modelist(struct hdmi *hdmi);
extern int hdmi_set_lcdc(struct hdmi *hdmi);
extern int hdmi_get_hotplug(void);
static int hdmi_set_info(struct rk29fb_screen *screen, unsigned int vic);
extern void hdmi_init_lcdc(struct rk29fb_screen *screen, struct rk29lcd_info *lcd_info);
extern int hdmi_sys_init(void);
extern int hdmi_sys_parse_edid(struct hdmi* hdmi);
extern const char *hdmi_get_video_mode_name(unsigned char vic);
extern int hdmi_videomode_to_vic(struct fb_videomode *vmode);
extern const struct fb_videomode* hdmi_vic_to_videomode(int vic);
extern int hdmi_add_videomode(const struct fb_videomode *mode, struct list_head *head);
extern struct hdmi_video_timing * hdmi_find_mode(int vic);
extern int hdmi_find_best_mode(struct hdmi* hdmi, int vic);
extern int hdmi_ouputmode_select(struct hdmi *hdmi, int edid_ok);
extern int hdmi_switch_fb(struct hdmi *hdmi, int vic);
extern void hdmi_work(struct work_struct *work);
#endif
