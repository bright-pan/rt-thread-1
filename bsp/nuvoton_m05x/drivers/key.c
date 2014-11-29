/*
 * File      : key.c
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

#include <stdlib.h>
#include "key.h"
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

/* Initial gpio key pin  */
int rt_hw_key_init(void)
{
#ifdef KEY_SW_PORT
    /* Configure the key sw pin */
    set_key_sw(1);
    GPIO_SetMode(KEY_SW_PORT, KEY_SW_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_SW_PORT, KEY_SW_PIN, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_P2P3P4_IRQn);
#endif
    
#ifdef KEY_PWR_PORT
    /* Configure the key power pin */
    set_key_pwr(1);
    GPIO_SetMode(KEY_PWR_PORT, KEY_PWR_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_PWR_PORT, KEY_PWR_PIN, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_P2P3P4_IRQn);
#endif

#ifdef KEY_DOWN_PORT
    /* Configure the key down pin */
    set_key_down(1);
    GPIO_SetMode(KEY_DOWN_PORT, KEY_DOWN_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_DOWN_PORT, KEY_DOWN_PIN, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_P2P3P4_IRQn);
#endif
    
#ifdef KEY_UP_PORT
    /* Configure the key up pin */
    set_key_up(1);
    GPIO_SetMode(KEY_UP_PORT, KEY_UP_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_UP_PORT, KEY_UP_PIN, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_P2P3P4_IRQn);
#endif
    return 0;
}

/**
 * @brief       Port0/Port1 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Port0/Port1 default IRQ, declared in startup_M051Series.s.
 */
void GPIOP0P1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    /* To check if P1.3 interrupt occurred */
    if(GPIO_GET_INT_FLAG(P1, BIT3))
    {
        GPIO_CLR_INT_FLAG(P1, BIT3);
        //printf("P1.3 INT occurred.\n");
    }
    else
    {
        /* Un-expected interrupt. Just clear all PORT0, PORT1 interrupts */
        P0->ISRC = P0->ISRC;
        P1->ISRC = P1->ISRC;
        RT_DEBUG_LOG(DEBUG_KEY, ("Un-expected PORT0, PORT1 interrupts.\n"));
    }
    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * @brief       Port2/Port3/Port4 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Port2/Port3/Port4 default IRQ, declared in startup_M051Series.s.
 */
void GPIOP2P3P4_IRQHandler(void)
{
    register int cnts, i;
    /* enter interrupt */
    rt_interrupt_enter();

    if(GPIO_GET_INT_FLAG(KEY_SW_PORT, KEY_SW_BIT))
    {
        cnts = 0;
        for (i = 0; i < 5000; i++) {
            if (get_key_sw()) {
                cnts++;
            } else {
                cnts--;
            }
            if (cnts > 500)
            {
                RT_DEBUG_LOG(DEBUG_KEY, ("it is key switch...\n"));
                break;
            }
            if (cnts < -500)
            {
                break;
            }
        }
        GPIO_CLR_INT_FLAG(KEY_SW_PORT, KEY_SW_BIT);
    }
    else if(GPIO_GET_INT_FLAG(KEY_PWR_PORT, KEY_PWR_BIT))
    {
        cnts = 0;
        for (i = 0; i < 5000; i++) {
            if (get_key_pwr()) {
                cnts++;
            } else {
                cnts--;
            }
            if (cnts > 500)
            {
                RT_DEBUG_LOG(DEBUG_KEY, ("it is key power...\n"));
                break;
            }
            if (cnts < -500)
            {
                break;
            }
        }
        GPIO_CLR_INT_FLAG(KEY_PWR_PORT, KEY_PWR_BIT);
    }
    else if(GPIO_GET_INT_FLAG(KEY_DOWN_PORT, KEY_DOWN_BIT))
    {
        cnts = 0;
        for (i = 0; i < 5000; i++) {
            if (get_key_down()) {
                cnts++;
            } else {
                cnts--;
            }
            if (cnts > 500)
            {
                RT_DEBUG_LOG(DEBUG_KEY, ("it is key down...\n"));
                break;
            }
            if (cnts < -500)
            {
                break;
            }
        }
        GPIO_CLR_INT_FLAG(KEY_DOWN_PORT, KEY_DOWN_BIT);
    }
    else if(GPIO_GET_INT_FLAG(KEY_UP_PORT, KEY_UP_BIT))
    {
        cnts = 0;
        for (i = 0; i < 5000; i++) {
            if (get_key_up()) {
                cnts++;
            } else {
                cnts--;
            }
            if (cnts > 500)
            {
                RT_DEBUG_LOG(DEBUG_KEY, ("it is key up...\n"));
                break;
            }
            if (cnts < -500)
            {
                break;
            }
        }
        GPIO_CLR_INT_FLAG(KEY_UP_PORT, KEY_UP_BIT);
    }
    else
    {
        /* Un-expected interrupt. Just clear all PORT2, PORT3 and PORT4 interrupts */
        P2->ISRC = P2->ISRC;
        P3->ISRC = P3->ISRC;
        P4->ISRC = P4->ISRC;
        RT_DEBUG_LOG(DEBUG_KEY, ("Un-expected PORT2, PORT3 and PORT4 interrupts.\n"));
    }

    /* leave interrupt */
    rt_interrupt_leave();
}

/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_key_init);
