/*
 * ETA/OS test application
 *
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/error.h>
#include <etaos/mem.h>
#include <etaos/irq.h>
#include <etaos/timer.h>
#include <etaos/pwm.h>
#include <etaos/platform.h>

#include <asm/pgm.h>
#include <asm/io.h>
#include <asm/pwm.h>

#include <uapi/etaos/test.h>

int main(void)
{
	struct pwm_state state;
	struct pwm *pwm;
	int idx = 0;

	printf_P(PSTR("Application started (m: %u)\n"), mm_heap_available());

	pgpio_pin_request(9);
	pgpio_direction_output(9, false);
	pgpio_pin_release(9);

	pgpio_pin_request(10);
	pgpio_direction_output(10, false);
	pgpio_pin_release(10);

	set_bit(PWM_DUTY_CYCLE_FRACTION_FLAG, &state.flags);
	set_bit(PWM_ENABLED_FLAG, &state.flags);
	set_bit(PWM_USE_OC_PINS, &state.flags);
	state.cycle.fraction = 2.0/3.0;

	pwm = pwm_get_chip(0);
	pwm_start(pwm, 5000);
	pwm_update_channel(pwm, 0, &state);

	state.cycle.fraction = 1.0/3.0;
	pwm_update_channel(pwm, 1, &state);

	while(idx < 5) {
		idx++;
		sleep(1000);
	}

	printf_P(PSTR(CALYPSO_EXIT));

	return -EOK;
}
