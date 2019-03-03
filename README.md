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

