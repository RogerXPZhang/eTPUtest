/**************************************************************************
 * FILE NAME: $RCSfile: mpc5777c_vars_c.h,v $  COPYRIGHT (c) Freescale 2015 *
 * DESCRIPTION:                                     All Rights Reserved   *
 * Variables that define some features of the MPC5777C.                   *
 * !!!!This file must only be included once in every project!!!!          *
 *========================================================================*
 * ORIGINAL AUTHOR: Milan Brejl [r54529]                                *
 *
 **************************************************************************/

/* eTPU characteristics definition */
struct eTPUC_struct *eTPUC = (struct eTPUC_struct *)0xFFE00000;

uint32_t fs_etpuc_code_start =     0xFFE10000;
uint32_t fs_etpuc_data_ram_start = 0xFFE08000;
uint32_t fs_etpuc_data_ram_end =   0xFFE097FC;
uint32_t fs_etpuc_data_ram_ext =   0xFFE0C000;

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
 *
 ********************************************************************/


