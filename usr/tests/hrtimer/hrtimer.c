/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/hrtimer.h>
#include <etaos/thread.h>
#include <etaos/sched.h>
#include <etaos/mem.h>

#include <asm/pgm.h>
#include <asm/io.h>

#define LED_PORT PORTB
#define LED_DDR  DDRB
#define LED_PIN  5

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

static void hrtimer1_handle_func(struct hrtimer *hrt, void *arg)
{
	static bool value = true;

	set_led(value);
	value = !value;
}

int main(void)
{
	struct hrtimer *timer;
	struct hrtimer_source *src;
	int i;

	printf_P(PSTR("Application started (M:%u)!\n"), mm_heap_available());

	LED_DDR |= BIT(LED_PIN);
	set_led(false);

	hrtimer_create(hr_sys_clk, 2000000, hrtimer1_handle_func,
			NULL, 0UL);

	src = container_of(hr_sys_clk, struct hrtimer_source, base);
	timer = src->timers;

	for(i = 0; timer; i++) {
		printf_P(PSTR("Timer %i has %u ticks left.\n"),
				i, timer->ticks);
		timer = timer->next;
	}

	while(true) {
		printf("Ola\n");
		sleep(1000);
	}

	return 0;
}

