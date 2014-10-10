/*
 * ETA/OS test application
 * Author: Michel Megens
 */

#include <etaos/kernel.h>
#include <etaos/stdio.h>
#include <etaos/mem.h>
#include <etaos/sched.h>
#include <etaos/thread.h>
#include <etaos/mem.h>
#include <etaos/spinlock.h>

static struct thread td1, td2, td3, td4;

static unsigned char s1[128], s2[128], s3[128], s4[128];

DEFINE_THREAD_QUEUE(tst_q);

THREAD(handle, arg)
{
	while(1);
}

static void sched_test_queues(void)
{
	thread_initialise(&td1, "T1", &handle, NULL, 128, s1, 150);
	thread_initialise(&td2, "T2", &handle, NULL, 128, s2, 160);
	thread_initialise(&td3, "T3", &handle, NULL, 128, s3, 100);
	thread_initialise(&td4, "T4", &handle, NULL, 128, s4, 170);
}

static void print_queue(struct thread *qhead)
{
	struct thread *carriage;

	for(carriage = qhead; carriage; 
			carriage = carriage->se.next) {
		printf("%s::%lu\n", carriage->name, carriage->flags);
	}
	
}

static void test_schedule(struct rq *rq)
{
	rq_remove_thread(&td3);

	rq->current = &td3;
	td3.on_rq = true;
	set_bit(THREAD_RUNNING_FLAG, &td3.flags);
	set_bit(THREAD_NEED_RESCHED_FLAG, &td3.flags);

	schedule();
	printf("RunQ: Current %s\n", rq->current->name);
	print_queue(rq->rq.run_queue);
}

static void print_test_queue(struct rq *rq)
{
	printf("Test Q:\n");
	print_queue(tst_q.qhead);
	printf("Wake Q:\n");
	printf("%s::%lu\n", rq->wake_queue->name, rq->wake_queue->flags);
}

static void test_thread_queue(struct rq *rq)
{
	struct thread *tp;

	thread_add_to_wake_q(rq->current);
	queue_add_thread(&tst_q, rq->current);
	printf("Flags: %s: %lu\n", rq->current->name, rq->current->flags);
	
	printf("RunQ\n");
	print_queue(rq->rq.run_queue);
	print_test_queue(rq);

	tp = rq->wake_queue;
	rq_remove_wake_thread(rq, tp);
	set_bit(THREAD_RUNNING_FLAG, &tp->flags);
	set_bit(THREAD_NEED_RESCHED_FLAG, &rq->current->flags);
	queue_remove_thread(&tst_q, tp);
	printf("Flags: %s: %lu\n", rq->current->name, rq->current->flags);

	thread_add_to_wake_q(rq->current);
	queue_add_thread(&tst_q, rq->current);
	printf("Flags: %s: %lu\n", rq->current->name, rq->current->flags);

	printf("RunQ\n");
	print_queue(rq->rq.run_queue);
	print_test_queue(rq);
}

int main(void)
{
	struct rq *rq;

	printf("Application started (M:%u)!\n", mm_heap_available());
	stdin->get(stdin);
	rq = sched_get_cpu_rq();

	sched_test_queues();
	test_schedule(rq);
	test_thread_queue(rq);

	printf("PROG EXIT");
	return 0;
}

