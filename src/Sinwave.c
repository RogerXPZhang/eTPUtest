#include "MPC5634M_MLQC80.h"
#include "etpu_gct.h"

#define DEFAULTFREQUENCY	60
#define INNERACTUATORMASK  0b00000001
#define OUTERACTUATORMASK  0b00000010

#define NUMBEROFLIMS	2
#define PWMDEADTIME		160         //4us dead time: 4/0.0125/2=160
#define DEADTIME_ENABLE


uint16_t globalMax_a,globalMax_b;

uint8_t channelMask[2] = 
{
	OUTERACTUATORMASK,
	INNERACTUATORMASK	
};
volatile uint8_t OFFSETA = 0;
volatile uint8_t OFFSETB = 86;
volatile uint8_t OFFSETC = 171;
uint8_t varDirect = 0;

uint8_t indexA,indexB,indexC;
uint16_t varFrequency;
uint16_t flgPWMfreq;

/*
const uint16_t SIN_DATA_RAW[] = {
	128,  131,  134,  137,  140,  143,  146,  149,  152,  155,  158,  162,  165,  167,  170,  173,  
	176,  179,  182,  185,  188,  190,  193,  196,  198,  201,  203,  206,  208,  211,  213,  215,  
	218,  220,  222,  224,  226,  228,  230,  232,  234,  235,  237,  238,  240,  241,  243,  244,  
	245,  246,  248,  249,  250,  250,  251,  252,  253,  253,  254,  254,  254,  255,  255,  255,  
	255,  255,  255,  255,  254,  254,  254,  253,  253,  252,  251,  250,  250,  249,  248,  246,  
	245,  244,  243,  241,  240,  238,  237,  235,  234,  232,  230,  228,  226,  224,  222,  220,  
	218,  215,  213,  211,  208,  206,  203,  201,  198,  196,  193,  190,  188,  185,  182,  179,  
	176,  173,  170,  167,  165,  162,  158,  155,  152,  149,  146,  143,  140,  137,  134,  131,  
	128,  124,  121,  118,  115,  112,  109,  106,  103,  100,  97,   93,   90,   88,   85,   82, 
	79,   76,   73,   70,   67,   65,   62,   59,   57,   54,   52,   49,   47,   44,   42,   40, 
	37,   35,   33,   31,   29,   27,   25,   23,   21,   20,   18,   17,   15,   14,   12,   11, 
	10,   9,    7,    6,    5,    5,    4,    3,    2,    2,    1,    1,    1,    0,    0,    0,  
	0,    0,    0,    0,    1,    1,    1,    2,    2,    3,    4,    5,    5,    6,    7,    9,  
	10,   11,   12,   14,   15,   17,   18,   20,   21,   23,   25,   27,   29,   31,   33,   35, 
	37,   40,   42,   44,   47,   49,   52,   54,   57,   59,   62,   65,   67,   70,   73,   76, 
	79,   82,   85,   88,   90,   93,   97,   100,  103,  106,  109,  112,  115,  118,  121,  124};
*/

const uint16_t SIN_DATA_RAW[256] = {
 128,131,133,136,139,142,144,147
,150,153,155,158,161,163,166,168
,171,173,176,178,181,183,186,188
,190,192,195,197,199,201,203,205
,207,209,211,213,215,216,218,220
,221,223,224,225,227,228,229,230
,231,232,233,234,235,236,237,237
,238,238,239,239,239,240,240,240
,240,240,240,240,239,239,239,238
,238,237,237,236,235,234,233,232
,231,230,229,228,227,225,224,223
,221,220,218,216,215,213,211,209
,207,205,203,201,199,197,195,192
,190,188,186,183,181,178,176,173
,171,168,166,163,161,158,155,153
,150,147,144,142,139,136,133,131
,128,125,123,120,117,114,112,109
,106,103,101,98,95,93,90,88
,85,83,80,78,75,73,70,68
,66,64,61,59,57,55,53,51
,49,47,45,43,41,40,38,36
,35,33,32,31,29,28,27,26
,25,24,23,22,21,20,19,19
,18,18,17,17,17,16,16,16
,16,16,16,16,17,17,17,18
,18,19,19,20,21,22,23,24
,25,26,27,28,29,31,32,33
,35,36,38,40,41,43,45,47
,49,51,53,55,57,59,61,64
,66,68,70,73,75,78,80,83
,85,88,90,93,95,98,101,103
,106,109,112,114,117,120,123,125
};

