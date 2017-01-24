/*******************************************************************************
* FILE NAME: etpu_pwmmac.c         COPYRIGHT (c) Freescale Semiconductor 2005
*                                              All Rights Reserved
*
* DESCRIPTION:
* This file contains the eTPU function PWM Master for AC Motors (PWMMAC) and
* PWM Full Range (PWMF) API.
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
#include "etpu_util.h"          /* Utility routines for working with the eTPU */
#include "etpu_pwmmac.h"        /* eTPU PWMMAC API */

extern uint32_t fs_etpu_data_ram_start;

/*******************************************************************************
*  1st quadrant of sin wave with the 3rd harmonic - look up table
*       sin3h = 2/sqrt(3)*(sin(angle) + 1/6*sin(3*angle))
*******************************************************************************/
uint16_t fs_etpu_pwmmac_sin3h_lut[] = {
    0x0000, 0x0570, 0x0AE1, 0x104F, 0x15BB, 0x1B25, 0x208A, 0x25EB, 
    0x2B46, 0x309C, 0x35EA, 0x3B32, 0x4070, 0x45A6, 0x4AD2, 0x4FF4, 
    0x550B, 0x5A16, 0x5F14, 0x6406, 0x68EA, 0x6DBF, 0x7286, 0x773E, 
    0x7BE5, 0x807C, 0x8502, 0x8976, 0x8DD9, 0x9228, 0x9665, 0x9A8F, 
    0x9EA4, 0xA2A6, 0xA693, 0xAA6B, 0xAE2E, 0xB1DC, 0xB574, 0xB8F6, 
    0xBC61, 0xBFB7, 0xC2F5, 0xC61D, 0xC92F, 0xCC29, 0xCF0C, 0xD1D9, 
    0xD48D, 0xD72C, 0xD9B2, 0xDC22, 0xDE7B, 0xE0BD, 0xE2E9, 0xE4FD, 
    0xE6FB, 0xE8E3, 0xEAB5, 0xEC71, 0xEE17, 0xEFA8, 0xF124, 0xF28B, 
    0xF3DD, 0xF51B, 0xF646, 0xF75D, 0xF861, 0xF952, 0xFA32, 0xFAFF, 
    0xFBBB, 0xFC66, 0xFD01, 0xFD8C, 0xFE08, 0xFE74, 0xFED3, 0xFF23, 
    0xFF66, 0xFF9C, 0xFFC6, 0xFFE4, 0xFFF7, 0xFFFF, 0xFFFE, 0xFFF3, 
    0xFFDF, 0xFFC2, 0xFF9E, 0xFF72, 0xFF40, 0xFF08, 0xFECA, 0xFE87, 
    0xFE3F, 0xFDF4, 0xFDA5, 0xFD53, 0xFCFE, 0xFCA8, 0xFC50, 0xFBF8, 
    0xFB9E, 0xFB45, 0xFAEC, 0xFA94, 0xFA3D, 0xF9E7, 0xF994, 0xF943, 
    0xF8F5, 0xF8AA, 0xF862, 0xF81D, 0xF7DD, 0xF7A1, 0xF769, 0xF736, 
    0xF708, 0xF6DF, 0xF6BB, 0xF69C, 0xF683, 0xF66F, 0xF661, 0xF658, 
    0xF655
};

