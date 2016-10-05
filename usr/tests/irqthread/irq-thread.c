/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/sched.h>
#include <etaos/thread.h>
#include <etaos/evm.h>
#include <etaos/mem.h>
#include <etaos/mutex.h>
#include <etaos/time.h>
#include <etaos/tick.h>
#include <etaos/ipm.h>
#include <etaos/irq.h>

#include <asm/pgm.h>
#include <asm/io.h>

#include <uapi/etaos/test.h>

volatile int counter = 0;

static irqreturn_t threaded_irq_handle(struct irq_data *data, void *not)
{
	if(counter < 2) {
		printf("[irq]:\tThreaded IRQ triggered\n");
		counter++;
	}
	return IRQ_HANDLED;
}

int main(void)
{
	printf_P(PSTR("Application started!\n"));

	irq_request(EXT_IRQ0_NUM, &threaded_irq_handle, IRQ_FALLING_MASK | 
			IRQ_THREADED_MASK, NULL);
	
	while(counter < 2) {
		printf("[main]:\tMain thread woken up!\n");
		sleep(1000);
	}

	printf(CALYPSO_EXIT);
	return 0;
}

