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
*   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly
*   in accordance with the applicable license terms.  By expressly accepting
*   such terms or by downloading, installing, activating and/or otherwise using
*   the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms.  If you do not agree to
*   be bound by the applicable license terms, then you may not retain,
*   install, activate or otherwise use the software.
==================================================================================================*/

#ifndef WDOG_IP_CFG_H
#define WDOG_IP_CFG_H

/**
*   @file
*
*   @addtogroup WDOG_ip_config WDOG IPL Configuration
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
*                                          INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Std_Types.h"
#include "Wdog_Ip_PBcfg.h"

/*==================================================================================================
*                                 SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define WDOG_IP_VENDOR_ID_CFG_H                    43
#define WDOG_IP_AR_RELEASE_MAJOR_VERSION_CFG_H     4
#define WDOG_IP_AR_RELEASE_MINOR_VERSION_CFG_H     7
#define WDOG_IP_AR_RELEASE_REVISION_VERSION_CFG_H  0
#define WDOG_IP_SW_MAJOR_VERSION_CFG_H             3
#define WDOG_IP_SW_MINOR_VERSION_CFG_H             0
#define WDOG_IP_SW_PATCH_VERSION_CFG_H             0

/*==================================================================================================
*                                       FILE VERSION CHECKS
==================================================================================================*/
#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Checks against Std_Types.h */
    #if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION_CFG_H != STD_AR_RELEASE_MAJOR_VERSION) || \
         (WDOG_IP_AR_RELEASE_MINOR_VERSION_CFG_H != STD_AR_RELEASE_MINOR_VERSION) \
        )
        #error "AutoSar Version Numbers of Wdog_Ip_Cfg.h and Std_Types.h are different"
    #endif
#endif
/* Check if current file and Wdog_Ip_PBcfg.h file are of the same vendor */
#if (WDOG_IP_VENDOR_ID_CFG_H != WDOG_IP_VENDOR_ID_PBCFG)
    #error "Wdog_Ip_Cfg.h and Wdog_Ip_PBcfg.h have different vendor IDs"
#endif

/* Check if current file and Wdog_Ip_PBcfg.h file are of the same Autosar version */
#if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION_CFG_H != WDOG_IP_AR_RELEASE_MAJOR_VERSION_PBCFG) || \
     (WDOG_IP_AR_RELEASE_MINOR_VERSION_CFG_H != WDOG_IP_AR_RELEASE_MINOR_VERSION_PBCFG) || \
     (WDOG_IP_AR_RELEASE_REVISION_VERSION_CFG_H != WDOG_IP_AR_RELEASE_REVISION_VERSION_PBCFG) \
    )
    #error "AutoSar Version Numbers of Wdog_Ip_Cfg.h and Wdog_Ip_PBcfg.h are different"
#endif

/* Check if current file and Wdog_Ip_PBcfg.h file are of the same software version */
#if ((WDOG_IP_SW_MAJOR_VERSION_CFG_H != WDOG_IP_SW_MAJOR_VERSION_PBCFG) || \
     (WDOG_IP_SW_MINOR_VERSION_CFG_H != WDOG_IP_SW_MINOR_VERSION_PBCFG) || \
     (WDOG_IP_SW_PATCH_VERSION_CFG_H != WDOG_IP_SW_PATCH_VERSION_PBCFG) \
    )
    #error "Software Version Numbers of Wdog_Ip_Cfg.h and Wdog_Ip_PBcfg.h are different"
#endif
/*==================================================================================================
*                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
*                                              ENUMS
==================================================================================================*/

/*==================================================================================================
*                                  STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                  GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
*                                       FUNCTION PROTOTYPES
==================================================================================================*/


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* WDOG_IP_CFG_H */


