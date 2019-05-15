# Custom FiiO X5iii Kernel
Created by jkbuha, props to @FiiO for releasing

Features
=========

CPU overclock from 1416MHz to 1752MHz

GPU overclock from 200MHz to 800MHz

DDR overclock from 133MHz to 720MHz

New Schedulers: Zen, Maple, Row, Sio, VR, BFQ

New Governors: InteractiveX

New TCP/IP: Westwood

init.d support

init.d optimisation scripts - thanks to prispewnic of 4PDA!

frandom generator instead of urandom

ARM RSEM implementation

Kernel LZ4 compression instead of LZO

fsync disable

UKSM instead of KSM

ZRAM implementation

zsmalloc implementation

zcache implementation

Changelog
=========

Version 3.11

Miscellaneous bugfixes

Version 3.1

Single installer for all kernel versions
New optimised voltages for 1416, 1704 and 2016 kernels (more stable and better performance)
Customisable removal of unwanted system applications
Customisable removal of system equalizer
Customising different audio_flinger libraries (different sound profiles to suit taste)
Enable/Disable Boot-up animation
Enable/Disable custom application on bootup

Version 3.02

New: 1704MHz release for players that can work at higher frequencies than 1416MHz (install this version if you previously had reboots at 2016MHz)
Enhanced: Increased stability without performance decrease
Enhanced: CPU down-volting for performance kernel version (2016MHz)
Fix: Removed additional CPU frequencies for 1416MHz kernel version (increased stability)

Version 3.01
New: 1752MHz intermediary version
Enhanced: More step frequencies from 504 to 2016MHz
Enhanced: Better handling of frequencies at high CPU loads
Enhanced: Powersave governor enabled by default

Version 3.0
New: Upstreamed relevant portions of kernel source code to Linux 3.10.49
New: AdFree hosts file - 67825 exceptions for no spamware, faster browsing
New: Support for DSD256
New: CPU Boost algorithm to increase frequency on screen touch
New: RQ Affinity for faster I/O scheduling
New: CPU SMP feature for faster processing
New: PegasusQ CPU governor
New: Improved interactiveX governor with increased performance and more power saving on idle (2 core shutdown)
New: Custom AK4490 driver with up to 768kHz oversampling
New: Custom plugin module for AK4490 to add more audio filters in future
New: CPU Maximum overclocking to 2016MHz! (Super significant performance improvement)
New: Current CPU frequency 1896MHz (Significant performance improvement and good stability)
New: GPU overclocking to 500MHz (More fluid screen responsiveness)
New: DDR overclocking to 720MHz (Ultra-fast memory and software processing)
New: Reduced Graphics requirements from 225MB down to 80MB (20% more available RAM)
New: Adaptive Low Memory Killer (A-LMK) algorithm
New: A-LMK optimised scripting (thanks again to prispewnic)
New: Compaction algorithm for low memory killer
New: FiiO RK3188/RK3188T efuse algorithm handling
New: VM Pressure algorithm implementation
Enhanced: zram and zcache now compressed with LZ4
Enhanced: better voltage regulation code
Enhanced: support for apll, cpll and gpll clocks on RK30 board
Enhanced: better GPIO code specific to FiiO player
Enhanced: removed unnecessary encryption
Enhanced: lowered minimum screen brightness for better battery lifetime

Version 2.8
Performance Build
New: Disabled CRC checking in all kernel code (significant performance improvement)
New: Optimised compilation parameters (-Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer)
New: Fine-tuned kernel optimisation for RK3188 (-mtune=cortex-a9)
New: Optimised ARM RWSEM algorithm

Version 2.7
New: Redesigned Busybox installer
New: Redesigned Permissive SE Linux installer
New: Kernel version now displayed in settings
Fix: Corrections to anyone who was not seeing the msg "Kernel optimisation Ready" prompt

Version 2.6
New: UKSM instead of KSM (much faster)
New: ZRAM (more RAM)
New: fsync_disable (much, much faster)
New: ROW I/O scheduler for SD cards (much faster)
New: LZ4 Compression instead of LZO (faster, more compression)
New: zsmalloc implementation instead of xsmalloc
New: zcache enabling (faster)
New: ARM RSEM implementation (faster)
New: frandom generator instead of random (much, much faster)
New: MSG pop-up notification daemon
Fix: Lowered voltage for some FiiO players with lower-grade VCOs
Fix: Corrected git file dependencies
Fix: Disabled system animation by default

Version 2.5
New: Increased max clock speed to 1752MHz (faster)
New: BFQ I/O scheduler (faster)
New: Westwood TCP/IP congestion protocol (much faster)
New: InteractiveX governor (much, much, much faster)
Fix: Stability checks to voltage
Fix: iX handling changes

Version 2.4
New: Maple I/O Scheduler (faster)
New: init.d full support (much, much faster)
New: init.d scripts (thanks prispewnic @ 4PDA)

Version 2.3
New: Zen I/O scheduler (much faster)

Version 2.2
Fix: Changes to scheduler priorities during screen off (faster)

Version 2.1
New: Additional GPU and DDR overclocking (much, much faster)

Version 1.4
New: Update to Linaro gcc 4.9.4 (significantly much faster)

Version 1.3
New: SIO I/O scheduler (faster)
New: VR I/O scheduler (faster)
New: CPU clock to 1704MHz (much, much faster)

Version 1.2
New: Additional voltage optimisation and DDR overclocking

Version 1.1
New: GPIO optimisation and documentation
New: CPU clock to 1518MHz (faster)

Version 1.0
Initial system release and branch-off from FiiO GPL code


Want to help increase support for after-market Android players? 
Drop me an email at jkbuha [at] protonmail.com

