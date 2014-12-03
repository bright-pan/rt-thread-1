/*
 * File      : power.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2014, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-11-29     Bright      first implementation
 */

#include "power.h"

#define PWR_ADC_DEBUG 1

#define PWR_ADC_MODE_LOAD 0
#define PWR_ADC_MODE_BOOST 1

#define PWR_ADC_CHANNEL_LOAD_DETECT_H 0
#define PWR_ADC_CHANNEL_LOAD_DETECT_L 1 

#define PWR_ADC_CHANNEL_FEEDBACK_CURRENT_H 2
#define PWR_ADC_CHANNEL_FEEDBACK_CURRENT_L 3

#define PWR_ADC_CHANNEL_FEEDBACK 4
#define PWR_ADC_CHANNEL_VBAT 6

static uint8_t pwr_adc_mode = PWR_ADC_MODE_LOAD;

__INLINE void set_pwr_adc_mode(uint8_t mode)
{
    pwr_adc_mode = mode;
}

void pwr_adc_boost_configure(void)
{
    NVIC_DisableIRQ(ADC_IRQn);
    ADC_DisableInt(ADC, ADC_ADF_INT);
    ADC_POWER_DOWN(ADC);

    set_pwr_adc_mode(PWR_ADC_MODE_BOOST);
    /* Set the ADC operation mode as continuous scan, input mode as differential and
    enable analog input channel 0, 2, 4, 6*/
    ADC_Open(ADC, ADC_ADCR_DIFFEN_DIFFERENTIAL, ADC_ADCR_ADMD_SINGLE_CYCLE, 
             BIT_MASK(PWR_ADC_CHANNEL_FEEDBACK_CURRENT_H) |
             BIT_MASK(PWR_ADC_CHANNEL_FEEDBACK) | BIT_MASK(PWR_ADC_CHANNEL_VBAT));
    /* Power on ADC module */
    ADC_POWER_ON(ADC);
    /* clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
    /* Enable the ADC interrupt */
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
}

void pwr_adc_load_configure(void)
{
    NVIC_DisableIRQ(ADC_IRQn);
    ADC_DisableInt(ADC, ADC_ADF_INT);
    ADC_POWER_DOWN(ADC);
    set_pwr_adc_mode(PWR_ADC_MODE_LOAD);
    /* Set the ADC operation mode as continuous scan, input mode as differential and
    enable analog input channel 0, 2, */
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, 
             BIT_MASK(PWR_ADC_CHANNEL_LOAD_DETECT_H) |
             BIT_MASK(PWR_ADC_CHANNEL_LOAD_DETECT_L));
    /* Power on ADC module */
    ADC_POWER_ON(ADC);
    /* clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
    /* Enable the ADC interrupt */
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
}

void pwr_adc_start(void)
{
    /* start A/D conversion */
    ADC_START_CONV(ADC);
}

void pwr_adc_stop(void)
{
    /* start A/D conversion */
    ADC_STOP_CONV(ADC);
}

/*---------------------------------------------------------------------------------------------------------*/
/* ADC interrupt handler                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void ADC_IRQHandler(void)
{
    uint16_t value;
    if (pwr_adc_mode == PWR_ADC_MODE_BOOST)
    {
        if (ADC_GET_INT_FLAG(ADC, ADC_ADF_INT))
        {
            while(ADC_IS_DATA_VALID(ADC, PWR_ADC_CHANNEL_FEEDBACK_CURRENT_H)) /* Check the VALID bits */
            {
                value = (uint16_t)ADC_GET_CONVERSION_DATA(ADC, PWR_ADC_CHANNEL_FEEDBACK_CURRENT_H);
                RT_DEBUG_LOG(PWR_ADC_DEBUG, ("PWR_ADC_MODE_LOAD: channel %d, 0x%x(%d)\n", PWR_ADC_CHANNEL_FEEDBACK_CURRENT_H, value, value));
            }
            while(ADC_IS_DATA_VALID(ADC, PWR_ADC_CHANNEL_FEEDBACK)) /* Check the VALID bits */
            {
                value = (uint16_t)ADC_GET_CONVERSION_DATA(ADC, PWR_ADC_CHANNEL_FEEDBACK);
                RT_DEBUG_LOG(PWR_ADC_DEBUG, ("PWR_ADC_MODE_LOAD: channel %d, 0x%x(%d)\n", PWR_ADC_CHANNEL_FEEDBACK, value, value));
            }
            while(ADC_IS_DATA_VALID(ADC, PWR_ADC_CHANNEL_VBAT)) /* Check the VALID bits */
            {
                value = (uint16_t)ADC_GET_CONVERSION_DATA(ADC, PWR_ADC_CHANNEL_VBAT);
                RT_DEBUG_LOG(PWR_ADC_DEBUG, ("PWR_ADC_MODE_LOAD: channel %d, 0x%x(%d)\n", PWR_ADC_CHANNEL_VBAT, value, value));
            }
            ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT); /* clear the A/D conversion flag */
        }
    } else {
        if (ADC_GET_INT_FLAG(ADC, ADC_ADF_INT))
        {
            while(ADC_IS_DATA_VALID(ADC, PWR_ADC_CHANNEL_LOAD_DETECT_H)) /* Check the VALID bits */
            {
                value = (uint16_t)ADC_GET_CONVERSION_DATA(ADC, PWR_ADC_CHANNEL_LOAD_DETECT_H);
                RT_DEBUG_LOG(PWR_ADC_DEBUG, ("PWR_ADC_MODE_LOAD: channel %d, 0x%x(%d)\n", PWR_ADC_CHANNEL_LOAD_DETECT_H, value, value));
            }
            while(ADC_IS_DATA_VALID(ADC, PWR_ADC_CHANNEL_LOAD_DETECT_L)) /* Check the VALID bits */
            {
                value = (uint16_t)ADC_GET_CONVERSION_DATA(ADC, PWR_ADC_CHANNEL_LOAD_DETECT_L);
                RT_DEBUG_LOG(PWR_ADC_DEBUG, ("PWR_ADC_MODE_LOAD: channel %d, 0x%x(%d)\n", PWR_ADC_CHANNEL_LOAD_DETECT_L, value, value));
            }
            ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT); /* clear the A/D conversion flag */
        }
    }
}

