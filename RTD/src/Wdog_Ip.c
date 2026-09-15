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
#include "Wdog_Ip.h"
#include "Devassert.h"
#include "OsIf.h"
#include "Mcal.h"
#include "SchM_Wdg.h"

/*==================================================================================================
*                                 SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define WDOG_IP_VENDOR_ID_C                      43
#define WDOG_IP_AR_RELEASE_MAJOR_VERSION_C       4
#define WDOG_IP_AR_RELEASE_MINOR_VERSION_C       7
#define WDOG_IP_AR_RELEASE_REVISION_VERSION_C    0
#define WDOG_IP_SW_MAJOR_VERSION_C               3
#define WDOG_IP_SW_MINOR_VERSION_C               0
#define WDOG_IP_SW_PATCH_VERSION_C               0

/*==================================================================================================
*                                       FILE VERSION CHECKS
==================================================================================================*/

/* Check if current file and Wdog_Ip header file are of the same vendor */
#if (WDOG_IP_VENDOR_ID_C != WDOG_IP_VENDOR_ID)
#error "Wdog_Ip.c and Wdog_Ip.h have different vendor ids"
#endif

/* Check if current file and Wdog_Ip header file are of the same Autosar version */
#if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION_C     != WDOG_IP_AR_RELEASE_MAJOR_VERSION) || \
     (WDOG_IP_AR_RELEASE_MINOR_VERSION_C     != WDOG_IP_AR_RELEASE_MINOR_VERSION) || \
     (WDOG_IP_AR_RELEASE_REVISION_VERSION_C  != WDOG_IP_AR_RELEASE_REVISION_VERSION))
#error "AutoSar Version Numbers of Wdog_Ip.c and Wdog_Ip.h are different"
#endif

/* Check if current file and Wdog_Ip header file are of the same software version */
#if ((WDOG_IP_SW_MAJOR_VERSION_C != WDOG_IP_SW_MAJOR_VERSION) || \
     (WDOG_IP_SW_MINOR_VERSION_C != WDOG_IP_SW_MINOR_VERSION) || \
     (WDOG_IP_SW_PATCH_VERSION_C != WDOG_IP_SW_PATCH_VERSION))
#error "Software Version Numbers of Wdog_Ip.c and Wdog_Ip.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Check if current file and Devassert header file are of the same Autosar version */
    #if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION_C    != DEVASSERT_AR_RELEASE_MAJOR_VERSION) || \
        (WDOG_IP_AR_RELEASE_MINOR_VERSION_C     != DEVASSERT_AR_RELEASE_MINOR_VERSION))
    #error "AutoSar Version Numbers of Wdog_Ip.c and Devassert.h are different"
    #endif

    /* Check if current file and OsIf header file are of the same Autosar version */
    #if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION_C    != OSIF_AR_RELEASE_MAJOR_VERSION) || \
        (WDOG_IP_AR_RELEASE_MINOR_VERSION_C     != OSIF_AR_RELEASE_MINOR_VERSION))
    #error "AutoSar Version Numbers of Wdog_Ip.c and OsIf.h are different"
    #endif

    /* Check if current file and Mcal header file are of the same Autosar version */
    #if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION_C    != MCAL_AR_RELEASE_MAJOR_VERSION) || \
        (WDOG_IP_AR_RELEASE_MINOR_VERSION_C     != MCAL_AR_RELEASE_MINOR_VERSION))
    #error "AutoSar Version Numbers of Wdog_Ip.c and Mcal.h are different"
    #endif

    /* Check if current file and Schm_Wdg header file are of the same Autosar version */
    #if ((WDOG_IP_AR_RELEASE_MAJOR_VERSION_C    != SCHM_WDG_AR_RELEASE_MAJOR_VERSION) || \
        (WDOG_IP_AR_RELEASE_MINOR_VERSION_C     != SCHM_WDG_AR_RELEASE_MINOR_VERSION))
    #error "AutoSar Version Numbers of Wdog_Ip.c and SchM_Wdg.h are different"
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

#if (WDOG_IP_ENABLE == STD_ON)

#define WDG_START_SEC_CONST_UNSPECIFIED

#include "Wdg_MemMap.h"

