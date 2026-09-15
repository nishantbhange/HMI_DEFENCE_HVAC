/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : Wdog,Ewm
*   Dependencies         : none
*
*   Autosar Version      : 4.7.0
*   Autosar Revision     : ASR_REL_4_7_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 3.0.0
*   Build Version        : S32K1_RTD_3_0_0_QLP04_D2509_ASR_REL_4_7_REV_0000_20250930
*
*   Copyright 2020-2025 NXP
*
*   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/


/**
*   @file
*
*   @addtogroup Wdog_Ip
*
*   @file
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/

#include "Wdog_Ip_PBcfg.h"

/*==================================================================================================
*                                 SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define WDOG_IP_VENDOR_ID_PBCFG_C                    43
#define WDOG_IP_AR_RELEASE_MAJOR_VERSION_PBCFG_C     4
#define WDOG_IP_AR_RELEASE_MINOR_VERSION_PBCFG_C     7
#define WDOG_IP_AR_RELEASE_REVISION_VERSION_PBCFG_C  0
#define WDOG_IP_SW_MAJOR_VERSION_PBCFG_C             3
#define WDOG_IP_SW_MINOR_VERSION_PBCFG_C             0
#define WDOG_IP_SW_PATCH_VERSION_PBCFG_C             0

/*==================================================================================================
*                                       FILE VERSION CHECKS
==================================================================================================*/
/* Check if current file and Wdog_Ip_PBcfg header file are of the same vendor */
#if (WDOG_IP_VENDOR_ID_PBCFG_C != WDOG_IP_VENDOR_ID_PBCFG)
#error "Wdog_Ip_PBcfg.c and Wdog_Ip_PBcfg.h have different vendor ids"
#endif

/* Check if current file and Wdog_Ip_PBcfg header file are of the same Autosar version */
#if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION_PBCFG_C    != WDOG_IP_AR_RELEASE_MAJOR_VERSION_PBCFG) || \
     (WDOG_IP_AR_RELEASE_MINOR_VERSION_PBCFG_C    != WDOG_IP_AR_RELEASE_MINOR_VERSION_PBCFG) || \
     (WDOG_IP_AR_RELEASE_REVISION_VERSION_PBCFG_C != WDOG_IP_AR_RELEASE_REVISION_VERSION_PBCFG))
#error "AutoSar Version Numbers of Wdog_Ip_PBcfg.c and Wdog_Ip_PBcfg.h are different"
#endif

/* Check if current file and Wdog_Ip_PBcfg header file are of the same software version */
#if ((WDOG_IP_SW_MAJOR_VERSION_PBCFG_C != WDOG_IP_SW_MAJOR_VERSION_PBCFG) || \
     (WDOG_IP_SW_MINOR_VERSION_PBCFG_C != WDOG_IP_SW_MINOR_VERSION_PBCFG) || \
     (WDOG_IP_SW_PATCH_VERSION_PBCFG_C != WDOG_IP_SW_PATCH_VERSION_PBCFG))
#error "Software Version Numbers of Wdog_Ip_PBcfg.c and Wdog_Ip_PBcfg.h are different"
#endif

/*==================================================================================================
*                           LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                          LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
*                                         LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                         LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                        GLOBAL VARIABLES
==================================================================================================*/

#define WDG_START_SEC_CONFIG_DATA_UNSPECIFIED

#include "Wdg_MemMap.h"


const Wdog_Ip_ConfigType Wdog_Ip_Cfg0 = 
{
    (Wdog_Ip_ClkSourceType)             WDOG_IP_LPO_CLOCK, /*.clkSource */
                        {
    (boolean)               FALSE,         /*.bWait */
    (boolean)               FALSE,         /*.bStop */
    (boolean)               FALSE          /*.bDebug */
                        },
    (boolean)           TRUE,              /*.UpdateEnable */
    (boolean)           FALSE,              /*.IntEnable */
    (boolean)           FALSE,              /*.WinEnable */
    (uint16)             0x00U,              /*.WindowValue */
    (uint16)            0xbb8U,              /*.TimeoutValue */
    (boolean)           TRUE,              /*.PrescalerEnable */
    NULL_PTR           /*.pfWdogCallback */
};


#define WDG_STOP_SEC_CONFIG_DATA_UNSPECIFIED

#include "Wdg_MemMap.h"

/*==================================================================================================
*                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                         LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                        GLOBAL FUNCTIONS
==================================================================================================*/

#ifdef __cplusplus
}
#endif

/** @} */

