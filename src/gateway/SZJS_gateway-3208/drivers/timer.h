#ifndef __TIMER_H__
#define __TIMER_H__

//#include "proj_arch.h"

#define TIM5_DELAY_DEFAULT  (50-1)  /* 50 * 20uS = 1000uS */

extern void tim1_init(void);
extern void tim2_init(void);
extern void tim3_init(void);


#endif // End of timer.h
