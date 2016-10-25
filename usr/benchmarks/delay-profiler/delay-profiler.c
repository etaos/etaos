/*
 * ETA/OS profiler for the scheduler.
 *
 * Author: Michel Megnes
 * Date: 18 - 09 - 2016
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/bitops.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/delay.h>
#include <etaos/mem.h>

#include <asm/io.h>
#include <asm/pgm.h>

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

int main(void)
{
	bool value = false;

	LED_DDR |= 1 << LED_PIN;
	printf_P(PSTR("Application started! (%u)\n"), mm_heap_available());

	while(true) {
		set_led(value);
		value = !value;
		delay_us(200);
	}

	return -EOK;
}

