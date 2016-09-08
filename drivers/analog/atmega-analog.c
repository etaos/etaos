/*
 *  ETA/OS - Analog ATmega AVR module
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @addtogroup analog-atmega
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/bitops.h>
#include <etaos/device.h>
#include <etaos/analog.h>
#include <etaos/init.h>
#include <etaos/mem.h>
#include <etaos/irq.h>

#include <asm/io.h>

static mutex_t avr_adc_conversion_mtx;

static int avr_adc_get(struct analog_pin *pin)
{
	int num = pin->num;
	uint8_t low, high;

	if(num > PIN_A7) {
		/*
		 * We ajust the MUX here. If the analog pins
		 * A8 to A15 are to be accessed, bit 0x20
		 * should be set.
		 */
		num -= 8;
		ADCSRB |= BIT(MUX5);
	} else {
		ADCSRB &= ~BIT(MUX5);
	}

	ADMUX = (num & 0x7) | BIT(REFS0);
	ADCSRA |= BIT(ADSC);
	mutex_wait(&avr_adc_conversion_mtx);

	low = ADCL;
	high = ADCH;

	return low | (high << 8);
}

static int avr_adc_ioctl(struct analog_chip *chip, unsigned long reg, void *arg)
{
	return -EINVAL;
}

static irqreturn_t avr_adc_irq(struct irq_data *irq, void *data)
{
	mutex_unlock_from_irq(&avr_adc_conversion_mtx);
	return IRQ_HANDLED;
}

static struct analog_chip avr_adc_chip = {
	.num = ADC_PINS,
	.dev.name = "avr-adc",
	.ioctl = &avr_adc_ioctl,
	.get = &avr_adc_get,
};

static void __used avr_adc_init(void)
{
	mutex_init(&avr_adc_conversion_mtx);
	analog_chip_init(&avr_adc_chip);
	analog_syschip = &avr_adc_chip;

	irq_request(ADC_COMPLETED_NUM, &avr_adc_irq, IRQ_FALLING_MASK, NULL);
	/* set the prescaler to 128 */
	ADCSRA |= BIT(ADPS0) | BIT(ADPS1) | BIT(ADPS2);
	ADCSRA |= BIT(ADIE);
	ADCSRA |= BIT(ADEN);
}

module_init(avr_adc_init);

/** @} */

