/*******************************************************************************
* FILE NAME: etpu_pwmmac.h         COPYRIGHT (c) Freescale Semiconductor 2005
*                                              All Rights Reserved
*
* DESCRIPTION:
* This file contains the prototypes and defines for the eTPU function PWM
* Master for AC Motors (PWMMAC) and PWM Full Range (PWMF) API.
================================================================================
* REV      AUTHOR      DATE        DESCRIPTION OF CHANGE
* ---   -----------  ----------    ---------------------
* 0.1   M. Brejl     23/Jun/05     Initial version ported from PWMMDC
*       M. Princ                   eTPU function API of the set3.
* 2.0   M. Brejl     16/Aug/05     Inclusion of sin3h LUT, and dead-time 
*                                  compensation.
* 2.1   M. Princ     19/Oct/05     SVM inputs [a,b] and amplitude 
*                                  in format 3.21. 
* 2.2   M. Brejl     09/Jan/13     fs_etpu_pwmmac_set_dead_time() added. 
*******************************************************************************/
#ifndef _FS_ETPU_PWMMAC_H_
#define _FS_ETPU_PWMMAC_H_

#include "etpu_util.h"
#include "etpu_pwmmac_auto.h" /* Auto generated header file for eTPU PWMMAC */
#include "etpu_pwmf_auto.h"	  /* Auto generated header file for eTPU PWMF */
#include "etpu_mc_sin_auto.h" /* Auto generated header file for eTPU SIN */

/*******************************************************************************
*  1st quadrant of sin wave with the 3rd harmonic - look up table
*       sin3h = 2/sqrt(3)*(sin(angle) + 1/6*sin(3*angle))
*******************************************************************************/
uint16_t fs_etpu_pwmmac_sin3h_lut[];

/*******************************************************************************
*                            Definitions
*******************************************************************************/

#define FS_ETPU_PWMMAC_NOT_USED             (uint8_t)(-1)

/* modulation options */
#define FS_ETPU_PWMMAC_MOD_NO                FS_ETPU_PWMMAC_MODULATION_NO         /* no modulation: update duty-cycles */
#define FS_ETPU_PWMMAC_MOD_VOLTAGE_UNSIGNED  FS_ETPU_PWMMAC_MODULATION_UNSIGNED   /* voltage unsigned */
#define FS_ETPU_PWMMAC_MOD_VOLTAGE_SIGNED    FS_ETPU_PWMMAC_MODULATION_SIGNED     /* voltage signed */
#define FS_ETPU_PWMMAC_MOD_SVM_STD           FS_ETPU_PWMMAC_MODULATION_SVMSTD     /* SVM Standard */
#define FS_ETPU_PWMMAC_MOD_SVM_U0N           FS_ETPU_PWMMAC_MODULATION_SVMU0N     /* SVM - U0N */
#define FS_ETPU_PWMMAC_MOD_SVM_U7N           FS_ETPU_PWMMAC_MODULATION_SVMU7N     /* SVM - U7N */
#define FS_ETPU_PWMMAC_MOD_PWM_ICT           FS_ETPU_PWMMAC_MODULATION_PWMICT     /* Inverse Clark Modulation */
#define FS_ETPU_PWMMAC_MOD_SINE_TABLE        FS_ETPU_PWMMAC_MODULATION_SINE_TABLE /* sine table modulation */
#define FS_ETPU_PWMMAC_MOD_USER_TABLE        FS_ETPU_PWMMAC_MODULATION_USER_TABLE /* user-defined table modulation */

/* update options */
#define FS_ETPU_PWMMAC_NORMAL                FS_ETPU_PWMMAC_INIT_NORMAL      /* normal update */
#define FS_ETPU_PWMMAC_HALF_CYCLE            FS_ETPU_PWMMAC_INIT_HALF_CYCLE  /* half-cycle update */

/* alignment options */
#define FS_ETPU_PWMMAC_EDGE_ALIGNED          FS_ETPU_PWMMAC_FM_EDGE_ALIGNED   /* edge-aligned */
#define FS_ETPU_PWMMAC_CENTER_ALIGNED        FS_ETPU_PWMMAC_FM_CENTER_ALIGNED /* center-aligned */

/* phases_type options */
#define FS_ETPU_PWMMAC_FULL_RANGE_SINGLE_CHANNELS   0  /* full range - single channels */
#define FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS       1  /* full range - complementary pairs */

/* phaseA_negate_duty phaseB_negate_duty phaseC_negate_duty options */
#define FS_ETPU_PWMMAC_DUTY_POS              0                               /* do not negate duty-cycle */
#define FS_ETPU_PWMMAC_DUTY_NEG              FS_ETPU_PWMMAC_OPTION_DUTY_NEG  /* negate duty-cycle */

