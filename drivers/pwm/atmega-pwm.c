/*
 *  ETA/OS - AVR PWM bus
 *  Copyright (C) 2017   Michel Megens <dev@bietje.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @addtogroup atmpwm
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/bitops.h>
#include <etaos/error.h>
#include <etaos/time.h>
#include <etaos/clocksource.h>
#include <etaos/pwm.h>
#include <etaos/mem.h>
#include <etaos/irq.h>
#include <etaos/init.h>
#include <etaos/stdio.h>

#include <asm/timer.h>
#include <asm/io.h>
#include <asm/pgm.h>
#include <asm/pwm.h>

#define ATMEGA_MAX_FREQ 8e6 //!< Maximum PWM frequency
#define ATMEGA_MIN_FREQ 1   //!< Minimum PWM frequency

/**
 * @brief ATmega PWM I/O registers.
 */
struct atmega_pwm_iobase {
	volatile uint16_t *icr; //!< Input compare
	volatile uint8_t *tccra, //!< Control A
			 *tccrb, //!< Control B
			 *timsk; //!< Interrupt mask
	uint8_t prescaler; //!< Prescaler bits
	uint16_t top; //!< OCR top value
};

/**
 * @brief ATmega PWM channel datastructure.
 */
struct atmega_pwm_channel {
	uint8_t irq_flag; //!< IRQ enable flag
	uint8_t oc_pin; //!< Output compare flag
	uint8_t irq; //!< IRQ number
	volatile uint16_t *ocr; //!< Output compare register.
	struct pwm_channel channel; //!< PWM channel
};

static const uint16_t prescaler_data[] = {
	1, 8, 64, 256, 1024,
};

static struct atmega_pwm_channel *channel_to_atmega_channel(struct pwm_channel *channel)
{
	return container_of(channel, struct atmega_pwm_channel, channel);
}

static inline uint32_t raw_atmega_pwm_calc_freq(uint16_t top, uint16_t ps)
{
	float freq;

	freq = CONFIG_FCPU;
	freq /= 2.0 * top * ps;
	return (uint32_t)freq;
}

static inline uint16_t atmega_pwm_guess_prescaler(struct pwm *pwm)
{
#define NUM_PRESCALERS 5
	struct atmega_pwm_iobase *io;
	int idx;
	uint16_t prescaler;
	uint32_t tfreq, bfreq;

	for(idx = 0; idx < NUM_PRESCALERS; idx++) {
		prescaler = prescaler_data[idx];
		tfreq = raw_atmega_pwm_calc_freq(1, prescaler);
		bfreq = raw_atmega_pwm_calc_freq(0xFFFF, prescaler);

		if(pwm->frequency > bfreq && pwm->frequency < tfreq) {
			io = (struct atmega_pwm_iobase*)pwm->iobase;
			io->prescaler = idx + 1;
			idx += 1;
			idx &= 0x3;

			return prescaler;
		}
	}

	return 0;
}

static inline uint16_t atmega_pwm_calc_top(struct pwm *pwm)
{
	float top, ps;

	ps = (float)atmega_pwm_guess_prescaler(pwm);
	top = CONFIG_FCPU;
	top = top / (2.0 * ps * (float)pwm->frequency);
	return (uint16_t)top;
}

static int atmega_pwm_update_hz(struct pwm *pwm, uint32_t freq)
{
	uint16_t top;
	struct atmega_pwm_iobase *io;

	if(freq > ATMEGA_MAX_FREQ || freq < ATMEGA_MIN_FREQ)
		return -EINVAL;

	pwm->frequency = freq;
	top = atmega_pwm_calc_top(pwm);
	io = (struct atmega_pwm_iobase*)pwm->iobase;
	io->top = top;
	*io->tccrb &= ~3; /* Disable timer */
	*(io->icr) = top;
	*io->tccrb |= io->prescaler;

	return -EOK;
}

static void atmega_pwm_start(struct pwm *pwm, uint32_t freq)
{
	/*struct atmega_pwm_iobase *io;

	io = (struct atmega_pwm_iobase*)pwm->iobase;*/
	atmega_pwm_update_hz(pwm, freq);
}

static void atmega_pwm_stop(struct pwm *pwm)
{
	struct atmega_pwm_iobase *io;

	io = (struct atmega_pwm_iobase*)pwm->iobase;
	*io->tccrb &= ~3; /* Disable timer */
}


static void atmega_pwm_update_channel(struct pwm *pwm, int chanid,
					struct pwm_state *state)
{
	float top, period;
	struct atmega_pwm_iobase *io;
	struct pwm_channel *channel;
	struct atmega_pwm_channel *atmchan;

