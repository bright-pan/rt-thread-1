/*
 * File      : led.h
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

#ifndef __LED_H__
#define __LED_H__

#include <rthw.h>
#include <rtthread.h>
#include "M051Series.h"
#include "untils.h"

#define LED_ON 1
#define LED_OFF 0
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
    return oport(LED_TEST_PORT_IDX, LED_TEST_PIN, dat);
}

__INLINE int set_led_red(char dat)
{
    return oport(LED_RED_PORT_IDX, LED_RED_PIN, dat);
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

#endif
