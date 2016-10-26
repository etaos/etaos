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

#include <uapi/etaos/test.h>

static unsigned char test_thread_stack[CONFIG_STACK_SIZE];
static struct thread *test_t;

class TestClass {
	public:
		TestClass(int a, int b);
		int getA();
		int getB();

	private:
		int a;
		int b;
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

THREAD(test_th_handle, arg)
{
	TestClass *tc;
	int i;

	tc = new TestClass(10, 11);
	nice(150);

	for(i = 0; i < 5; i++) {
		printf("[tt] CPP test: %i::%i\n", tc->getA(), tc->getB());
		Kernel::sleep(500);
	}

	kill();
}

int main(void)
{
	TestClass *tc;
	int i, value = 0;

	test_t = thread_create( "tst", &test_th_handle, NULL,
				CONFIG_STACK_SIZE, test_thread_stack, 80);

	tc = new TestClass(5,6);
	Platform::pin_out(CPU::pin13, value);

	for(i = 0; i < 5; i++) {
		printf("[mt] CPP test: %i::%i\n", tc->getA(), tc->getB());
		Platform::pin_write(CPU::pin13, value);
		value = !value;
		Kernel::sleep(500);
	}

	printf(CALYPSO_EXIT);
	return 0;
}

