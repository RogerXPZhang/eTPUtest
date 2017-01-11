#include "includes.h"

uint8_t cntSCIRx;
uint8_t buffSCI[20];

void initESCI_A(void) 
{
  ESCI_A.CR2.R = 0x2000;      /* Module is enabled (default setting ) */
  ESCI_A.CR1.R = 0x0023002C;  /* SBR = Clk/16/target frequency, 115200 baud, 8 bits, no parity, Rx interrupt is enabled, Tx & Rx enabled */
  SIU.PCR[89].R = 0x400;      /* Configure pad for primary func: TxDA */
  SIU.PCR[90].R = 0x400;      /* Configure pad for primary func: RxDA */
}


void SCI_transmitData(uint8_t *data,uint8_t length) 
{
	uint8_t	j;                                 /* Dummy variable */
	for (j=0; j<length;j++) 
	{  
		while (ESCI_A.SR.B.TDRE == 0) {}       /* Wait for transmit data reg empty = 1 */
		ESCI_A.SR.R = 0x80000000;              /* Clear TDRE flag */
		ESCI_A.DR.B.D = data[j];         	 /* Transmit 8 bits Data */
	}
}

void SCI_readDataTask(uint8_t *data, uint8_t *length)
{
	*length = cntSCIRx;
	while(cntSCIRx)
	{
		data[cntSCIRx-1] = buffSCI[cntSCIRx-1];
		cntSCIRx--;
	}
}

void SCI_A_ISR(void)
{
	ESCI_A.SR.B.RDRF = 0;
	buffSCI[cntSCIRx] = ESCI_A.DR.B.D;
	cntSCIRx++;
}