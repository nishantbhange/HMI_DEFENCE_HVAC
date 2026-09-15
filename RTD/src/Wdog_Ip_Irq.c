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

#include "Mcal.h"
#include "Wdog_Ip.h"
#include "Wdog_Ip_Cfg_Defines.h"
#include "OsIf.h"

/*==================================================================================================
*                                 SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define WDOG_IP_IRQ_VENDOR_ID_C                      43
#define WDOG_IP_IRQ_AR_RELEASE_MAJOR_VERSION_C       4
#define WDOG_IP_IRQ_AR_RELEASE_MINOR_VERSION_C       7
#define WDOG_IP_IRQ_AR_RELEASE_REVISION_VERSION_C    0
#define WDOG_IP_IRQ_SW_MAJOR_VERSION_C               3
#define WDOG_IP_IRQ_SW_MINOR_VERSION_C               0
#define WDOG_IP_IRQ_SW_PATCH_VERSION_C               0

/*==================================================================================================
*                                       FILE VERSION CHECKS
==================================================================================================*/

/* Check if current file and Wdog_Ip_Cfg_Defines configuration header file are of the same vendor */
#if (WDOG_IP_IRQ_VENDOR_ID_C != WDOG_IP_CFG_DEFINES_VENDOR_ID)
    #error "Wdog_Ip_Irq.c and Wdog_Ip_Cfg_Defines.h have different vendor ids"
#endif

#if ((WDOG_IP_IRQ_AR_RELEASE_MAJOR_VERSION_C    != WDOG_IP_CFG_DEFINES_AR_RELEASE_MAJOR_VERSION) || \
     (WDOG_IP_IRQ_AR_RELEASE_MINOR_VERSION_C    != WDOG_IP_CFG_DEFINES_AR_RELEASE_MINOR_VERSION) || \
     (WDOG_IP_IRQ_AR_RELEASE_REVISION_VERSION_C != WDOG_IP_CFG_DEFINES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Wdog_Ip_Irq.c and Wdog_Ip_Cfg_Defines.h are different"
#endif

#if ((WDOG_IP_IRQ_SW_MAJOR_VERSION_C != WDOG_IP_CFG_DEFINES_SW_MAJOR_VERSION) || \
     (WDOG_IP_IRQ_SW_MINOR_VERSION_C != WDOG_IP_CFG_DEFINES_SW_MINOR_VERSION) || \
     (WDOG_IP_IRQ_SW_PATCH_VERSION_C != WDOG_IP_CFG_DEFINES_SW_PATCH_VERSION)    \
    )
    #error "Software Version Numbers of Wdog_Ip_Irq.c and Wdog_Ip_Cfg_Defines.h are different"
#endif

/* Check if current file and Wdog_Ip header file are of the same vendor */
#if (WDOG_IP_IRQ_VENDOR_ID_C != WDOG_IP_VENDOR_ID)
    #error "Wdog_Ip_Irq.c and Wdog_Ip.h have different vendor ids"
#endif

#if ((WDOG_IP_IRQ_AR_RELEASE_MAJOR_VERSION_C    != WDOG_IP_AR_RELEASE_MAJOR_VERSION) || \
     (WDOG_IP_IRQ_AR_RELEASE_MINOR_VERSION_C    != WDOG_IP_AR_RELEASE_MINOR_VERSION) || \
     (WDOG_IP_IRQ_AR_RELEASE_REVISION_VERSION_C != WDOG_IP_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Wdog_Ip_Irq.c and Wdog_Ip.h are different"
#endif

#if ((WDOG_IP_IRQ_SW_MAJOR_VERSION_C != WDOG_IP_SW_MAJOR_VERSION) || \
     (WDOG_IP_IRQ_SW_MINOR_VERSION_C != WDOG_IP_SW_MINOR_VERSION) || \
     (WDOG_IP_IRQ_SW_PATCH_VERSION_C != WDOG_IP_SW_PATCH_VERSION)    \
    )
    #error "Software Version Numbers of Wdog_Ip_Irq.c and Wdog_Ip.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Check if current file and Mcal header file are of the same Autosar version */
    #if ((WDOG_IP_IRQ_AR_RELEASE_MAJOR_VERSION_C    != MCAL_AR_RELEASE_MAJOR_VERSION) || \
         (WDOG_IP_IRQ_AR_RELEASE_MINOR_VERSION_C    != MCAL_AR_RELEASE_MINOR_VERSION)    \
        )
        #error "AutoSar Version Numbers of Wdog_Ip_Irq.c and Mcal.h are different"
    #endif

    /* Check if current file and Osif header file are of the same Autosar version */
    #if ((WDOG_IP_IRQ_AR_RELEASE_MAJOR_VERSION_C    != OSIF_AR_RELEASE_MAJOR_VERSION) || \
         (WDOG_IP_IRQ_AR_RELEASE_MINOR_VERSION_C    != OSIF_AR_RELEASE_MINOR_VERSION)    \
        )
        #error "AutoSar Version Numbers of Wdog_Ip_Irq.c and Osif.h are different"
    #endif
#endif /* DISABLE_MCAL_INTERMODULE_ASR_CHECK */

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

/*==================================================================================================
*                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                         LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                    GLOBAL FUNCTIONS PROTOTYPES
==================================================================================================*/
#if (WDOG_IP_ENABLE == STD_ON)
#ifdef WDG_ROM
    #if (WDG_ROM == 1U)
        #define WDG_START_SEC_CODE
    #endif
#else
    #if (WDG_RAM == 0U)
        #define WDG_START_SEC_RAMCODE
    #endif
#endif

#include "Wdg_MemMap.h"

#ifdef WDOG_IP_ISR0_USED
    #if (WDOG_IP_ISR0_USED == STD_ON)
        ISR(Wdog_Ip_Isr);
    #endif
#endif

/*==================================================================================================
*                                        GLOBAL FUNCTIONS
==================================================================================================*/

#ifdef WDOG_IP_ISR0_USED
#if (WDOG_IP_ISR0_USED == STD_ON)
/**
* @brief   Wdog handler for timeout interrupt.
* @details This function implements the ISR occurring on the WDOG timeout.
*
* @isr
*
*/
ISR(Wdog_Ip_Isr)
{
    Wdog_Ip_IrqHandler(0U);

    EXIT_INTERRUPT();
}
#endif
#endif

#ifdef WDG_ROM
    #if (WDG_ROM == 1U)
        #define WDG_STOP_SEC_CODE
    #endif
#else
    #if (WDG_RAM == 0U)
        #define WDG_STOP_SEC_RAMCODE
    #endif
#endif

#include "Wdg_MemMap.h"

#endif /* (WDOG_IP_ENABLE == STD_ON) */

#ifdef __cplusplus
}
#endif

/** @} */
