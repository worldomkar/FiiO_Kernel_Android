cmd_kernel/time/built-in.o :=  /home/jkbuha/gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-ld.bfd -EL    -r -o kernel/time/built-in.o kernel/time/timekeeping.o kernel/time/ntp.o kernel/time/clocksource.o kernel/time/jiffies.o kernel/time/timer_list.o kernel/time/timecompare.o kernel/time/timeconv.o kernel/time/posix-clock.o kernel/time/alarmtimer.o kernel/time/clockevents.o kernel/time/tick-common.o kernel/time/tick-broadcast.o kernel/time/tick-oneshot.o kernel/time/tick-sched.o 
