/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 *         A simple Contiki application for meassuring latencies in Contiki
 * \author Oleg Hahm <oleg@riot-os.org>
 */

#include <stdio.h>
#include <msp430.h>
#include "contiki.h"
#include "sys/mt.h"
#include "isr_compat.h"
#include "latency.h"
    
static struct mt_thread rt_thread;

PROCESS(testing_process, "Start testing process");
AUTOSTART_PROCESSES(&testing_process);

ISR(PORT1, port1_isr)
{
    TRIGGER_PIN_ON;
    P1IFG &= ~BIT7;
    mt_exec(&rt_thread);
}

/*---------------------------------------------------------------------------*/
static void thread_rt(void *data)
{
    puts("I'm waiting to get informed");
    while (1) {
        mt_yield();
        TRIGGER_PIN_OFF;
    }
    mt_exit();
}

static void thread_busy(void *data)
{
         puts("I'm so busy, I will just go on and on and on...");
         puts("I stop");
    mt_exit();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(testing_process, ev, data)
{
    static struct mt_thread busy_thread;

    static struct etimer timer;

    PROCESS_BEGIN();

    mt_init();
    mt_start(&rt_thread, thread_rt, NULL);
    mt_start(&busy_thread, thread_busy, NULL);

    TRIGGER_PIN_INIT;
    TRIGGER_PIN_OFF;
    
    P1SEL = 0x00;       /* must be <> 1 to use interrupts */
    P1DIR &= ~BIT7;     /* set to input */
    P1IES &= ~BIT7;     /* Enables external interrupt on high edge */
    P1IE  |= BIT7;      /* Enable interrupt */
    P1IFG &= ~BIT7;     /* Clears the interrupt flag */

    etimer_set(&timer, CLOCK_SECOND / 2);

    mt_exec(&busy_thread);

    puts("Finished, tidying up");
    mt_stop(&rt_thread);
    mt_stop(&busy_thread);
    mt_remove();

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
