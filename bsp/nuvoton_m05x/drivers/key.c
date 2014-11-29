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
#define KEY_SW_PORT P3
#define KEY_SW_BIT_IDX 3
#define KEY_SW_PIN P33
#define KEY_SW_BIT bits_mask(KEY_SW_BIT_IDX)
#define KEY_SW_SET(dat) (KEY_SW_PIN = dat)

/*
    KEY power: P34
*/
#define KEY_PWR_PORT P3
#define KEY_PWR_BIT_IDX 4
#define KEY_PWR_PIN P34
#define KEY_PWR_BIT bits_mask(KEY_PWR_BIT_IDX)
#define KEY_PWR_SET(dat) (KEY_PWR_PIN = dat)
/*
    KEY down: P35
*/
#define KEY_DOWN_PORT P3
#define KEY_DOWN_BIT_IDX 5
#define KEY_DOWN_PIN P35
#define KEY_DOWN_BIT bits_mask(KEY_DOWN_BIT_IDX)
#define KEY_DOWN_SET(dat) (KEY_DOWN_PIN = dat)
/*
    KEY up: P43
*/
#define KEY_UP_PORT P4
#define KEY_UP_BIT_IDX 3
#define KEY_UP_PIN P43
#define KEY_UP_BIT bits_mask(KEY_UP_BIT_IDX)
#define KEY_UP_SET(dat) (KEY_UP_PIN = dat)

/* Initial gpio key pin  */
int rt_hw_key_init(void)
{
#ifdef KEY_SW_PORT
    /* Configure the key sw pin */
    KEY_SW_SET(1);
    GPIO_SetMode(KEY_SW_PORT, KEY_SW_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_SW_PORT, KEY_SW_BIT_IDX, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_P2P3P4_IRQn);
#endif
    
#ifdef KEY_PWR_PORT
    /* Configure the key power pin */
    KEY_PWR_SET(1);
    GPIO_SetMode(KEY_PWR_PORT, KEY_PWR_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_PWR_PORT, KEY_PWR_BIT_IDX, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_P2P3P4_IRQn);
#endif

#ifdef KEY_DOWN_PORT
    /* Configure the key down pin */
    KEY_DOWN_SET(1);
    GPIO_SetMode(KEY_DOWN_PORT, KEY_DOWN_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_DOWN_PORT, KEY_DOWN_BIT_IDX, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_P2P3P4_IRQn);
#endif
    
#ifdef KEY_UP_PORT
    /* Configure the key up pin */
    KEY_UP_SET(1);
    GPIO_SetMode(KEY_UP_PORT, KEY_UP_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_UP_PORT, KEY_UP_BIT_IDX, GPIO_INT_FALLING);
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
            if (KEY_SW_PIN) {
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
            if (KEY_PWR_PIN) {
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
            if (KEY_DOWN_PIN) {
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
            if (KEY_UP_PIN) {
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
