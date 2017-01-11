#include "includes.h"

uint8_t fPWMRunStatus;  //PWMSTATUS_OFF, PWMSTATUS_RUNTIME, PWMSTATUS_RUNPULSE
uint8_t fButton0,fButton1,fButton2;
uint32_t tDebounceButton0,tDebounceButton1,tDebounceButton2;
uint32_t tPWMRuntime;
uint32_t cntEncoder;
uint8_t fIndexHit;
uint8_t fWaitForIndex;
uint32_t cntPWMRunpulse, cntPWMStartCount;

/*
void initEMIOS_0ch0(void) {        		// EMIOS 0 CH 0: Modulus Up Counter 
	EMIOS.CH[0].CADR.R = 39999999;   	// Period will be 39999999+1 = 40000000 clocks (2000 msec)
	EMIOS.CH[0].CCR.B.MODE = 0x50; 	// Modulus Counter Buffered (MCB) 
	EMIOS.CH[0].CCR.B.BSL = 0x3;   	// Use internal counter 
	EMIOS.CH[0].CCR.B.UCPRE= 3;     // Set channel prescaler to divide by 4, * step is 1/20mhz=0.05us
	EMIOS.CH[0].CCR.B.FREN = 1;   	// Freeze channel counting when in debug mode
	EMIOS.CH[0].CCR.B.UCPREN = 1;   // Enable prescaler; uses default divide by 1
}
*/

void GPIOn_ISR(void)
{
	if(SIU.EISR.B.EIF8)
	{
		fIndexHit = 1;
		SIU.EISR.B.EIF8 = 1;
	}
	if(SIU.EISR.B.EIF9)
	{
		cntEncoder++;
		SIU.EISR.B.EIF9 = 1;
		
	}
}

void initGPIO(void)
{		
	SIU.PCR[116].R = 0x200;  						// LED5, as GPIO output
	SIU.PCR[119].R = 0x200;  						// LED6, as GPIO output
	SIU.GPDO[116].B.PDO = 1; 						// output high, LED off
	SIU.GPDO[119].B.PDO = 1; 						// output high, LED off	
	SIU.PCR[179].R = 0x100;	 						// Button0, as GPIO input
	SIU.PCR[181].R = 0x100;	 						// Button1, as GPIO input
	SIU.PCR[183].R = 0x100;	 						// Switch0, as GPIO input
	SIU.PCR[134].R = 0x900;	 						// index, as IRQ8, input (eTPU 20)
	SIU.PCR[135].R = 0x900;	 						// encoder, as IRQ9, input (eTPU 21)
	SIU.PCR[137].R = 0x100;	 						// direction switch, input

	SIU.GPDO[179].B.PDO = 0;	
	SIU.GPDO[181].B.PDO = 0;	
	SIU.GPDO[183].B.PDO = 0;
	
	SIU.IREER.B.IREE8 = 1;		//rising edge trigger is enabled
	SIU.IREER.B.IREE9 = 1;
		
	SIU.DIRER.B.EIRE9 = 1;		//enable encoder interrupt
}

void inline readSetADCFreq(void)
{
	uint32_t adcFreq;
	
	SendConvCmd();									// Set ADC request
	ReadResult(&adcFreq);							// read ADC result
	if(adcFreq>=0xD4)			
		setFrequencySinWAVE((adcFreq-0xD4)/80);		// Set sine wave frequency	
}

void inline runPWMForDuration(uint32_t time)
{
	if(!fPWMRunStatus)
	{
		
		tPWMRuntime = time/10;
		plug(SIU.GPDI[137].B.PDI); 						// Set phase direction
		readSetADCFreq();
		startSinWAVE();									// Start sine wave output
		fPWMRunStatus = PWMSTATUS_RUNTIME;
		SIU.GPDO[119].B.PDO = 0;
	}	
}

void inline runPWMForPulse(uint32_t pulse)
{
	if(!fPWMRunStatus)
	{
		cntPWMRunpulse = pulse;
		cntPWMStartCount = cntEncoder;
		plug(SIU.GPDI[137].B.PDI); 						// Set phase direction
		readSetADCFreq();
		startSinWAVE();									// Start sine wave output
		fPWMRunStatus = PWMSTATUS_RUNPULSE;
		SIU.GPDO[119].B.PDO = 0;
	}	
}

void GPIOScheduleTaskShort(void)
{
	if((fWaitForIndex)&&(fIndexHit))					// Check if index hits
	{
		//find index
		fIndexHit = 0;
		fWaitForIndex = 0;
		SIU.DIRER.B.EIRE8 = 0;							// disable index interrupt
		runPWMForPulse(1100ul);							// run 1100 pulses
	}
	
	switch(fPWMRunStatus)
	{
		case PWMSTATUS_OFF:
			break;
		case PWMSTATUS_RUNTIME:
		{
			if(tPWMRuntime)
			{
				tPWMRuntime--;
			}
			else
			{
				stopSinWAVE();
				fPWMRunStatus = PWMSTATUS_OFF;
				SIU.GPDO[119].B.PDO = 1;		
			}			
			break;
		}
		case PWMSTATUS_RUNPULSE:
		{
			if(cntEncoder>cntPWMStartCount+cntPWMRunpulse)
			{
				stopSinWAVE();
				fPWMRunStatus = PWMSTATUS_OFF;
				SIU.GPDO[119].B.PDO = 1;
			}				
			break;	
		}
		default:break;
	}	
	
	if(!SIU.GPDI[179].B.PDI)		//polling button0, for short run
	{
		if(!fButton0)
		{
			fButton0 = 1;
			tDebounceButton0 = timerSYS;			
		}		
	}
	else
	{
		if(fButton0)
		{
			if((timerSYS < tDebounceButton0)||(timerSYS - tDebounceButton0 >= BUTTONDEBOUNCETIME))
			{
				runPWMForDuration(150ul);
				fButton0 = 0;
			}			
		}
	}
	
	if(!SIU.GPDI[181].B.PDI)		//polling button1, for long run
	{
		if(!fButton1)
		{
			fButton1 = 1;
			tDebounceButton1 = timerSYS;			
		}	
	}
	else
	{
		if(fButton1)
		{
			if((timerSYS < tDebounceButton1)||(timerSYS - tDebounceButton1 >= BUTTONDEBOUNCETIME))
			{
				runPWMForDuration(1000ul);
				fButton1 = 0;
			}
		}
	}
	
	if(!SIU.GPDI[183].B.PDI)		//polling button2, for triggered run
	{
		if(!fButton2)
		{
			fButton2 = 1;
			tDebounceButton2 = timerSYS;			
		}		
	}
	else
	{
		if(fButton2)
		{
			if((timerSYS < tDebounceButton2)||(timerSYS - tDebounceButton2 >= BUTTONDEBOUNCETIME))
			{
				
				fWaitForIndex = 1;
				SIU.DIRER.B.EIRE8 = 1;		//enable index interrupt
				fButton2 = 0;
			}	
		}
	}
}

void GPIOScheduleTaskLong(void)
{
	if(!SIU.GPDI[137].B.PDI)
	{
		SIU.GPDO[116].B.PDO = 1;
	}
	else
	{
		SIU.GPDO[116].B.PDO = 0;
	}
}