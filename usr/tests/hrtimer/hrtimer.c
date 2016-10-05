/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/stdio.h>
#include <etaos/hrtimer.h>
#include <etaos/mem.h>
#include <etaos/bitops.h>
#include <etaos/delay.h>

#include <asm/pgm.h>
#include <asm/io.h>

#include <uapi/etaos/test.h>

#define LED_PORT PORTB
#define LED_DDR  DDRB
#define LED_PIN  5

static volatile unsigned long counter = 0UL;

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
	counter += 1;
}

int main(void)
{
	struct hrtimer *timer;
	int i = 0;

	printf_P(PSTR("Application started!\n"));

	LED_DDR |= BIT(LED_PIN);
	set_led(false);

	timer = hrtimer_create(hr_sys_clk, 2000000, hrtimer1_handle_func,
			NULL, 0UL);

	for(; i < 5; i++) {
		delay(1000);
	}

	hrtimer_stop(timer);

	printf_P(PSTR("HR timer test: "));
	if(counter > 0UL)
		printf("[OK]\n");
	else
		printf("[ERR]\n");

	printf(CALYPSO_EXIT);
	return 0;
}

