#include "includes.h"
//#include "mpc563m_vars.h"
#include "etpu_gct.h"
#include "etpu_pwmmac.h"
#include "etpu_util.h"



#define SW_VERSION_MAJOR	0
#define SW_VERSION_MINOR	5


uint32_t varPWMChangeCycle;
vuint8_t Fault = FAULT_NO;
int32_t speed_required_rpm = 0;

char sInitWord[50];



#define WAITFORTARGET	3
#define WAITFORCOMMAND	2
#define WAITFORPARA		1
#define COMMANDCOMPLETE 0

#define COMMAND_NONE				0
#define COMMAND_SHOW_ENCODER		1
#define COMMAND_RUN					2
#define COMMAND_RUN_ON_POSITION		3
#define COMMAND_PLUG				4

struct strCommand
{
	uint8_t comID;
	uint32_t parameter;
	uint8_t selTarget;
	uint8_t commandState;
}command;


void enableIrq(void) {
  INTC.CPR.B.PRI = 0;	       		/* MPC563x: Lower INTC's current priority */
  asm(" wrteei 1");	    	   		/* Enable external interrupts */
}

void disableIrq(void) {
  INTC.CPR.B.PRI = 0;	       		/* MPC563x: Lower INTC's current priority */
  asm(" wrteei 0");	    	   		/* Enable external interrupts */
}

void printSCI(const char *str)
{
	uint32_t len;
	char strOutput[50];
	len = sprintf(strOutput,str);
	SCI_transmitData(strOutput,len);
}

void HWInitPowerOn(void)
{
	initSysclk();      /* Set sysclk = 80MHz running from PLL */	
	//SIU.PCR[116].R = 0x0200; /* Set external interrupt PIN*/

	//init_SinWAVE();
	//initADC0();
	//initGPIO();

	SIU.PCR[121].R = 0x0E00;          /* Configure pad for signal ETPU_A[7] output*/
	SIU.PCR[122].R = 0x0E00;          /* Configure pad for signal ETPU_A[8] output*/
	SIU.PCR[123].R = 0x0E00;          /* Configure pad for signal ETPU_A[9] output*/
	SIU.PCR[124].R = 0x0E00;          /* Configure pad for signal ETPU_A[10] output*/
	SIU.PCR[125].R = 0x0E00;          /* Configure pad for signal ETPU_A[11] output*/
	SIU.PCR[126].R = 0x0E00;          /* Configure pad for signal ETPU_A[12] output*/
	SIU.PCR[127].R = 0x0E00;          /* Configure pad for signal ETPU_A[13] output*/
	
	SIU.PCR[114].R = 0x0200; 
	
	 
	 
}

void ParaInitPowerOn(void)
{

}

void INTInitPowerOn(void)
{
	init_PIT_BackgroundTask();	
	init_PIT_Sinwave();
	
	/* Install interrupts */	
	INTC_InstallINTCInterruptHandler(PIT1_ISR,PIT1INT,2);
	INTC_InstallINTCInterruptHandler(PIT0_ISR,PIT0INT,5);
	INTC_InstallINTCInterruptHandler(PIT2_ISR,PIT2INT,2);
	//INTC_InstallINTCInterruptHandler(etpu_globalexception_isr,67,1);
	//INTC_InstallINTCInterruptHandler(etpu_ch7_ISR,ETPUCH7,2);
}


void showControlMenu(void)
{
	printSCI("Menu:\n");
	printSCI("Please input command:\n");
}


 
void main (void) 
{
	//uint32_t len;
	int32_t error_code;
	//uint16_t i;
	
	ParaInitPowerOn();
	HWInitPowerOn();
	INTInitPowerOn();
	//fs_etpu_interrupt_disable();
	/*
	for(i=0;i<256;i++)
	{
		etpuSineTable[i] = 0x7FFFFF*SIN_DATA_RAW[i]/255;
	}
	*/
	setFrequencySinWAVE(80);
	setAmplitudeSinWAVE(100);
	
	error_code = my_system_etpu_init();

	enableIrq();


	
	my_system_etpu_start();
	PIT.TIMER[0].TCTRL.R = 0X00000003;  //  Turn on TIMER 0 TIE and TEN to fire
	
	//fs_etpu_pwmmac_update_vector(PWMMAC0_MASTER,0x7FFFFF,0);



	for(;;)   //main loop
	{
		/* =======Super Fast Task========= */
	
		sinWaveTask();

		
		/* =======Every 10 ms task======== */
		
		if(10==cnt10msTask)
		{
			//fs_etpu_pwmmac_update_duty_cycles(PWMMAC0_MASTER,etpuSineTable[OFFSETA++],etpuSineTable[OFFSETB++],etpuSineTable[OFFSETC++]);
			
			//GPIOScheduleTaskShort();	
			/*
			tphaseA+=1000;
			tphaseB+=1000;
			tphaseC+=1000;
			error_code = fs_etpu_pwmmac_update_duty_cycles(PWMMAC0_MASTER,tphaseA,tphaseB,tphaseC);
			*/
			cnt10msTask = 0;
		}
		
		
		
		
		if(cnt100msTask)
		{		
			//GPIOScheduleTaskLong();
			
			cnt100msTask = 0;
		}
	}
}


