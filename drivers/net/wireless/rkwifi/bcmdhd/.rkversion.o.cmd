cmd_drivers/net/wireless/rkwifi/bcmdhd/rkversion.o := /home/jkbuha/gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-gcc -Wp,-MD,drivers/net/wireless/rkwifi/bcmdhd/.rkversion.o.d  -nostdinc -isystem /home/jkbuha/gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabi/bin/../lib/gcc/arm-linux-gnueabi/4.9.4/include -I/home/jkbuha/src/FiiO_Kernel_Android/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-rk3188/include -Iarch/arm/plat-rk/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -marm -fno-dwarf2-cfi-asm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -Wall -Wstrict-prototypes -Dlinux -DBCMDRIVER -DSDTEST -DBCMDONGLEHOST -DUNRELEASEDCHIP -DBCMDMA32 -DBCMFILEIMAGE -DDHDTHREAD -DDHD_DEBUG -DSHOW_EVENTS -DBCMDBG -DGET_OTP_MAC_ENABLE -DWIFI_ACT_FRAME -DARP_OFFLOAD_SUPPORT -DSUPPORT_PM2_ONLY -DKEEP_ALIVE -DPKT_FILTER_SUPPORT -DPNO_SUPPORT -DDHDTCPACK_SUPPRESS -DDHD_DONOT_FORWARD_BCMEVENT_AS_NETWORK_PKT -DRXFRAME_THREAD -DBCMSDIOH_TXGLOM_EXT -DENABLE_INSMOD_NO_FW_LOAD -Idrivers/net/wireless/rkwifi/bcmdhd -Idrivers/net/wireless/rkwifi/bcmdhd/include -DBCMSDIO -DMMC_SDIO_ABORT -DBCMLXSDMMC -DUSE_SDIOFIFO_IOVAR -DBDC -DPROP_TXSTATUS -DDHD_USE_IDLECOUNT -DBCMSDIOH_TXGLOM -DCUSTOM_SDIO_F2_BLKSIZE=128 -DOOB_INTR_ONLY -DHW_OOB -DCUSTOMER_OOB -DCUSTOMER_HW2 -DGET_CUSTOM_MAC_ENABLE -DSOFTAP -DWL_WIRELESS_EXT -DUSE_IW -DWL_CFG80211 -DWLP2P -DWL_CFG80211_STA_EVENT -DWL_ENABLE_P2P_IF -DWL_IFACE_COMB_NUM_CHANNELS -DCUSTOM_ROAM_TRIGGER_SETTING=-65 -DCUSTOM_ROAM_DELTA_SETTING=15 -DCUSTOM_KEEP_ALIVE_SETTING=28000 -DCUSTOM_PNO_EVENT_LOCK_xTIME=7 -DWL_SUPPORT_AUTO_CHANNEL -DWL_SUPPORT_BACKPORTED_KPATCHES -DESCAN_RESULT_PATCH -DVSDB -DWL_CFG80211_VSDB_PRIORITIZE_SCAN_REQUEST -DWLTDLS -DMIRACAST_AMPDU_SIZE=8 -DPROP_TXSTATUS_VSDB -DBUILD_IN_KERNEL    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(rkversion)"  -D"KBUILD_MODNAME=KBUILD_STR(bcmdhd)" -c -o drivers/net/wireless/rkwifi/bcmdhd/rkversion.o drivers/net/wireless/rkwifi/bcmdhd/rkversion.c

source_drivers/net/wireless/rkwifi/bcmdhd/rkversion.o := drivers/net/wireless/rkwifi/bcmdhd/rkversion.c

deps_drivers/net/wireless/rkwifi/bcmdhd/rkversion.o := \
    $(wildcard include/config/bcmdhd/fw/path.h) \
    $(wildcard include/config/bcmdhd/nvram/path.h) \

drivers/net/wireless/rkwifi/bcmdhd/rkversion.o: $(deps_drivers/net/wireless/rkwifi/bcmdhd/rkversion.o)

$(deps_drivers/net/wireless/rkwifi/bcmdhd/rkversion.o):
