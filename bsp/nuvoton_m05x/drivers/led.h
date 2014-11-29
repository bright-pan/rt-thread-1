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

#define LED_ON 1
#define LED_OFF 0

__INLINE int set_led_test(char dat);
__INLINE int set_led_red(char dat);
__INLINE int set_led_yellow(char dat);
__INLINE int set_led_green(char dat);

#endif
