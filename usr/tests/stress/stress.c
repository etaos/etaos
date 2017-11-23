/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/unistd.h>
#include <etaos/stdlib.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/thread.h>
#include <etaos/condition.h>
#include <etaos/event.h>
#include <etaos/hrtimer.h>
#include <etaos/irq.h>
#include <etaos/mem.h>
#include <etaos/mutex.h>
#include <etaos/time.h>
#include <etaos/tick.h>
#include <etaos/gpio.h>
#include <etaos/vfs.h>
#include <etaos/platform.h>
#include <etaos/analog.h>
#include <etaos/tmp36.h>
#include <etaos/ipm.h>
#include <etaos/panic.h>
#include <etaos/xorlist.h>
#include <etaos/python.h>
#include <etaos/pwm.h>
#include <etaos/dht11.h>

#include <etaos/sram/23k256.h>

#include <asm/pgm.h>
#include <asm/io.h>
#include <asm/pwm.h>

#include "eepromify.h"

static struct ipm_queue ipm_q;
static uint8_t ee_value = 0x0;

static const char sram_test[] = "sram test";
static const char ee_test[]   = "ee test";

#define EE_BYTE_ADDR 0x20
#define EE_STRING_ADDR 0x40

#define SRAM_BYTE_ADDR 0x100
#define SRAM_STRING_ADDR 0x200

#define IRQ_THREAD_TEXT "IRQ thread triggerd!"
#define IRQ_THREAD_FILENAME "/ramfs/irqthread.txt"

extern int ee_stress_read_byte(uint8_t addr, uint8_t *rb);
extern int ee_stress_write_byte(uint8_t addr, uint8_t byte);
extern int ee_stress_write(uint8_t addr, const void *buff, size_t len);
extern int ee_stress_read(uint8_t addr, void *buff, size_t len);

extern int sram_stress_read_byte(uint16_t addr, uint8_t *store);
extern int sram_stress_write_byte(uint16_t addr, uint8_t byte);
extern int sram_stress_read(uint16_t addr, void *buff, size_t len);
extern int sram_stress_write(uint16_t addr, const void *buff, size_t len);

static char *current_thread_name(void)
{
	return current_thread()->name;
}

static void dht_test(void)
{
	int fd;
	float f;

	fd = open("/dev/dht11", _FDEV_SETUP_RW);
	if(fd < 0)
		panic_P(PSTR("Couldn't open DHT device!\n"));

	read(fd, &f, sizeof(f));
	close(fd);

	printf_P(PSTR("[2][%s]: Humidity: %f%%\n"), current_thread_name(), f);
}

static void bmp_test(void)
{
	int fd;
	int32_t bmp;

	fd = open("/dev/bmp085", _FDEV_SETUP_RW);
	if(fd < 0)
		panic_P(PSTR("Couldn't open BMP device!\n"));

	read(fd, &bmp, sizeof(bmp));
	close(fd);

	printf_P(PSTR("[0][%s]:   Pressure: %liPa\n"), current_thread_name(), bmp);
}

static void dht_setup(void)
{
	int fd;
	struct gpio_pin *pin;

	pin = platform_pin_to_gpio(7);

	fd = open("/dev/dht11", _FDEV_SETUP_RW);
	if(fd < 0)
		panic_P(PSTR("Couldn't open DHT device!\n"));

	ioctl(filep(fd), DHT_MODE_DHT22, NULL);
	ioctl(filep(fd), DHT_SET_PIN, pin);
	close(fd);
}

static irqreturn_t threaded_irq_handle(struct irq_data *data, void *arg)
{
	const char *filename = arg;
	char *buffer;
	int fd;
	size_t length;

	fd = open(filename, _FDEV_SETUP_RWA);
	if(fd > 0) {
		length = ftell(filep(fd));
		buffer = kzalloc(length);
		read(fd, buffer, length);
		close(fd);

		printf_P(PSTR("[irq]:       %s\n"), buffer);
		kfree(buffer);
	} else {
		fprintf_P(stderr, PSTR("RAMFS read failed!\n"));
	}

	return IRQ_HANDLED;
}