/*! @brief Table of Base addresses for WDOG instances. */
static WDOG_Type * const Wdog_Ip_sBase[] = IP_WDOG_BASE_PTRS;

#define WDG_STOP_SEC_CONST_UNSPECIFIED

#include "Wdg_MemMap.h"

#define WDG_START_SEC_VAR_CLEARED_UNSPECIFIED_NO_CACHEABLE

#include "Wdg_MemMap.h"

static Wdog_Ip_CallbackPtrType Wdog_Ip_apCallbackPtr[WDOG_INSTANCE_COUNT];

#define WDG_STOP_SEC_VAR_CLEARED_UNSPECIFIED_NO_CACHEABLE

#include "Wdg_MemMap.h"

#define WDG_START_SEC_VAR_CLEARED_BOOLEAN_NO_CACHEABLE

#include "Wdg_MemMap.h"

static boolean Wdog_Ip_abStatus[WDOG_INSTANCE_COUNT];

#define WDG_STOP_SEC_VAR_CLEARED_BOOLEAN_NO_CACHEABLE

#include "Wdg_MemMap.h"

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

/*!
 * @brief Verifies if the WDOG is enabled.
 *
 * This function verifies the state of the WDOG.
 *
 * @param[in] Base WDOG Base pointer.
 * @return the state of the WDOG.
 */
static inline boolean Wdog_Ip_IsEnabled(const WDOG_Type * Base)
{
    return ((Base->CS & WDOG_CS_EN_MASK) >> WDOG_CS_EN_SHIFT) != 0U;
}

/*!
 * @brief Refreshes the WDOG counter
 *
 * @param[in] Base WDOG Base pointer.
 */
static inline void Wdog_Ip_Trigger(WDOG_Type * const Base)
{
    OsIf_SuspendAllInterrupts();

    if ((Base->CS & WDOG_CS_CMD32EN_MASK) != 0U)
    {
        Base->CNT = WDOG_IP_FEATURE_TRIGGER_VALUE_U32;
    }
    else
    {
        Base->CNT = WDOG_IP_FEATURE_TRIGGER16_FIRST_VALUE_U16;
        (void)Base->CNT;
        Base->CNT = WDOG_IP_FEATURE_TRIGGER16_SECOND_VALUE_U16;
    }

    OsIf_ResumeAllInterrupts();
}

/*!
 * @brief Gets the Interrupt Flag.
 *
 * This function gets the Interrupt Flag (FLG).
 *
 * @param[in] Base WDOG Base pointer.
 */
static inline boolean Wdog_Ip_GetIntFlag(const WDOG_Type * const Base)
{
    return ((Base->CS & WDOG_CS_FLG_MASK) >> WDOG_CS_FLG_SHIFT) != 0U;
}

/*!
 * @brief Gets the Interrupt Enable status.
 *
 * This function gets the Interrupt Enable bit status (INT).
 *
 * @param[in] Base WDOG Base pointer.
 */
static inline boolean Wdog_Ip_IsIntEnabled(const WDOG_Type * const Base)
{
    return ((Base->CS & WDOG_CS_INT_MASK) >> WDOG_CS_INT_SHIFT) != 0U;
}

/*!
 * @brief Write the WDOG unlock sequence.
 *
 * This function checks writes the WDOG unlock sequence
 *
 * @param[in] Base WDOG Base pointer.
 */
static inline void Wdog_Ip_UnlockCmd(WDOG_Type * const Base)
{
    if ((Base->CS & WDOG_CS_CMD32EN_MASK) != 0U)
    {
        Base->CNT = WDOG_IP_FEATURE_UNLOCK_VALUE_U32;
        (void)Base->CNT;
    }
    else
    {
        Base->CNT = WDOG_IP_FEATURE_UNLOCK16_FIRST_VALUE_U16;
        (void)Base->CNT;
        Base->CNT = WDOG_IP_FEATURE_UNLOCK16_SECOND_VALUE_U16;
    }
}

/*!
 * @brief Unlocks the WDOG module.
 *
 * This function unlocks the WDOG. If the unlock is unsuccessfull,
 * the status WDOG_IP_STATUS_TIMEOUT is returned.
 *
 * @param[in] Base WDOG Base pointer.
 * @return TRUE if the module is unlocked, FALSE if the module is locked.
 */
