/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/thread.h>

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

	tc = new TestClass(10, 11);
	nice(150);

	while(true) {
		printf("[tt] CPP test: %i::%i\n", tc->getA(), tc->getB());
		sleep(1000);
	}
}

int main(void)
{
	TestClass *tc;

	test_t = thread_create( "tst", &test_th_handle, NULL,
				CONFIG_STACK_SIZE, test_thread_stack, 80);

	tc = new TestClass(5,6);

	while(true) {
		printf("[mt] CPP test: %i::%i\n", tc->getA(), tc->getB());
		sleep(500);
	}

	return 0;
}

