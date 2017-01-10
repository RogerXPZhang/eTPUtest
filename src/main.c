#include "includes.h"


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

void etpu_globalexception_isr(void)
{
   int32_t  hsrr;

   /* If Microcode Global Exception is asserted */
   if (eTPU->MCR.B.MGE1 == 1)
   {
      Fault = FAULT_MICROCODE_GE;
   }
   /* If Illegal Instruction Flag is asserted */
   else if(eTPU->MCR.B.ILF1 == 1)
   {
      Fault = FAULT_ILLEGAL_INSTR;
   }
   /* If SCM MISC Flag is asserted 
      Note that this flag could be asserted on valid SCM contents on MCF523x 
      due to a bug, see device errata */
   else if(eTPU->MCR.B.SCMMISF == 1)
   {
      Fault = FAULT_MISC;
   }

   /* Reset required speed */
   speed_required_rpm = 0;

   /* Disable generation of PWM signals */
   hsrr = fs_etpu_pwmmac_disable_3ph(PWMMAC0_MASTER,FS_ETPU_PWMMAC_PIN_LOW,FS_ETPU_PWMMAC_PIN_LOW);

   if(hsrr==0)
   {
       /* Clear eTPU global interrupt flag */
       eTPU->MCR.B.GEC = 1;
   }
}

void printSCI(const char *str)
{
	uint32_t len;
	char strOutput[50];
	len = sprintf(strOutput,str);
	SCI_transmitData(strOutput,len);
}

uint8_t sCommandParse(uint8_t *data, uint8_t *length)
{
	uint8_t ii,jj;
	uint8_t tempPara[3];
	
	
	/* initial command state */
	command.commandState = WAITFORTARGET;
	command.comID = COMMAND_NONE;
	command.parameter = 0;
	
	
	for(ii=0;ii<*length;ii++)
	{
		if(command.commandState==WAITFORTARGET)
		{
			switch(data[ii])
			{
				case 'i':
				{
					command.selTarget = INNERACTUATOR;
					command.commandState = WAITFORCOMMAND;
					break;
				}
				case 'o':
				{
					command.selTarget = OUTERACTUATOR;
					command.commandState = WAITFORCOMMAND;
					break;
				}
				case 'b':
				{
					command.selTarget = BOTHACTUATOR;
					command.commandState = WAITFORCOMMAND;
					break;
				}
				case 'e':
				{
					/* show encoder value */
					command.comID = COMMAND_SHOW_ENCODER;
					command.commandState = COMMANDCOMPLETE;
					break;
				}
				default:
					break;
			}
		}
		else if(command.commandState==WAITFORCOMMAND)
		{
			switch(data[ii])
			{
				case 'z':
				{
					
					command.comID = COMMAND_RUN;
					command.commandState = WAITFORPARA;
					break;	
				}
				case 'n':
				{
					command.comID = COMMAND_RUN_ON_POSITION;
					command.commandState = WAITFORPARA;
					break;						
				}
				case 'p':
				{
					command.comID = COMMAND_PLUG;
					command.commandState = COMMANDCOMPLETE;	
				}
				default:
					break;
			}
		}
		else if(command.commandState==WAITFORPARA)
		{
			/* read the rest numbers */
			uint8_t numOfData = 0;
			for(;ii<*length;ii++)
			{
				if((data[ii]>='0')&&(data[ii]<='9'))
				{
					tempPara[numOfData++] = data[ii];
				}
			}
			if(!numOfData)
			{
				for(jj=0;jj<numOfData;jj++)
				{
					command.parameter += pow(10,numOfData-jj-1);
				}
				command.commandState = COMMANDCOMPLETE;
			}
		}
	}
	return command.commandState;
}

void HWInitPowerOn(void)
{
	initSysclk();      /* Set sysclk = 80MHz running from PLL */	
	//SIU.PCR[116].R = 0x0200; /* Set external interrupt PIN*/
	//init_SinWAVE();
	//initADC0();
	//initGPIO();
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
	INTC_InstallINTCInterruptHandler(etpu_globalexception_isr,67,1);
}


void showControlMenu(void)
{
	printSCI("Menu:\n");
	printSCI("Please input command:\n");
}


 
void main (void) 
{
	//uint32_t len;
	ParaInitPowerOn();
	HWInitPowerOn();
	INTInitPowerOn();
	my_system_etpu_init();

	enableIrq();
	
	my_system_etpu_start();
	
	/* Print SW version and Sys info */
	//len = sprintf(sInitWord,"SW Version: %d.%d\n",SW_VERSION_MAJOR,SW_VERSION_MINOR);
	//SCI_transmitData(sInitWord,len);

	//setAmplitudeSinWAVE(100);
	//setFrequencySinWAVE(80);


	for(;;)   //main loop
	{
		/* =======Super Fast Task========= */
	
		//sinWaveTask();

		
		/* =======Every 10 ms task======== */
		
		if(10==cnt10msTask)
		{
			//GPIOScheduleTaskShort();	
		/*==============================================*/
		/*
			uint8_t dataLen;
			uint8_t dataSerialData[20];
			
			SCI_readDataTask(dataSerialData,&dataLen);
			if(dataLen)
			{
				sCommandParse(dataSerialData,&dataLen);
			}
		*/
		/*==============================================*/
			cnt10msTask = 0;
		}
		
		
		
		
		if(cnt100msTask)
		{		
			//GPIOScheduleTaskLong();
			cnt100msTask = 0;
		}
	}
}