static Wdog_Ip_StatusType Wdog_Ip_Unlock(WDOG_Type * const Base)
{
    Wdog_Ip_StatusType Status = WDOG_IP_STATUS_SUCCESS;

    uint32 TimeoutTicks = OsIf_MicrosToTicks(WDOG_IP_OSIF_TIMEOUT_VAL, WDOG_IP_TIMEOUT_METHOD);
    uint32 CurrentTicks = OsIf_GetCounter(WDOG_IP_TIMEOUT_METHOD); /* initialize current counter */
    uint32 ElapsedTicks = 0u; /* elapsed will give timeout */

    Wdog_Ip_UnlockCmd(Base);

    /*Waiting for registers to be unlocked (ULK == 1) */
    while ((((Base->CS & WDOG_CS_ULK_MASK) >> WDOG_CS_ULK_SHIFT) == 0U) && (ElapsedTicks < TimeoutTicks))
    {
        ElapsedTicks += OsIf_GetElapsed(&CurrentTicks, WDOG_IP_TIMEOUT_METHOD);
    }

    if (ElapsedTicks >= TimeoutTicks)
    {
        Status = WDOG_IP_STATUS_TIMEOUT;
    }

    return Status;
}

/*!
 * @brief Clears the Interrupt Flag.
 *
 * This function clears the Interrupt Flag (FLG).
 *
 * @param[in] Base WDOG Base pointer.
 */
static inline void Wdog_Ip_ClearIntFlag(WDOG_Type * const Base)
{
    if (WDOG_IP_STATUS_SUCCESS == Wdog_Ip_Unlock(Base))
    {
        Base->CS = (Base->CS & ~WDOG_CS_FLG_MASK) | WDOG_CS_FLG(1U);

#ifdef ERRATA_E9005
        /* Read-after-write sequence to guarantee required serialization of memory operations */
        (void)Base->CS;
#endif
    }
}

/*!
 * @brief Verifies if the WDOG updates are allowed.
 *
 * This function verifies if software is allowed to reconfigure the WDOG without
 * a reset.
 *
 * @param[in] Base WDOG Base pointer.
 * @return the state of the WDOG updates:
 *         - FALSE: updates are not allowed
 *         - TRUE: updates are allowed
 */
static inline boolean Wdog_Ip_IsUpdateEnabled(const WDOG_Type * Base)
{
    return ((Base->CS & WDOG_CS_UPDATE_MASK) >> WDOG_CS_UPDATE_SHIFT) != 0U;
}

/*!
 * @brief Checks if the last configuration of the WDOG was successful.
 *
 * This function checks if the last configuration of the WDOG was successful.
 *
 * @param[in] Base WDOG Base pointer.
 * @return WDOG_IP_STATUS_SUCCESS if the configuration was successful, WDOG_IP_STATUS_TIMEOUT if not.
 */
static Wdog_Ip_StatusType Wdog_Ip_IsReconfigurationComplete(const WDOG_Type * Base)
{
    Wdog_Ip_StatusType Status = WDOG_IP_STATUS_SUCCESS;

    uint32 TimeoutTicks = OsIf_MicrosToTicks(WDOG_IP_OSIF_TIMEOUT_VAL, WDOG_IP_TIMEOUT_METHOD);
    uint32 CurrentTicks = OsIf_GetCounter(WDOG_IP_TIMEOUT_METHOD); /* initialize current counter */
    uint32 ElapsedTicks = 0u; /* elapsed will give timeout */

    /* Waiting for the reconfiguration is successful (RCS==1) */
    while ((((Base->CS & WDOG_CS_RCS_MASK) >> WDOG_CS_RCS_SHIFT) == 0U) && (ElapsedTicks < TimeoutTicks))
    {
        ElapsedTicks += OsIf_GetElapsed(&CurrentTicks, WDOG_IP_TIMEOUT_METHOD);
    }

    if (ElapsedTicks >= TimeoutTicks)
    {
        Status = WDOG_IP_STATUS_TIMEOUT;
    }

    return Status;
}