const uint32_t SIN_DATA_RAW24[] = {
0x800000,0x83242a,0x8647d9,0x896a90,0x8c8bd3,0x8fab27,0x92c810,0x95e214,
0x98f8b8,0x9c0b82,0x9f19f9,0xa223a4,0xa5280c,0xa826b9,0xab1f34,0xae110a,
0xb0fbc5,0xb3def2,0xb6ba1f,0xb98cdc,0xbc56ba,0xbf1749,0xc1ce1e,0xc47acd,
0xc71cec,0xc9b414,0xcc3fdf,0xcebfe8,0xd133cc,0xd39b2a,0xd5f5a4,0xd842dc,
0xda8279,0xdcb420,0xded77c,0xe0ec37,0xe2f201,0xe4e888,0xe6cf80,0xe8a69e,
0xea6d98,0xec2428,0xedca0c,0xef5f02,0xf0e2cb,0xf2552c,0xf3b5eb,0xf504d2,
0xf641ae,0xf76c4e,0xf88483,0xf98a23,0xfa7d04,0xfb5d03,0xfc29fb,0xfce3ce,
0xfd8a5e,0xfe1d93,0xfe9d55,0xff0991,0xff6236,0xffa736,0xffd887,0xfff621,
0xffffff,0xfff621,0xffd887,0xffa736,0xff6236,0xff0991,0xfe9d55,0xfe1d93,
0xfd8a5e,0xfce3ce,0xfc29fb,0xfb5d03,0xfa7d04,0xf98a23,0xf88483,0xf76c4e,
0xf641ae,0xf504d2,0xf3b5eb,0xf2552c,0xf0e2cb,0xef5f02,0xedca0c,0xec2428,
0xea6d98,0xe8a69e,0xe6cf80,0xe4e888,0xe2f201,0xe0ec37,0xded77c,0xdcb420,
0xda8279,0xd842dc,0xd5f5a4,0xd39b2a,0xd133cc,0xcebfe8,0xcc3fdf,0xc9b414,
0xc71cec,0xc47acd,0xc1ce1e,0xbf1749,0xbc56ba,0xb98cdc,0xb6ba1f,0xb3def2,
0xb0fbc5,0xae110a,0xab1f34,0xa826b9,0xa5280c,0xa223a4,0x9f19f9,0x9c0b82,
0x98f8b8,0x95e214,0x92c810,0x8fab27,0x8c8bd3,0x896a90,0x8647d9,0x83242a,
0x800000,0x7cdbd5,0x79b826,0x76956f,0x73742c,0x7054d8,0x6d37ef,0x6a1deb,
0x670747,0x63f47d,0x60e606,0x5ddc5b,0x5ad7f3,0x57d946,0x54e0cb,0x51eef5,
0x4f043a,0x4c210d,0x4945e0,0x467323,0x43a945,0x40e8b6,0x3e31e1,0x3b8532,
0x38e313,0x364beb,0x33c020,0x314017,0x2ecc33,0x2c64d5,0x2a0a5b,0x27bd23,
0x257d86,0x234bdf,0x212883,0x1f13c8,0x1d0dfe,0x1b1777,0x19307f,0x175961,
0x159267,0x13dbd7,0x1235f3,0x10a0fd,0xf1d34,0xdaad3,0xc4a14,0xafb2d,
0x9be51,0x893b1,0x77b7c,0x675dc,0x582fb,0x4a2fc,0x3d604,0x31c31,
0x275a1,0x1e26c,0x162aa,0xf66e,0x9dc9,0x58c9,0x2778,0x9de,
0x0,0x9de,0x2778,0x58c9,0x9dc9,0xf66e,0x162aa,0x1e26c,
0x275a1,0x31c31,0x3d604,0x4a2fc,0x582fb,0x675dc,0x77b7c,0x893b1,
0x9be51,0xafb2d,0xc4a14,0xdaad3,0xf1d34,0x10a0fd,0x1235f3,0x13dbd7,
0x159267,0x175961,0x19307f,0x1b1777,0x1d0dfe,0x1f13c8,0x212883,0x234bdf,
0x257d86,0x27bd23,0x2a0a5b,0x2c64d5,0x2ecc33,0x314017,0x33c020,0x364beb,
0x38e313,0x3b8532,0x3e31e1,0x40e8b6,0x43a945,0x467323,0x4945e0,0x4c210d,
0x4f043a,0x51eef5,0x54e0cb,0x57d946,0x5ad7f3,0x5ddc5b,0x60e606,0x63f47d,
0x670747,0x6a1deb,0x6d37ef,0x7054d8,0x73742c,0x76956f,0x79b826,0x7cdbd5
};

