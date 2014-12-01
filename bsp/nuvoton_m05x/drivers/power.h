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

void pwr_adc_boost_configure(void);
void pwr_adc_load_configure(void);
void pwr_adc_start(void);
void pwr_adc_stop(void);

#endif
