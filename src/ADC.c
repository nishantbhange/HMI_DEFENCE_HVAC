/*
 * ADC.c
 *
 *  Created on: Jul 10, 2026
 *      Author: Nishant Bhange
 */


#include "ADC.h"
#include "NVIC.h"

volatile ADC_Readings_t ADC_Data ;
volatile ADC_Ctrl_t ADC_Ctrl ;

ADC_Channel_t Current_ADC_Channel;

bool ADC_Init(){

	// FIRCDIV2 = 4: FIRCDIV2 divide by 8
	IP_SCG->FIRCDIV &=(~(SCG_FRC_DIV_MASK));
	IP_SCG->FIRCDIV |= (FRCDIV2_8<<SCG_FIRCDIV_BIT_FIRCDIV2);

	// Calibrate ADC0
	// Disable clock to change PCS */
	IP_PCC->PCCn[PCC_ADC0_INDEX] &=~ PCC_PCCn_CGC_MASK;
	/* PCS = 3: Select FIRCDIV2 */
	IP_PCC->PCCn[PCC_ADC0_INDEX] &=~(PCC_PCCN_PCS_MASK) ;
	IP_PCC->PCCn[PCC_ADC0_INDEX] |=(ADC0_CLK_OPTION_FIRCDIV2<<PCC_ADC0_BIT_PCS) ;

	/* Enable bus clock in ADC */
	IP_PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_CGC_MASK;

	//configure CFG1 clk divide ratio-1 , adc resolution 12 bit .
	IP_ADC0->CFG1 =(ADC_CLK_DIV_1<<ADC_CFG1_BIT_CLK_DIV_SEL)|
				        (ADC_MODE_12BIT<<ADC_CFG1_BIT_MODE);

	//configure CFG2 sample time =13
	IP_ADC0->CFG2 &=~(ADC0_CFG2_SAMPLT_MASK);
	IP_ADC0->CFG2|=(ADC_SAMPLE_TIME);

    // ADTRG = 0: SW trigger

	IP_ADC0->SC2 =(ADC_SOFTWARE_TRIGGER<<ADC_SC2_BIT_ADTRG)|
			       (ADC_CMP_FN_DI<<ADC_SC2_BIT_ACFE)       |
					(ADC_DMA_DI<<ADC_SC2_BIT_DMAEN)          |
					(ADC_VOLTAGE_REF_DEFAULT<<ADC_SC2_BIT_REFSEL) ;

	// ADCH: Module disabled for conversions
	IP_ADC0->SC1[0] = ADC_SC1_ADCH_DISABLE_MODULE;

	// CAL = 1: Start calibration sequence
	IP_ADC0->SC3 = ADC_SC3_CAL_MASK
			      | ADC_SC3_AVGE_MASK /* AVGE = 1: Enable hardware average */
	              | (ADC_HW_AVG_SAMPLE_32<<ADC_SC3_BIT_AVGS); /* AVGS = 11b: 32 samples averaged */
	//Wait for completion
	while(((IP_ADC0->SC1[0] & ADC_SC1_COCO_MASK)>>ADC_SC1_COCO_SHIFT) == 0);
      //check is calibration is fine
	if (IP_ADC0->SC3 & ADC_SC3_CAL_MASK)
	{
	    // Calibration failed
		return false ;

	}
	ADC_Ctrl.Status=ADC_FREE;
	return true ;
}

bool ADC_Task(ADC_Channel_t ADC_Channel  ){

	if(ADC_Ctrl.Status==ADC_FREE){
		Current_ADC_Channel = ADC_Channel;
		IP_ADC0->SC1[0]=ADC_Channel|ADC_SC1_AIEN_MASK;
		ADC_Ctrl.Status=ADC_BUSY;
		return true ;

	}
	return false;

}
