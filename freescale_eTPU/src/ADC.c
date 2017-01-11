#include "includes.h"

static uint32_t ResultInMv = 0;     /* ADC conversion result in millivolts */

void initADC0(void) {
  EQADC.CFPR[0].R = 0x80801001;       /* Send CFIFO 0 a ADC0 configuration command */
                                      /* enable ADC0 & sets prescaler= divide by 2 */ 
  EQADC.CFCR[0].R = 0x0410;           /* Trigger CFIFO 0 using Single Scan SW mode */
  while (EQADC.FISR[0].B.EOQF !=1) {} /* Wait for End Of Queue flag */
  EQADC.FISR[0].B.EOQF = 1;           /* Clear End Of Queue flag */
}

void SendConvCmd (void) {
  EQADC.CFPR[0].R = 0x80001100; /* Conversion command: convert channel 17 */
                                /* with ADC0, set EOQ, and send result to RFIFO 0 */ 
  EQADC.CFCR[0].R = 0x0410;     /* Trigger CFIFO 0 using Single Scan SW mode */
}

void ReadResult(uint32_t *result) {
  while (EQADC.FISR[0].B.RFDF != 1){}      /* Wait for RFIFO 0's Drain Flag to set */
  *result = EQADC.RFPR[0].R;                /* ADC result */        
  //ResultInMv = (uint32_t)((5000*Result)/0x3FFC);  /* ADC result in millivolts */        
  EQADC.FISR[0].B.RFDF = 1;                /* Clear RFIFO 0's Drain Flag */
  EQADC.FISR[0].B.EOQF = 1;                /* Clear CFIFO's End of Queue flag */
 }