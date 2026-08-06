/*
 * ADC.h
 *
 *  Created on: Jul 10, 2026
 *      Author: Nishant Bhange
 */

#ifndef ADC_H_
#define ADC_H_
#include<stdio.h>
#include<stdint.h>
#include"S32K144.h"
#include<stdbool.h>
#define ADC_SC1_BIT_AIEN           6
#define ADC_SC1_BIT_ADCH           0

#define ADC_SC2_BIT_ADTRG           6
#define ADC_SC2_BIT_ACFE            5
#define ADC_SC2_BIT_DMAEN           2
#define ADC_SC2_BIT_REFSEL          0x00

#define ADC_SOFTWARE_TRIGGER        0
#define ADC_CMP_FN_DI               0
#define ADC_DMA_DI                  0
#define ADC_VOLTAGE_REF_DEFAULT     0


#define ADC_CFG1_BIT_CLK_DIV_SEL   5
#define ADC_CFG1_BIT_MODE          2
#define ADC_CLK_DIV_1              0x00
#define ADC_MODE_12BIT             0x01
#define ADC_CH_12                  0x0C
#define ADC_SAMPLE_TIME            0x0C

#define FRCDIV2_8                  0x04
#define SCG_FIRCDIV_BIT_FIRCDIV2   8

#define PCC_ADC0_BIT_PCS           24
#define ADC0_CLK_OPTION_FIRCDIV2   0x3

#define ADC_SC3_BIT_AVGS           0
#define ADC_HW_AVG_SAMPLE_32       0x3

#define SCG_FRC_DIV_MASK           (0x00000F00)
#define PCC_PCCN_PCS_MASK          (0x0F000000)
#define ADC0_CFG2_SAMPLT_MASK      (0x000000FF)

#define ADC_SC1_ADCH_DISABLE_MODULE         (0x1FU)

typedef enum
{
    ADC_TEMP_SENSOR     = 12,
    ADC_COMPRESSOR_CT   = 9,
    ADC_CONDENSER_CT    = 8,
    ADC_BLOWER_CT       = 15
} ADC_Channel_t;

typedef enum{
ADC_FREE,
ADC_BUSY

}ADC_State;

typedef struct {
uint8_t ADC_State;


}ADC_t;

typedef struct{
float Temp_Sensor_Val;
float ADC_Condenser_Val;
float ADC_Compressor_Val;
float ADC_Blower_Val;

}ADC_Readings_t;

extern volatile ADC_Readings_t ADC_Data ;
extern volatile ADC_t ADC ;

bool ADC_Init(void);
bool ADC_Task(ADC_Channel_t ADC_Channel  );


#endif /* ADC_H_ */
