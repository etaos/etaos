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

#include <asm/io.h>

static irqreturn_t threaded_irq_handle(struct irq_data *data, void *not)
{
	printf("Threaded IRQ triggered\n");
	return IRQ_HANDLED;
}

int main(void)
{
	printf("Application started (M:%u)!\n", mm_heap_available());

	irq_request(EXT_IRQ0_NUM, &threaded_irq_handle, IRQ_FALLING_MASK | 
			IRQ_THREADED_MASK, NULL);
	
	while(true) {
		printf("Main thread woken up!\n");
		sleep(1000);
	}

	return 0;
}

