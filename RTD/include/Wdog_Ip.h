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

#ifndef WDOG_IP_H
#define WDOG_IP_H

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

#include "Wdog_Ip_Types.h"
#include "Wdog_Ip_DeviceRegisters.h"
#include "Wdog_Ip_Cfg.h"

/*==================================================================================================
*                                 SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define WDOG_IP_VENDOR_ID                    43
#define WDOG_IP_MODULE_ID                    102
#define WDOG_IP_AR_RELEASE_MAJOR_VERSION     4
#define WDOG_IP_AR_RELEASE_MINOR_VERSION     7
#define WDOG_IP_AR_RELEASE_REVISION_VERSION  0
#define WDOG_IP_SW_MAJOR_VERSION             3
#define WDOG_IP_SW_MINOR_VERSION             0
#define WDOG_IP_SW_PATCH_VERSION             0

/*==================================================================================================
*                                       FILE VERSION CHECKS
==================================================================================================*/

/* Check if current file and Wdog_Ip_Types header file are of the same vendor */
#if (WDOG_IP_VENDOR_ID != WDOG_IP_TYPES_VENDOR_ID)
#error "Wdog_Ip.h and Wdog_Ip_Types.h have different vendor ids"
#endif

#if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION    != WDOG_IP_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (WDOG_IP_AR_RELEASE_MINOR_VERSION    != WDOG_IP_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (WDOG_IP_AR_RELEASE_REVISION_VERSION != WDOG_IP_TYPES_AR_RELEASE_REVISION_VERSION))
#error "AutoSar Version Numbers of Wdog_Ip.h and Wdog_Ip_Types.h are different"
#endif

#if ((WDOG_IP_SW_MAJOR_VERSION != WDOG_IP_TYPES_SW_MAJOR_VERSION) || \
     (WDOG_IP_SW_MINOR_VERSION != WDOG_IP_TYPES_SW_MINOR_VERSION) || \
     (WDOG_IP_SW_PATCH_VERSION != WDOG_IP_TYPES_SW_PATCH_VERSION))
#error "Software Version Numbers of Wdog_Ip.h and Wdog_Ip_Types.h are different"
#endif

/* Check if current file and Wdog_Ip_DeviceRegisters header file are of the same vendor */
#if (WDOG_IP_VENDOR_ID != WDOG_IP_DEVICE_REGISTERS_VENDOR_ID)
#error "Wdog_Ip.h and Wdog_Ip_DeviceRegisters.h have different vendor ids"
#endif

#if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION    != WDOG_IP_DEVICE_REGISTERS_AR_RELEASE_MAJOR_VERSION) || \
     (WDOG_IP_AR_RELEASE_MINOR_VERSION    != WDOG_IP_DEVICE_REGISTERS_AR_RELEASE_MINOR_VERSION) || \
     (WDOG_IP_AR_RELEASE_REVISION_VERSION != WDOG_IP_DEVICE_REGISTERS_AR_RELEASE_REVISION_VERSION))
#error "AutoSar Version Numbers of Wdog_Ip.h and Wdog_Ip_DeviceRegisters.h are different"
#endif

#if ((WDOG_IP_SW_MAJOR_VERSION != WDOG_IP_DEVICE_REGISTERS_SW_MAJOR_VERSION) || \
     (WDOG_IP_SW_MINOR_VERSION != WDOG_IP_DEVICE_REGISTERS_SW_MINOR_VERSION) || \
     (WDOG_IP_SW_PATCH_VERSION != WDOG_IP_DEVICE_REGISTERS_SW_PATCH_VERSION))
#error "Software Version Numbers of Wdog_Ip.h and Wdog_Ip_DeviceRegisters.h are different"
#endif

/* Check if current file and Wdog_Ip_Cfg header file are of the same vendor */
#if (WDOG_IP_VENDOR_ID != WDOG_IP_VENDOR_ID_CFG_H)
#error "Wdog_Ip.h and Wdog_Ip_Cfg.h have different vendor ids"
#endif

#if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION    != WDOG_IP_AR_RELEASE_MAJOR_VERSION_CFG_H) || \
     (WDOG_IP_AR_RELEASE_MINOR_VERSION    != WDOG_IP_AR_RELEASE_MINOR_VERSION_CFG_H) || \
     (WDOG_IP_AR_RELEASE_REVISION_VERSION != WDOG_IP_AR_RELEASE_REVISION_VERSION_CFG_H) \
    )
#error "AutoSar Version Numbers of Wdog_Ip.h and Wdog_Ip_Cfg.h are different"
#endif

#if ((WDOG_IP_SW_MAJOR_VERSION != WDOG_IP_SW_MAJOR_VERSION_CFG_H) || \
     (WDOG_IP_SW_MINOR_VERSION != WDOG_IP_SW_MINOR_VERSION_CFG_H) || \
     (WDOG_IP_SW_PATCH_VERSION != WDOG_IP_SW_PATCH_VERSION_CFG_H) \
    )
#error "Software Version Numbers of Wdog_Ip.h and Wdog_Ip_Cfg.h are different"
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

void Wdog_Ip_IrqHandler(uint8 Instance);

/*!
 * @brief Initializes the WDOG driver.
 *
 * @param[in] Instance WDOG peripheral instance number
 * @param[in] ConfigPtr pointer to the WDOG user configuration structure
 * @return operation status
 *        - WDOG_IP_STATUS_SUCCESS: WDOG initialization was successful.
 *        - WDOG_IP_STATUS_ERROR: Operation failed. Possible causes: WDOG configuration updates are not allowed;
 *          If window mode enabled and window value greater than or equal to the timeout value.
 *        - WDOG_IP_STATUS_TIMEOUT: The unlock operation was unsuccesful and has timed out.
 */