/* swap options */
#define FS_ETPU_PWMMAC_NO_SWAP               0                          /* do not swap dead-time insertion */
#define FS_ETPU_PWMMAC_SWAP                  FS_ETPU_PWMMAC_OPTION_SWAP /* swap dead-time insertion */

/* base_ch_polarity and compl_ch_polarity options */
#define FS_ETPU_PWMMAC_ACTIVE_HIGH           0  /* active high */
#define FS_ETPU_PWMMAC_ACTIVE_LOW            2  /* active low */

/* base_ch_disable_pin_state and compl_ch_disable_pin_state options */
#define FS_ETPU_PWMMAC_PIN_LOW               FS_ETPU_PWMF_STOP_LOW  /* pin low */
#define FS_ETPU_PWMMAC_PIN_HIGH              FS_ETPU_PWMF_STOP_HIGH /* pin high */

/* num_phases options */
#define FS_ETPU_PWMMAC_1_PHASE               1  /* 1 phase  (A) */
#define FS_ETPU_PWMMAC_2_PHASES              2  /* 2 phases (A,B) */
#define FS_ETPU_PWMMAC_3_PHASES              3  /* 3 phases (A,B,C) */

/* dtc options */
#define FS_ETPU_PWMMAC_DTC_COMPL_COMPENSATES 0x00  /* complementary channel compensates the dead-time */
#define FS_ETPU_PWMMAC_DTC_BOTH_COMPENSATE   0xFF  /* both channels compensate the dead-time */
#define FS_ETPU_PWMMAC_DTC_BASE_COMPENSATES  0xFE  /* base channel compensates the dead-time */

/*******************************************************************************
*                       Function Prototypes
*******************************************************************************/

/* Initialization of PWMMAC channel and 3 phases. */
int32_t fs_etpu_pwmmac_init_3ph( uint8_t   channel,
                                 uint8_t   priority,
                                 uint8_t   phaseA_channel,
                                 uint8_t   phaseA_negate_duty,
                                 uint8_t   phaseB_channel,
                                 uint8_t   phaseB_negate_duty,
                                 uint8_t   phaseC_channel,
                                 uint8_t   phaseC_negate_duty,
                                 uint8_t   modulation,
                                 uint16_t* p_table,
                                 uint8_t   update,
                                 uint8_t   alignment,
                                 uint8_t   phases_type,
                                 uint8_t   swap,
                                 uint8_t   base_ch_disable_pin_state,
                                 uint8_t   compl_ch_disable_pin_state,
                                 uint24_t  start_offset,
                                 uint24_t  period,
                                 uint24_t  update_time,
                                 uint24_t  dead_time,
                                 uint24_t  min_pw);

/* Initialization of PWMMAC channel and 2 phases. */
int32_t fs_etpu_pwmmac_init_2ph( uint8_t   channel,
                                 uint8_t   priority,
                                 uint8_t   phaseA_channel,
                                 uint8_t   phaseA_negate_duty,
                                 uint8_t   phaseB_channel,
                                 uint8_t   phaseB_negate_duty,
                                 uint8_t   modulation,
                                 uint16_t* p_table,
                                 uint8_t   update,
                                 uint8_t   alignment,
                                 uint8_t   phases_type,
                                 uint8_t   swap,
                                 uint8_t   base_ch_disable_pin_state,
                                 uint8_t   compl_ch_disable_pin_state,
                                 uint24_t  start_offset,
                                 uint24_t  period,
                                 uint24_t  update_time,
                                 uint24_t  dead_time,
                                 uint24_t  min_pw);

/* Initialization of PWMMAC channel and 1 phase. */
int32_t fs_etpu_pwmmac_init_1ph( uint8_t   channel,
                                 uint8_t   priority,
                                 uint8_t   phaseA_channel,
                                 uint8_t   phaseA_negate_duty,
                                 uint8_t   modulation,
                                 uint16_t* p_table,
                                 uint8_t   update,
                                 uint8_t   alignment,
                                 uint8_t   phases_type,
                                 uint8_t   swap,
                                 uint8_t   base_ch_disable_pin_state,
                                 uint8_t   compl_ch_disable_pin_state,
                                 uint24_t  start_offset,
                                 uint24_t  period,
                                 uint24_t  update_time,
                                 uint24_t  dead_time,
                                 uint24_t  min_pw);

