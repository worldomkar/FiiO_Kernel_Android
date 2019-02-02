cmd_arch/arm/lib/csumpartialcopyuser.o := /home/jkbuha/gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-gcc -Wp,-MD,arch/arm/lib/.csumpartialcopyuser.o.d  -nostdinc -isystem /home/jkbuha/gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabi/bin/../lib/gcc/arm-linux-gnueabi/4.9.4/include -I/home/jkbuha/src/FiiO_Kernel_Android/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-rk3188/include -Iarch/arm/plat-rk/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float        -c -o arch/arm/lib/csumpartialcopyuser.o arch/arm/lib/csumpartialcopyuser.S

source_arch/arm/lib/csumpartialcopyuser.o := arch/arm/lib/csumpartialcopyuser.S

deps_arch/arm/lib/csumpartialcopyuser.o := \
    $(wildcard include/config/cpu.h) \
  /home/jkbuha/src/FiiO_Kernel_Android/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/jkbuha/src/FiiO_Kernel_Android/arch/arm/include/asm/linkage.h \
  /home/jkbuha/src/FiiO_Kernel_Android/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/rk/check/uaccess.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  /home/jkbuha/src/FiiO_Kernel_Android/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/jkbuha/src/FiiO_Kernel_Android/arch/arm/include/asm/hwcap.h \
  /home/jkbuha/src/FiiO_Kernel_Android/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
  /home/jkbuha/src/FiiO_Kernel_Android/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  /home/jkbuha/src/FiiO_Kernel_Android/arch/arm/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  arch/arm/lib/csumpartialcopygeneric.S \

arch/arm/lib/csumpartialcopyuser.o: $(deps_arch/arm/lib/csumpartialcopyuser.o)

$(deps_arch/arm/lib/csumpartialcopyuser.o):