#if (WDOG_IP_DEINIT == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_HwDeInit
 * Description   : De-init WDOG module.
 *
 *END**************************************************************************/
static void Wdog_Ip_HwDeInit(WDOG_Type * const Base)
{
    /* Disable WDOG, enables support for 32-bit refresh/unlock command, LPO clock source,
       allow updates and disable watchdog interrupts, window mode, wait/debug/stop mode */
    Base->CS    = (WDOG_IP_FEATURE_CS_RESET_VALUE_U16 & (~WDOG_CS_EN_MASK)) | (WDOG_CS_UPDATE_MASK); /* make sure the WDOG is disabled and updated allowed after reset */
    /* Default timeout value */
    Base->TOVAL = WDOG_IP_FEATURE_TO_RESET_VALUE_U16;
    /* Clear window value */
    Base->WIN   = WDOG_IP_FEATURE_WIN_RESET_VALUE_U16;

    /* Refresh counter value */
    Wdog_Ip_Trigger(Base);
}
#endif /* (WDOG_IP_DEINIT == STD_ON) */

/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_HwConfig
 * Description   : Configures the WDOG_CS register.
 *
 *END**************************************************************************/
static Wdog_Ip_StatusType Wdog_Ip_HwConfig(const Wdog_Ip_ConfigType * ConfigPtr, uint32 * ValueU32Cs)
{
    Wdog_Ip_StatusType Status = WDOG_IP_STATUS_SUCCESS;

    /* If window mode enabled and window value greater than or equal to the timeout value. Or timeout value is 0 */
    if (((ConfigPtr->WinEnable) && (ConfigPtr->WindowValue >= ConfigPtr->TimeoutValue)) \
        || (ConfigPtr->TimeoutValue <= WDOG_IP_FEATURE_MINIMUM_TIMEOUT_VALUE_U16))
    {
        Status = WDOG_IP_STATUS_ERROR;
    }
    else
    {
        /* Clear the bits used for configuration */
        *ValueU32Cs &= ~(WDOG_CS_WIN_MASK | WDOG_CS_PRES_MASK | WDOG_CS_CLK_MASK | WDOG_CS_INT_MASK |
                WDOG_CS_UPDATE_MASK | WDOG_CS_DBG_MASK | WDOG_CS_WAIT_MASK | WDOG_CS_STOP_MASK);
        /* Construct CS register new value */
        *ValueU32Cs |=  WDOG_CS_CLK(ConfigPtr->clkSource);
        *ValueU32Cs |=  WDOG_CS_PRES(ConfigPtr->PrescalerEnable ? 1UL : 0UL);
        *ValueU32Cs |=  WDOG_CS_INT(ConfigPtr->IntEnable ? 1UL : 0UL);
        *ValueU32Cs |=  WDOG_CS_UPDATE(ConfigPtr->UpdateEnable ? 1UL : 0UL);
        *ValueU32Cs |=  WDOG_CS_DBG(ConfigPtr->opMode.bDebug ? 1UL : 0UL);
        *ValueU32Cs |=  WDOG_CS_WAIT(ConfigPtr->opMode.bWait ? 1UL : 0UL);
        *ValueU32Cs |=  WDOG_CS_STOP(ConfigPtr->opMode.bStop ? 1UL : 0UL);

        /* Reset interrupt flags */
        *ValueU32Cs |=  WDOG_CS_FLG_MASK;
    }

    return Status;
}

#if (WDOG_IP_ENABLE_TEST_MODE == STD_ON)
/*!
 * @brief Read the current WDOG test mode.
 *
 * This function reads the test mode setting of the WDOG from the CS register.
 *
 * @param[in] Base WDOG Base pointer.
 * @return uint32 value of the CS_TST setting.
 */
static inline uint32 Wdog_Ip_ReadTestMode(const WDOG_Type * Base)
{
    return ((Base->CS & WDOG_CS_TST_MASK) >> WDOG_CS_TST_SHIFT);
}
#endif /* WDOG_IP_ENABLE_TEST_MODE == STD_ON */

/*==================================================================================================
*                                 GLOBAL FUNCTIONS PROTOTYPES
==================================================================================================*/