/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_init_3ph
*PURPOSE      : To initialize eTPU channels to implement PWMMAC with 3 phases.
*INPUTS NOTES : This function has 20 parameters:
*                   channel  - This is the PWMMAC master channel number.
*                              0-31 for ETPU_A and 64-95 for ETPU_B.
*                   priority - This is the priority to assign to the PWMMAC
*                              channel.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
*                              FS_ETPU_PRIORITY_LOW.
*             phaseA_channel - This parameter determines the channel number
*                              of phase A.
*                              0-31 for ETPU_A and 64-95 for ETPU_B
*         phaseA_negate_duty - This parameter sets the "Negate duty-cycle"
*                              option.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_DUTY_POS or
*                              FS_ETPU_PWMMAC_DUTY_NEG
*             phaseB_channel - This parameter determines the channel number
*                              of phase B. Applies only if num_phases >= 2.
*                              0-31 for ETPU_A and 64-95 for ETPU_B
*         phaseB_negate_duty - This parameter sets the "Negate duty-cycle"
*                              option.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_DUTY_POS or
*                              FS_ETPU_PWMMAC_DUTY_NEG
*             phaseC_channel - This parameter determines the channel number
*                              of phase C. Applies only if num_phases >= 3.
*                              0-31 for ETPU_A and 64-95 for ETPU_B
*         phaseC_negate_duty - This parameter sets the "Negate duty-cycle"
*                              option.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_DUTY_POS or
*                              FS_ETPU_PWMMAC_DUTY_NEG
*               modulation   - This parameter determines the type of modulation.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_MOD_NO or
*                              FS_ETPU_PWMMAC_MOD_VOLTAGE_UNSIGNED or
*                              FS_ETPU_PWMMAC_MOD_VOLTAGE_SIGNED or
*                              FS_ETPU_PWMMAC_MOD_SVM_STD or
*                              FS_ETPU_PWMMAC_MOD_SVM_U0N or
*                              FS_ETPU_PWMMAC_MOD_SVM_U7N or
*                              FS_ETPU_PWMMAC_MOD_SINE_WAVE or
*                              FS_ETPU_PWMMAC_MOD_SINE_TABLE or
*                              FS_ETPU_PWMMAC_MOD_USER_TABLE
*               p_table      - This is the pointer to the first quadrant look-up
*                              table. This parameter applies only if 
*                              modulation = FS_ETPU_PWMMAC_MOD_USER_TABLE. 
*                              The table is an array of unsigned 16-bit fract 
*                              values, and of length of 129. As an example, see
*                              the above stated definition of 
*                              fs_etpu_pwmmac_sin3h_lut. It defines the shape
*                              of a sine wave with injection of 3rd harmonics. 
*               update       - This parameter determines the type of update.
*                              This parameter should be assigned value of
*                              FS_ETPU_PWMMAC_NORMAL or
*                              FS_ETPU_PWMMAC_HALF_CYCLE.
*               alignment    - This parameter determines the type of PWM.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_EDGE_ALIGNED or
*                              FS_ETPU_PWMMAC_CENTER_ALIGNED
*               phases_type  - This parameter determines the type of all
*                              PWMF or PWMC phases.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_FULL_RANGE_SINGLE_CHANNELS or
*                              FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS
*                      swap  - This parameter sets the
*                              "Swap dead-time insertion" option.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_NO_SWAP or
*                              FS_ETPU_PWMMAC_SWAP
*  base_ch_disable_pin_state - This is the required output state of the base
*                              channel pin, after initialization.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
* compl_ch_disable_pin_state - This is the required output state of the
*                              complementary channel pin, after initialization.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
*                              This parameter applies only if phase_type is
*                              FS_ETPU_PWMMAC_COMPL_PAIRS.
*               start_offset - This parameter is used to synchronize
*                              various eTPU functions that generate a signal.
*                              The first PWM period starts start_offset
*                              TCR1 clocks after initialization.
*               period       - This parameter determines the PWM period
*                              as number of TCR1 cycles.
*               update_time  - This parameter determines the time that is
*                              necessary to perform the update of all PWM
*                              phases as number of TCR1 cycles.
*               dead_time    - This parameter determines the dead-time
*                              as number of TCR1 cycles.
*               min_pw       - This parameter determines the minimum pulse
*                              width as number of TCR1 cycles.
*
*RETURNS NOTES: Error code is channel could not be initialized. Error code that
*               can be returned are: FS_ETPU_ERROR_VALUE, FS_ETPU_ERROR_MALLOC.
*******************************************************************************/
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
                                 uint24_t  min_pw)
{
   uint32_t* pba;
   uint32_t* phaseA_pba;
   uint32_t* phaseB_pba;
   uint32_t* phaseC_pba;
   uint32_t* table_pba;
   uint32_t  phase_config;
   uint8_t   fm_phases_type;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95)||
      (priority>FS_ETPU_PRIORITY_HIGH)||
      ((phaseA_channel>31)&&(phaseA_channel<64))||(phaseA_channel>95)||
      (phaseA_negate_duty>FS_ETPU_PWMMAC_DUTY_NEG)||
      ((phaseB_channel>31)&&(phaseB_channel<64))||(phaseB_channel>95)||
      (phaseB_negate_duty>FS_ETPU_PWMMAC_DUTY_NEG)||
      ((phaseC_channel>31)&&(phaseC_channel<64))||(phaseC_channel>95)||
      (phaseC_negate_duty>FS_ETPU_PWMMAC_DUTY_NEG)||
      (modulation>FS_ETPU_PWMMAC_MOD_USER_TABLE)||
      ((update!=FS_ETPU_PWMMAC_NORMAL)&&
      (update!=FS_ETPU_PWMMAC_HALF_CYCLE))||
      (alignment>FS_ETPU_PWMMAC_CENTER_ALIGNED)||
      (phases_type>FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)||
      (swap>FS_ETPU_PWMMAC_SWAP)||(swap==1)||
      ((base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH))||
      ((compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH)))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /****************************************
    * PRAM allocation.
    ***************************************/
   if ((pba=fs_etpu_malloc(FS_ETPU_PWMMAC_NUM_PARMS)) == 0)
   {
      return(FS_ETPU_ERROR_MALLOC);
   }

   if (modulation == FS_ETPU_PWMMAC_MOD_USER_TABLE)
   {
     if ((table_pba=fs_etpu_malloc(0x0108)) == 0)
     {
        return(FS_ETPU_ERROR_MALLOC);
     }
     fs_memcpy32 ( table_pba, (uint32_t *)p_table, 0x0104);
   }

   /****************************************
    * Write chan config registers and FM bits.
    ***************************************/
   /* Master */
   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_SINGLE_CHANNELS)
   {
      fm_phases_type = FS_ETPU_PWMMAC_FM_SINGLE_CHANNELS;
   }
   else
   {
      fm_phases_type = FS_ETPU_PWMMAC_FM_COMPLEMEN_PAIRS;
   }
   eTPU->CHAN[channel].CR.R = (FS_ETPU_PWMMAC_TABLE_SELECT << 24) +
                              (FS_ETPU_PWMMAC_FUNCTION_NUMBER << 16) +
                              (((uint32_t)pba - fs_etpu_data_ram_start) >> 3);
   eTPU->CHAN[channel].SCR.R = (uint32_t)(alignment + fm_phases_type);

   /* Phases */
   phase_config = (FS_ETPU_PWMF_TABLE_SELECT << 24) +
                  (FS_ETPU_PWMF_FUNCTION_NUMBER << 16);
   /* Phase A */
   phaseA_pba = (uint32_t*)((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEA_OFFSET);
   eTPU->CHAN[phaseA_channel].CR.R = phase_config +
                         (((uint32_t)phaseA_pba - fs_etpu_data_ram_start) >> 3);
   /* Phase B */
   phaseB_pba = (uint32_t*)((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEB_OFFSET);
   eTPU->CHAN[phaseB_channel].CR.R = phase_config +
                         (((uint32_t)phaseB_pba - fs_etpu_data_ram_start) >> 3);
   /* Phase C */
   phaseC_pba = (uint32_t*)((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEC_OFFSET);
   eTPU->CHAN[phaseC_channel].CR.R = phase_config +
                         (((uint32_t)phaseC_pba - fs_etpu_data_ram_start) >> 3);

   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)
   {
      /* PhaseA */
      eTPU->CHAN[phaseA_channel+1].CR.R = phase_config +
                         (((uint32_t)phaseA_pba - fs_etpu_data_ram_start) >> 3);
      /* PhaseB */
      eTPU->CHAN[phaseB_channel+1].CR.R = phase_config +
                         (((uint32_t)phaseB_pba - fs_etpu_data_ram_start) >> 3);
      /* PhaseC */
      eTPU->CHAN[phaseC_channel+1].CR.R = phase_config +
                         (((uint32_t)phaseC_pba - fs_etpu_data_ram_start) >> 3);
   }
   /****************************************
    * Write parameters.
    ***************************************/
   switch (modulation)
   {
      case FS_ETPU_PWMMAC_MOD_NO:
         *(phaseA_pba + ((FS_ETPU_PWMF_DUTY_OFFSET - 1)>>2)) = 0x000000;
         *(phaseB_pba + ((FS_ETPU_PWMF_DUTY_OFFSET - 1)>>2)) = 0x000000;
         *(phaseC_pba + ((FS_ETPU_PWMF_DUTY_OFFSET - 1)>>2)) = 0x000000;
         break;
      case FS_ETPU_PWMMAC_MOD_VOLTAGE_UNSIGNED:
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET - 1)>>2)) = 0x800000;
         break;
      case FS_ETPU_PWMMAC_MOD_SINE_TABLE:
         *(pba + ((FS_ETPU_PWMMAC_PTABLE_OFFSET - 1)>>2)) = FS_ETPU_MC_SIN_MCSINLUT_OFFSET & 0x1FFF;
      case FS_ETPU_PWMMAC_MOD_VOLTAGE_SIGNED:
      case FS_ETPU_PWMMAC_MOD_SVM_STD:
      case FS_ETPU_PWMMAC_MOD_SVM_U0N:
      case FS_ETPU_PWMMAC_MOD_SVM_U7N:
      case FS_ETPU_PWMMAC_MODULATION_PWMICT:
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET)>>2)) = 0x000000;
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET + 0x4)>>2)) = 0x000000;
         break;
      case FS_ETPU_PWMMAC_MOD_USER_TABLE:
         *(pba + ((FS_ETPU_PWMMAC_PTABLE_OFFSET - 1)>>2)) = ((uint32_t)table_pba) & 0x1FFF;
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET)>>2)) = 0x000000;
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET + 0x4)>>2)) = 0x000000;
         
   }

   *(pba + ((FS_ETPU_PWMMAC_STARTOFFSET_OFFSET - 1)>>2)) = start_offset;
   *(pba + ((FS_ETPU_PWMMAC_UPDATETIME_OFFSET  - 1)>>2)) = update_time;
   *(pba + ((FS_ETPU_PWMMAC_DEADTIME_OFFSET    - 1)>>2)) = dead_time;
   *(pba + ((FS_ETPU_PWMMAC_MINPW_OFFSET       - 1)>>2)) = min_pw;
   *(pba + ((FS_ETPU_PWMMAC_PERIOD_OFFSET      - 1)>>2)) = period;
   *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASES_OFFSET) = FS_ETPU_PWMMAC_3_PHASES;
   *((uint8_t*)pba + FS_ETPU_PWMMAC_MODULATION_OFFSET) = modulation;
   *((uint8_t*)phaseA_pba + FS_ETPU_PWMF_CHANNUM_OFFSET) = phaseA_channel;
   *((uint8_t*)phaseA_pba + FS_ETPU_PWMF_DTC_OFFSET) = FS_ETPU_PWMMAC_DTC_BOTH_COMPENSATE;
   *((uint8_t*)phaseA_pba + FS_ETPU_PWMF_OPTIONS_OFFSET) =
                                           (uint8_t)(phaseA_negate_duty + swap);
   *((uint8_t*)phaseB_pba + FS_ETPU_PWMF_CHANNUM_OFFSET) = phaseB_channel;
   *((uint8_t*)phaseB_pba + FS_ETPU_PWMF_DTC_OFFSET) = FS_ETPU_PWMMAC_DTC_BOTH_COMPENSATE;
   *((uint8_t*)phaseB_pba + FS_ETPU_PWMF_OPTIONS_OFFSET) =
                                           (uint8_t)(phaseB_negate_duty + swap);
   *((uint8_t*)phaseC_pba + FS_ETPU_PWMF_CHANNUM_OFFSET) = phaseC_channel;
   *((uint8_t*)phaseC_pba + FS_ETPU_PWMF_DTC_OFFSET) = FS_ETPU_PWMMAC_DTC_BOTH_COMPENSATE;
   *((uint8_t*)phaseC_pba + FS_ETPU_PWMF_OPTIONS_OFFSET) =
                                           (uint8_t)(phaseC_negate_duty + swap);

   /****************************************
    * Write HSR.
    ***************************************/
   eTPU->CHAN[phaseA_channel].HSRR.R = base_ch_disable_pin_state;
   eTPU->CHAN[phaseB_channel].HSRR.R = base_ch_disable_pin_state;
   eTPU->CHAN[phaseC_channel].HSRR.R = base_ch_disable_pin_state;
   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)
   {
      eTPU->CHAN[phaseA_channel+1].HSRR.R = compl_ch_disable_pin_state;
      eTPU->CHAN[phaseB_channel+1].HSRR.R = compl_ch_disable_pin_state;
      eTPU->CHAN[phaseC_channel+1].HSRR.R = compl_ch_disable_pin_state;
   }
   eTPU->CHAN[channel].HSRR.R = update;

   /****************************************
    * Set channel priority and enable.
    ***************************************/
   fs_etpu_enable(phaseA_channel, priority);
   fs_etpu_enable(phaseB_channel, priority);
   fs_etpu_enable(phaseC_channel, priority);
   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)
   {
      fs_etpu_enable((uint8_t)(phaseA_channel+1), priority);
      fs_etpu_enable((uint8_t)(phaseB_channel+1), priority);
      fs_etpu_enable((uint8_t)(phaseC_channel+1), priority);
   }
   fs_etpu_enable(channel, priority);

   return(0);
}

