/*
 * ETA/OS profiler for the scheduler.
 *
 * Author: Michel Megnes
 * Date: 18 - 09 - 2016
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/stdio.h>

#include <asm/io.h>
#include <asm/pgm.h>

#define LED_PORT PORTB
#define LED_DDR  DDRB
#define LED_PIN  7

static uint8_t profile_thread_stack[CONFIG_STACK_SIZE];
static struct thread *profile_thread;

static inline void set_led(bool val)
{
	if(val) {
		/* turn the led on */
		LED_PORT |= BIT(LED_PIN);
	} else {
		/* turn the led off */
		LED_PORT &= ~BIT(LED_PIN);
	}
}

THREAD(profile_thread_handle, arg)
{
	while(true) {
		set_led(false);
		yield();
		schedule();
	}
}

int main(void)
{
	struct thread *tp;

	printf_P(PSTR("Application started! (%u)\n"), mm_heap_available());
	LED_DDR |= BIT(LED_PIN);
	set_led(false);
	tp = current_thread();

	profile_thread = thread_create("profiler", &profile_thread_handle,
			tp, CONFIG_STACK_SIZE, profile_thread_stack, 120);
	
	while(true) {
		set_led(true);
		yield();
		schedule();
	}

	return -EOK;
}

