#ifndef _VXB_TIMER_LIB_H_
#define _VXB_TIMER_LIB_H_

#define VXB_TIMER_CAN_INTERRUPT 0x1
#define VXB_TIMER_INTERMEDIATE_COUNT 0x2
#define VXB_TIMER_SIZE_16 0x4
#define VXB_TIMER_SIZE_32 0x8
#define VXB_TIMER_SIZE_64 0x10
#define VXB_TIMER_CANNOT_DISABLE 0x20
#define VXB_TIMER_STOP_WHILE_READ 0x40
#define VXB_TIMER_AUTO_RELOAD 0x80
#define VXB_TIMER_CANNOT_MODIFY_ROLLOVER 0x100
#define VXB_TIMER_CANNOT_SUPPORT_ALL_FREQS 0x200
#define VXB_TIMER_SIZE_23 0x400
#define VXB_TIMER_IEEE1588 0x800
#define VXB_TIMER_DEADLINE 0x1000
#define VXB_TIMER_CPU_LOCAL 0x2000

#define SYSCLK_OPTIMUM_MINFRQ 10
#define SYSCLK_OPTIMUM_MAXFRQ 5000
#define SYSCLK_MINFRQ_LIMIT 60
#define SYSCLK_MAXFRQ_LIMIT 300

#define KHz 1000

void vxbMsDelay(int ms);
void vxbUsDelay(int us);

#endif