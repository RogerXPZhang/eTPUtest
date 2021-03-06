
/****************************************************************/
/* WARNING this file is automatically generated DO NOT EDIT IT! */
/*                                                              */
/* This file provides an interface between eTPU code and CPU    */
/* code. All references to the PWMMAC function should be made   */
/* with information in this file. This allows only symbolic     */
/* information to be referenced which allows the eTPU code to be*/
/* optimized without effecting the CPU code.                    */
/****************************************************************/
#ifndef _ETPU_PWMMAC_AUTO_H_
#define _ETPU_PWMMAC_AUTO_H_

/****************************************************************
* Function Configuration Information.
****************************************************************/
#define FS_ETPU_PWMMAC_FUNCTION_NUMBER  1 
#define FS_ETPU_PWMMAC_TABLE_SELECT     1 
#define FS_ETPU_PWMMAC_NUM_PARMS        0x0088 

/****************************************************************
* Host Service Request Definitions.
****************************************************************/
#define FS_ETPU_PWMMAC_INIT_NORMAL        7     
#define FS_ETPU_PWMMAC_INIT_HALF_CYCLE    5 
#define FS_ETPU_PWMMAC_UPDATE             2          

/****************************************************************
* Parameter Definitions.
****************************************************************/
#define FS_ETPU_PWMMAC_PHASES_OFFSET         0x0000
#define FS_ETPU_PWMMAC_STARTOFFSET_OFFSET    0x0001
#define FS_ETPU_PWMMAC_MODULATION_OFFSET     0x0004
#define FS_ETPU_PWMMAC_UPDATETIME_OFFSET     0x0005
#define FS_ETPU_PWMMAC_DEADTIME_OFFSET       0x0009
#define FS_ETPU_PWMMAC_MINPW_OFFSET          0x000D
#define FS_ETPU_PWMMAC_PERIOD_OFFSET         0x0011
#define FS_ETPU_PWMMAC_PERIODCOPY_OFFSET     0x0015
#define FS_ETPU_PWMMAC_EDGETIME_OFFSET       0x0019
#define FS_ETPU_PWMMAC_INPUTS_OFFSET         0x001C
#define FS_ETPU_PWMMAC_PTABLE_OFFSET         0x0025
#define FS_ETPU_PWMMAC_SECTOR_OFFSET         0x0008
#define FS_ETPU_PWMMAC_PHASESSTARTED_OFFSET  0x000C
#define FS_ETPU_PWMMAC_PHASEA_OFFSET         0x0028
#define FS_ETPU_PWMMAC_PHASEB_OFFSET         0x0048
#define FS_ETPU_PWMMAC_PHASEC_OFFSET         0x0068

/****************************************************************
* Value Definitions.
****************************************************************/
#define FS_ETPU_PWMMAC_MODULATION_NO         0        
#define FS_ETPU_PWMMAC_MODULATION_UNSIGNED   1  
#define FS_ETPU_PWMMAC_MODULATION_SIGNED     2    
#define FS_ETPU_PWMMAC_MODULATION_SVMSTD     3    
#define FS_ETPU_PWMMAC_MODULATION_SVMU0N     4    
#define FS_ETPU_PWMMAC_MODULATION_SVMU7N     5    
#define FS_ETPU_PWMMAC_MODULATION_PWMICT     6    
#define FS_ETPU_PWMMAC_MODULATION_SINE_TABLE 7
#define FS_ETPU_PWMMAC_MODULATION_USER_TABLE 8

#define FS_ETPU_PWMMAC_FM_EDGE_ALIGNED      0
#define FS_ETPU_PWMMAC_FM_CENTER_ALIGNED    1

#define FS_ETPU_PWMMAC_FM_SINGLE_CHANNELS   0
#define FS_ETPU_PWMMAC_FM_COMPLEMEN_PAIRS   2

#define FS_ETPU_PWMMAC_OPTION_DUTY_NEG      1 
#define FS_ETPU_PWMMAC_OPTION_SWAP          2 

#define FS_ETPU_PWMMAC_PHASES_STARTED       1 
#endif
