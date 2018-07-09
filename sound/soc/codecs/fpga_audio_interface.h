#ifndef __FPGA_AUDIO_INTERFACE_H
#define __FPGA_AUDIO_INTERFACE_H

// enum  FPGA_COMMAND{
    // FPGA_START              =0x09,
    // FPGA_SUSPEND            =0x40,
    // FPGA_AUDIO_DSD          =0xcc,
    // AUDIO_SAMPLERATE_48K    =0x53,
    // AUDIO_SAMPLERATE_96K    =0x51,
    // AUDIO_SAMPLERATE_192K   =0x50,
    // AUDIO_SAMPLERATE_44_1K  =0x4b,
    // AUDIO_SAMPLERATE_88_2K  =0x49,
    // AUDIO_SAMPLERATE_176_4K =0x48,
// };
enum  FPGA_COMMAND16{
    FPGA_SPI_INITSQ           =0xFF,//0x09*2+1,
    FPGA_PRE_CMD              =0xFE,//0x09*2+1,
    FPGA_START16              =0x13,//0x09*2+1,
    FPGA_SUSPEND16            =0x80,//0x40*2+0,
    FPGA_AUDIO_DSD16          =0x199,//0xcc*2+1,
    AUDIO_SAMPLERATE_24K16    =0xAF,//0x53*2+1,
    AUDIO_SAMPLERATE_32K16    =0xAD,//0x53*2+1,
    AUDIO_SAMPLERATE_64K16    =0xAA,//0x53*2+1,
    AUDIO_SAMPLERATE_128K16   =0xA2,//0x53*2+1,
    AUDIO_SAMPLERATE_48K16    =0xA7,//0x53*2+1,
    AUDIO_SAMPLERATE_96K16    =0xA3,//0x51*2+1,
    AUDIO_SAMPLERATE_192K16   =0xA1,//0x50*2+1,
    AUDIO_SAMPLERATE_384K16   =0xE0,//0x50*2+1,
    AUDIO_SAMPLERATE_22_05K16  =0x9F,//0x4b*2+1,
    AUDIO_SAMPLERATE_44_1K16  =0x97,//0x4b*2+1,
    AUDIO_SAMPLERATE_88_2K16  =0x93,//0x49*2+1,
    AUDIO_SAMPLERATE_176_4K16 =0x91,//0x48*2+1,
    AUDIO_SAMPLERATE_352_8K16 =0xD0,//0x48*2+1,

    AUDIO_FORMAT_DOP_64=0x199,
    AUDIO_FORMAT_DOP_128=0x198,
    AUDIO_FORMAT_DOP_256=0x190,
    
    AUDIO_FORMAT_SOP_96K=0x23,
    AUDIO_FORMAT_SOP_192K=0x21,
    AUDIO_FORMAT_SOP_384K=0x60,
    AUDIO_FORMAT_SOP_88_2K=0x13,
    AUDIO_FORMAT_SOP_176_4K=0x11,
    AUDIO_FORMAT_SOP_352_8K=0x50,


    //for X5 clock 512FS
    AUDIO_SAMPLERATE_32K_S512FS=0xA5,
    AUDIO_SAMPLERATE_48K_S512FS=0xA3,
    AUDIO_SAMPLERATE_64K_S512FS=0xA2,
    AUDIO_SAMPLERATE_96K_S512FS=0xA1,
    AUDIO_SAMPLERATE_192K_S512FS=0xA0,

    AUDIO_SAMPLERATE_44_1K_S512FS=0x93,
    AUDIO_SAMPLERATE_88_2K_S512FS=0x91,
    AUDIO_SAMPLERATE_176_4K_S512FS=0x90,

    // for X5 SOP
    AUDIO_SAMPLERATE_352_8K_S512FS=0x110,
    AUDIO_SAMPLERATE_384K_S512FS=0x120,

    //for X7II clock 1024FS
     AUDIO_SAMPLERATE_32K_S1024FS=0xAD,
     AUDIO_SAMPLERATE_48K_S1024FS=0xAB,
     AUDIO_SAMPLERATE_64K_S1024FS=0xAA,
     AUDIO_SAMPLERATE_96K_S1024FS=0xA9,
     AUDIO_SAMPLERATE_192K_S1024FS=0xA8,
     // AUDIO_SAMPLERATE_384K_S1024FS=0x20,
     AUDIO_SAMPLERATE_384K_S1024FS=0x60,
     AUDIO_SAMPLERATE_44_1K_S1024FS=0x9B,
     AUDIO_SAMPLERATE_88_2K_S1024FS=0x99,
     AUDIO_SAMPLERATE_176_4K_S1024FS=0x98,
     AUDIO_SAMPLERATE_352_8K_S1024FS=0x50,

     AUDIO_SAMPLERATE_SUSPEND16 =0x1000,
#if 0

    AUDIO_SAMPLERATE_48K_S1024FS=0xAB,
    AUDIO_SAMPLERATE_96K_S1024FS=0xA9,
    AUDIO_SAMPLERATE_192K_S1024FS=0xA8,

    AUDIO_SAMPLERATE_44_1K_S1024FS=0x9B,
    AUDIO_SAMPLERATE_88_2K_S1024FS=0x99,
    AUDIO_SAMPLERATE_176_4K_S1024FS=0x98,
    AUDIO_SAMPLERATE_352_8K_S1024FS=0x50,
#endif


    //for X7II
    AUDIO_SAMPLERATE_48K    =0x48,
    AUDIO_SAMPLERATE_96K    =0x44,
    AUDIO_SAMPLERATE_192K   =0x42,
    AUDIO_SAMPLERATE_384K   =0x41,
    AUDIO_SAMPLERATE_44_1K  =0x28,
    AUDIO_SAMPLERATE_88_2K  =0x24,
    AUDIO_SAMPLERATE_176_4K =0x22,
    AUDIO_SAMPLERATE_352_8K =0x21,

    AUDIO_SAMPLERATE_32K    =0x4C,
    AUDIO_SAMPLERATE_64K    =0x46,
    AUDIO_SAMPLERATE_128K   =0x43,

};


typedef enum fmt_type{

    FMT_SOP=0x00,
    FMT_PCM=0x01,
    FMT_DSD=0x03,

    // SPDIF & DSD
    FMT_FIIO=0x2,
} fpga_fmt_t;


// extern int audio_fpga_bridge_sendCommand( enum FPGA_COMMAND cmd);
// extern int audio_fpga_bridge_sendCommand(u8 cmd);
extern int audio_fpga_bridge_sendCommand(u16 cmd);
extern void audioBridge_rate_set(int rate,fpga_fmt_t fmt);
extern const struct snd_kcontrol_new* get_audiobirdge_control_table(int* num_controls);
extern bool audio_fpga_current_mode(fpga_fmt_t mod);

struct fpga_interface_platform_data{
    int reset_pin ;

};


#endif  // __FPGA_AUDIO_INTERFACE_H 