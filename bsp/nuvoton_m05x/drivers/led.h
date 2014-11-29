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

__INLINE void led_test_set(uint8_t dat);
__INLINE void led_red_set(uint8_t dat);
__INLINE void led_yellow_set(uint8_t dat);
__INLINE void led_green_set(uint8_t dat);

#endif
