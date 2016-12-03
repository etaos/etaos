/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/device.h>

#include <uapi/etaos/test.h>

#include <asm/io.h>
#include <asm/simulavr.h>

static struct platform_device exit_dev = {
	.name = "exit-dev",
	.io_base = NULL,
};

static int exit_dev_write(struct file* file, const void *data, size_t len);

static struct dev_file_ops exit_dev_fops = {
	.write = &exit_dev_write,
};

static int exit_dev_write(struct file* file, const void *data, size_t len)
{
	return 0;
}

static const char *found_str = "[OK] Found added device: %s and %s\n";
static const char *not_found_str = "[ERR] Added device not found!\n";

static int run_dev_test(void)
{
	int err;
	struct device *found, *found2;

	err = dev_register_pdev(&exit_dev, &exit_dev_fops);
	device_create("exit2-dev", NULL, NULL);
	found = dev_get_by_name("exit-dev");
	found2 = dev_get_by_name("exit2-dev");
	if(found && found2)
		printf(found_str, found->name, found2->name);
	else
		printf(not_found_str);


	return err;
}

int main(void)
{
	struct device *found, *found2;

	printf("Application started!\n");
	run_dev_test();

	found = dev_get_by_name("exit-dev");
	found->release(found);
	found2 = dev_get_by_name("exit2-dev");
	found2->release(found2);

	found = dev_get_by_name("exit-dev");
	found2 = dev_get_by_name("exit2-dev");
	if(!found && !found2)
		printf("[OK] devices deleted succefully!\n");
	else
		printf("[ERR] devices not deleted succesfully!\n");

	printf(CALYPSO_EXIT);
#ifdef CONFIG_SIMUL_AVR
	simul_avr_exit(1);
#endif
	return 0;
}