/* Enable PWM generation - 3 phases. */
int32_t fs_etpu_pwmmac_enable_3ph( uint8_t   channel,
                                   uint8_t   base_ch_polarity,
                                   uint8_t   compl_ch_polarity);

/* Enable PWM generation - 2 phases. */
int32_t fs_etpu_pwmmac_enable_2ph( uint8_t   channel,
                                   uint8_t   base_ch_polarity,
                                   uint8_t   compl_ch_polarity);

/* Enable PWM generation - 1 phase. */
int32_t fs_etpu_pwmmac_enable_1ph( uint8_t   channel,
                                   uint8_t   base_ch_polarity,
                                   uint8_t   compl_ch_polarity);

/* Disable PWM generation - 3 phases. */
int32_t fs_etpu_pwmmac_disable_3ph( uint8_t channel,
                                    uint8_t base_ch_disable_pin_state,
                                    uint8_t compl_ch_disable_pin_state);

/* Disable PWM generation - 2 phases. */
int32_t fs_etpu_pwmmac_disable_2ph( uint8_t channel,
                                    uint8_t base_ch_disable_pin_state,
                                    uint8_t compl_ch_disable_pin_state);

/* Disable PWM generation - 1 phase. */
int32_t fs_etpu_pwmmac_disable_1ph( uint8_t channel,
                                    uint8_t base_ch_disable_pin_state,
                                    uint8_t compl_ch_disable_pin_state);
/* Set PWM period */
int32_t fs_etpu_pwmmac_set_period( uint8_t  channel,
                                   uint24_t period);

/* Update of input voltage in case of unsigned modulation */
int32_t fs_etpu_pwmmac_update_voltage_unsigned( uint8_t  channel,
                                                uint24_t voltage);

/* Update of input voltage in case of signed modulation */
int32_t fs_etpu_pwmmac_update_voltage_signed( uint8_t channel,
                                              int24_t voltage);

/* Update of phase duty-cycles in case of no modulation */
int32_t fs_etpu_pwmmac_update_duty_cycles( uint8_t channel,
                                           int24_t dutyA,
                                           int24_t dutyB,
                                           int24_t dutyC);

/* Update of input parameters in case of any SVM modulation */
int32_t fs_etpu_pwmmac_update_svm( uint8_t channel,
                                   int24_t alpha,
                                   int24_t beta);

/* Update of input parameters in case of any table modulation */
int32_t fs_etpu_pwmmac_update_vector( uint8_t channel,
                                      int24_t amplitude,
                                      int24_t angle);

/* Set dead-time insertion type */
int32_t fs_etpu_pwmmac_set_dtc( uint8_t phase_channel,
                                uint8_t dtc);

/* Get current sector (applies for SVMs only) */
uint8_t fs_etpu_pwmmac_get_sector( uint8_t channel);

/* Set dead-time */
void fs_etpu_pwmmac_set_dead_time( uint8_t  channel,
                                   uint24_t dead_time);


