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
#include <etaos/i2c.h>
#include <etaos/list.h>
#include <etaos/bitops.h>
#include <etaos/time.h>
#include <etaos/tick.h>

#include <asm/io.h>

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

#define I2C_PRES_1 B0
#define I2C_PRES_4 B1
#define I2C_PRES_16 B10
#define I2C_PRES_64 B11

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

static int msg_index;
static struct i2c_msg *atmega_xfer_msgs;
static volatile uint8_t twi_state;
static volatile uint8_t twi_slarw;
static volatile uint8_t twi_stop;
static volatile uint8_t twi_rep_start;

static struct i2c_bus atmega_i2c_bus = {

};

