/************************************************************************
 *    This file was generated by eTPU Graphical Configuration Tool        
 *                                                                        
 *    DO NOT EDIT !!! DO NOT EDIT !!! DO NOT EDIT !!! DO NOT EDIT !!!     
 *                                                                        
 *  DESCRIPTION:
 *  This file contains my_system_etpu_init and my_system_etpu_start
 *  functions and related items. The function initialize the eTPU 
 *  global and channel settings, based on the user settings 
 *  in the eTPU GCT, and runs the eTPU.               
 *  A header file of the same name .h is also generated.                   
 *
 *  Creation Date:  24-01-2017 2:00:22 PM 
 *
 **************************************************************************/

/* general routines */
#include "etpu_util.h"             /* Utility routines for working eTPU */
#include "mpc563m_vars.h"          /* Processor dependent variables */

/* eTPU function set used */
#include "etpu_set/etpu_set.h"       

/* eTPU functions APIs */
#include "pwmmac/etpu_pwmmac.h" /* PWMMAC - PWM Master for AC Motors */

/* our private header file (generated by eTPU GCT) */
#include "etpu_gct.h"



/* External object declarations */
/* ... no external variables used */


/**************************************************************************
 * 
 * Global Variables
 *
 **************************************************************************/

uint32_t *fs_free_param;

/**************************************************************************
 *
 * CPU and clock configuration:
 *
 *   CPU = MPC563xM
 *   SYSCLK = 80 MHz
 *   Input_TCRCLK_A = OFF
 *   Input_TCRCLK_B = OFF
 *   
 **************************************************************************/

uint32_t etpu_a_tcr1_freq = 80000000;
uint32_t etpu_a_tcr2_freq = 10000000;
uint32_t etpu_b_tcr1_freq = 0;
uint32_t etpu_b_tcr2_freq = 10000000;


/**************************************************************************
 * 
 * etpu_config structure filled accodring to the user settings
 *
 **************************************************************************/

struct etpu_config_t my_etpu_config =
{
  /* etpu_config.mcr */
  FS_ETPU_VIS_OFF	/* SCM not visible (VIS=0) */
  | FS_ETPU_MISC_ENABLE	/* SCM operation enabled (SCMMISEN=1) */,

  /* etpu_config.misc */
  FS_ETPU_MISC,

  /* etpu_config.ecr_a */
  FS_ETPU_ENTRY_TABLE_ADDR	/* entry table base address = shifted FS_ETPU_ENTRY_TABLE */
  | FS_ETPU_FCSS_DIV2	/* channel filter clock source using FPSCK divider */
  | FS_ETPU_CHAN_FILTER_2SAMPLE	/* channel filter mode = two-sample mode (CDFC=0) */
  | FS_ETPU_ENGINE_ENABLE	/* engine is enabled (MDIS=0) */
  | FS_ETPU_FILTER_CLOCK_DIV2	/* channel filter clock = etpuclk div 2 (FPSCK=0), see also FCSS if using eTPU2 */
  | FS_ETPU_PRIORITY_PASSING_ENABLE	/* Scheduler priority passing enabled (SPPDIS=0) */,

  /* etpu_config.tbcr_a */
  FS_ETPU_ANGLE_MODE_DISABLE	/* TCR2 angle mode is disabled (AM=0) */
  | FS_ETPU_TCR2_PRESCALER(1)	/* TCR2 prescaler = 1 (TCR2P='1-1') */
  | FS_ETPU_TCR1_PRESCALER(1)	/* TCR1 prescaler = 1 (TCR1P='1-1') */
  | FS_ETPU_TCRCLK_INPUT_DIV2CLOCK	/* TCRCLK signal is filtered with filter clock = etpuclk div 2 (TCRCF=x0) */
  | FS_ETPU_TCR2CTL_DIV8	/* TCR2 source = etpuclk div 8 (TCR2CTL=4) */
  | FS_ETPU_TCRCLK_MODE_2SAMPLE	/* TCRCLK signal is filtered using two-sample mode (TCRCF=0x) */
  | FS_ETPU_TCR1CS_DIV1	/* TCR1 source bypassing divider, full system clock */
  | FS_ETPU_TCR1CTL_DIV2	/* TCR1 source = etpuclk div 2 (see also TCR1CS if using eTPU2) */,

  /* etpu_config.stacr_a */
  FS_ETPU_TCR1_STAC_SERVER	/* TCR1 resource operates as server (RSC1=1) */
  | FS_ETPU_TCR2_STAC_SERVER	/* TCR2 resource operates as server (RSC2=1) */
  | FS_ETPU_TCR1_STAC_DISABLE	/* TCR1 operation on STAC bus disabled (REN1=0) */
  | FS_ETPU_TCR2_STAC_DISABLE	/* TCR2 operation on STAC bus disabled (REN2=0) */
  | FS_ETPU_TCR1_STAC_SRVSLOT(0)	/* TCR1 resource server slot = 0 (SRV1=0) */
  | FS_ETPU_TCR2_STAC_SRVSLOT(0)	/* TCR2 resource server slot = 0 (SRV2=0) */,

  /* etpu_config.ecr_b */
  0,

  /* etpu_config.tbcr_b */
  0,

  /* etpu_config.stacr_b */
  0,