	io = (struct atmega_pwm_iobase*)pwm->iobase;
	top = io->top;
	channel = pwm->channels[chanid];
	atmchan = channel_to_atmega_channel(channel);

	if(!test_bit(PWM_ENABLED_FLAG, &state->flags)) {
		*io->timsk &= ~atmchan->irq_flag;
		*io->tccra &= ~BIT(atmchan->oc_pin);
		clear_bit(PWM_ENABLED_FLAG, &channel->state.flags);
		return;
	}

	if(test_bit(PWM_DUTY_CYCLE_FRACTION_FLAG, &state->flags)) {
		top *= state->cycle.fraction;
	} else {
		/*
		 * Duty cycle is given in time units,
		 * calculate the fraction and apply it
		 */
		period = 1.0 / pwm->frequency;
		period *= 1.0e6; /* convert to us */
		period = (float)state->cycle.time / period;
		top *= period;
	}

	if(test_bit(PWM_USE_OC_PINS, &state->flags)) {
		*io->timsk &= ~atmchan->irq;
		set_bit(PWM_USE_OC_PINS, &channel->state.flags);
		*io->tccra |= BIT(atmchan->oc_pin);
	} else {
		*io->timsk |= atmchan->irq_flag;
	}

	*atmchan->ocr = (uint16_t)top;
}

static irqreturn_t atmega_pwm_irq(struct irq_data *irq, void *data)
{
	struct pwm_channel *channel;

	channel = (struct pwm_channel*)data;
	if(test_and_clear_bit(PWM_PIN_HIGH_FLAG, &channel->state.flags)) {
		gpio_pin_write(channel->output, 0);
	} else {
		gpio_pin_write(channel->output, 1);
		set_bit(PWM_PIN_HIGH_FLAG, &channel->state.flags);
	}
	return IRQ_HANDLED;
}

static __used void atmega_pwm_init_chip(struct pwm *pwm, int id)
{
	struct pwm_channel *chan;
	struct atmega_pwm_channel *atmchan;
	int idx = 0;

	spinlock_init(&pwm->lock);
	for(; idx < pwm->num; idx++) {
		chan = pwm->channels[idx];
		atmchan = channel_to_atmega_channel(chan);
		chan->base = pwm;
		chan->output = NULL;
		irq_request(atmchan->irq, &atmega_pwm_irq, 0UL, chan);
	}

	pwm_register_chip(pwm, id);
}

struct pwm_driver atmega_pwm_driver = {
	.start = &atmega_pwm_start,
	.stop  = &atmega_pwm_stop,
	.update_channel = &atmega_pwm_update_channel,
	.update_frequency = &atmega_pwm_update_hz,
};

#ifdef CONFIG_PWM0
struct atmega_pwm_iobase pwmio0 = {
	.icr  = &ICR1,
	.tccra = &TCCR1A,
	.tccrb = &TCCR1B,
	.timsk = &TIMSK1,
	.top = 8000,
	.prescaler = 1,
};

struct atmega_pwm_channel pwm0c0 = {
	.ocr = &OCR1A,
	.irq_flag = 2,
	.oc_pin = COM1A1,
	.irq = TIMER1_OCA_VECTOR_NUM,
};
struct atmega_pwm_channel pwm0c1 = {
	.ocr = &OCR1B,
	.irq_flag = 4,
	.oc_pin = COM1B1,
	.irq = TIMER1_OCB_VECTOR_NUM,
};

#ifdef COMC
struct atmega_pwm_channel pwm0c2 = {
	.ocr = &OCR1C,
	.irq_flag = 8,
	.oc_pin = COM1C1,
	.irq = TIMER1_OCC_VECTOR_NUM,
};
#endif


struct pwm pwm0 = {
	.iobase = (void*) &pwmio0,
#ifdef COMC
	.channels = {&pwm0c0.channel, &pwm0c1.channel, &pwm0c2.channel},
	.num = 3,
#else
	.channels = {&pwm0c0.channel, &pwm0c1.channel},
	.num = 2,
#endif
	.driver = &atmega_pwm_driver,
};

static void pwm0_init()
{
	atmega_pwm_init_chip(&pwm0,0);
	pwm0.src = pwm_get_clock0();
}

#else
#define pwm0_init()
#endif

#ifdef CONFIG_PWM1
struct atmega_pwm_iobase pwmio1 = {
	.icr  = &ICR3,
	.tccra = &TCCR3A,
	.tccrb = &TCCR3B,
	.timsk = &TIMSK3,
	.top = 8000,
	.prescaler = 1,
};

