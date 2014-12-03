/*
 * File      : oled.c
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

#include "oled.h"

/* Initial gpio oled pin  */
int rt_hw_oled_init(void)
{
    /* Configure the oled enable pin */
    set_oled_enable(OLED_OFF);
    GPIO_SetMode(OLED_ENABLE_PORT, OLED_ENABLE_PIN, GPIO_PMD_OUTPUT);
    return 0;
}

/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_oled_init);

#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT(set_oled_enable, );

#endif
