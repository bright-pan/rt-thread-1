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
#define LED_TEST_PORT P3
#define LED_TEST_BIT_IDX 6
#define LED_TEST_PIN P36
#define LED_TEST_DATA(dat) (LED_TEST_PIN = dat)
#define LED_TEST_BIT bits_mask(6)

/*
    LED red: P41
*/
#define LED_RED_PORT P4
#define LED_RED_BIT_IDX 1
#define LED_RED_PIN P41
#define LED_RED_DATA(dat) (LED_RED_PIN = dat)
#define LED_RED_BIT bits_mask(LED_RED_BIT_IDX)

/*
    LED yellow: P04
*/
#define LED_YELLOW_PORT P0
#define LED_YELLOW_BIT_IDX 4
#define LED_YELLOW_PIN P04
#define LED_YELLOW_DATA(dat) (LED_YELLOW_PIN = dat)
#define LED_YELLOW_BIT bits_mask(LED_YELLOW_BIT_IDX)

/*
    LED green: P05
*/
#define LED_GREEN_PORT P0
#define LED_GREEN_BIT_IDX 5
#define LED_GREEN_PIN P05
#define LED_GREEN_DATA(dat) (LED_GREEN_PIN = dat)
#define LED_GREEN_BIT bits_mask(LED_GREEN_BIT_IDX)

/* Initial gpio led pin  */
int rt_hw_led_init(void)
{
#ifdef LED_TEST_PORT
    /* Configure the led test pin */
    LED_TEST_DATA(0);
    GPIO_SetMode(LED_TEST_PORT, LED_TEST_BIT, GPIO_PMD_OUTPUT);
#endif
    
#ifdef LED_RED_PORT
    /* Configure the led red pin */
    LED_RED_DATA(0);
    GPIO_SetMode(LED_RED_PORT, LED_RED_BIT, GPIO_PMD_OUTPUT);
#endif

#ifdef LED_YELLOW_PORT
    /* Configure the led yellow pin */
    LED_YELLOW_DATA(0);
    GPIO_SetMode(LED_YELLOW_PORT, LED_YELLOW_BIT, GPIO_PMD_OUTPUT);
#endif
    
#ifdef LED_GREEN_PORT
    /* Configure the led green pin */
    LED_GREEN_DATA(0);
    GPIO_SetMode(LED_GREEN_PORT, LED_GREEN_BIT, GPIO_PMD_OUTPUT);
#endif
    return 0;
}

__INLINE void led_test_set(uint8_t dat)
{
    LED_TEST_DATA(dat);
}

__INLINE void led_red_set(uint8_t dat)
{
    LED_RED_DATA(dat);
}

__INLINE void led_yellow_set(uint8_t dat)
{
    LED_YELLOW_DATA(dat);
}

__INLINE void led_green_set(uint8_t dat)
{
    LED_GREEN_DATA(dat);
}

/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_led_init);

#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT(led_test_set, );
FINSH_FUNCTION_EXPORT(led_red_set, );
FINSH_FUNCTION_EXPORT(led_yellow_set, );
FINSH_FUNCTION_EXPORT(led_green_set, );

#endif
