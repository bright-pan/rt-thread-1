/*
 * File      : oled.h
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

#ifndef __OLED_H__
#define __OLED_H__

#include <rthw.h>
#include <rtthread.h>
#include "M051Series.h"
#include "untils.h"

#define OLED_ON 1
#define OLED_OFF 0
/*
    OLED enable: P3.6
*/
#define OLED_ENABLE_PORT_IDX 3
#define OLED_ENABLE_PIN 6
#define OLED_ENABLE_PORT PORT_MAP[OLED_ENABLE_PORT_IDX]
#define OLED_ENABLE_BIT BIT_MASK(OLED_ENABLE_PIN)

__INLINE int set_oled_enable(char dat)
{
    return oport(OLED_ENABLE_PORT_IDX, OLED_ENABLE_PIN, dat);
}

#endif
