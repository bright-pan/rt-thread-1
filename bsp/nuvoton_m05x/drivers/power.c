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

int pwr_adc_boost_configure(void)
{
    /* re-configure adc module */
    NVIC_DisableIRQ(ADC_IRQn);
    ADC_DisableInt(ADC, ADC_ADF_INT);
    ADC_POWER_DOWN(ADC);
    SYS_ResetModule(ADC_RST);

    set_pwr_adc_mode(PWR_ADC_MODE_BOOST);
    /* Set the ADC operation mode as continuous scan, input mode as differential and
    enable analog input channel 0, 2, 4, 6*/
    ADC_Open(ADC, ADC_ADCR_DIFFEN_DIFFERENTIAL, ADC_ADCR_ADMD_SINGLE_CYCLE, 
             BIT_MASK(PWR_ADC_CHANNEL_FEEDBACK_CURRENT_H) |
             BIT_MASK(PWR_ADC_CHANNEL_FEEDBACK) | BIT_MASK(PWR_ADC_CHANNEL_VBAT));
    /* Power on ADC module */
    ADC_POWER_ON(ADC);
    /* Configure the hardware trigger condition and enable hardware trigger; PWM trigger delay: (4*10) system clock cycles*/
    ADC_EnableHWTrigger(ADC, ADC_ADCR_TRGS_PWM, 0);
    /* clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
    /* Enable the ADC interrupt */
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
    return 0;
}

int pwr_adc_load_configure(void)
{
    /* re-configure adc module */
    NVIC_DisableIRQ(ADC_IRQn);
    ADC_DisableInt(ADC, ADC_ADF_INT);
    ADC_POWER_DOWN(ADC);
    SYS_ResetModule(ADC_RST);
    /* set pwr adc mode */
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
    return 0;
}

int pwr_adc_start(void)
{
    /* start A/D conversion */
    ADC_START_CONV(ADC);

    return 0;
}

int pwr_adc_stop(void)
{
    /* start A/D conversion */
    ADC_STOP_CONV(ADC);

    return 0;
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
    
    /* Configure pwr adc enable pin */
    set_pwr_adc_enable(1); // disable pwr adc
    GPIO_SetMode(PWR_ADC_ENABLE_PORT, PWR_PWM_BUCK_ENABLE_BIT, GPIO_PMD_OUTPUT); 

    /* Enable ADC module clock */
    CLK_EnableModuleClock(ADC_MODULE);
    /* ADC clock source is 50MHz, set divider to 2, ADC clock is 50/4 MHz */
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HCLK, CLK_CLKDIV_ADC(4));
    /* Reset PWMA channel0~channel3 */
    SYS_ResetModule(ADC_RST);
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
#define PWR_PWMA_CHANNEL_BUCK_H 1
//#define PWR_PWM_CHANNEL_BUCK_L 1
void PWMA_IRQHandler(void)
{
    if (PWM_GetPeriodIntFlag(PWMA, PWR_PWMA_CHANNEL_BOOST_H))
    {
        PWM_DisableOutput(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BOOST_H));
        PWM_ForceStop(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BOOST_H));
        // Clear channel 0 period interrupt flag
        PWM_ClearPeriodIntFlag(PWMA, PWR_PWMA_CHANNEL_BOOST_H);
    }
    if (PWM_GetPeriodIntFlag(PWMA, PWR_PWMA_CHANNEL_BUCK_H))
    {
        PWM_DisableOutput(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BUCK_H));
        PWM_ForceStop(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BUCK_H));
        // Clear channel 1 period interrupt flag
        PWM_ClearPeriodIntFlag(PWMA, PWR_PWMA_CHANNEL_BUCK_H);
    }
}

int pwr_pwm_boost_configure(uint32_t freq, uint8_t duty)
{
    /* re-configure adc module */
    pwr_pwm_boost_stop();
    NVIC_DisableIRQ(PWMA_IRQn);
    PWM_EnablePeriodInt(PWMA, PWR_PWMA_CHANNEL_BOOST_H, PWM_PERIOD_INT_UNDERFLOW);
    ADC_POWER_DOWN(ADC);
    SYS_ResetModule(ADC_RST);
    /* Enable PWM Output pin */
    //PWM_EnableOutput(PWMA, BIT_MASK(PWR_PWM_CHANNEL_BOOST_H) | BIT_MASK(PWR_PWM_CHANNEL_BOOST_L));
    PWM_ConfigOutputChannel(PWMA, PWR_PWMA_CHANNEL_BOOST_H, freq, duty);
    PWM_EnableADCTrigger(PWMA, PWR_PWMA_CHANNEL_BOOST_H, PWM_PERIOD_TRIGGER_ADC); 
    /* Enable Timer period Interrupt */
    PWM_ClearPeriodIntFlag(PWMA, PWR_PWMA_CHANNEL_BOOST_H);
    PWM_EnablePeriodInt(PWMA, PWR_PWMA_CHANNEL_BOOST_H, PWM_PERIOD_INT_UNDERFLOW);
    /* Enable PWMB NVIC */
    NVIC_EnableIRQ(PWMA_IRQn);
    
    return 0;
}