THREAD(test_th_handle2, arg)
{
	unsigned char sram_readback = 0;
	unsigned long rand;
	int fd;
	struct file *file;
	size_t flen;
	char *romdata = NULL;
	float sram_entry = 3.1415F;
	struct tm *tm;
	time_t now;

	sram_stress_write_byte(SRAM_BYTE_ADDR, 0x78);
	sram_stress_write(SRAM_STRING_ADDR, &sram_entry, sizeof(sram_entry));

	while(true) {
		sram_stress_read_byte(SRAM_BYTE_ADDR, &sram_readback);
		rand = random_m(100);
		printf_P(PSTR("[2][%s]: SRAM: %u :: RAND: %u\n"),
				current_thread_name(), sram_readback, rand);

		dht_test();
		fd = open("/romfs/test.txt", O_RDONLY);
		if(fd >= 0) {
			file = filep(fd);

			lseek(file, 0, SEEK_END);
			flen = ftell(file);
			lseek(file, 0, SEEK_SET);

			romdata = kzalloc(flen + 1);
			read(fd, romdata, flen);
			romdata[flen] = '\0';
			close(fd);
		}

		printf_P(PSTR("[2][%s]: ROMFS: %s\n"),
				current_thread_name(), romdata);
		kfree(romdata);
		now = time(NULL);
		tm = localtime(&now);

		printf_P(PSTR("[2][%s]: Date: %i-%i-%i, %i:%i\n"),
				current_thread_name(),
					tm->tm_mday,
					tm->tm_mon,
					tm->tm_year + 1900,
					tm->tm_hour,
					tm->tm_min);

		sleep(1000);
	}
}

static struct condition dbg_condi;
static float main_thr_data = 0.0f;

THREAD(test_th_handle, arg)
{
	struct ipm msg;
	uint8_t readback = 0;
	float sram_data;
	char ee_string[sizeof(ee_test)];
	int fd;
	time_t now;
	float local;

	fd = open(IRQ_THREAD_FILENAME, _FDEV_SETUP_RW);
	if(fd > 0) {
		write(fd, IRQ_THREAD_TEXT, sizeof(IRQ_THREAD_TEXT));
		close(fd);
	} else {
		panic_P(PSTR("Couldn't open RAMFS file!\n"));
	}

	irq_request(EXT_IRQ4_VECTOR_NUM, &threaded_irq_handle,
			IRQ_THREADED_MASK | IRQ_RISING_MASK, IRQ_THREAD_FILENAME);
	irq_assign_pin(EXT_IRQ4_VECTOR_NUM, platform_pin_to_gpio(2));
	time(&now);

	while(true) {
		ee_stress_read_byte(EE_BYTE_ADDR, &readback);
		printf_P(PSTR("[%u][%s]: ee-byte read: %u\n"), 1,
				current_thread_name(), readback);

		condition_lock(&dbg_condi);
		while(main_thr_data == 0.0f)
			condition_wait(&dbg_condi);

		local = main_thr_data;
		main_thr_data = 0.0f;
		condition_unlock(&dbg_condi);

		printf_P(PSTR("[1][%s]: main-data: %f - "),
				current_thread_name(), local);

		ipm_get_msg(&ipm_q, &msg);
		ipm_reset_queue(&ipm_q);

		write(to_fd(stdout), msg.data, msg.len);

		sram_stress_read(SRAM_STRING_ADDR, &sram_data,
				sizeof(sram_data));
		ee_stress_read(EE_STRING_ADDR, ee_string, sizeof(ee_string));
		printf_P(PSTR("[1][%s]: SRAM::EEPROM %f::%s\n"),
				current_thread_name(),
				sram_data, ee_string);


		if(time_after(time(NULL), now + 5ULL)) {
			time(&now);
			irq_soft_trigger(EXT_IRQ4_VECTOR_NUM);
		}

		sleep(500);
	}
}

struct xorlist_test_entry {
	struct xorlist_head entry;
	int i;
};

static void test_xorlist(void)
{
	struct xorlist_head *head, *prev, *cur, *tmphead;
	struct xorlist_test_entry *entry;
	int i, total;

	entry = kzalloc(sizeof(*entry));
	entry->i = 0;
	cur = head = &entry->entry;
	prev = NULL;

	for(total = 0, i = 1; i < 20; i++) {
		entry = kzalloc(sizeof(*entry));
		entry->i = i;
		xlist_add(prev, cur, &entry->entry);
		prev = cur;
		cur = &entry->entry;
		total += i;
	}

	/* Iterate back over */
	prev = cur = NULL;
	xlist_for_each(prev, head, cur, tmphead) {
		entry = container_of(cur, struct xorlist_test_entry, entry);
		total -= entry->i;
	}

	if(total)
		fprintf(stderr, "xlist test failed!\n");
}

