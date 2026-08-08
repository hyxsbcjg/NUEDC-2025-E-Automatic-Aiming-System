#include "ADC.h"


volatile bool ADC_Flag = false;


uint16_t adc_getValue(void)
{
	
    uint16_t gAdcResult = 0;

	
	ADC_Flag = false;
	DL_ADC12_startConversion(ADC_INST);
	while(ADC_Flag == false);
	
	gAdcResult = DL_ADC12_getMemResult(ADC_INST,ADC_ADCMEM_ADC_Channel0);
	
	DL_ADC12_enableConversions(ADC_INST);

    return gAdcResult;
}






void ADC_INST_IRQHandler(void)
{
	switch(DL_ADC12_getPendingInterrupt(ADC_INST))
	{
		case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
			ADC_Flag = true;
			DL_ADC12_clearInterruptStatus(ADC_INST, DL_ADC12_IIDX_MEM0_RESULT_LOADED);
		break;
		default:
			
		break;
	}
}