  /* etpu_config.wdtr_a */
  FS_ETPU_WDM_DISABLED	/* Watchdog Mode = disabled (WDM=0) */
  | FS_ETPU_WDTR_WDCNT(0)	/* watchdog count in microcycles */,

  /* etpu_config.wdtr_b */
  0
};

/**************************************************************************
 * 
 *  FUNCTION : my_system_etpu_init
 * 
 *  PURPOSE  : Initializes the eTPU settings and channel settings, based
 *             on the user settings in the eTPU GCT:
 *
 *             1. Initialize global setting using fs_etpu_init function
 *                and the my_etpu_config structure.
 *             2. Initialize channel setting using channel function APIs 
 *                or standard eTPU routines.
 *
 *  RETURNS  : If the initialization of all channel functions passed OK, 
 *             the return value is 0. If the initialization of a channel 
 *             failed, the returned value is a number of the channel that
 *             failed to initialized increased by 1.
 *
 **************************************************************************/

int32_t my_system_etpu_init()
{
  int32_t err_code;

  /*
   *  Initialization of eTPU global settings
   */

  fs_etpu_init( my_etpu_config, (uint32_t *)etpu_code, sizeof(etpu_code),
                (uint32_t *)etpu_globals, sizeof(etpu_globals));

  /*
   *  Initialization of eTPU channel settings
   */

  /* 
   *  eTPU API Function initialization: 'PWMMAC - PWM Master for AC Motors'
   */
  err_code = fs_etpu_pwmmac_init_3ph (PWMMAC0_MASTER,	/* engine: A; channel: 7 */
                                      FS_ETPU_PRIORITY_MIDDLE,	/* priority: Middle */
                                      PWMMAC0_PHASEA_BASE,	/* engine: A; channel: 8 */
                                      FS_ETPU_PWMMAC_DUTY_POS,	/* phaseA_negate_duty: do not negate duty-cycle */
                                      PWMMAC0_PHASEB_BASE,	/* engine: A; channel: 10 */
                                      FS_ETPU_PWMMAC_DUTY_POS,	/* phaseB_negate_duty: do not negate duty-cycle */
                                      PWMMAC0_PHASEC_BASE,	/* engine: A; channel: 12 */
                                      FS_ETPU_PWMMAC_DUTY_POS,	/* phaseC_negate_duty: do not negate duty-cycle */
                                      FS_ETPU_PWMMAC_MOD_NO,	/* modulation: no modulation: update duty-cycles */
                                      fs_etpu_pwmmac_sin3h_lut,	/* p_table: parameter missing */
                                      FS_ETPU_PWMMAC_NORMAL,	/* update: normal update */
                                      FS_ETPU_PWMMAC_CENTER_ALIGNED,	/* alignment: center-aligned */
                                      FS_ETPU_PWMMAC_FULL_RANGE_COMPL_PAIRS,	/* phases_type: full range - complementary pairs */
                                      FS_ETPU_PWMMAC_NO_SWAP,	/* swap: do not swap dead-time insertion */
                                      FS_ETPU_PWMMAC_PIN_LOW,	/* base_ch_disable_pin_state: pin low */
                                      FS_ETPU_PWMMAC_PIN_LOW,	/* compl_ch_disable_pin_state: pin low */
                                      0,	/* start_offset: 0 */
                                      5000,	/* period: 5000 */
                                      2500,	/* update_time: 2500 */
                                      160,	/* dead_time: 160 */
                                      80);	/* min_pw: 80 */
  if (err_code != 0)
  	return ((PWMMAC0_MASTER) + 1);
  
  

  return(0);
}

/**************************************************************************
 *
 *  FUNCTION : my_system_etpu_start
 *
 *  PURPOSE  : Initializes the eTPU settings and channel settings, based
 *             on the user settings in the eTPU GCT:
 *
 *             1. Initialize
 *                - Interrupt Enable
 *                - DMA Enable
 *                - Output Disable
 *                options of each channel.
 *             2. Run eTPU using Global Timebase Enable (GTBE) bit.
 *
 **************************************************************************/

void my_system_etpu_start()
{
  /*
   *  Initialization of Interrupt Enable, DMA Enable
   *  and Output Disable channel options
   */

  uint8_t i;
  uint32_t odis_a;
  uint32_t opol_a;

  eTPU->CIER_A.R = ETPU_CIE_A;

  eTPU->CDTRER_A.R = ETPU_DTRE_A;

  odis_a = ETPU_ODIS_A;
  opol_a = ETPU_OPOL_A;
  for(i=0; i<32; i++) {
    eTPU->CHAN[i].CR.B.ODIS = odis_a & 0x00000001;
    eTPU->CHAN[i].CR.B.OPOL = opol_a & 0x00000001;
    odis_a >>= 1;
    opol_a >>= 1;
  }

  /*
   *  Synchronous start of all TCR time bases
   */

  fs_timer_start();
  fs_etpu_pwmmac_enable_3ph(PWMMAC0_MASTER,FS_ETPU_PWMMAC_ACTIVE_HIGH,FS_ETPU_PWMMAC_ACTIVE_LOW);
}

/**************************************************************************
 *
 * Private GCT-generated data may follow this comment
 *
 **************************************************************************/

