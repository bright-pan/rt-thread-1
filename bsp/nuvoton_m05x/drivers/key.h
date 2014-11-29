/*
 * File      : gpio_key.h
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

#ifndef __GPIO_KEY_H__
#define __GPIO_KEY_H__

#include <rthw.h>
#include <rtthread.h>
#include "M051Series.h"
#include "untils.h"

#define DEBUG_KEY 1

/*
    KEY sw: P33
*/
#define KEY_SW_PORT_IDX 3
#define KEY_SW_PIN 3
#define KEY_SW_PORT PORT_MAP[KEY_SW_PORT_IDX]
#define KEY_SW_BIT BIT_MASK(KEY_SW_PIN)

/*
    KEY power: P34
*/
#define KEY_PWR_PORT_IDX 3
#define KEY_PWR_PIN 4
#define KEY_PWR_PORT PORT_MAP[KEY_PWR_PORT_IDX]
#define KEY_PWR_BIT BIT_MASK(KEY_PWR_PIN)

/*
    KEY down: P35
*/
#define KEY_DOWN_PORT_IDX 3
#define KEY_DOWN_PIN 5
#define KEY_DOWN_PORT PORT_MAP[KEY_DOWN_PORT_IDX]
#define KEY_DOWN_BIT BIT_MASK(KEY_DOWN_PIN)

/*
    KEY up: P43
*/
#define KEY_UP_PORT_IDX 4
#define KEY_UP_PIN 3
#define KEY_UP_PORT PORT_MAP[KEY_UP_PORT_IDX]
#define KEY_UP_BIT BIT_MASK(KEY_UP_PIN)

__INLINE int set_key_sw(char dat)
{
    oport(KEY_SW_PORT_IDX, KEY_SW_PIN, dat);
    return dat;
}

__INLINE int set_key_pwr(char dat)
{
    oport(KEY_PWR_PORT_IDX, KEY_PWR_PIN, dat);
    return dat;
}

__INLINE int set_key_down(char dat)
{
    oport(KEY_DOWN_PORT_IDX, KEY_DOWN_PIN, dat);
    return dat;
}

__INLINE int set_key_up(char dat)
{
    oport(KEY_UP_PORT_IDX, KEY_UP_PIN, dat);
    return dat;
}

__INLINE int get_key_sw(void)
{
    return iport(KEY_SW_PORT_IDX, KEY_SW_PIN);
}

__INLINE int get_key_pwr(void)
{
    return iport(KEY_PWR_PORT_IDX, KEY_PWR_PIN);
}

__INLINE int get_key_down(void)
{
    return iport(KEY_DOWN_PORT_IDX, KEY_DOWN_PIN);
}

__INLINE int get_key_up(void)
{
    return iport(KEY_UP_PORT_IDX, KEY_UP_PIN);
}

#endif