/*==================================================================================================
*                                        GLOBAL FUNCTIONS
==================================================================================================*/

/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_Init
 * Description   : initialize the WDOG driver
 *
 * @implements     Wdog_Ip_Init_Activity
 *END**************************************************************************/
Wdog_Ip_StatusType Wdog_Ip_Init(const uint8 Instance,
                       const Wdog_Ip_ConfigType * const ConfigPtr)
{
    WDOG_Type * Base = Wdog_Ip_sBase[Instance];

#if (WDOG_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(Instance < WDOG_INSTANCE_COUNT);
    DevAssert(ConfigPtr != NULL_PTR);
    DevAssert(!Wdog_Ip_IsEnabled(Base));
#endif

    Wdog_Ip_StatusType Status = WDOG_IP_STATUS_SUCCESS;
    uint32 TempCs = 0x00U;

#ifdef MCAL_ENABLE_FAULT_INJECTION
    MCAL_FAULT_INJECTION_POINT(WDOG_FIP_INIT);
#endif

    if (!Wdog_Ip_IsUpdateEnabled(Base))
    {
        Status = WDOG_IP_STATUS_ERROR;
    }
    else
    {
        /* Configure the WDOG module */
        Status = Wdog_Ip_HwConfig(ConfigPtr, &TempCs);
    }

    if (WDOG_IP_STATUS_SUCCESS == Status)
    {
        /* Enable WDOG in 32-bit mode */
        TempCs |=  WDOG_CS_EN_MASK | WDOG_CS_CMD32EN_MASK;

        /*************************************************************************************
           OsIf_SuspendAllInterrupts() and OsIf_ResumeAllInterrupts() are used to
           guard uinterruptible code sequences of unlocking the WDOG and writing to
           the WDOG registers. When programming target is flash, the Suspend/Resume interrupts
           sequence takes too long and the WDOG locks before writing to the registers, so
           care must be taken to guard all time-critical code, not just the unlock sequence.
        **************************************************************************************/
        OsIf_SuspendAllInterrupts();

        if (WDOG_IP_STATUS_SUCCESS == Wdog_Ip_Unlock(Base))
        {
            Base->TOVAL = ConfigPtr->TimeoutValue;

            if (ConfigPtr->WinEnable)
            {
                TempCs |=  WDOG_CS_WIN_MASK;

                Base->WIN = ConfigPtr->WindowValue;
            }

            /* Write CS value to register */
            Base->CS = TempCs;

            if (TRUE == ConfigPtr->IntEnable)
            {
                Wdog_Ip_apCallbackPtr[Instance] = ConfigPtr->pfWdogCallback;
            }
            else
            {
                Wdog_Ip_apCallbackPtr[Instance] = NULL_PTR;
            }

            Status = Wdog_Ip_IsReconfigurationComplete(Base);
        }
        else
        {
            Status = WDOG_IP_STATUS_TIMEOUT;
        }
        OsIf_ResumeAllInterrupts();
    }

    if (WDOG_IP_STATUS_SUCCESS == Status)
    {
        /* Set driver status is initialized */
        Wdog_Ip_abStatus[Instance] = TRUE;
    }
    return Status;
}
#if (WDOG_IP_DEINIT == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_DeInit
 * Description   : De-initialize the WDOG driver
 *
 * @implements     Wdog_Ip_DeInit_Activity
 *END**************************************************************************/
Wdog_Ip_StatusType Wdog_Ip_DeInit(const uint8 Instance)
{
#if (WDOG_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(Instance < WDOG_INSTANCE_COUNT);
#endif

    WDOG_Type * Base = Wdog_Ip_sBase[Instance];
    Wdog_Ip_StatusType Status = WDOG_IP_STATUS_SUCCESS;

#ifdef MCAL_ENABLE_FAULT_INJECTION
    MCAL_FAULT_INJECTION_POINT(WDOG_FIP_DEINIT);
#endif

    /* If allowed reconfigures WDOG */
    if (Wdog_Ip_IsUpdateEnabled(Base))
    {
        OsIf_SuspendAllInterrupts();

        if (WDOG_IP_STATUS_SUCCESS == Wdog_Ip_Unlock(Base))
        {
            /* Disable WDOG */
            Wdog_Ip_HwDeInit(Base);

            Wdog_Ip_apCallbackPtr[Instance] = NULL_PTR;

            Status = Wdog_Ip_IsReconfigurationComplete(Base);
        }
        else
        {
            Status = WDOG_IP_STATUS_TIMEOUT;
        }

        OsIf_ResumeAllInterrupts();
    }
    else
    {
        Status = WDOG_IP_STATUS_ERROR;
    }

    if (WDOG_IP_STATUS_SUCCESS == Status)
    {
        /* Set driver status is uninit */
        Wdog_Ip_abStatus[Instance] = FALSE;
    }

    return Status;
}
#endif/* (WDOG_IP_DEINIT == STD_ON) */