uint16_t SIN_DATA_A[256];
uint16_t SIN_DATA_B[256];

uint16_t SIN_DATA_A_T[256],SIN_DATA_B_T[256];

uint16_t *sinon_a = SIN_DATA_A;
uint16_t *sinon_b = SIN_DATA_B;

uint16_t *sinoff_a = SIN_DATA_A_T;
uint16_t *sinoff_b = SIN_DATA_B_T;

void init_PIT_Sinwave(void)
{
	PIT.PITMCR.R = 0X00000001;      //Timers are stopped in debug mode              
	PIT.TIMER[0].LDVAL.R = 1000000*80/256/varFrequency;   //reset value 80, interrupt cycle is 1us                          
}


void inline initEMIOS(void) {
	EMIOS.MCR.B.GPRE = 0;     /* Divide 80 MHz sysclk by 0+1 = 1 for 80MHz eMIOS clk*/
	EMIOS.MCR.B.ETB = 0;      /* External time base is disabled; Ch 23 drives ctr bus A */
	EMIOS.MCR.B.GPREN = 1;	/* Enable eMIOS clock */
	EMIOS.MCR.B.GTBE = 1;		/* Enable global time base */
	EMIOS.MCR.B.FRZ = 1;		/* Enable stopping channels when in debug mode */
}

void inline initEMIOSch23(void) {        /* EMIOS CH 23: Modulus Up Counter */
	EMIOS.CH[23].CADR.R = 5119;	  	  /* step is 1/40mhz=0.0125us, period will be 5120*0.0125= 64us */
	EMIOS.CH[23].CCR.B.MODE = 0x50; /* MPC551x, MPC563x: Mod Ctr Bufd (MCB) int clk */
	EMIOS.CH[23].CCR.B.BSL = 0x3;	  /* Use internal counter */
	EMIOS.CH[23].CCR.B.UCPRE = 0;	  /* Set channel prescaler to divide by 1 */
	EMIOS.CH[23].CCR.B.FREN = 1; 	  /* Freeze channel counting when in debug mode */
	EMIOS.CH[23].CCR.B.UCPREN = 1;  /* Enable prescaler; uses default divide by 1 */
}


void PIT0_ISR(void)
{ 
	flgPWMfreq++;
	PIT.TIMER[0].TFLG.B.TIF = 1; 
}

void setFrequencySinWAVE(uint32_t f)
{
	PIT.TIMER[0].LDVAL.R = 1000000*80/256/f;
}

void setAmplitudeSinWAVE(uint8_t amp)
{
	uint16_t ii;
	uint16_t* tempSwitcha,*tempSwitchb;
	
	
	if((amp>0)&&(amp<=100))
	{
		for(ii = 0; ii < 256; ++ii)
		{
		  //sinoff_a[ii] = 1280 + (uint16_t)((uint32_t)amp * (uint32_t)SIN_DATA_RAW[ii]*5/100);
		  sinoff_a[ii] = (uint16_t)((uint32_t)amp * (uint32_t)SIN_DATA_RAW[ii]*10/100);
		  sinoff_b[ii] = 5119 - sinoff_a[ii];
		}
		tempSwitcha = sinon_a;
		tempSwitchb = sinon_b;
		sinon_a = sinoff_a;
		sinon_b = sinoff_b;		
		sinoff_a = tempSwitcha;
		sinoff_b = tempSwitchb;
		//globalMax_a = 1280+(uint16_t)((uint16_t)amp * 255 * 5 / 100);
		globalMax_a = (uint16_t)((uint16_t)amp * 255 * 10 / 100);
		globalMax_b	= 5119 - globalMax_a;			
	}
}