/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_init_2ph
*PURPOSE      : To initialize eTPU channels to implement PWMMAC with 2 phases.
*INPUTS NOTES : This function has 18 parameters:
*                   channel  - This is the PWMMAC master channel number.
*                              0-31 for ETPU_A and 64-95 for ETPU_B.
*                   priority - This is the priority to assign to the PWMMAC
*                              channel.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
*                              FS_ETPU_PRIORITY_LOW.
*             phaseA_channel - This parameter determines the channel number
*                              of phase A.
*                              0-31 for ETPU_A and 64-95 for ETPU_B
*         phaseA_negate_duty - This parameter sets the "Negate duty-cycle"
*                              option.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_DUTY_POS or
*                              FS_ETPU_PWMMAC_DUTY_NEG
*             phaseB_channel - This parameter determines the channel number
*                              of phase B. Applies only if num_phases >= 2.
*                              0-31 for ETPU_A and 64-95 for ETPU_B
*         phaseB_negate_duty - This parameter sets the "Negate duty-cycle"
*                              option.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_DUTY_POS or
*                              FS_ETPU_PWMMAC_DUTY_NEG
*               modulation   - This parameter determines the type of modulation.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_MOD_NO or
*                              FS_ETPU_PWMMAC_MOD_VOLTAGE_UNSIGNED or
*                              FS_ETPU_PWMMAC_MOD_VOLTAGE_SIGNED or
*                              FS_ETPU_PWMMAC_MOD_SVM_STD or
*                              FS_ETPU_PWMMAC_MOD_SVM_U0N or
*                              FS_ETPU_PWMMAC_MOD_SVM_U7N or
*                              FS_ETPU_PWMMAC_MOD_SINE_WAVE or
*                              FS_ETPU_PWMMAC_MOD_SINE_TABLE or
*                              FS_ETPU_PWMMAC_MOD_USER_TABLE
*               p_table      - This is the pointer to the first quadrant table. 
*                              This parameter applies only if 
*                              modulation = FS_ETPU_PWMMAC_MOD_USER_TABLE. 
*                              The table is an array of unsigned 16-bit fract 
*                              values, and of length of 129.
*               update       - This parameter determines the type of update.
*                              This parameter should be assigned value of
*                              FS_ETPU_PWMMAC_NORMAL or
*                              FS_ETPU_PWMMAC_HALF_CYCLE.
*               alignment    - This parameter determines the type of PWM.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_EDGE_ALIGNED or
*                              FS_ETPU_PWMMAC_CENTER_ALIGNED
*               phases_type  - This parameter determines the type of all
*                              PWMF or PWMC phases.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_FULL_RANGE_SINGLE_CHANNELS or
*                              FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS
*                      swap  - This parameter sets the
*                              "Swap dead-time insertion" option.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_NO_SWAP or
*                              FS_ETPU_PWMMAC_SWAP
*  base_ch_disable_pin_state - This is the required output state of the base
*                              channel pin, after disable of PWM generation.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
* compl_ch_disable_pin_state - This is the required output state of the
*                              complementary channel pin, after disable of PWM
*                              generation.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
*                              This parameter applies only if phase_type is
*                              FS_ETPU_PWMMAC_COMPL_PAIRS.
*               start_offset - This parameter is used to synchronize
*                              various eTPU functions that generate a signal.
*                              The first PWM period starts start_offset
*                              TCR1 clocks after initialization.
*               period       - This parameter determines the PWM period
*                              as number of TCR1 cycles.
*               update_time  - This parameter determines the time that is
*                              necessary to perform the update of all PWM
*                              phases as number of TCR1 cycles.
*               dead_time    - This parameter determines the dead-time
*                              as number of TCR1 cycles.
*               min_pw       - This parameter determines the minimum pulse
*                              width as number of TCR1 cycles.
*
*RETURNS NOTES: Error code is channel could not be initialized. Error code that
*               can be returned are: FS_ETPU_ERROR_VALUE, FS_ETPU_ERROR_MALLOC.
*******************************************************************************/
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
                                 uint24_t  min_pw)
{
   uint32_t* pba;
   uint32_t* phaseA_pba;
   uint32_t* phaseB_pba;
   uint32_t* table_pba;
   uint32_t  phase_config;
   uint8_t   fm_phases_type;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95)||
      (priority>FS_ETPU_PRIORITY_HIGH)||
      ((phaseA_channel>31)&&(phaseA_channel<64))||(phaseA_channel>95)||
      (phaseA_negate_duty>FS_ETPU_PWMMAC_DUTY_NEG)||
      ((phaseB_channel>31)&&(phaseB_channel<64))||(phaseB_channel>95)||
      (phaseB_negate_duty>FS_ETPU_PWMMAC_DUTY_NEG)||
      (modulation>FS_ETPU_PWMMAC_MOD_USER_TABLE)||
      ((update!=FS_ETPU_PWMMAC_NORMAL)&&
      (update!=FS_ETPU_PWMMAC_HALF_CYCLE))||
      (alignment>FS_ETPU_PWMMAC_CENTER_ALIGNED)||
      (phases_type>FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)||
      (swap>FS_ETPU_PWMMAC_SWAP)||(swap==1)||
      ((base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH))||
      ((compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH)))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /****************************************
    * PRAM allocation.
    ***************************************/
   if ((pba=fs_etpu_malloc(FS_ETPU_PWMMAC_NUM_PARMS))== 0)
   {
      return(FS_ETPU_ERROR_MALLOC);
   }

   if (modulation == FS_ETPU_PWMMAC_MOD_USER_TABLE)
   {
     if ((table_pba=fs_etpu_malloc(0x0108)) == 0)
     {
        return(FS_ETPU_ERROR_MALLOC);
     }
     fs_memcpy32 ( table_pba, (uint32_t *)p_table, 0x0104);
   }

   /****************************************
    * Write chan config registers and FM bits.
    ***************************************/
   /* Master */
   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_SINGLE_CHANNELS)
   {
      fm_phases_type = FS_ETPU_PWMMAC_FM_SINGLE_CHANNELS;
   }
   else
   {
      fm_phases_type = FS_ETPU_PWMMAC_FM_COMPLEMEN_PAIRS;
   }
   eTPU->CHAN[channel].CR.R = (FS_ETPU_PWMMAC_TABLE_SELECT << 24) +
                              (FS_ETPU_PWMMAC_FUNCTION_NUMBER << 16) +
                              (((uint32_t)pba - fs_etpu_data_ram_start) >> 3);
   eTPU->CHAN[channel].SCR.R = (uint32_t)(alignment + fm_phases_type);

   /* Phases */
   phase_config = (FS_ETPU_PWMF_TABLE_SELECT << 24) +
                  (FS_ETPU_PWMF_FUNCTION_NUMBER << 16);
   /* Phase A */
   phaseA_pba = (uint32_t*)((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEA_OFFSET);
   eTPU->CHAN[phaseA_channel].CR.R = phase_config +
                         (((uint32_t)phaseA_pba - fs_etpu_data_ram_start) >> 3);
   /* Phase B */
   phaseB_pba = (uint32_t*)((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEB_OFFSET);
   eTPU->CHAN[phaseB_channel].CR.R = phase_config +
                         (((uint32_t)phaseB_pba - fs_etpu_data_ram_start) >> 3);

   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)
   {
      /* PhaseA */
      eTPU->CHAN[phaseA_channel+1].CR.R = phase_config +
                         (((uint32_t)phaseA_pba - fs_etpu_data_ram_start) >> 3);
      /* PhaseB */
      eTPU->CHAN[phaseB_channel+1].CR.R = phase_config +
                         (((uint32_t)phaseB_pba - fs_etpu_data_ram_start) >> 3);
   }
   /****************************************
    * Write parameters.
    ***************************************/
   switch (modulation)
   {
      case FS_ETPU_PWMMAC_MOD_NO:
         *(phaseA_pba + ((FS_ETPU_PWMF_DUTY_OFFSET - 1)>>2)) = 0x000000;
         *(phaseB_pba + ((FS_ETPU_PWMF_DUTY_OFFSET - 1)>>2)) = 0x000000;
         break;
      case FS_ETPU_PWMMAC_MOD_VOLTAGE_UNSIGNED:
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET - 1)>>2)) = 0x800000;
         break;
      case FS_ETPU_PWMMAC_MOD_SINE_TABLE:
         *(pba + ((FS_ETPU_PWMMAC_PTABLE_OFFSET - 1)>>2)) = FS_ETPU_MC_SIN_MCSINLUT_OFFSET & 0x1FFF;
      case FS_ETPU_PWMMAC_MOD_VOLTAGE_SIGNED:
      case FS_ETPU_PWMMAC_MOD_SVM_STD:
      case FS_ETPU_PWMMAC_MOD_SVM_U0N:
      case FS_ETPU_PWMMAC_MOD_SVM_U7N:
      case FS_ETPU_PWMMAC_MODULATION_PWMICT:
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET)>>2)) = 0x000000;
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET + 0x4)>>2)) = 0x000000;
         break;
      case FS_ETPU_PWMMAC_MOD_USER_TABLE:
         *(pba + ((FS_ETPU_PWMMAC_PTABLE_OFFSET - 1)>>2)) = ((uint32_t)table_pba) & 0x1FFF;
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET)>>2)) = 0x000000;
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET + 0x4)>>2)) = 0x000000;
   }

   *(pba + ((FS_ETPU_PWMMAC_STARTOFFSET_OFFSET - 1)>>2)) = start_offset;
   *(pba + ((FS_ETPU_PWMMAC_UPDATETIME_OFFSET  - 1)>>2)) = update_time;
   *(pba + ((FS_ETPU_PWMMAC_DEADTIME_OFFSET    - 1)>>2)) = dead_time;
   *(pba + ((FS_ETPU_PWMMAC_MINPW_OFFSET       - 1)>>2)) = min_pw;
   *(pba + ((FS_ETPU_PWMMAC_PERIOD_OFFSET      - 1)>>2)) = period;
   *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASES_OFFSET) = FS_ETPU_PWMMAC_2_PHASES;
   *((uint8_t*)pba + FS_ETPU_PWMMAC_MODULATION_OFFSET) = modulation;
   *((uint8_t*)phaseA_pba + FS_ETPU_PWMF_CHANNUM_OFFSET) = phaseA_channel;
   *((uint8_t*)phaseA_pba + FS_ETPU_PWMF_DTC_OFFSET) = FS_ETPU_PWMMAC_DTC_BOTH_COMPENSATE;
   *((uint8_t*)phaseA_pba + FS_ETPU_PWMF_OPTIONS_OFFSET) =
                                           (uint8_t)(phaseA_negate_duty + swap);
   *((uint8_t*)phaseB_pba + FS_ETPU_PWMF_CHANNUM_OFFSET) = phaseB_channel;
   *((uint8_t*)phaseB_pba + FS_ETPU_PWMF_DTC_OFFSET) = FS_ETPU_PWMMAC_DTC_BOTH_COMPENSATE;
   *((uint8_t*)phaseB_pba + FS_ETPU_PWMF_OPTIONS_OFFSET) =
                                           (uint8_t)(phaseB_negate_duty + swap);

   /****************************************
    * Write HSR.
    ***************************************/
   eTPU->CHAN[phaseA_channel].HSRR.R = base_ch_disable_pin_state;
   eTPU->CHAN[phaseB_channel].HSRR.R = base_ch_disable_pin_state;
   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)
   {
      eTPU->CHAN[phaseA_channel+1].HSRR.R = compl_ch_disable_pin_state;
      eTPU->CHAN[phaseB_channel+1].HSRR.R = compl_ch_disable_pin_state;
   }
   eTPU->CHAN[channel].HSRR.R = update;

   /****************************************
    * Set channel priority and enable.
    ***************************************/
   fs_etpu_enable(phaseA_channel, priority);
   fs_etpu_enable(phaseB_channel, priority);
   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)
   {
      fs_etpu_enable((uint8_t)(phaseA_channel+1), priority);
      fs_etpu_enable((uint8_t)(phaseB_channel+1), priority);
   }
   fs_etpu_enable(channel, priority);

   return(0);
}