int rt_hw_pwr_adc_init(void)
{
    /* Enable ADC module clock */
    CLK_EnableModuleClock(ADC_MODULE);
    /* ADC clock source is 50MHz, set divider to 2, ADC clock is 50/4 MHz */
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HCLK, CLK_CLKDIV_ADC(4));
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Disable the P1.0 - P1.7 digital input path to avoid the leakage current */
    GPIO_DISABLE_DIGITAL_PATH(P1, 0xFF);
    /* Configure the P1.0 - P1.3 ADC analog input pins */
    SYS->P1_MFP &= ~(SYS_MFP_P10_Msk | SYS_MFP_P11_Msk | SYS_MFP_P12_Msk | SYS_MFP_P13_Msk |
                     SYS_MFP_P14_Msk | SYS_MFP_P15_Msk | SYS_MFP_P16_Msk | SYS_MFP_P17_Msk);
    SYS->P1_MFP |= SYS_MFP_P10_AIN0 | SYS_MFP_P11_AIN1 | SYS_MFP_P12_AIN2 | SYS_MFP_P13_AIN3 |
                   SYS_MFP_P14_AIN4 | SYS_MFP_P15_AIN5 | SYS_MFP_P16_AIN6 | SYS_MFP_P17_AIN7;
    return 0;
}

#define PWR_PWMA_CHANNEL_BOOST_H 0
//#define PWR_PWM_CHANNEL_BOOST_L 1

void PWMA_IRQHandler(void)
{
    static uint32_t cnt;
    static uint32_t out;

    // Channel 0 frequency is 100Hz, every 1 second enter this IRQ handler 100 times.
    if (PWM_GetPeriodIntFlag(PWMA, PWR_PWMA_CHANNEL_BOOST_H))
    {
        if(out)
            PWM_EnableOutput(PWMA, 0xF);
        else
            PWM_DisableOutput(PWMA, 0xF);
        out ^= 1;
        cnt = 0;
        
        // Clear channel 0 period interrupt flag
        PWM_ClearPeriodIntFlag(PWMA, PWR_PWMA_CHANNEL_BOOST_H);
    }
}

void pwr_pwm_boost_configure(uint32_t freq, uint8_t duty)
{
    /* Enable PWM Output pin */
    //PWM_EnableOutput(PWMA, BIT_MASK(PWR_PWM_CHANNEL_BOOST_H) | BIT_MASK(PWR_PWM_CHANNEL_BOOST_L));
    PWM_ConfigOutputChannel(PWMA, PWR_PWMA_CHANNEL_BOOST_H, freq, duty);
    PWM_EnableADCTrigger(PWMA, PWR_PWMA_CHANNEL_BOOST_H, PWM_PERIOD_TRIGGER_ADC); 
    /* Enable Timer period Interrupt */
    //PWM_ClearPeriodIntFlag(PWMA, PWR_PWMA_CHANNEL_BOOST_H);
    //PWM_EnablePeriodInt(PWMA, PWR_PWMA_CHANNEL_BOOST_H, PWM_PERIOD_INT_UNDERFLOW);
    /* Enable PWMB NVIC */
    //NVIC_EnableIRQ(PWMA_IRQn);
}

void pwr_pwm_boost_start(void)
{
    /* Enable PWM Timer */
    PWM_EnableOutput(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BOOST_H));
    PWM_Start(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BOOST_H));
}
void pwr_pwm_boost_stop(void)
{
    /* Enable PWM Timer */
    PWM_DisableOutput(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BOOST_H));
    PWM_ForceStop(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BOOST_H));
}

int rt_hw_pwr_pwm_init(void)
{
    /* Enable PWM module clock */
    CLK_EnableModuleClock(PWM01_MODULE);
    CLK_EnableModuleClock(PWM23_MODULE);
    /* Select PWM module clock source */
    CLK_SetModuleClock(PWM01_MODULE, CLK_CLKSEL1_PWM01_S_HCLK, 0);
    CLK_SetModuleClock(PWM23_MODULE, CLK_CLKSEL1_PWM01_S_HCLK, 0);
    /* Reset PWMA channel0~channel3 */
    SYS_ResetModule(PWM03_RST);
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set P4 multi-function pins for PWMA Channel0 */
    SYS->P4_MFP &= ~(SYS_MFP_P40_Msk);
    SYS->P4_MFP |= (SYS_MFP_P40_PWM0);
    return 0;
}

/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_pwr_adc_init);
INIT_DEVICE_EXPORT(rt_hw_pwr_pwm_init);

#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT_ALIAS(pwr_adc_load_configure,pa_load, );
FINSH_FUNCTION_EXPORT_ALIAS(pwr_adc_boost_configure, pa_boost, );
FINSH_FUNCTION_EXPORT_ALIAS(pwr_adc_start, pa_start, );
FINSH_FUNCTION_EXPORT_ALIAS(pwr_adc_stop, pa_stop, );

FINSH_FUNCTION_EXPORT_ALIAS(pwr_pwm_boost_configure, pp_boost, );
FINSH_FUNCTION_EXPORT_ALIAS(pwr_pwm_boost_start, pp_start, );
FINSH_FUNCTION_EXPORT_ALIAS(pwr_pwm_boost_stop, pp_stop, );

#endif
