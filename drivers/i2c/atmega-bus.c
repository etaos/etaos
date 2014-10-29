/*
 *  ETA/OS - I2C bus
 *  Copyright (C) 2014   Michel Megens
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

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/types.h>
#include <etaos/irq.h>
#include <etaos/i2c.h>
#include <etaos/list.h>
#include <etaos/bitops.h>
#include <etaos/time.h>
#include <etaos/tick.h>
#include <etaos/mutex.h>
#include <etaos/irq.h>

#include <asm/io.h>
#include <asm/irq.h>

#define TWINT	7
#define TWEA	6
#define TWSTA	5
#define TWSTO	4
#define TWWC	3
#define TWEN	2
#define TWIE	0

/* status reg */
#define TWPS1	1
#define TWPS0	0
#define I2C_NOINFO 0xF8

/* addr reg */
#define TWGCE 	0

#define I2C_PRES_1  0
#define I2C_PRES_4  1
#define I2C_PRES_16 2
#define I2C_PRES_64 3

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#define I2C_FRQ(x, n) \
		(F_CPU/(16+(2*x*n)))
	
#define I2C_CALC_TWBR(__freq, __pres) \
	(F_CPU - (16*__freq)) / (2*__pres*__freq)
#endif

#define TWBR  MEM_IO8(0xB8)
#define TWCR  MEM_IO8(0xBC)
#define TWSR  MEM_IO8(0xB9)
#define TWDR  MEM_IO8(0xBB)
#define TWAR  MEM_IO8(0xBA)
#define TWAMR MEM_IO8(0xBD)

/* Master */
#define TW_START		0x08
#define TW_REP_START		0x10

/* Master Transmitter */
#define TW_MT_SLA_ACK		0x18
#define TW_MT_SLA_NACK		0x20
#define TW_MT_DATA_ACK		0x28
#define TW_MT_DATA_NACK		0x30
#define TW_MT_ARB_LOST		0x38

/* Master Receiver */
#define TW_MR_ARB_LOST		0x38
#define TW_MR_SLA_ACK		0x40
#define TW_MR_SLA_NACK		0x48
#define TW_MR_DATA_ACK		0x50
#define TW_MR_DATA_NACK		0x58

/* Slave Transmitter */
#define TW_ST_SLA_ACK		0xA8
#define TW_ST_ARB_LOST_SLA_ACK	0xB0
#define TW_ST_DATA_ACK		0xB8
#define TW_ST_DATA_NACK		0xC0
#define TW_ST_LAST_DATA		0xC8

/* Slave Receiver */
#define TW_SR_SLA_ACK		0x60
#define TW_SR_ARB_LOST_SLA_ACK	0x68
#define TW_SR_GCALL_ACK		0x70
#define TW_SR_ARB_LOST_GCALL_ACK 0x78
#define TW_SR_DATA_ACK		0x80
#define TW_SR_DATA_NACK		0x88
#define TW_SR_GCALL_DATA_ACK	0x90
#define TW_SR_GCALL_DATA_NACK	0x98
#define TW_SR_STOP		0xA0

/* Misc */
#define TW_NO_INFO		0xF8
#define TW_BUS_ERROR		0x00

#define TWGO (BIT(TWINT) | BIT(TWEN) | BIT(TWIE))

static volatile uint8_t tw_mm_error = 0;
static volatile bool tw_if_busy = false;
static volatile uint8_t msg_index = 0;
static volatile uint8_t msg_num = 0;
static struct i2c_msg *atmega_xfer_msgs = NULL;

static mutex_t mtr_xfer_mutex;

#define i2c_msg_byte(_msg, _idx) (((uint8_t*)msg->buff)[_idx])
#define i2c_write_msg(_msg, _idx, _b) (((uint8_t*)msg->buff)[_idx] = _b)