/******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_init_1ph
*PURPOSE      : To initialize an eTPU channel to implement PWMMAC with 1 phase.
*INPUTS NOTES : This function has 16 parameters:
*                   channel  - This is the PWMMAC master channel number.
*                              0-31 for ETPU_A and 64-95 for ETPU_B.
*                   priority - This is the priority to assign to the PWMMAC
*                              channel.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
*                              FS_ETPU_PRIORITY_LOW.
*             phaseA_channel - This parameter determines the channel number
*                              of phase A.
*                              0-31 for ETPU_A and 64-95 for ETPU_B
*         phaseA_negate_duty - This parameter sets the "Negate duty-cycle"
*                              option.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_DUTY_POS or
*                              FS_ETPU_PWMMAC_DUTY_NEG
*               modulation   - This parameter determines the type of modulation.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_MOD_NO or
*                              FS_ETPU_PWMMAC_MOD_VOLTAGE_UNSIGNED or
*                              FS_ETPU_PWMMAC_MOD_VOLTAGE_SIGNED or
*                              FS_ETPU_PWMMAC_MOD_SVM_STD or
*                              FS_ETPU_PWMMAC_MOD_SVM_U0N or
*                              FS_ETPU_PWMMAC_MOD_SVM_U7N or
*                              FS_ETPU_PWMMAC_MOD_SINE_WAVE or
*                              FS_ETPU_PWMMAC_MOD_SINE_TABLE or
*                              FS_ETPU_PWMMAC_MOD_USER_TABLE
*               p_table      - This is the pointer to the first quadrant table. 
*                              This parameter applies only if 
*                              modulation = FS_ETPU_PWMMAC_MOD_USER_TABLE. 
*                              The table is an array of unsigned 16-bit fract 
*                              values, and of length of 129.
*               update       - This parameter determines the type of update.
*                              This parameter should be assigned value of
*                              FS_ETPU_PWMMAC_NORMAL or
*                              FS_ETPU_PWMMAC_HALF_CYCLE.
*               alignment    - This parameter determines the type of PWM.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_EDGE_ALIGNED or
*                              FS_ETPU_PWMMAC_CENTER_ALIGNED
*               phases_type  - This parameter determines the type of all
*                              PWMF or PWMC phases.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_FULL_RANGE_SINGLE_CHANNELS or
*                              FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS
*                      swap  - This parameter sets the
*                              "Swap dead-time insertion" option.
*                              This parameter should be assigned a value of:
*                              FS_ETPU_PWMMAC_NO_SWAP or
*                              FS_ETPU_PWMMAC_SWAP
*  base_ch_disable_pin_state - This is the required output state of the base
*                              channel pin, after disable of PWM generation.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
* compl_ch_disable_pin_state - This is the required output state of the
*                              complementary channel pin, after disable of PWM
*                              generation.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
*                              This parameter applies only if phase_type is
*                              FS_ETPU_PWMMAC_COMPL_PAIRS.
*               start_offset - This parameter is used to synchronize
*                              various eTPU functions that generate a signal.
*                              The first PWM period starts start_offset
*                              TCR1 clocks after initialization.
*               period       - This parameter determines the PWM period
*                              as number of TCR1 cycles.
*               update_time  - This parameter determines the time that is
*                              necessary to perform the update of all PWM
*                              phases as number of TCR1 cycles.
*               dead_time    - This parameter determines the dead-time
*                              as number of TCR1 cycles.
*               min_pw       - This parameter determines the minimum pulse
*                              width as number of TCR1 cycles.
*
*RETURNS NOTES: Error code is channel could not be initialized. Error code that
*               can be returned are: FS_ETPU_ERROR_VALUE, FS_ETPU_ERROR_MALLOC.
******************************************************************************/
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
                                 uint24_t  min_pw)
{
   uint32_t* pba;
   uint32_t* phaseA_pba;
   uint32_t* table_pba;
   uint32_t  phase_config;
   uint8_t   fm_phases_type;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95)||
      (priority>FS_ETPU_PRIORITY_HIGH)||
      ((phaseA_channel>31)&&(phaseA_channel<64))||(phaseA_channel>95)||
      (phaseA_negate_duty>FS_ETPU_PWMMAC_DUTY_NEG)||
      (modulation>FS_ETPU_PWMMAC_MOD_USER_TABLE)||
      ((update!=FS_ETPU_PWMMAC_NORMAL)&&
      (update!=FS_ETPU_PWMMAC_HALF_CYCLE))||
      (alignment>FS_ETPU_PWMMAC_CENTER_ALIGNED)||
      (phases_type>FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)||
      (swap>FS_ETPU_PWMMAC_SWAP)||(swap==1)||
      ((base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH))||
      ((compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH)))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /****************************************
    * PRAM allocation.
    ***************************************/
   if ((pba=fs_etpu_malloc(FS_ETPU_PWMMAC_NUM_PARMS))== 0)
   {
      return(FS_ETPU_ERROR_MALLOC);
   }

   if (modulation == FS_ETPU_PWMMAC_MOD_USER_TABLE)
   {
     if ((table_pba=fs_etpu_malloc(0x0108)) == 0)
     {
        return(FS_ETPU_ERROR_MALLOC);
     }
     fs_memcpy32 ( table_pba, (uint32_t *)p_table, 0x0104);
   }

   /****************************************
    * Write chan config registers and FM bits.
    ***************************************/
   /* Master */
   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_SINGLE_CHANNELS)
   {
      fm_phases_type = FS_ETPU_PWMMAC_FM_SINGLE_CHANNELS;
   }
   else
   {
      fm_phases_type = FS_ETPU_PWMMAC_FM_COMPLEMEN_PAIRS;
   }
   eTPU->CHAN[channel].CR.R = (FS_ETPU_PWMMAC_TABLE_SELECT << 24) +
                              (FS_ETPU_PWMMAC_FUNCTION_NUMBER << 16) +
                              (((uint32_t)pba - fs_etpu_data_ram_start) >> 3);
   eTPU->CHAN[channel].SCR.R = (uint32_t)(alignment + fm_phases_type);

   /* Phases */
   phase_config = (FS_ETPU_PWMF_TABLE_SELECT << 24) +
                  (FS_ETPU_PWMF_FUNCTION_NUMBER << 16);
   /* Phase A */
   phaseA_pba = (uint32_t*)((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEA_OFFSET);
   eTPU->CHAN[phaseA_channel].CR.R = phase_config +
                         (((uint32_t)phaseA_pba - fs_etpu_data_ram_start) >> 3);

   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)
   {
      /* PhaseA */
      eTPU->CHAN[phaseA_channel+1].CR.R = phase_config +
                         (((uint32_t)phaseA_pba - fs_etpu_data_ram_start) >> 3);
   }
   /****************************************
    * Write parameters.
    ***************************************/
   switch (modulation)
   {
      case FS_ETPU_PWMMAC_MOD_NO:
         *(phaseA_pba + ((FS_ETPU_PWMF_DUTY_OFFSET - 1)>>2)) = 0x000000;
         break;
      case FS_ETPU_PWMMAC_MOD_VOLTAGE_UNSIGNED:
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET - 1)>>2)) = 0x800000;
         break;
      case FS_ETPU_PWMMAC_MOD_SINE_TABLE:
         *(pba + ((FS_ETPU_PWMMAC_PTABLE_OFFSET - 1)>>2)) = FS_ETPU_MC_SIN_MCSINLUT_OFFSET & 0x1FFF;
      case FS_ETPU_PWMMAC_MOD_VOLTAGE_SIGNED:
      case FS_ETPU_PWMMAC_MOD_SVM_STD:
      case FS_ETPU_PWMMAC_MOD_SVM_U0N:
      case FS_ETPU_PWMMAC_MOD_SVM_U7N:
      case FS_ETPU_PWMMAC_MODULATION_PWMICT:
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET)>>2)) = 0x000000;
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET + 0x4)>>2)) = 0x000000;
         break;
      case FS_ETPU_PWMMAC_MOD_USER_TABLE:
         *(pba + ((FS_ETPU_PWMMAC_PTABLE_OFFSET - 1)>>2)) = ((uint32_t)table_pba) & 0x1FFF;
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET)>>2)) = 0x000000;
         *(pba + ((FS_ETPU_PWMMAC_INPUTS_OFFSET + 0x4)>>2)) = 0x000000;
   }

   *(pba + ((FS_ETPU_PWMMAC_STARTOFFSET_OFFSET - 1)>>2)) = start_offset;
   *(pba + ((FS_ETPU_PWMMAC_UPDATETIME_OFFSET  - 1)>>2)) = update_time;
   *(pba + ((FS_ETPU_PWMMAC_DEADTIME_OFFSET    - 1)>>2)) = dead_time;
   *(pba + ((FS_ETPU_PWMMAC_MINPW_OFFSET       - 1)>>2)) = min_pw;
   *(pba + ((FS_ETPU_PWMMAC_PERIOD_OFFSET      - 1)>>2)) = period;
   *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASES_OFFSET) = FS_ETPU_PWMMAC_1_PHASE;
   *((uint8_t*)pba + FS_ETPU_PWMMAC_MODULATION_OFFSET) = modulation;
   *((uint8_t*)phaseA_pba + FS_ETPU_PWMF_CHANNUM_OFFSET) = phaseA_channel;
   *((uint8_t*)phaseA_pba + FS_ETPU_PWMF_DTC_OFFSET) = FS_ETPU_PWMMAC_DTC_BOTH_COMPENSATE;
   *((uint8_t*)phaseA_pba + FS_ETPU_PWMF_OPTIONS_OFFSET) =
                                           (uint8_t)(phaseA_negate_duty + swap);

   /****************************************
    * Write HSR.
    ***************************************/
   eTPU->CHAN[phaseA_channel].HSRR.R = base_ch_disable_pin_state;
   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)
   {
      eTPU->CHAN[phaseA_channel+1].HSRR.R = compl_ch_disable_pin_state;
   }
   eTPU->CHAN[channel].HSRR.R = update;

   /****************************************
    * Set channel priority and enable.
    ***************************************/
   fs_etpu_enable(phaseA_channel, priority);
   if (phases_type == FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS)
   {
      fs_etpu_enable((uint8_t)(phaseA_channel+1), priority);
   }
   fs_etpu_enable(channel, priority);

   return(0);
}


