#ifndef _SYSTIMER_H_
#define _SYSTIMER_H_

extern uint8_t cnt10msTask,cnt100msTask;
extern uint32_t timerSYS;


extern void PIT1_ISR(void);
extern void PIT2_ISR(void);

#endif