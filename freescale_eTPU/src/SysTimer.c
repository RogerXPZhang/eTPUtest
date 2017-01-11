#include "includes.h"

uint8_t cnt10msTask,cnt100msTask;
uint32_t timerSYS;

void initSysclk (void) {

  FMPLL.ESYNCR1.B.CLKCFG = 0X7;       /* Change clk to PLL normal mode from crystal */  
  FMPLL.SYNCR.R = 0x18080000;   // 0001 1000 0000 1000 0000 0000 0000 0000      PREDIV = 1; MFD = 16; RFD = 1
  while (FMPLL.SYNSR.B.LOCK != 1) {}; /* Wait for FMPLL to LOCK  */
  FMPLL.SYNCR.R = 0x18000000;   // 0001 1000 0000 0000 0000 0000 0000 0000      PREDIV = 1; MFD = 16; RFD = 0      fsys = fref * (MFD+4)/(PREDIV+1)/2^RFD = 8*20/2/1 = 80mhz
}

void init_PIT_BackgroundTask(void)
{
  PIT.MCR.R = 0X00000001;     	 // Timers are stopped in debug mode                 
  PIT.TIMER[1].LDVAL.R = 80000;  	 // Sys freq = 80Mhz, reset value 80000, interrupt cycle is 1ms     
  PIT.TIMER[1].TCTRL.R = 0X00000003; // TIE: Interrupt will be requested whenever TIF is set  									  
  									 // TEN: Timer will be active
  									 
  PIT.TIMER[2].LDVAL.R = 8000000;  	 // Sys freq = 80Mhz, reset value 80000, interrupt cycle is 100ms     
  PIT.TIMER[2].TCTRL.R = 0X00000003; // TIE: Interrupt will be requested whenever TIF is set  									                         
}

void PIT1_ISR(void)
{ 
  PIT.TIMER[1].TFLG.B.TIF = 1; 
  cnt10msTask++;
  timerSYS++;
}

void PIT2_ISR(void)
{ 
  PIT.TIMER[2].TFLG.B.TIF = 1; 
  cnt100msTask++;
}