void init_SinWAVE(void)
{
	uint16_t ii;
	
	/* Initialize data */
	
	for(ii=0;ii<256;ii++)
	{
		//SIN_DATA_A[ii]=1280+5*SIN_DATA_RAW[ii];
		SIN_DATA_A[ii]=10*SIN_DATA_RAW[ii];
		SIN_DATA_B[ii]=5119 - SIN_DATA_A[ii];		
	}
	
	varFrequency = DEFAULTFREQUENCY;
	
	initEMIOS();	   /* Initialize eMIOS to provide 1 MHz clock to channels */
	initEMIOSch23();   /* Initialize eMIOS channel 23 as modulus counter*/
	
	/* config channel 8,9,10 as PWM output */

	EMIOS.CH[8].CCR.B.BSL = 0x0;	  /* Use counter bus A (default) */
	EMIOS.CH[8].CCR.B.EDPOL = 0;	  /* Polarity-leading edge sets output/trailing clears*/
	EMIOS.CH[8].CCR.B.MODE = 0x60;  	/* Mode is OPWM Buffered */
	
	EMIOS.CH[9].CCR.B.BSL = 0x0;	  /* Use counter bus A (default) */
	EMIOS.CH[9].CCR.B.EDPOL = 0;	  /* Polarity-leading edge sets output/trailing clears*/
	EMIOS.CH[9].CCR.B.MODE = 0x60;  /* Mode is OPWM Buffered */
	
	EMIOS.CH[10].CCR.B.BSL = 0x0;	  /* Use counter bus A (default) */
	EMIOS.CH[10].CCR.B.EDPOL = 0;	  /* Polarity-leading edge sets output/trailing clears*/
	EMIOS.CH[10].CCR.B.MODE = 0x60;  /* Mode is OPWM Buffered */
	
	/* config channel 11,12,14 as inverted PWM output */

	EMIOS.CH[11].CCR.B.BSL = 0x0;	  /* Use counter bus A (default) */
	EMIOS.CH[11].CCR.B.EDPOL = 1;	  /* Polarity-leading edge clears output/trailing sets*/
	EMIOS.CH[11].CCR.B.MODE = 0x60;  /* Mode is OPWM Buffered */

	EMIOS.CH[12].CCR.B.BSL = 0x0;	  /* Use counter bus A (default) */
	EMIOS.CH[12].CCR.B.EDPOL = 1;	  /* Polarity-leading edge clears output/trailing sets*/
	EMIOS.CH[12].CCR.B.MODE = 0x60;  /* Mode is OPWM Buffered */
	
	EMIOS.CH[14].CCR.B.BSL = 0x0;	  /* Use counter bus A (default) */
	EMIOS.CH[14].CCR.B.EDPOL = 1;	  /* Polarity-leading edge clears output/trailing sets*/
	EMIOS.CH[14].CCR.B.MODE = 0x60;  /* Mode is OPWM Buffered */
}