struct atmega_pwm_channel pwm1c0 = {
	.ocr = &OCR3A,
	.irq_flag = 2,
	.oc_pin = COM3A1,
	.irq = TIMER3_OCA_VECTOR_NUM,
};
struct atmega_pwm_channel pwm1c1 = {
	.ocr = &OCR3B,
	.irq_flag = 4,
	.oc_pin = COM3B1,
	.irq = TIMER3_OCB_VECTOR_NUM,
};

#ifdef COMC
struct atmega_pwm_channel pwm1c2 = {
	.ocr = &OCR3C,
	.irq_flag = 8,
	.oc_pin = COM3C1,
	.irq = TIMER3_OCC_VECTOR_NUM,
};
#endif

struct pwm pwm1 = {
	.iobase = (void*) &pwmio1,
#ifdef COMC
	.channels = {&pwm1c0.channel, &pwm1c1.channel, &pwm1c2.channel},
	.num = 3,
#else
	.channels = {&pwm1c0.channel, &pwm1c1.channel},
	.num = 2,
#endif
	.driver = &atmega_pwm_driver,
};

static void pwm1_init()
{
	atmega_pwm_init_chip(&pwm1, 1);
	pwm1.src = pwm_get_clock1();
}
#else
#define pwm1_init()
#endif

#ifdef CONFIG_PWM2
struct atmega_pwm_iobase pwmio2 = {
	.icr  = &ICR4,
	.tccra = &TCCR4A,
	.tccrb = &TCCR4B,
	.timsk = &TIMSK4,
	.top = 8000,
	.prescaler = 1,
};

struct atmega_pwm_channel pwm2c0 = {
	.ocr = &OCR4A,
	.irq_flag = 2,
	.oc_pin = COM4A1,
	.irq = TIMER4_OCA_VECTOR_NUM,
};
struct atmega_pwm_channel pwm2c1 = {
	.ocr = &OCR4B,
	.irq_flag = 4,
	.oc_pin = COM4B1,
	.irq = TIMER4_OCB_VECTOR_NUM,
};

#ifdef COMC
struct atmega_pwm_channel pwm2c2 = {
	.ocr = &OCR4C,
	.irq_flag = 8,
	.oc_pin = COM4C1,
	.irq = TIMER4_OCC_VECTOR_NUM,
};
#endif

struct pwm pwm2 = {
	.iobase = (void*) &pwmio2,
#ifdef COMC
	.channels = {&pwm2c0.channel, &pwm2c1.channel, &pwm2c2.channel},
	.num = 3,
#else
	.channels = {&pwm2c0.channel, &pwm2c1.channel},
	.num = 2,
#endif
	.driver = &atmega_pwm_driver,
};

static void pwm2_init()
{
	atmega_pwm_init_chip(&pwm2, 2);
	pwm2.src = pwm_get_clock2();
}

#else
#define pwm2_init()
#endif

#ifdef CONFIG_PWM3
struct atmega_pwm_iobase pwmio3 = {
	.icr  = &ICR5,
	.tccra = &TCCR5A,
	.tccrb = &TCCR5B,
	.timsk = &TIMSK5,
	.top = 8000,
	.prescaler = 1,
};

struct atmega_pwm_channel pwm3c0 = {
	.ocr = &OCR5A,
	.irq_flag = 2,
	.oc_pin = COM5A1,
	.irq = TIMER5_OCA_VECTOR_NUM,
};
struct atmega_pwm_channel pwm3c1 = {
	.ocr = &OCR5B,
	.irq_flag = 4,
	.oc_pin = COM5B1,
	.irq = TIMER5_OCB_VECTOR_NUM,
};

#ifdef COMC
struct atmega_pwm_channel pwm3c2 = {
	.ocr = &OCR5C,
	.irq_flag = 8,
	.oc_pin = COM5C1,
	.irq = TIMER5_OCC_VECTOR_NUM,
};
#endif

struct pwm pwm3 = {
	.iobase = (void*) &pwmio3,
#ifdef COMC
	.channels = {&pwm3c0.channel, &pwm3c1.channel, &pwm3c2.channel},
	.num = 3,
#else
	.channels = {&pwm3c0.channel, &pwm3c1.channel},
	.num = 2,
#endif
	.driver = &atmega_pwm_driver,
};

static void pwm3_init()
{
	atmega_pwm_init_chip(&pwm3, 3);
	pwm3.src = pwm_get_clock3();
}
#else
#define pwm3_init()
#endif

static void __used atmega_pwm_init(void)
{
	pwm0_init();
	pwm1_init();
	pwm2_init();
	pwm3_init();
}

device_init(atmega_pwm_init);

/** @} */