int pwr_pwm_boost_start(void)
{
    /* Enable PWM Timer */
    PWM_EnableOutput(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BOOST_H));
    PWM_Start(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BOOST_H));
    
    return 0;
}
int pwr_pwm_boost_stop(void)
{
    /* Enable PWM Timer */
    PWM_DisableOutput(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BOOST_H));
    PWM_ForceStop(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BOOST_H));

    return 0;
}

int pwr_pwm_buck_configure(uint32_t freq, uint8_t duty)
{
    /* Enable PWM Output pin */
    //PWM_EnableOutput(PWMA, BIT_MASK(PWR_PWM_CHANNEL_BOOST_H) | BIT_MASK(PWR_PWM_CHANNEL_BOOST_L));
    PWM_ConfigOutputChannel(PWMA, PWR_PWMA_CHANNEL_BUCK_H, freq, duty);
    PWM_EnableADCTrigger(PWMA, PWR_PWMA_CHANNEL_BUCK_H, PWM_PERIOD_TRIGGER_ADC); 
    /* Enable Timer period Interrupt */
    PWM_ClearPeriodIntFlag(PWMA, PWR_PWMA_CHANNEL_BUCK_H);
    PWM_EnablePeriodInt(PWMA, PWR_PWMA_CHANNEL_BUCK_H, PWM_PERIOD_INT_UNDERFLOW);
    /* Enable PWMB NVIC */
    NVIC_EnableIRQ(PWMA_IRQn);
    
    return 0;
}

int pwr_pwm_buck_start(void)
{
    /* Enable PWM Timer */
    PWM_EnableOutput(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BUCK_H));
    PWM_Start(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BUCK_H));
    return 0;
}

int pwr_pwm_buck_stop(void)
{
    /* Enable PWM Timer */
    PWM_DisableOutput(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BUCK_H));
    PWM_ForceStop(PWMA, BIT_MASK(PWR_PWMA_CHANNEL_BUCK_H));

    return 0;
}

int rt_hw_pwr_pwm_init(void)
{

    /* Configure pwr pwm enable pin */
    set_pwr_pwm_enable(0); // disable pwr
    GPIO_SetMode(PWR_PWM_ENABLE_PORT, PWR_PWM_ENABLE_BIT, GPIO_PMD_OUTPUT);
    /* Configure pwr pwm boost enable pin */
    set_pwr_pwm_boost_enable(0); // disable pwr pwm boost
    GPIO_SetMode(PWR_PWM_BOOST_ENABLE_PORT, PWR_PWM_BOOST_ENABLE_BIT, GPIO_PMD_OUTPUT);
    /* Configure pwr pwm buck enable pin */
    set_pwr_pwm_buck_enable(0); // disable pwr pwm buck
    GPIO_SetMode(PWR_PWM_BUCK_ENABLE_PORT, PWR_PWM_BUCK_ENABLE_BIT, GPIO_PMD_OUTPUT);    
    
    /* Enable PWM module clock */
    CLK_EnableModuleClock(PWM01_MODULE);
    //CLK_EnableModuleClock(PWM23_MODULE);
    /* Select PWM module clock source */
    CLK_SetModuleClock(PWM01_MODULE, CLK_CLKSEL1_PWM01_S_HCLK, 0);
    //CLK_SetModuleClock(PWM23_MODULE, CLK_CLKSEL1_PWM01_S_HCLK, 0);
    /* Reset PWMA channel0~channel3 */
    SYS_ResetModule(PWM03_RST);
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set P2 multi-function pins for PWMA Channel0,1 */
    SYS->P2_MFP &= ~(SYS_MFP_P20_Msk | SYS_MFP_P21_Msk);
    SYS->P2_MFP |= (SYS_MFP_P20_PWM0 | SYS_MFP_P21_PWM1);
    return 0;
}

int rt_hw_pwr_init(void)
{
    
    /* initial pwr adc function */
    rt_hw_pwr_adc_init();

    /* initial pwr pwm function */
    rt_hw_pwr_pwm_init();
    
    return 0;
}

/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_pwr_init);

#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT_ALIAS(set_pwr_pwm_enable,set_pp, );
FINSH_FUNCTION_EXPORT_ALIAS(set_pwr_pwm_boost_enable, set_ppbt, );
FINSH_FUNCTION_EXPORT_ALIAS(set_pwr_pwm_buck_enable, set_bbbk, );
FINSH_FUNCTION_EXPORT_ALIAS(set_pwr_adc_enable, set_pa, );

FINSH_FUNCTION_EXPORT_ALIAS(pwr_adc_load_configure,pa_load, );
FINSH_FUNCTION_EXPORT_ALIAS(pwr_adc_boost_configure, pa_boost, );
FINSH_FUNCTION_EXPORT_ALIAS(pwr_adc_start, pa_start, );
FINSH_FUNCTION_EXPORT_ALIAS(pwr_adc_stop, pa_stop, );

FINSH_FUNCTION_EXPORT_ALIAS(pwr_pwm_boost_configure, pp_boost, );
FINSH_FUNCTION_EXPORT_ALIAS(pwr_pwm_boost_start, pp_start, );
FINSH_FUNCTION_EXPORT_ALIAS(pwr_pwm_boost_stop, pp_stop, );

#endif