/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_enable_3ph
*PURPOSE      : Enable PWM generation - 3 phases
*INPUTS NOTES : This function has 3 parameters:
*   channel           - This is the PWMMAC master channel number.
*                       This parameter must be assigned the same value
*                       as the channel parameter of the initialization
*                       function was assigned.
*   base_ch_polarity  - This parameter determines the polarity
*                       of the base channel.
*                       This parameter should be assigned a value of:
*                       FS_ETPU_PWMMAC_ACTIVE_HIGH or
*                       FS_ETPU_PWMMAC_ACTIVE_LOW
*   compl_ch_polarity - This parameter determines the polarity
*                       of the complementary channel.
*                       This parameter should be assigned a value of:
*                       FS_ETPU_PWMMAC_ACTIVE_LOW or
*                       FS_ETPU_PWMMAC_ACTIVE_HIGH
*                       This parameter applies only if phase_type is
*                       FS_ETPU_PWMMAC_COMPL_PAIRS.
*
*RETURNS NOTES: This function returns 0 if PWM phases were successfully enabled.
*               In case phase channels have any pending HSRs phases are not
*               enabled and this function should be called again later. In this
*               case a sum of pending HSR numbers is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
*******************************************************************************/
int32_t fs_etpu_pwmmac_enable_3ph( uint8_t   channel,
                                   uint8_t   base_ch_polarity,
                                   uint8_t   compl_ch_polarity)
{
   uint32_t* pba;
   uint8_t   phaseA_channel;
   uint8_t   phaseB_channel;
   uint8_t   phaseC_channel;
   uint32_t  hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95)||
      ((base_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_HIGH)&&
      (base_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_LOW))||
      ((compl_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_HIGH)&&
      (compl_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_LOW)))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /* get phase channel numbers */
   pba = fs_etpu_data_ram(channel);
   phaseA_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEA_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);
   phaseB_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEB_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);
   phaseC_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEC_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);

   /* if phases channels have any pending HSRs do not write new HSRs but return
      sum of pending HSR numbers */
   hsrr = eTPU->CHAN[phaseA_channel].HSRR.R;
   hsrr += eTPU->CHAN[phaseB_channel].HSRR.R;
   hsrr += eTPU->CHAN[phaseC_channel].HSRR.R;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      hsrr += eTPU->CHAN[phaseA_channel+1].HSRR.R;
      hsrr += eTPU->CHAN[phaseB_channel+1].HSRR.R;
      hsrr += eTPU->CHAN[phaseC_channel+1].HSRR.R;
   }
   if (hsrr != 0) return ((int32_t)hsrr);

   /* set FM bits */
   eTPU->CHAN[phaseA_channel].SCR.R = (uint32_t)base_ch_polarity;
   eTPU->CHAN[phaseB_channel].SCR.R = (uint32_t)base_ch_polarity;
   eTPU->CHAN[phaseC_channel].SCR.R = (uint32_t)base_ch_polarity;
   /* write HSR */
   eTPU->CHAN[phaseA_channel].HSRR.R = FS_ETPU_PWMF_START;
   eTPU->CHAN[phaseB_channel].HSRR.R = FS_ETPU_PWMF_START;
   eTPU->CHAN[phaseC_channel].HSRR.R = FS_ETPU_PWMF_START;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      /* set FM bits */
      eTPU->CHAN[phaseA_channel+1].SCR.R = (uint32_t)(1+compl_ch_polarity);
      eTPU->CHAN[phaseB_channel+1].SCR.R = (uint32_t)(1+compl_ch_polarity);
      eTPU->CHAN[phaseC_channel+1].SCR.R = (uint32_t)(1+compl_ch_polarity);
      /* write HSR */
      eTPU->CHAN[phaseA_channel+1].HSRR.R = FS_ETPU_PWMF_START;
      eTPU->CHAN[phaseB_channel+1].HSRR.R = FS_ETPU_PWMF_START;
      eTPU->CHAN[phaseC_channel+1].HSRR.R = FS_ETPU_PWMF_START;
   }

   /* set flag phases started */
   *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASESSTARTED_OFFSET) =
                                                  FS_ETPU_PWMMAC_PHASES_STARTED;

   return(0);
}


