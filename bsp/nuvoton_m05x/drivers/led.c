/*
 * File      : led.c
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

#include "led.h"

/* Initial gpio led pin  */
int rt_hw_led_init(void)
{
#ifdef LED_TEST_PORT
    /* Configure the led test pin */
    set_led_test(LED_OFF);
    GPIO_SetMode(LED_TEST_PORT, LED_GREEN_BIT, GPIO_PMD_OUTPUT);
#endif

    /* Configure the led red pin */
    set_led_red(LED_OFF);
    GPIO_SetMode(LED_RED_PORT, LED_RED_BIT, GPIO_PMD_OUTPUT);

    /* Configure the led yellow pin */
    set_led_yellow(LED_OFF);
    GPIO_SetMode(LED_YELLOW_PORT, LED_YELLOW_BIT, GPIO_PMD_OUTPUT);

    /* Configure the led green pin */
    set_led_green(LED_OFF);
    GPIO_SetMode(LED_GREEN_PORT, LED_GREEN_BIT, GPIO_PMD_OUTPUT);

    return 0;
}


/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_led_init);

#ifdef RT_USING_FINSH
#include <finsh.h>
#ifdef LED_TEST_PORT
FINSH_FUNCTION_EXPORT(set_led_test,);
#endif
FINSH_FUNCTION_EXPORT(set_led_red,);
FINSH_FUNCTION_EXPORT(set_led_yellow,);
FINSH_FUNCTION_EXPORT(set_led_green,);

#endif
