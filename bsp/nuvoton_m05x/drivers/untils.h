/*
 * File      : untils.h
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

#ifndef __UNTILS_H__
#define __UNTILS_H__

#include <rthw.h>
#include <rtthread.h>
#include "M051Series.h"

#define BIT_MASK(n) (1UL<<(n))

extern GPIO_T *PORT_MAP[];

__INLINE int iport(char port, char pin);
__INLINE int oport(char port, char pin, char dat);

#endif