#define PREEMPT_PIN 9
static volatile unsigned long preempt_counter;
THREAD(preempt_thread, arg)
{
	int value = 0;
	struct gpio_pin *pin;

	pgpio_pin_request(PREEMPT_PIN);
	pgpio_direction_output(PREEMPT_PIN, value);
	pgpio_pin_release(PREEMPT_PIN);

	pin = platform_pin_to_gpio(PREEMPT_PIN);
	preempt_counter = 0UL;

	while(true) {
		__raw_gpio_pin_write(pin, value);
		value = !value;
		preempt_counter++;
	}
}

static struct gpio_pin *led_pin;
static bool led_pin_value = true;

static void hrtimer1_handle_func(struct hrtimer *hrt, void *arg)
{
	__raw_gpio_pin_write(led_pin, led_pin_value);
	led_pin_value = !led_pin_value;
}

static void setup_pwm(void)
{
	struct pwm_state state;
	struct pwm *pwm;

	pgpio_pin_request(12);
	pgpio_direction_output(12, false);
	pgpio_pin_release(12);

	pgpio_pin_request(11);
	pgpio_direction_output(11, false);
	pgpio_pin_release(11);

	set_bit(PWM_DUTY_CYCLE_FRACTION_FLAG, &state.flags);
	set_bit(PWM_ENABLED_FLAG, &state.flags);
	set_bit(PWM_USE_OC_PINS, &state.flags);
	state.cycle.fraction = 2.0/3.0;

	pwm = pwm_get_chip(0);
	pwm_start(pwm, 5000);
	pwm_update_channel(pwm, 0, &state);

	state.cycle.fraction = 1.0/3.0;
	pwm_update_channel(pwm, 1, &state);
}

static time_t read_time_from_config(void)
{
	int fd;
	time_t rv;

	fd = open("/dev/atmega-eeprom", _FDEV_SETUP_RW);

	if(fd < 0)
		panic_P(PSTR("Couldn't open ATmega EEPROM!"));

	lseek(filep(fd), (size_t)&timestamp, SEEK_SET);
	read(fd, &rv, sizeof(time_t));
	close(fd);

	return rv;
}

int main(void)
{
	const char * ip_msg = "IPM message\n";
	uint8_t readback = 0;
	time_t now;
	float temperature;

	printf_P(PSTR("Application started (m: %u)\n"), mm_heap_available());

	dht_setup();
	test_xorlist();
	setup_pwm();
	condition_init(&dbg_condi);
	ipm_queue_init(&ipm_q, 2);
	ee_stress_write_byte(EE_BYTE_ADDR, 0xAC);
	ee_stress_write(EE_STRING_ADDR, ee_test, strlen(ee_test)+1);

	thread_create("test-1", &test_th_handle, NULL, NULL);
	thread_create("test-2", &test_th_handle2, NULL, NULL);
	thread_create("preempt", &preempt_thread, NULL, NULL);

	python_init();
	python_start("main");

	getc(stdin);
	now = read_time_from_config();
	stime(now);

	pgpio_pin_request(10);
	pgpio_direction_output(10, false);
	led_pin = platform_pin_to_gpio(10);
	pgpio_pin_release(10);
	hrtimer_create(hr_sys_clk, 2000000, hrtimer1_handle_func,
			NULL, 0UL);

	while(true) {
		condition_lock(&dbg_condi);
		main_thr_data = 12.3456f;
		condition_signal(&dbg_condi);
		condition_unlock(&dbg_condi);
		bmp_test();

		ipm_post_msg(&ipm_q, ip_msg, strlen(ip_msg));

		ee_stress_read_byte(EE_BYTE_ADDR, &readback);
		ee_value += 1;
		ee_stress_write_byte(EE_BYTE_ADDR, ee_value);
		printf_P(PSTR("[%u][%s]:   ee-byte read: %u\n"), 0,
				current_thread_name(), readback);

		temperature = tmp36_read(PIN_A0);
		printf_P(PSTR("[0][%s]:   Memory available: %u :: "
					"Temperature: %f :: Preempt count: %lu\n"),
				current_thread_name(),
				mm_heap_available(),
				temperature, preempt_counter);

		sleep(500);
	}
	return 0;
}