/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_Service
 * Description   : Refreshes the WDOG counter
 *
 * @implements     Wdog_Ip_Service_Activity
 *END**************************************************************************/
void Wdog_Ip_Service(const uint8 Instance)
{
#if (WDOG_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(Instance < WDOG_INSTANCE_COUNT);
#endif

    WDOG_Type * Base = Wdog_Ip_sBase[Instance];

    Wdog_Ip_Trigger(Base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_SetTimeout
 * Description   : Set time value of the WDOG timeout.
 *
 * @implements     Wdog_Ip_SetTimeout_Activity
 *END**************************************************************************/
Wdog_Ip_StatusType Wdog_Ip_SetTimeout(const uint8 Instance,
                             uint16 Timeout, uint16 WindowValue)
{
#if (WDOG_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(Instance < WDOG_INSTANCE_COUNT);
#endif

    WDOG_Type * Base = Wdog_Ip_sBase[Instance];
    Wdog_Ip_StatusType Status = WDOG_IP_STATUS_SUCCESS;
    boolean windowEnable = (WindowValue > (uint16)0) ? (boolean)1U : (boolean)0U;

#ifdef MCAL_ENABLE_FAULT_INJECTION
    MCAL_FAULT_INJECTION_POINT(WDOG_FIP_SETTIMEOUT);
#endif

    /* If allowed reconfigures WDOG */
    if (Wdog_Ip_IsUpdateEnabled(Base))
    {
        OsIf_SuspendAllInterrupts();

        if (WDOG_IP_STATUS_SUCCESS == Wdog_Ip_Unlock(Base))
        {
            Base->TOVAL = Timeout;

            /* If enable window mode */
            if (windowEnable)
            {
                /* Set window value for the WDOG */
                Base->WIN = WindowValue;
            }

            Base->CS = (Base->CS & ~WDOG_CS_WIN_MASK) | WDOG_CS_WIN(windowEnable ? 1UL : 0UL);

            Status = Wdog_Ip_IsReconfigurationComplete(Base);
        }
        else
        {
            Status = WDOG_IP_STATUS_TIMEOUT;
        }

        OsIf_ResumeAllInterrupts();
    }
    else
    {
        Status = WDOG_IP_STATUS_ERROR;
    }

    return Status;
}

#if (WDOG_IP_ENABLE_TEST_MODE == STD_ON)
/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_SetTestMode
 * Description   : Set test mode of the WDOG.
 *
 * @implements     Wdog_Ip_SetTestMode_Activity
 *END**************************************************************************/
Wdog_Ip_StatusType Wdog_Ip_SetTestMode(const uint8 Instance,
                              Wdog_Ip_TestModeType TestMode)
{
#if (WDOG_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(Instance < WDOG_INSTANCE_COUNT);
#endif

    WDOG_Type * Base = Wdog_Ip_sBase[Instance];
    uint32 RegValue = Base->CS;
    Wdog_Ip_StatusType Status = WDOG_IP_STATUS_SUCCESS;

#ifdef MCAL_ENABLE_FAULT_INJECTION
    MCAL_FAULT_INJECTION_POINT(WDOG_FIP_SET_TEST_MODE);
#endif

    /* If allowed reconfigures WDOG */
    if (Wdog_Ip_IsUpdateEnabled(Base))
    {
        RegValue &= ~(WDOG_CS_TST_MASK);
        RegValue |= WDOG_CS_TST(TestMode);

        OsIf_SuspendAllInterrupts();

        if (WDOG_IP_STATUS_SUCCESS == Wdog_Ip_Unlock(Base))
        {
            Base->CS = RegValue;
            Status = Wdog_Ip_IsReconfigurationComplete(Base);
        }
        else
        {
            Status = WDOG_IP_STATUS_TIMEOUT;
        }

        OsIf_ResumeAllInterrupts();
    }
    else
    {
        Status = WDOG_IP_STATUS_ERROR;
    }

    return Status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_GetTestMode
 * Description   : Get the current test mode of the WDOG.
 *
 * @implements     Wdog_Ip_GetTestMode_Activity
 *END**************************************************************************/
Wdog_Ip_TestModeType Wdog_Ip_GetTestMode(const uint8 Instance)
{
#if (WDOG_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(Instance < WDOG_INSTANCE_COUNT);
#endif

    WDOG_Type * Base = Wdog_Ip_sBase[Instance];
    Wdog_Ip_TestModeType testMode = WDOG_IP_TST_DISABLED;

#ifdef MCAL_ENABLE_FAULT_INJECTION
    MCAL_FAULT_INJECTION_POINT(WDOG_FIP_GETTESTMODE);
#endif

    switch (Wdog_Ip_ReadTestMode(Base))
    {
        case 0U:
            testMode = WDOG_IP_TST_DISABLED;
            break;
        case 1U:
            testMode = WDOG_IP_TST_USER;
            break;
        case 2U:
            testMode = WDOG_IP_TST_LOW;
            break;
        case 3U:
            testMode = WDOG_IP_TST_HIGH;
            break;
        default:
            testMode = WDOG_IP_TST_DISABLED;
            break;
    }

    return testMode;
}
#endif /* WDOG_IP_ENABLE_TEST_MODE == STD_ON */

/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_Config
 * Description   : Configures the WDOG
 *
 * @implements     Wdog_Ip_Config_Activity
 *END**************************************************************************/
Wdog_Ip_StatusType Wdog_Ip_Config(const uint8 Instance,
                            const Wdog_Ip_ConfigType * const ConfigPtr)
{
#if (WDOG_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(Instance < WDOG_INSTANCE_COUNT);
    DevAssert(ConfigPtr != NULL_PTR);
#endif

    WDOG_Type * Base           = Wdog_Ip_sBase[Instance];
    Wdog_Ip_StatusType Status            = WDOG_IP_STATUS_SUCCESS;
    uint32 TempCs = 0x00U;

#ifdef MCAL_ENABLE_FAULT_INJECTION
    MCAL_FAULT_INJECTION_POINT(WDOG_FIP_1_IPCONFIG);
#endif

    boolean WdogEnabled = Wdog_Ip_IsEnabled(Base);
    boolean UpdateEnabled = Wdog_Ip_IsUpdateEnabled(Base);

    if (FALSE == UpdateEnabled)
    {
        Status = WDOG_IP_STATUS_ERROR;
    }
    else
    {
        if (TRUE == WdogEnabled)
        {
            /* Stop the WDOG counter */
            Base->CS &= ~WDOG_CS_EN_MASK;
        }

        /* Configure the WDOG module */
        Status = Wdog_Ip_HwConfig(ConfigPtr, &TempCs);
    }

    if (WDOG_IP_STATUS_SUCCESS == Status)
    {
        if (TRUE == WdogEnabled)
        {
            /* Enable WDOG in 32-bit mode */
            TempCs |=  WDOG_CS_EN_MASK | WDOG_CS_CMD32EN_MASK;
        }

        OsIf_SuspendAllInterrupts();

        if (WDOG_IP_STATUS_SUCCESS == Wdog_Ip_Unlock(Base))
        {
            Base->TOVAL = ConfigPtr->TimeoutValue;

            if (ConfigPtr->WinEnable)
            {
                TempCs |=  WDOG_CS_WIN_MASK;

                Base->WIN = ConfigPtr->WindowValue;
            }

            /* Write CS value to register */
            Base->CS = TempCs;

            if (ConfigPtr->IntEnable)
            {
                Wdog_Ip_apCallbackPtr[Instance] = ConfigPtr->pfWdogCallback;
            }
            else
            {
                Wdog_Ip_apCallbackPtr[Instance] = NULL_PTR;
            }

            Status = Wdog_Ip_IsReconfigurationComplete(Base);
        }
        else
        {
            Status = WDOG_IP_STATUS_TIMEOUT;
        }

        OsIf_ResumeAllInterrupts();
    }

    return Status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_StartTimer
 * Description   : Starts the WDOG counter.
 *
 * @implements     Wdog_Ip_StartTimer_Activity
 *END**************************************************************************/
Wdog_Ip_StatusType Wdog_Ip_StartTimer(const uint8 Instance)
{
#if (WDOG_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(Instance < WDOG_INSTANCE_COUNT);
#endif

    WDOG_Type * Base = Wdog_Ip_sBase[Instance];
    Wdog_Ip_StatusType Status = WDOG_IP_STATUS_SUCCESS;

#ifdef MCAL_ENABLE_FAULT_INJECTION
    MCAL_FAULT_INJECTION_POINT(WDOG_FIP_STARTTIMER);
#endif

    if (FALSE == Wdog_Ip_IsEnabled(Base))
    {
        if (Wdog_Ip_IsUpdateEnabled(Base))
        {
            OsIf_SuspendAllInterrupts();

            if (WDOG_IP_STATUS_SUCCESS == Wdog_Ip_Unlock(Base))
            {
                Base->CS |= WDOG_CS_EN_MASK;

                Status = Wdog_Ip_IsReconfigurationComplete(Base);
            }
            else
            {
                Status = WDOG_IP_STATUS_TIMEOUT;
            }

            OsIf_ResumeAllInterrupts();
        }
        else
        {
            Status = WDOG_IP_STATUS_ERROR;
        }
    }

    return Status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : Wdog_Ip_StopTimer
 * Description   : Stops the WDOG counter.
 *
 * @implements     Wdog_Ip_StopTimer_Activity
 *END**************************************************************************/
Wdog_Ip_StatusType Wdog_Ip_StopTimer(const uint8 Instance)
{
#if (WDOG_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(Instance < WDOG_INSTANCE_COUNT);
#endif

    WDOG_Type * Base = Wdog_Ip_sBase[Instance];
    Wdog_Ip_StatusType Status = WDOG_IP_STATUS_SUCCESS;

    if (TRUE == Wdog_Ip_IsEnabled(Base))
    {
        #ifdef MCAL_ENABLE_FAULT_INJECTION
            MCAL_FAULT_INJECTION_POINT(WDOG_FIP_STOPTIMER);
        #endif

        if (Wdog_Ip_IsUpdateEnabled(Base))
        {
            OsIf_SuspendAllInterrupts();

            if (WDOG_IP_STATUS_SUCCESS == Wdog_Ip_Unlock(Base))
            {
                Base->CS &= ~WDOG_CS_EN_MASK;

                Status = Wdog_Ip_IsReconfigurationComplete(Base);
            }
            else
            {
                Status = WDOG_IP_STATUS_TIMEOUT;
            }

            OsIf_ResumeAllInterrupts();
        }
        else
        {
            Status = WDOG_IP_STATUS_ERROR;
        }
    }

    return Status;
}

/**
* @Description    This function handles the WDOG interrupt request.
*
* @implements     Wdog_Ip_IrqHandler_Activity
*/
void Wdog_Ip_IrqHandler(uint8 Instance)
{
    WDOG_Type * Base = Wdog_Ip_sBase[Instance];

#if (WDOG_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(Instance < WDOG_INSTANCE_COUNT);
#endif

    if(Wdog_Ip_abStatus[Instance] == TRUE)
    {
        if  (Wdog_Ip_GetIntFlag(Base) && Wdog_Ip_IsIntEnabled(Base))
        {
            Wdog_Ip_ClearIntFlag(Base);

            if (Wdog_Ip_apCallbackPtr[Instance] != ((void *)0))
            {
                Wdog_Ip_apCallbackPtr[Instance]();
            }
        }
    }
    else
    {
        Wdog_Ip_ClearIntFlag(Base);
    }
}

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
