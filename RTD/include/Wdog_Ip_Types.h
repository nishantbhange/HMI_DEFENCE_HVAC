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

#ifndef WDOG_IP_TYPES_H
#define WDOG_IP_TYPES_H

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

#include "Std_Types.h"
#include "Wdog_Ip_Cfg_Defines.h"
/*==================================================================================================
*                                 SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define WDOG_IP_TYPES_VENDOR_ID                    43
#define WDOG_IP_TYPES_MODULE_ID                    102
#define WDOG_IP_TYPES_AR_RELEASE_MAJOR_VERSION     4
#define WDOG_IP_TYPES_AR_RELEASE_MINOR_VERSION     7
#define WDOG_IP_TYPES_AR_RELEASE_REVISION_VERSION  0
#define WDOG_IP_TYPES_SW_MAJOR_VERSION             3
#define WDOG_IP_TYPES_SW_MINOR_VERSION             0
#define WDOG_IP_TYPES_SW_PATCH_VERSION             0

/*==================================================================================================
*                                       FILE VERSION CHECKS
==================================================================================================*/

/* Check if current file and Wdog_Ip_Cfg_Defines configuration header file are of the same vendor */
#if (WDOG_IP_TYPES_VENDOR_ID != WDOG_IP_CFG_DEFINES_VENDOR_ID)
#error "Wdog_Ip_Types.h and Wdog_Ip_Cfg_Defines.h have different vendor ids"
#endif

#if ((WDOG_IP_TYPES_AR_RELEASE_MAJOR_VERSION    != WDOG_IP_CFG_DEFINES_AR_RELEASE_MAJOR_VERSION) || \
     (WDOG_IP_TYPES_AR_RELEASE_MINOR_VERSION    != WDOG_IP_CFG_DEFINES_AR_RELEASE_MINOR_VERSION) || \
     (WDOG_IP_TYPES_AR_RELEASE_REVISION_VERSION != WDOG_IP_CFG_DEFINES_AR_RELEASE_REVISION_VERSION))
#error "AutoSar Version Numbers of Wdog_Ip_Types.h and Wdog_Ip_Cfg_Defines.h are different"
#endif

#if ((WDOG_IP_TYPES_SW_MAJOR_VERSION != WDOG_IP_CFG_DEFINES_SW_MAJOR_VERSION) || \
     (WDOG_IP_TYPES_SW_MINOR_VERSION != WDOG_IP_CFG_DEFINES_SW_MINOR_VERSION) || \
     (WDOG_IP_TYPES_SW_PATCH_VERSION != WDOG_IP_CFG_DEFINES_SW_PATCH_VERSION))
#error "Software Version Numbers of Wdog_Ip_Types.h and Wdog_Ip_Cfg_Defines.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Check if current file and SchM_Wdg header file are of the same Autosar version */
    #if ((WDOG_IP_TYPES_AR_RELEASE_MAJOR_VERSION    != STD_AR_RELEASE_MAJOR_VERSION) || \
         (WDOG_IP_TYPES_AR_RELEASE_MINOR_VERSION     != STD_AR_RELEASE_MINOR_VERSION) \
        )
    #error "AutoSar Version Numbers of Swt_Ip_Cfg_Defines.h and Std_Types.h are different"
    #endif
#endif /* DISABLE_MCAL_INTERMODULE_ASR_CHECK */

/*==================================================================================================
*                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
*                                              ENUMS
==================================================================================================*/

/** @brief       Enum defining the possible type values for WDOG API
* @Implements    Wdog_Ip_StatusType_enumeration
*/
typedef enum
{
    WDOG_IP_STATUS_SUCCESS = 0x00,
    WDOG_IP_STATUS_ERROR   = 0x01,
    WDOG_IP_STATUS_TIMEOUT = 0x02
} Wdog_Ip_StatusType;

/*!
 * @brief Clock sources for the WDOG.
 * Implements : Wdog_Ip_ClkSourceType_Class
 */
typedef enum
{
    WDOG_IP_BUS_CLOCK                        = 0x00U, /*!< Bus clock */
    WDOG_IP_LPO_CLOCK                        = 0x01U, /*!< LPO clock */
    WDOG_IP_SOSC_CLOCK                       = 0x02U, /*!< SOSC clock */
    WDOG_IP_SIRC_CLOCK                       = 0x03U  /*!< SIRC clock */
} Wdog_Ip_ClkSourceType;

#if (WDOG_IP_ENABLE_TEST_MODE == STD_ON)
/*!
 * @brief Test modes for the WDOG.
 * Implements : Wdog_Ip_TestModeType_Class
 */
typedef enum
{
    WDOG_IP_TST_DISABLED                     = 0x00U, /*!< Test mode disabled */
    WDOG_IP_TST_USER                         = 0x01U, /*!< User mode enabled. (Test mode disabled.) */
    WDOG_IP_TST_LOW                          = 0x02U, /*!< Test mode enabled, only the low byte is used. */
    WDOG_IP_TST_HIGH                         = 0x03U  /*!< Test mode enabled, only the high byte is used. */
} Wdog_Ip_TestModeType;
#endif /* WDOG_IP_ENABLE_TEST_MODE == STD_ON */

/*==================================================================================================
*                                  STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*!
 * @brief WDOG callback type
 * Implements : Wdog_Ip_CallbackPtrType_Class
 */

typedef void (*Wdog_Ip_CallbackPtrType)(void);

/*!
 * @brief WDOG option mode configuration structure
 * Implements : Wdog_Ip_OpModeType_Class
 */
typedef struct
{
    boolean bWait;  /*!< Wait mode */
    boolean bStop;  /*!< Stop mode */
    boolean bDebug; /*!< Debug mode */
} Wdog_Ip_OpModeType;

/*!
 * @brief WDOG user configuration structure
 * Implements : Wdog_Ip_ConfigType_Class
 */
typedef struct
{
    Wdog_Ip_ClkSourceType    clkSource;        /*!< The clock source of the WDOG */
    Wdog_Ip_OpModeType       opMode;           /*!< The modes in which the WDOG is functional */
    boolean                  UpdateEnable;    /*!< If true, further updates of the WDOG are enabled */
    boolean                  IntEnable;       /*!< If true, an interrupt request is generated before reset */
    boolean                  WinEnable;       /*!< If true, window mode is enabled */
    uint16                   WindowValue;   /*!< The window value */
    uint16                   TimeoutValue;  /*!< The timeout value */
    boolean                  PrescalerEnable; /*!< If true, a fixed 256 prescaling of the counter reference clock is enabled */
    Wdog_Ip_CallbackPtrType  pfWdogCallback;   /*!< Interrupt callback */
} Wdog_Ip_ConfigType;

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

#endif /* WDOG_IP_TYPES_H */
