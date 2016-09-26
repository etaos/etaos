/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/stdlib.h>
#include <etaos/string.h>
#include <etaos/mem.h>
#include <etaos/thread.h>
#include <etaos/event.h>
#include <etaos/hrtimer.h>
#include <etaos/mem.h>
#include <etaos/mutex.h>
#include <etaos/time.h>
#include <etaos/tick.h>
#include <etaos/gpio.h>
#include <etaos/vfs.h>
#include <etaos/platform.h>
#include <etaos/analog.h>
#include <etaos/lm35.h>
#include <etaos/ipm.h>
#include <asm/pgm.h>

#include <etaos/sram/23k256.h>

static void *test_stack;
static uint8_t test_stack2[CONFIG_STACK_SIZE];

static struct ipm_queue ipm_q;
static uint8_t ee_value = 0x0;

static const char sram_test[] = "sram test";
static const char ee_test[]   = "ee test";

#define EE_BYTE_ADDR 0x20
#define EE_STRING_ADDR 0x40

#define SRAM_BYTE_ADDR 0x100
#define SRAM_STRING_ADDR 0x200

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

THREAD(test_th_handle2, arg)
{
	unsigned char sram_readback = 0;
	unsigned long rand;
	int fd;
	struct vfile *file;
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

		fd = open("test.txt", O_RDONLY);
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

THREAD(test_th_handle, arg)
{
	struct ipm msg;
	uint8_t readback = 0;
	float sram_data;
	char ee_string[sizeof(ee_test)];

	while(true) {
		ee_stress_read_byte(EE_BYTE_ADDR, &readback);
		printf_P(PSTR("[%u][%s]: ee-byte read: %u\n"), 1, 
				current_thread_name(), readback);

		ipm_get_msg(&ipm_q, &msg);
		ipm_reset_queue(&ipm_q);

		printf_P(PSTR("[1][%s]: "), current_thread_name());
		write(to_fd(stdout), msg.data, msg.len);

		sram_stress_read(SRAM_STRING_ADDR, &sram_data,
				sizeof(sram_data));
		ee_stress_read(EE_STRING_ADDR, ee_string, sizeof(ee_string));
		printf_P(PSTR("[1][%s]: SRAM::EEPROM %f::%s\n"), 
				current_thread_name(),
				sram_data, ee_string);

		sleep(500);
	}
}

static struct gpio_pin *led_pin;

static void hrtimer1_handle_func(struct hrtimer *hrt, void *arg)
{
	static bool value = true;

	raw_gpio_pin_write(led_pin, value);
	value = !value;
}

int main(void)
{
	const char * ip_msg = "IPM message\n";
	uint8_t readback = 0;
	char buff[16];
	time_t now;
	float temperature;

	printf("Application started\n");

	ipm_queue_init(&ipm_q, 2);
	ee_stress_write_byte(EE_BYTE_ADDR, 0xAC);
	ee_stress_write(EE_STRING_ADDR, ee_test, strlen(ee_test)+1);

	test_stack = kzalloc(CONFIG_STACK_SIZE);
	thread_create("test-1", &test_th_handle, NULL,
			CONFIG_STACK_SIZE, test_stack, 150);
	thread_create("test-2", &test_th_handle2, NULL, CONFIG_STACK_SIZE,
			test_stack2, 120);



	read(to_fd(stdin), &buff[0], 10);
	buff[10] = 0;
	now = (time_t)atol(buff);
	stime(now);

	pgpio_pin_request(13);
	pgpio_direction_output(13, false);
	led_pin = platform_pin_to_gpio(13);
	hrtimer_create(hr_sys_clk, 2000000, hrtimer1_handle_func,
			NULL, 0UL);

	while(true) {
		ipm_post_msg(&ipm_q, ip_msg, strlen(ip_msg));

		ee_stress_read_byte(EE_BYTE_ADDR, &readback);
		ee_value += 1;
		ee_stress_write_byte(EE_BYTE_ADDR, ee_value);
		printf_P(PSTR("[%u][%s]:   ee-byte read: %u\n"), 0,
				current_thread_name(), readback);

		temperature = lm35_read(PIN_A0);
		printf_P(PSTR("[0][%s]:   Memory available: %u :: Temperature: %f\n"),
				current_thread_name(),
				mm_heap_available(),
				temperature);

		sleep(500);
	}
	return 0;
}
