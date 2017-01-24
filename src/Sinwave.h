#ifndef _SINWAVE_H_
#define _SINWAVE_H_


//extern void PIT2_ISR(void);
extern uint8_t OFFSETA,OFFSETB,OFFSETC;
extern const uint16_t SIN_DATA_RAW[];
extern void PIT0_ISR(void);

#define BOTHACTUATOR     0b00000011
#define INNERACTUATOR    0b00000001
#define OUTERACTUATOR    0b00000010





#endif