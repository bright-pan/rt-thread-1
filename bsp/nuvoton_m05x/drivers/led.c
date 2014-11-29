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
 * 2014-11-23     Bright      first implementation
 */

#include "led.h"
#include "untils.h"

/*
    LED test: P3.6
*/
#define LED_TEST_PORT_IDX 3
#define LED_TEST_PIN 6
#define LED_TEST_PORT PORT_MAP[LED_TEST_PORT_IDX]
#define LED_TEST_BIT BIT_MASK(LED_TEST_PIN)

/*
    LED red: P41
*/
#define LED_RED_PORT_IDX 4
#define LED_RED_PIN 1
#define LED_RED_PORT PORT_MAP[LED_RED_PORT_IDX]
#define LED_RED_BIT BIT_MASK(LED_RED_PIN)

/*
    LED yellow: P04
*/
#define LED_YELLOW_PORT_IDX 0
#define LED_YELLOW_PIN 4
#define LED_YELLOW_PORT PORT_MAP[LED_YELLOW_PORT_IDX]
#define LED_YELLOW_BIT BIT_MASK(LED_YELLOW_PIN)

/*
    LED green: P05
*/
#define LED_GREEN_PORT_IDX 0
#define LED_GREEN_PIN 5
#define LED_GREEN_PORT PORT_MAP[LED_GREEN_PORT_IDX]
#define LED_GREEN_BIT BIT_MASK(LED_GREEN_PIN)

__INLINE int set_led_test(char dat)
{
    oport(LED_TEST_PORT_IDX, LED_TEST_PIN, dat);
    return dat;
}

__INLINE int set_led_red(char dat)
{
    oport(LED_RED_PORT_IDX, LED_RED_PIN, dat);
    return dat;
}

__INLINE int set_led_yellow(char dat)
{
    oport(LED_YELLOW_PORT_IDX, LED_YELLOW_PIN, dat);
    return dat;
}

__INLINE int set_led_green(char dat)
{
    oport(LED_GREEN_PORT_IDX, LED_GREEN_PIN, dat);
    return dat;
}

/* Initial gpio led pin  */
int rt_hw_led_init(void)
{
#ifdef LED_TEST_PORT
    /* Configure the led test pin */
    set_led_test(LED_OFF);
    GPIO_SetMode(LED_TEST_PORT, LED_GREEN_BIT, GPIO_PMD_OUTPUT);
#endif
    
#ifdef LED_RED_PORT
    /* Configure the led red pin */
    set_led_red(LED_OFF);
    GPIO_SetMode(LED_RED_PORT, LED_RED_BIT, GPIO_PMD_OUTPUT);
#endif

#ifdef LED_YELLOW_PORT
    /* Configure the led yellow pin */
    set_led_yellow(LED_OFF);
    GPIO_SetMode(LED_YELLOW_PORT, LED_YELLOW_BIT, GPIO_PMD_OUTPUT);
#endif

#ifdef LED_GREEN_PORT
    /* Configure the led green pin */
    set_led_green(LED_OFF);
    GPIO_SetMode(LED_GREEN_PORT, LED_GREEN_BIT, GPIO_PMD_OUTPUT);
#endif
    return 0;
}


/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_led_init);

#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT(set_led_test, [0 1]);
FINSH_FUNCTION_EXPORT(set_led_red, [0 1]);
FINSH_FUNCTION_EXPORT(set_led_yellow, [0 1]);
FINSH_FUNCTION_EXPORT(set_led_green, [0 1]);

#endif
