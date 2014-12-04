/*
 * File      : power.h
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
#ifndef __POWER_H__
#define __POWER_H__

#include <rthw.h>
#include <rtthread.h>
#include "M051Series.h"
#include "untils.h"

/*
    pwr pwm enable: P42
*/
#define PWR_PWM_ENABLE_PORT_IDX 4
#define PWR_PWM_ENABLE_PIN 2
#define PWR_PWM_ENABLE_PORT PORT_MAP[PWR_PWM_ENABLE_PORT_IDX]
#define PWR_PWM_ENABLE_BIT BIT_MASK(PWR_PWM_ENABLE_PIN)

/*
    pwr pwm boost enable: P23
*/
#define PWR_PWM_BOOST_ENABLE_PORT_IDX 2
#define PWR_PWM_BOOST_ENABLE_PIN 3
#define PWR_PWM_BOOST_ENABLE_PORT PORT_MAP[PWR_PWM_BOOST_ENABLE_PORT_IDX]
#define PWR_PWM_BOOST_ENABLE_BIT BIT_MASK(PWR_PWM_BOOST_ENABLE_PIN)
/*
    pwr pwm buck enable: P22
*/
#define PWR_PWM_BUCK_ENABLE_PORT_IDX 2
#define PWR_PWM_BUCK_ENABLE_PIN 2
#define PWR_PWM_BUCK_ENABLE_PORT PORT_MAP[PWR_PWM_BUCK_ENABLE_PORT_IDX]
#define PWR_PWM_BUCK_ENABLE_BIT BIT_MASK(PWR_PWM_BUCK_ENABLE_PIN)
/*
    dat: 0, disable
         1, enable
*/
__INLINE int set_pwr_pwm_enable(char dat)
{
    return oport(PWR_PWM_ENABLE_PORT_IDX, PWR_PWM_ENABLE_PIN, dat);
}

__INLINE int set_pwr_pwm_boost_enable(char dat)
{
    return oport(PWR_PWM_BOOST_ENABLE_PORT_IDX, PWR_PWM_BOOST_ENABLE_PIN, dat);
}

__INLINE int set_pwr_pwm_buck_enable(char dat)
{
    oport(PWR_PWM_BUCK_ENABLE_PORT_IDX, PWR_PWM_BUCK_ENABLE_PIN, dat);
    return dat;
}

/*
    pwr adc enable: P00
*/
#define PWR_ADC_ENABLE_PORT_IDX 0
#define PWR_ADC_ENABLE_PIN 0
#define PWR_ADC_ENABLE_PORT PORT_MAP[PWR_ADC_ENABLE_PORT_IDX]
#define PWR_ADC_ENABLE_BIT BIT_MASK(PWR_ADC_ENABLE_PIN)

/*
    dat: 1, disable
         0, enable
*/
__INLINE int set_pwr_adc_enable(char dat)
{
    return oport(PWR_ADC_ENABLE_PORT_IDX, PWR_ADC_ENABLE_PIN, dat);
}

int pwr_adc_boost_configure(void);
int pwr_adc_load_configure(void);
int pwr_adc_start(void);
int pwr_adc_stop(void);

int pwr_pwm_boost_configure(uint32_t freq, uint8_t duty);
int pwr_pwm_boost_start(void);
int pwr_pwm_boost_stop(void);

int pwr_pwm_buck_configure(uint32_t freq, uint8_t duty);
int pwr_pwm_buck_start(void);
int pwr_pwm_buck_stop(void);

#endif