static irqreturn_t atmega_i2c_stc_irq(struct irq_data *irq, void *data)
{
	struct i2c_msg *msg;
	uint8_t twsr;
	register uint8_t twcr = TWCR;

	twsr = TWSR;
	twsr &= 0xF8;
	msg = &atmega_xfer_msgs[msg_index];

	switch(twsr) {
	case TW_START:
	case TW_REP_START:
		tw_if_busy = true;
		msg->idx = 0;

		if(test_bit(I2C_RD_FLAG, &msg->flags)) {
			TWDR = msg->dest_addr | 0x1;
		}
		else
			TWDR = msg->dest_addr;

		TWCR = TWGO | (twcr & BIT(TWEA));
		break;

	case TW_MT_SLA_ACK:
	case TW_MT_DATA_ACK:
		if(msg->idx < msg->len) {
			TWDR = i2c_msg_byte(msg, msg->idx);
			msg->idx++;
			TWCR = TWGO | (twcr & BIT(TWEA));
			break;
		}

		/* all outgoing bytes have been sent */
		msg->len = 0;
		if((msg_index + 1) < msg_num) {
			msg_index++;
			TWCR = TWGO | (twcr & BIT(TWEA)) | BIT(TWSTA);
			break;
		}

	case TW_MT_SLA_NACK:
	case TW_MT_DATA_NACK:
	case TW_MR_SLA_NACK:
		if(twsr == TW_MT_SLA_NACK || twsr == TW_MR_SLA_NACK) {
			tw_mm_error = -EINVAL;
			msg->len = 0;
			msg_num = 0;
		} else {
			tw_mm_error = -EOK;
		}

		mutex_unlock_from_irq(&mtr_xfer_mutex);
		TWCR = TWGO | BIT(TWSTO);
		tw_if_busy = 0;
		break;

	/* lost bus control */
	case TW_MT_ARB_LOST:
		TWCR |= TWSTA;
		tw_if_busy = 0;
		break;

	case TW_MR_DATA_ACK:
		i2c_write_msg(msg, msg->idx, TWDR);
		msg->idx++;

	case TW_MR_SLA_ACK:
		if((msg->idx + 1) < msg->len)
			TWCR = TWGO | BIT(TWEA);
		else
			TWCR = TWGO;
		break;

	case TW_MR_DATA_NACK:
		i2c_write_msg(msg, msg->idx, TWDR);
		msg->idx++;
		msg->len = 0;
		
		if((msg_index + 1) < msg_num) {
			msg_index++;
			TWCR = TWGO | BIT(TWEA) | BIT(TWSTA);
			break;
		}

		mutex_unlock_from_irq(&mtr_xfer_mutex);

		TWCR = TWGO | BIT(TWSTO);
		tw_if_busy = false;
		tw_mm_error = -EOK;
		break;
	
	default:
		mutex_unlock_from_irq(&mtr_xfer_mutex);
		tw_mm_error = -EINVAL;
		tw_if_busy = false;
		break;
	}

	return IRQ_HANDLED;
}

static int atmega_i2c_xfer(struct i2c_bus *bus, struct i2c_msg *msgs, int num)
{
	int ret;
	uint8_t twcr, twsr;

	twcr = 0;
	twsr = 0;
	while(tw_if_busy);

	irq_enter_critical();
	atmega_xfer_msgs = msgs;
	msg_index = 0;
	msg_num = num;

	if(!tw_if_busy) {
		twcr = TWCR;
		twsr = TWSR;
		if((twsr & 0xF8) == TW_NO_INFO)
			TWCR = BIT(TWEN) | BIT(TWIE) | BIT(TWSTA) | 
				(twcr & BIT(TWSTO));

	}
	irq_exit_critical();

	mutex_wait(&mtr_xfer_mutex);
	if(tw_mm_error)
		ret = tw_mm_error;
	else
		ret = -EOK; 
	
	return ret;
}

static unsigned char atmega_prescalers[] = {
	1,
	4,
	16,
	64,
};

static void atmega_i2c_setspeed(uint32_t speed)
{
	unsigned char i;
	unsigned char pres, bitrate_reg;
	uint32_t tmp_speed;

	for(i = 0; i < sizeof(atmega_prescalers); i++) {
		pres = atmega_prescalers[i];
		if(!pres) {
			i = -1;
			return;
		}
		bitrate_reg = (F_CPU - (16*speed)) / (2*pres*speed);
		tmp_speed = F_CPU / (( (2*bitrate_reg) * pres) + 16);
		if(tmp_speed == speed)
			break;
	}

	if(i < sizeof(atmega_prescalers) && tmp_speed == speed) {
		TWSR = i & 0x3;
		TWBR = bitrate_reg;
	}
}

static int atmega_i2c_ctrl(struct i2c_bus *bus, unsigned long ctrl, void *val)
{
	int retval;

	switch(ctrl) {
	case I2C_SET_SPEED:
		atmega_i2c_setspeed(*((uint32_t*)val));
		retval = -EOK;
		break;

	default:
		retval = -EINVAL;
		break;
	}

	return retval;
}

static struct i2c_bus atmega_i2c_bus = {
	.xfer = &atmega_i2c_xfer,
	.ctrl = &atmega_i2c_ctrl,
};

#define ATMEGA_I2C_TMO 500
#define ATMEGA_I2C_RETRY 3
#define ATMEGA_SPEED_DEFAULT 100000

void atmega_i2c_init(void)
{
	atmega_i2c_bus.timeout = ATMEGA_I2C_TMO;
	atmega_i2c_bus.retries = ATMEGA_I2C_RETRY;
	mutex_init(&mtr_xfer_mutex);

	i2c_init_bus(&atmega_i2c_bus);
	atmega_i2c_setspeed(ATMEGA_SPEED_DEFAULT);
	irq_request(TWI_STC_VECTOR_NUM, &atmega_i2c_stc_irq, IRQ_FALLING_MASK,
			&atmega_i2c_bus);
	TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWIE);

	i2c_sysbus = &atmega_i2c_bus;
}

