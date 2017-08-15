/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/thread.h>

#include <etaos/stl/kernel.h>
#include <etaos/stl/cpu.h>
#include <etaos/stl/platform.h>
#include <etaos/stl/sram.h>
#include <etaos/stl/eeprom.h>
#include <etaos/stl/thread.h>

#include <uapi/etaos/test.h>

class TestClass {
	public:
		TestClass(int a, int b);
		int getA();
		int getB();

	private:
		int a;
		int b;
};

class ThreadTest : public Thread {
	public:
		explicit ThreadTest(const char *name, void *arg) :
			Thread(name, arg) {}
		void run(void *arg);
};

TestClass::TestClass(int a, int b)
{
	this->a = a;
	this->b = b;
}

int TestClass::getA()
{
	return this->a;
}

int TestClass::getB()
{
	return this->b;
}

void ThreadTest::run(void *arg)
{
	TestClass *tc;
	SRAM *sram;
	char readback;
	int i;

	tc = new TestClass(10, 11);
	sram = new SRAM("/dev/23K256");
	sram->putc(0x20, 'E');
	nice(150);

	for(i = 0; i < 5; i++) {
		printf("[tt] CPP test: %i::%i\n", tc->getA(), tc->getB());
		readback = (*sram)[0x20];
		printf("[tt] SRAM test: %c\n", readback);
		Kernel::sleep(500);
	}

	delete sram;
	delete tc;
}

int main(void)
{
	TestClass *tc;
	EEPROM *ee;
	ThreadTest *tp;
	int i, value = 0;
	char readback;

	tp = new ThreadTest("test-1", NULL);
	tp->start();

	tc = new TestClass(5,6);
	ee = new EEPROM("/dev/24C02");
	Platform::pin_out(CPU::pin13, value);
	ee->putc(0x10, 'X');

	for(i = 0; i < 5; i++) {
		readback = (*ee)[0x10];
		printf("[mt] CPP test: %i::%i\n", tc->getA(), tc->getB());
		printf("[mt] EEPROM test: %c\n", readback);
		Platform::pin_write(CPU::pin13, value);
		value = !value;
		Kernel::sleep(500);
	}

	tp->join();
	delete ee;
	delete tp;
	printf(CALYPSO_EXIT);
	return 0;
}