/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_enable_2ph
*PURPOSE      : Enable PWM generation - 2 phases
*INPUTS NOTES : This function has 3 parameters:
*   channel           - This is the PWMMAC master channel number.
*                       This parameter must be assigned the same value
*                       as the channel parameter of the initialization
*                       function was assigned.
*   base_ch_polarity  - This parameter determines the polarity
*                       of the base channel.
*                       This parameter should be assigned a value of:
*                       FS_ETPU_PWMMAC_ACTIVE_HIGH or
*                       FS_ETPU_PWMMAC_ACTIVE_LOW
*   compl_ch_polarity - This parameter determines the polarity
*                       of the complementary channel.
*                       This parameter should be assigned a value of:
*                       FS_ETPU_PWMMAC_ACTIVE_LOW or
*                       FS_ETPU_PWMMAC_ACTIVE_HIGH
*                       This parameter applies only if phase_type is
*                       FS_ETPU_PWMMAC_COMPL_PAIRS.
*
*RETURNS NOTES: This function returns 0 if PWM phases were successfully enabled.
*               In case phase channels have any pending HSRs phases are not
*               enabled and this function should be called again later. In this
*               case a sum of pending HSR numbers is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
*******************************************************************************/
int32_t fs_etpu_pwmmac_enable_2ph( uint8_t   channel,
                                   uint8_t   base_ch_polarity,
                                   uint8_t   compl_ch_polarity)
{
   uint32_t* pba;
   uint8_t   phaseA_channel;
   uint8_t   phaseB_channel;
   uint32_t  hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95)||
      ((base_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_HIGH)&&
      (base_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_LOW))||
      ((compl_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_HIGH)&&
      (compl_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_LOW)))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /* get phase channel numbers */
   pba = fs_etpu_data_ram(channel);
   phaseA_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEA_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);
   phaseB_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEB_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);

   /* if phases channels have any pending HSRs do not write new HSRs but return
      sum of pending HSR numbers */
   hsrr = eTPU->CHAN[phaseA_channel].HSRR.R;
   hsrr += eTPU->CHAN[phaseB_channel].HSRR.R;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      hsrr += eTPU->CHAN[phaseA_channel+1].HSRR.R;
      hsrr += eTPU->CHAN[phaseB_channel+1].HSRR.R;
   }
   if (hsrr != 0) return ((int32_t)hsrr);

   /* set FM bits */
   eTPU->CHAN[phaseA_channel].SCR.R = (uint32_t)base_ch_polarity;
   eTPU->CHAN[phaseB_channel].SCR.R = (uint32_t)base_ch_polarity;
   /* write HSR */
   eTPU->CHAN[phaseA_channel].HSRR.R = FS_ETPU_PWMF_START;
   eTPU->CHAN[phaseB_channel].HSRR.R = FS_ETPU_PWMF_START;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      /* set FM bits */
      eTPU->CHAN[phaseA_channel+1].SCR.R = (uint32_t)(1+compl_ch_polarity);
      eTPU->CHAN[phaseB_channel+1].SCR.R = (uint32_t)(1+compl_ch_polarity);
      /* write HSR */
      eTPU->CHAN[phaseA_channel+1].HSRR.R = FS_ETPU_PWMF_START;
      eTPU->CHAN[phaseB_channel+1].HSRR.R = FS_ETPU_PWMF_START;
   }

   /* set flag phases started */
   *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASESSTARTED_OFFSET) =
                                                  FS_ETPU_PWMMAC_PHASES_STARTED;

   return(0);
}


/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_enable_1ph
*PURPOSE      : Enable PWM generation - 1 phase
*INPUTS NOTES : This function has 3 parameters:
*   channel           - This is the PWMMAC master channel number.
*                       This parameter must be assigned the same value
*                       as the channel parameter of the initialization
*                       function was assigned.
*   base_ch_polarity  - This parameter determines the polarity
*                       of the base channel.
*                       This parameter should be assigned a value of:
*                       FS_ETPU_PWMMAC_ACTIVE_HIGH or
*                       FS_ETPU_PWMMAC_ACTIVE_LOW
*   compl_ch_polarity - This parameter determines the polarity
*                       of the complementary channel.
*                       This parameter should be assigned a value of:
*                       FS_ETPU_PWMMAC_ACTIVE_LOW or
*                       FS_ETPU_PWMMAC_ACTIVE_HIGH
*                       This parameter applies only if phase_type is
*                       FS_ETPU_PWMMAC_COMPL_PAIRS.
*
*RETURNS NOTES: This function returns 0 if PWM phases were successfully enabled.
*               In case phase channels have any pending HSRs phases are not
*               enabled and this function should be called again later. In this
*               case a sum of pending HSR numbers is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
*******************************************************************************/
int32_t fs_etpu_pwmmac_enable_1ph( uint8_t   channel,
                                   uint8_t   base_ch_polarity,
                                   uint8_t   compl_ch_polarity)
{
   uint32_t* pba;
   uint8_t   phaseA_channel;
   uint32_t  hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95)||
      ((base_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_HIGH)&&
      (base_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_LOW))||
      ((compl_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_HIGH)&&
      (compl_ch_polarity!=FS_ETPU_PWMMAC_ACTIVE_LOW)))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /* get phase channel numbers */
   pba = fs_etpu_data_ram(channel);
   phaseA_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEA_OFFSET +
                       FS_ETPU_PWMF_CHANNUM_OFFSET);

   /* if phases channels have any pending HSRs do not write new HSRs but return
      sum of pending HSR numbers */
   hsrr = eTPU->CHAN[phaseA_channel].HSRR.R;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      hsrr += eTPU->CHAN[phaseA_channel+1].HSRR.R;
   }
   if (hsrr != 0) return ((int32_t)hsrr);

   /* set FM bits */
   eTPU->CHAN[phaseA_channel].SCR.R = (uint32_t)base_ch_polarity;
   /* write HSR */
   eTPU->CHAN[phaseA_channel].HSRR.R = FS_ETPU_PWMF_START;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      /* set FM bits */
      eTPU->CHAN[phaseA_channel+1].SCR.R = (uint32_t)(1+compl_ch_polarity);
      /* write HSR */
      eTPU->CHAN[phaseA_channel+1].HSRR.R = FS_ETPU_PWMF_START;
   }
   /* set flag phases started */
   *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASESSTARTED_OFFSET) =
                                                  FS_ETPU_PWMMAC_PHASES_STARTED;

   return(0);
}


/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_disable_3ph
*PURPOSE      : Disable PWM generation - 3 phases.
*INPUTS NOTES : This function has 3 parameters:
*  channel                   - This is the PWMMAC master channel number.
*                              This parameter must be assigned the same value
*                              as the channel parameter of the initialization
*                              function was assigned.
*  base_ch_disable_pin_state - This is the required output state of the base
*                              channel pins, after disable of PWM generation.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
* compl_ch_disable_pin_state - This is the required output state of the
*                              complementary channel pins, after disable of PWM
*                              generation.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
*                              This parameter applies only if phase_type is
*                              FS_ETPU_PWMMAC_COMPL_PAIRS.
*
*RETURNS NOTES: This function returns 0 if PWM phases were successfully
*               disabled. In case phase channels have any pending HSRs phases
*               are not disabled and this function should be called again later.
*               In this case a sum of pending HSR numbers is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
*******************************************************************************/
int32_t fs_etpu_pwmmac_disable_3ph( uint8_t channel,
                                    uint8_t base_ch_disable_pin_state,
                                    uint8_t compl_ch_disable_pin_state)
{
   uint32_t* pba;
   uint8_t   phaseA_channel;
   uint8_t   phaseB_channel;
   uint8_t   phaseC_channel;
   uint32_t  hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95)||
      ((base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH))||
      ((compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH)))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /* get phase channel numbers */
   pba = fs_etpu_data_ram(channel);
   phaseA_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEA_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);
   phaseB_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEB_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);
   phaseC_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEC_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);

   /* if phases channels have any pending HSRs do not write new HSRs but return
      sum of pending HSR numbers */
   hsrr = eTPU->CHAN[phaseA_channel].HSRR.R;
   hsrr += eTPU->CHAN[phaseB_channel].HSRR.R;
   hsrr += eTPU->CHAN[phaseC_channel].HSRR.R;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      hsrr += eTPU->CHAN[phaseA_channel+1].HSRR.R;
      hsrr += eTPU->CHAN[phaseB_channel+1].HSRR.R;
      hsrr += eTPU->CHAN[phaseC_channel+1].HSRR.R;
   }
   if (hsrr != 0) return ((int32_t)hsrr);

   /****************************************
    * Write HSR.
    ***************************************/
   eTPU->CHAN[phaseA_channel].HSRR.R = base_ch_disable_pin_state;
   eTPU->CHAN[phaseB_channel].HSRR.R = base_ch_disable_pin_state;
   eTPU->CHAN[phaseC_channel].HSRR.R = base_ch_disable_pin_state;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      eTPU->CHAN[phaseA_channel+1].HSRR.R = compl_ch_disable_pin_state;
      eTPU->CHAN[phaseB_channel+1].HSRR.R = compl_ch_disable_pin_state;
      eTPU->CHAN[phaseC_channel+1].HSRR.R = compl_ch_disable_pin_state;
   }

   return(0);
}