Wdog_Ip_StatusType Wdog_Ip_Init(const uint8 Instance,
                            const Wdog_Ip_ConfigType * const ConfigPtr);

#if (WDOG_IP_DEINIT == STD_ON)
/*!
 * @brief De-initializes the WDOG driver
 *
 * @param[in] Instance  WDOG peripheral instance number
 * @return operation status
 *        - WDOG_IP_STATUS_SUCCESS: If the WDOG module is de-initialized successfulully.
 *        - WDOG_IP_STATUS_ERROR: Operation failed. Possible causes: WDOG updates are not allowed.
 *        - WDOG_IP_STATUS_TIMEOUT: The unlock operation was unsuccesful and has timed out.
 */
Wdog_Ip_StatusType Wdog_Ip_DeInit(const uint8 Instance);
#endif

/*!
 * @brief Refreshes the WDOG counter.
 *
 * @param[in] Instance WDOG peripheral instance number
 */
void Wdog_Ip_Service(const uint8 Instance);

/*!
 * @brief Sets the value of the WDOG timeout.
 *
 * This function sets the value of the WDOG timeout and enables the window mode if WindowValue is greater than 0.
 *
 * @param[in] Instance WDOG peripheral instance number.
 * @param[in] Timeout the value of the WDOG timeout.
 * @param[in] WindowValue the value of the WDOG window.
 * @return operation status
 *        - WDOG_IP_STATUS_SUCCESS: The WDOG timeout and window are configured succesfully.
 *        - WDOG_IP_STATUS_ERROR: Operation failed. Possible causes: WDOG updates are not allowed.
 *        - WDOG_IP_STATUS_TIMEOUT: The unlock operation was unsuccesful and has timed out.
 */
Wdog_Ip_StatusType Wdog_Ip_SetTimeout(const uint8 Instance,
                            uint16 Timeout, uint16 WindowValue);

#if (WDOG_IP_ENABLE_TEST_MODE == STD_ON)
/*!
 * @brief Changes the WDOG test mode.
 *
 * This function changes the test mode of the WDOG. If the WDOG is tested in
 * mode, software should set this field to 0x01U in order to indicate that the
 * WDOG is functioning normally.
 *
 * @param[in] Instance WDOG peripheral instance number
 * @param[in] testMode Test modes for the WDOG.
 * @return operation status
 *        - WDOG_IP_STATUS_SUCCESS: The WDOG test mode is configured succesfully.
 *        - WDOG_IP_STATUS_ERROR: Operation failed. Possible causes: WDOG updates are not allowed.
 *        - WDOG_IP_STATUS_TIMEOUT: The unlock operation was unsuccesful and has timed out.
 */
Wdog_Ip_StatusType Wdog_Ip_SetTestMode(const uint8 Instance,
                            Wdog_Ip_TestModeType TestMode);

/*!
 * @brief Returns the current WDOG test mode.
 *
 * This function returns the test mode of the WDOG.
 *
 * @param[in] Instance WDOG peripheral instance number
 * @return Wdog_Ip_TestModeType the current WDOG test mode.
 */
Wdog_Ip_TestModeType Wdog_Ip_GetTestMode(const uint8 Instance);
#endif /* WDOG_IP_ENABLE_TEST_MODE == STD_ON */

/*!
 * @brief Configures the WDOG driver, but does not enable or disable it.
 *
 * @param[in] Instance WDOG peripheral instance number
 * @param[in] ConfigPtr pointer to the WDOG user configuration structure
 * @return operation status
 *        - WDOG_IP_STATUS_SUCCESS: WDOG configuration was successful.
 *        - WDOG_IP_STATUS_ERROR: Operation failed. Possible causes: WDOG configuration updates are not allowed;
 *          If window mode enabled and window value greater than or equal to the timeout value.
 *        - WDOG_IP_STATUS_TIMEOUT: The unlock operation was unsuccesful and has timed out.
 */
Wdog_Ip_StatusType Wdog_Ip_Config(const uint8 Instance,
                            const Wdog_Ip_ConfigType * const ConfigPtr);

/*!
 * @brief Starts the WDOG counter.
 *
 * @param[in] Instance WDOG peripheral instance number
 * @return operation status
 *        - WDOG_IP_STATUS_SUCCESS: WDOG timer was started successfully.
 *        - WDOG_IP_STATUS_ERROR: Operation failed. Possible causes: WDOG updates are not allowed.
 *        - WDOG_IP_STATUS_TIMEOUT: The unlock operation was unsuccesful and has timed out.
 */
Wdog_Ip_StatusType Wdog_Ip_StartTimer(const uint8 Instance);

/*!
 * @brief Stops the WDOG counter.
 *
 * @param[in] Instance WDOG peripheral instance number
 * @return operation status
 *        - WDOG_IP_STATUS_SUCCESS: WDOG timer was stopped successfully.
 *        - WDOG_IP_STATUS_ERROR: Operation failed. Possible causes: WDOG updates are not allowed.
 *        - WDOG_IP_STATUS_TIMEOUT: The unlock operation was unsuccesful and has timed out.
 */
Wdog_Ip_StatusType Wdog_Ip_StopTimer(const uint8 Instance);

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

#endif /* WDOG_IP_H */