void startSinWAVE(void)
{
	
	indexA = OFFSETA;
	indexB = OFFSETB;
	indexC = OFFSETC;
	

	EMIOS.CH[8].CADR.R = sinon_a[indexA];
	EMIOS.CH[8].CBDR.R = sinon_b[indexA];    /* Leading edge */
	
	EMIOS.CH[9].CADR.R = sinon_a[indexB];
	EMIOS.CH[9].CBDR.R = sinon_b[indexB]; 
	
	EMIOS.CH[10].CADR.R = sinon_a[indexC];
	EMIOS.CH[10].CBDR.R = sinon_b[indexC];
	
	
#ifdef DEADTIME_ENABLE
	
	EMIOS.CH[11].CADR.R = sinon_a[indexA] + PWMDEADTIME;
	EMIOS.CH[11].CBDR.R	= sinon_b[indexA] - PWMDEADTIME;		

	EMIOS.CH[12].CADR.R = sinon_a[indexB] + PWMDEADTIME;
	EMIOS.CH[12].CBDR.R	= sinon_b[indexB] - PWMDEADTIME;		

	EMIOS.CH[14].CADR.R = sinon_a[indexC] + PWMDEADTIME;
	EMIOS.CH[14].CBDR.R	= sinon_b[indexC] - PWMDEADTIME;		

#else
	EMIOS.CH[11].CADR.R = sinon_a[indexA];
	EMIOS.CH[11].CBDR.R = sinon_b[indexA];    /* Leading edge */
	
	EMIOS.CH[12].CADR.R = sinon_a[indexB];
	EMIOS.CH[12].CBDR.R = sinon_b[indexB]; 
	
	EMIOS.CH[14].CADR.R = sinon_a[indexC];
	EMIOS.CH[14].CBDR.R = sinon_b[indexC];	
#endif
			
	SIU.PCR[187].R = 0x0600; 		//set eMIOS8 to PWM mode	
	SIU.PCR[188].R = 0x0600; 		//set eMIOS9 to PWM mode	
	SIU.PCR[189].R = 0x0600; 		//set eMIOS10 to PWM mode	
	SIU.PCR[190].R = 0x0600; 		//set eMIOS11 to PWM mode	
	SIU.PCR[191].R = 0x0600; 		//set eMIOS12 to PWM mode	
	SIU.PCR[193].R = 0x0600; 		//set eMIOS14 to PWM mode	

			
	PIT.TIMER[0].TCTRL.R = 0X00000003;  //  Turn on TIMER 0 TIE and TEN to fire
	
}

void stopSinWAVE(void)
{
	SIU.PCR[187].R = 0x0000; 		//shut off eMIOS8 	
	SIU.PCR[188].R = 0x0000; 		//shut off eMIOS9 	
	SIU.PCR[189].R = 0x0000; 		//shut off eMIOS10	
	SIU.PCR[190].R = 0x0000; 		//shut off eMIOS11	
	SIU.PCR[191].R = 0x0000; 		//shut off eMIOS12	
	SIU.PCR[193].R = 0x0000; 		//shut off eMIOS14	
	
	PIT.TIMER[0].TCTRL.R = 0X00000000;  //  Trun off the timer
	flgPWMfreq = 0;				
}


void sinWaveTask(void)
{
	if(flgPWMfreq)
	{
		indexA++;
		indexB++;
		indexC++;
		
		EMIOS.CH[11].CADR.R = sinon_a[indexA] + PWMDEADTIME;
		EMIOS.CH[11].CBDR.R	= sinon_b[indexA] - PWMDEADTIME;		


		EMIOS.CH[12].CADR.R = sinon_a[indexB] + PWMDEADTIME;
		EMIOS.CH[12].CBDR.R	= sinon_b[indexB] - PWMDEADTIME;		



		EMIOS.CH[14].CADR.R = sinon_a[indexC] + PWMDEADTIME;
		EMIOS.CH[14].CBDR.R	= sinon_b[indexC] - PWMDEADTIME;		


		EMIOS.CH[8].CADR.R = sinon_a[indexA];
		EMIOS.CH[8].CBDR.R = sinon_b[indexA];    /* Leading edge */
		
		EMIOS.CH[9].CADR.R = sinon_a[indexB];
		EMIOS.CH[9].CBDR.R = sinon_b[indexB]; 
		
		EMIOS.CH[10].CADR.R = sinon_a[indexC];
		EMIOS.CH[10].CBDR.R = sinon_b[indexC];
		
		fs_etpu_pwmmac_update_duty_cycles(PWMMAC0_MASTER,SIN_DATA_RAW24[indexA],SIN_DATA_RAW24[indexB],SIN_DATA_RAW24[indexC]);
		
		
		
		
		flgPWMfreq = 0;
	}
	
}

void plug(uint8_t dir)
{
	if(!dir)
	{
		OFFSETA = 0;
		OFFSETB = 86;
		OFFSETC = 171;  		
	}
	else
	{
		OFFSETA = 0;
		OFFSETB = 171;
		OFFSETC = 86;    	
	}
}