/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_disable_2ph
*PURPOSE      : Disable PWM generation - 2 phases.
*INPUTS NOTES : This function has 3 parameters:
*  channel                   - This is the PWMMAC master channel number.
*                              This parameter must be assigned the same value
*                              as the channel parameter of the initialization
*                              function was assigned.
*  base_ch_disable_pin_state - This is the required output state of the base
*                              channel pins, after disable of PWM generation.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
* compl_ch_disable_pin_state - This is the required output state of the
*                              complementary channel pins, after disable of PWM
*                              generation.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
*                              This parameter applies only if phase_type is
*                              FS_ETPU_PWMMAC_COMPL_PAIRS.
*
*RETURNS NOTES: This function returns 0 if PWM phases were successfully
*               disabled. In case phase channels have any pending HSRs phases
*               are not disabled and this function should be called again later.
*               In this case a sum of pending HSR numbers is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
*******************************************************************************/
int32_t fs_etpu_pwmmac_disable_2ph( uint8_t channel,
                                    uint8_t base_ch_disable_pin_state,
                                    uint8_t compl_ch_disable_pin_state)
{
   uint32_t* pba;
   uint8_t   phaseA_channel;
   uint8_t   phaseB_channel;
   uint32_t  hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95)||
      ((base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH))||
      ((compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH)))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /* get phase channel numbers */
   pba = fs_etpu_data_ram(channel);
   phaseA_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEA_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);
   phaseB_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEB_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);

   /* if phases channels have any pending HSRs do not write new HSRs but return
      sum of pending HSR numbers */
   hsrr = eTPU->CHAN[phaseA_channel].HSRR.R;
   hsrr += eTPU->CHAN[phaseB_channel].HSRR.R;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      hsrr += eTPU->CHAN[phaseA_channel+1].HSRR.R;
      hsrr += eTPU->CHAN[phaseB_channel+1].HSRR.R;
   }
   if (hsrr != 0) return ((int32_t)hsrr);

   /****************************************
    * Write HSR.
    ***************************************/
   eTPU->CHAN[phaseA_channel].HSRR.R = base_ch_disable_pin_state;
   eTPU->CHAN[phaseB_channel].HSRR.R = base_ch_disable_pin_state;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      eTPU->CHAN[phaseA_channel+1].HSRR.R = compl_ch_disable_pin_state;
      eTPU->CHAN[phaseB_channel+1].HSRR.R = compl_ch_disable_pin_state;
   }

   return(0);
}

/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_disable_1ph
*PURPOSE      : Disable PWM generation - 1 phase.
*INPUTS NOTES : This function has 3 parameters:
*  channel                   - This is the PWMMAC master channel number.
*                              This parameter must be assigned the same value
*                              as the channel parameter of the initialization
*                              function was assigned.
*  base_ch_disable_pin_state - This is the required output state of the base
*                              channel pin, after disable of PWM generation.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
* compl_ch_disable_pin_state - This is the required output state of the
*                              complementary channel pin, after disable of PWM
*                              generation.
*                              This parameter should be assigned a value of
*                              FS_ETPU_PWMMAC_PIN_LOW or
*                              FS_ETPU_PWMMAC_PIN_HIGH.
*                              This parameter applies only if phase_type is
*                              FS_ETPU_PWMMAC_COMPL_PAIRS.
*
*RETURNS NOTES: This function returns 0 if PWM phases were successfully
*               disabled. In case phase channels have any pending HSRs phases
*               are not disabled and this function should be called again later.
*               In this case a sum of pending HSR numbers is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
*******************************************************************************/
int32_t fs_etpu_pwmmac_disable_1ph( uint8_t channel,
                                    uint8_t base_ch_disable_pin_state,
                                    uint8_t compl_ch_disable_pin_state)
{
   uint32_t* pba;
   uint8_t   phaseA_channel;
   uint32_t  hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95)||
      ((base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (base_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH))||
      ((compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_LOW)&&
      (compl_ch_disable_pin_state!=FS_ETPU_PWMMAC_PIN_HIGH)))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /* get phase channel numbers */
   pba = fs_etpu_data_ram(channel);
   phaseA_channel = *((uint8_t*)pba + FS_ETPU_PWMMAC_PHASEA_OFFSET +
                      FS_ETPU_PWMF_CHANNUM_OFFSET);

   /* if phases channels have any pending HSRs do not write new HSRs but return
      sum of pending HSR numbers */
   hsrr = eTPU->CHAN[phaseA_channel].HSRR.R;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      hsrr += eTPU->CHAN[phaseA_channel+1].HSRR.R;
   }
   if (hsrr != 0) return ((int32_t)hsrr);

   /****************************************
    * Write HSR.
    ***************************************/
   eTPU->CHAN[phaseA_channel].HSRR.R = base_ch_disable_pin_state;
   if (eTPU->CHAN[channel].SCR.B.FM1)
   {
      eTPU->CHAN[phaseA_channel+1].HSRR.R = compl_ch_disable_pin_state;
   }

   return(0);
}


/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_set_period
*PURPOSE      : Set PWM period.
*INPUTS NOTES : This function has 2 parameters:
*               channel   - This is the PWMMAC master channel number.
*                           This parameter must be assigned the same value
*                           as the channel parameter of the initialization
*                           function was assigned.
*               period    - This parameter determines the PWM period
*                           as number of TCR1 cycles.
*
*RETURNS NOTES: Error code that can be returned is: FS_ETPU_ERROR_VALUE.
*******************************************************************************/
int32_t fs_etpu_pwmmac_set_period( uint8_t  channel,
                                   uint24_t period)
{
   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /****************************************
    * Write parameter.
    ***************************************/
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_PERIOD_OFFSET, period);

   return(0);
}


/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_update_voltage_unsigned
*PURPOSE      : Update of input voltage in case of voltage-unsigned modulation.
*INPUTS NOTES : This function has 2 parameters:
*               channel   - This is the PWMMAC master channel number.
*                           This parameter must be assigned the same value
*                           as the channel parameter of the initialization
*                           function was assigned.
*               voltage   - This parameter determines the applied voltage
*                           in range (0,2^24).
*
*RETURNS NOTES: This function returns 0 if input voltage was successfully
*               updated. In case master channel has any pending HSR input
*               voltage is not updated and this function should be called again
*               later. In this case the pending HSR number is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
*******************************************************************************/
int32_t fs_etpu_pwmmac_update_voltage_unsigned( uint8_t  channel,
                                                uint24_t voltage)
{
   uint32_t  hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /****************************************
    * Write parameter.
    ***************************************/
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_INPUTS_OFFSET+1, voltage);

   /****************************************
    * Write HSR.
    ***************************************/
   hsrr = eTPU->CHAN[channel].HSRR.R;
   if (hsrr != 0) return ((int32_t)hsrr);
   fs_etpu_set_hsr(channel, FS_ETPU_PWMMAC_UPDATE);

   return(0);
}


/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_update_voltage_signed
*PURPOSE      : Update of input voltage in case of voltage-signed modulation.
*INPUTS NOTES : This function has 2 parameters:
*               channel   - This is the PWMMAC master channel number.
*                           This parameter must be assigned the same value
*                           as the channel parameter of the initialization
*                           function was assigned.
*               voltage   - This parameter determines the applied voltage
*                           in range (-2^23,2^23).
*
*RETURNS NOTES: This function returns 0 if input voltage was successfully
*               updated. In case master channel has any pending HSR input
*               voltage is not updated and this function should be called again
*               later. In this case the pending HSR number is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
*******************************************************************************/
int32_t fs_etpu_pwmmac_update_voltage_signed( uint8_t  channel,
                                              int24_t  voltage)
{
   uint32_t  hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95))
   {
       return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /****************************************
    * Write parameter.
    ***************************************/
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_INPUTS_OFFSET+1,
                             (uint24_t)voltage);

   /****************************************
    * Write HSR.
    ***************************************/
   hsrr = eTPU->CHAN[channel].HSRR.R;
   if (hsrr != 0) return ((int32_t)hsrr);
   fs_etpu_set_hsr(channel, FS_ETPU_PWMMAC_UPDATE);

   return(0);
}