/*******************************************************************************
*        Information for eTPU Graphical Configuration Tool
*******************************************************************************/
/* full function name: PWMMAC - PWM Master for AC Motors */ 
/* channel assignment when (fs_etpu_pwmmac_init_3ph: phases_type=FS_ETPU_PWMMAC_FULL_RANGE_SINGLE_CHANNELS); load: pwmmac3ph
   master = channel
   phaseA = phaseA_channel
   phaseB = phaseB_channel
   phaseC = phaseC_channel
*/
/* channel assignment when (fs_etpu_pwmmac_init_2ph: phases_type=FS_ETPU_PWMMAC_FULL_RANGE_SINGLE_CHANNELS); load: pwmmac2ph
   master = channel
   phaseA = phaseA_channel
   phaseB = phaseB_channel
*/
/* channel assignment when (fs_etpu_pwmmac_init_1ph: phases_type=FS_ETPU_PWMMAC_FULL_RANGE_SINGLE_CHANNELS); load: pwmmac1ph
   master = channel
   phaseA = phaseA_channel
*/
/* channel assignment when (fs_etpu_pwmmac_init_3ph: phases_type=FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS); load: pwmmac3ph
   master = channel
   phaseA_base = phaseA_channel
   phaseA_compl = phaseA_channel + 1
   phaseB_base = phaseB_channel
   phaseB_compl = phaseB_channel + 1
   phaseC_base = phaseC_channel
   phaseC_compl = phaseC_channel + 1
*/
/* channel assignment when (fs_etpu_pwmmac_init_2ph: phases_type=FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS); load: pwmmac2ph
   master = channel
   phaseA_base = phaseA_channel
   phaseA_compl = phaseA_channel + 1
   phaseB_base = phaseB_channel
   phaseB_compl = phaseB_channel + 1
*/
/* channel assignment when (fs_etpu_pwmmac_init_1ph: phases_type=FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS); load: pwmmac1ph
   master = channel
   phaseA_base = phaseA_channel
   phaseA_compl = phaseA_channel + 1
*/
/* load expression pwmmac3ph
base = 100*etpu_tcr1_freq/(period*etpu_clock_freq)
update_thread = \
  if(modulation==FS_ETPU_PWMMAC_MOD_NO              ,28, \
  if(modulation==FS_ETPU_PWMMAC_MOD_VOLTAGE_UNSIGNED,48, \
  if(modulation==FS_ETPU_PWMMAC_MOD_VOLTAGE_SIGNED  ,48, \
  if(modulation==FS_ETPU_PWMMAC_MOD_SVM_STD         ,194, \
  if(modulation==FS_ETPU_PWMMAC_MOD_SVM_U0N         ,194, \
  if(modulation==FS_ETPU_PWMMAC_MOD_SVM_U7N         ,194, \
  if(modulation==FS_ETPU_PWMMAC_MOD_PWM_ICT         ,178, \
  if(modulation==FS_ETPU_PWMMAC_MOD_SINE_TABLE      ,188, \
  if(modulation==FS_ETPU_PWMMAC_MOD_USER_TABLE      ,242,0)))))))))
phase = if(update==FS_ETPU_PWMMAC_NORMAL, 2*28+68, 2*28+2*68)*base

master = if(update==FS_ETPU_PWMMAC_NORMAL, update_thread+434, \
                                         2*update_thread+398+450)*base
phaseA = phase
phaseB = phase
phaseC = phase
phaseA_base = phase
phaseB_base = phase
phaseC_base = phase
phaseA_compl = phase
phaseB_compl = phase
phaseC_compl = phase
*/
/* load expression pwmmac2ph
base = 100*etpu_tcr1_freq/(period*etpu_clock_freq)
update_thread = if(modulation==FS_ETPU_PWMMAC_MOD_NO, 28, 48)
phase = if(update==FS_ETPU_PWMMAC_NORMAL, 2*28+68, 2*28+2*68)*base

master = if(update==FS_ETPU_PWMMAC_NORMAL, update_thread+312, \
                                         2*update_thread+288+324)*base
phaseA = phase
phaseB = phase
phaseA_base = phase
phaseB_base = phase
phaseA_compl = phase
phaseB_compl = phase
*/
/* load expression pwmmac1ph
base = 100*etpu_tcr1_freq/(period*etpu_clock_freq)
update_thread = if(modulation==FS_ETPU_PWMMAC_MOD_NO, 28, 48)
phase = if(update==FS_ETPU_PWMMAC_NORMAL, 2*28+68, 2*28+2*68)*base

master = if(update==FS_ETPU_PWMMAC_NORMAL, update_thread+190, \
                                         2*update_thread+178+198)*base
phaseA = phase
phaseA_base = phase
phaseA_compl = phase
*/

#endif

/*********************************************************************
 *
 * Copyright:
 *	Freescale Semiconductor, INC. All Rights Reserved.
 *  You are hereby granted a copyright license to use, modify, and
 *  distribute the SOFTWARE so long as this entire notice is
 *  retained without alteration in any modified and/or redistributed
 *  versions, and that such modified versions are clearly identified
 *  as such. No licenses are granted by implication, estoppel or
 *  otherwise under any patents or trademarks of Freescale
 *  Semiconductor, Inc. This software is provided on an "AS IS"
 *  basis and without warranty.
 *
 *  To the maximum extent permitted by applicable law, Freescale
 *  Semiconductor DISCLAIMS ALL WARRANTIES WHETHER EXPRESS OR IMPLIED,
 *  INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A
 *  PARTICULAR PURPOSE AND ANY WARRANTY AGAINST INFRINGEMENT WITH
 *  REGARD TO THE SOFTWARE (INCLUDING ANY MODIFIED VERSIONS THEREOF)
 *  AND ANY ACCOMPANYING WRITTEN MATERIALS.
 *
 *  To the maximum extent permitted by applicable law, IN NO EVENT
 *  SHALL Freescale Semiconductor BE LIABLE FOR ANY DAMAGES WHATSOEVER
 *  (INCLUDING WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
 *  BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR OTHER
 *  PECUNIARY LOSS) ARISING OF THE USE OR INABILITY TO USE THE SOFTWARE.
 *
 *  Freescale Semiconductor assumes no responsibility for the
 *  maintenance and support of this software
 ********************************************************************/