/*******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_update_duty_cycles
*PURPOSE      : Update of phase duty-cycles in case of no modulation.
*INPUTS NOTES : This function has 4 parameters:
*               channel   - This is the PWMMAC master channel number.
*                           This parameter must be assigned the same value
*                           as the channel parameter of the initialization
*                           function was assigned.
*               dutyA     - This parameter determines the phase A duty-cycle
*                           in range (-2^23,2^23).
*               dutyB     - This parameter determines the phase B duty-cycle
*                           in range (-2^23,2^23).
*               dutyC     - This parameter determines the phase C duty-cycle
*                           in range (-2^23,2^23).
*
*RETURNS NOTES: This function returns 0 if duty-cycles were successfully
*               updated. In case master channel has any pending HSR duty-cycles
*               are not updated and this function should be called again
*               later. In this case the pending HSR number is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
*******************************************************************************/
int32_t fs_etpu_pwmmac_update_duty_cycles( uint8_t channel,
                                           uint24_t dutyA,
                                           uint24_t dutyB,
                                           uint24_t dutyC)
{
   uint32_t hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /****************************************
    * Write parameters.
    ***************************************/
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_PHASEA_OFFSET +
                             FS_ETPU_PWMF_DUTY_OFFSET, dutyA);
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_PHASEB_OFFSET +
                             FS_ETPU_PWMF_DUTY_OFFSET, dutyB);
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_PHASEC_OFFSET +
                             FS_ETPU_PWMF_DUTY_OFFSET, dutyC);

   /****************************************
    * Write HSR.
    ***************************************/
   hsrr = eTPU->CHAN[channel].HSRR.R;
   if (hsrr != 0) return ((int32_t)hsrr);
   fs_etpu_set_hsr(channel, FS_ETPU_PWMMAC_UPDATE);

   return(0);
}

/******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_update_svm
*PURPOSE      : Update of input parameters in case of any SVM modulation.
*INPUTS NOTES : This function has 3 parameters:
*               channel   - This is the PWMMAC master channel number.
*                           This parameter must be assigned the same value
*                           as the channel parameter of the initialization
*                           function was assigned.
*               alpha     - This parameter determines the alpha component
*                           of the applied voltage in range (-2^23,2^23).
*               beta      - This parameter determines the beta component
*                           of the applied voltage in range (-2^23,2^23).
*               
*RETURNS NOTES: This function returns 0 if duty-cycles were successfully
*               updated. In case master channel has any pending HSR duty-cycles
*               are not updated and this function should be called again
*               later. In this case the pending HSR number is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
******************************************************************************/
int32_t fs_etpu_pwmmac_update_svm( uint8_t channel,
                                   int24_t alpha,
                                   int24_t beta)
{
   uint32_t hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   if (alpha & 0x00800000)
   {
       alpha |= 0xFF000000;
   }
   
   if (beta & 0x00800000)
   {
       beta |= 0xFF000000;
   }
   
   /****************************************
    * Write parameters.
    ***************************************/
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_INPUTS_OFFSET+1, (uint24_t)(alpha>>2));
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_INPUTS_OFFSET+5, (uint24_t)(beta>>2));

   /****************************************
    * Write HSR.
    ***************************************/
   hsrr = eTPU->CHAN[channel].HSRR.R;
   if (hsrr != 0) return ((int32_t)hsrr);
   fs_etpu_set_hsr(channel, FS_ETPU_PWMMAC_UPDATE);

   return(0);
}


/******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_update_vector
*PURPOSE      : Update of input parameters in case of any table modulation.
*INPUTS NOTES : This function has 3 parameters:
*               channel   - This is the PWMMAC master channel number.
*                           This parameter must be assigned the same value
*                           as the channel parameter of the initialization
*                           function was assigned.
*               amplitude - This parameter determines the applied voltage
*                           amplitude in range (-2^23,2^23).
*               angle     - This parameter determines the applied voltage
*                           angle in degrees (-2^23,2^23).
*               
*RETURNS NOTES: This function returns 0 if duty-cycles were successfully
*               updated. In case master channel has any pending HSR duty-cycles
*               are not updated and this function should be called again
*               later. In this case the pending HSR number is returned.
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
******************************************************************************/
int32_t fs_etpu_pwmmac_update_vector( uint8_t channel,
                                      int24_t amplitude,
                                      int24_t angle)
{
   uint32_t hsrr;

   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   if (amplitude & 0x00800000)
   {
       amplitude |= 0xFF000000;
   }
   
   /****************************************
    * Write parameters.
    ***************************************/
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_INPUTS_OFFSET+1, (uint24_t)angle);
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_INPUTS_OFFSET+5, (uint24_t)(amplitude>>2));

   /****************************************
    * Write HSR.
    ***************************************/
   hsrr = eTPU->CHAN[channel].HSRR.R;
   if (hsrr != 0) return ((int32_t)hsrr);
   fs_etpu_set_hsr(channel, FS_ETPU_PWMMAC_UPDATE);

   return(0);
}

/******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_set_dtc
*PURPOSE      : Set dtc - dead-time correction type.
*INPUTS NOTES : This function has 2 parameters:
*               phase_channel - This is the phase A, B or C channel number.
*                               This parameter must be assigned the same value
*                               as the phaseA_channel (phaseB_channel, 
*                               phaseC_channel) parameter of the initialization
*                               function was assigned.
*               dtc           - This parameter sets dead-time insertion type.
*                               This parameter should be assigned a value of:        
*                               FS_ETPU_PWMMAC_DTC_COMPL_COMPENSATES or        
*                               FS_ETPU_PWMMAC_DTC_BOTH_COMPENSATE or        
*                               FS_ETPU_PWMMAC_DTC_BASE_COMPENSATES        
*               
*RETURNS NOTES: Error code that can be returned is: FS_ETPU_ERROR_VALUE.
******************************************************************************/
int32_t fs_etpu_pwmmac_set_dtc( uint8_t phase_channel,
                                uint8_t dtc)
{
   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((phase_channel>31)&&(phase_channel<64))||(phase_channel>95)||
      ((dtc!=FS_ETPU_PWMMAC_DTC_BASE_COMPENSATES)&&
      (dtc!=FS_ETPU_PWMMAC_DTC_BOTH_COMPENSATE)&&
      (dtc!=FS_ETPU_PWMMAC_DTC_COMPL_COMPENSATES)))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /****************************************
    * Write parameters.
    ***************************************/
   fs_etpu_set_chan_local_8(phase_channel, FS_ETPU_PWMF_DTC_OFFSET, dtc);       

   return(0);
}

/******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_get_sector
*PURPOSE      : Get current sector value (1-6).
*               Applies only in case of any SVM modulation.
*INPUTS NOTES : This function has 1 parameter:
*               channel   - This is the PWMMAC master channel number.
*                           This parameter must be assigned the same value
*                           as the channel parameter of the initialization
*                           function was assigned.
*               
*RETURNS NOTES: Sector value (uint8_t) 1 to 6. 
*               Error code that can be returned is: FS_ETPU_ERROR_VALUE.
******************************************************************************/
uint8_t fs_etpu_pwmmac_get_sector( uint8_t channel)
{
   uint8_t sector=0;
      
   /****************************************
    * Parameters bounds check.
    ***************************************/
   #ifdef FS_ETPU_MC_PARAM_CHECK
   if(((channel>31)&&(channel<64))||(channel>95))
   {
      return(FS_ETPU_ERROR_VALUE);
   }
   #endif

   /****************************************
    * Read parameters.
    ***************************************/
      sector = fs_etpu_get_chan_local_8(channel, FS_ETPU_PWMMAC_SECTOR_OFFSET);

   return(sector);  
}

/******************************************************************************
*FUNCTION     : fs_etpu_pwmmac_set_dead_time
*PURPOSE      : Set dead-time.
*INPUTS NOTES : This function has 2 parameters:
*               channel   - This is the PWMMAC master channel number.
*                           This parameter must be assigned the same value
*                           as the channel parameter of the initialization
*                           function was assigned.
*               dead_time - This parameter determines the dead-time
*                           as number of TCR1 cycles.
*               
*RETURNS NOTES: none.
******************************************************************************/
void fs_etpu_pwmmac_set_dead_time( uint8_t  channel,
                                   uint24_t dead_time)
{
   fs_etpu_set_chan_local_24(channel, FS_ETPU_PWMMAC_DEADTIME_OFFSET, dead_time);       
}


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
