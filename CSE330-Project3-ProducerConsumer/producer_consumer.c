// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/atomic.h>
#include <linux/cred.h>
#include <linux/uidgid.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Johnson");
MODULE_DESCRIPTION("CSE330 Project 3: producer_consumer");

/* ---------- Module parameters ---------- */
static int prod = 1;
static int cons = 1;
static int size = 8;
static int uid  = 1000;

module_param(prod, int, 0444);
MODULE_PARM_DESC(prod, "Number of producer threads (non-negative)");
module_param(cons, int, 0444);
MODULE_PARM_DESC(cons, "Number of consumer threads (non-negative)");
module_param(size, int, 0444);
MODULE_PARM_DESC(size, "Shared buffer size (positive)");
module_param(uid, int, 0444);
MODULE_PARM_DESC(uid, "UID of the test user");

/* ---------- Ring buffer item & buffer ---------- */
struct pc_item {
	kuid_t who;
	u64    seq;
};

struct pc_ring {
	struct pc_item *buf;
	int cap;
	int head;
	int tail;
	spinlock_t lock;
	struct semaphore empty;
	struct semaphore full;
};

/* ---------- Globals ---------- */
static struct pc_ring ringbuf;
static struct task_struct **prod_tasks;
static struct task_struct **cons_tasks;

static atomic64_t total_produced;
static atomic64_t total_consumed;
static atomic64_t total_dropped;

/* ---------- Helpers (kept for later steps) ---------- */
static bool uid_matches_test_user(void)
{
	kuid_t t = make_kuid(current_user_ns(), uid);
	return uid_valid(t) && uid_eq(current_uid(), t);
}

/* Minimal ring buffer scaffolding (not used in Step 1) */
static void rb_init(struct pc_ring *rb, int cap)
{
	rb->cap  = cap;
	rb->head = 0;
	rb->tail = 0;
	spin_lock_init(&rb->lock);
	sema_init(&rb->empty, cap);
	sema_init(&rb->full, 0);
}

/* Placeholders for later steps; compile clean now */
static bool rb_enqueue(struct pc_ring *rb, const struct pc_item *in)
{
	/* stub for future steps */
	return false;
}
static bool rb_dequeue(struct pc_ring *rb, struct pc_item *out)
{
	/* stub for future steps */
	return false;
}

/* ---------- Producer / Consumer threads (Step 1 behaviour) ---------- */
static int producer_fn(void *data)
{
	long id = (long)data;

	pr_info("producer_consumer: producer[%ld] started\n", id);

	while (!kthread_should_stop()) {
		/* Step 1: just idle */
		ssleep(1);
		cond_resched();
	}

	pr_info("producer_consumer: producer[%ld] stopping\n", id);
	return 0;
}

static int consumer_fn(void *data)
{
	long id = (long)data;

	pr_info("producer_consumer: consumer[%ld] started\n", id);

	while (!kthread_should_stop()) {
		/* Step 1: just idle */
		ssleep(1);
		cond_resched();
	}

	pr_info("producer_consumer: consumer[%ld] stopping\n", id);
	return 0;
}

/* ---------- Module init / exit ---------- */
static int __init pc_init(void)
{
	int i, ret = 0;

	/* basic validation */
	if (size <= 0) {
		pr_err("producer_consumer: invalid size=%d (must be >0)\n", size);
		return -EINVAL;
	}
	if (prod < 0 || cons < 0) {
		pr_err("producer_consumer: prod=%d cons=%d must be non-negative\n", prod, cons);
		return -EINVAL;
	}

	/* counters */
	atomic64_set(&total_produced, 0);
	atomic64_set(&total_consumed, 0);
	atomic64_set(&total_dropped,  0);

	/* ring buffer */
	ringbuf.buf = kmalloc_array(size, sizeof(*ringbuf.buf), GFP_KERNEL);
	if (!ringbuf.buf) {
		pr_err("producer_consumer: failed to allocate ring buffer (%d)\n", size);
		return -ENOMEM;
	}
	rb_init(&ringbuf, size);

	/* kthread handles */
	prod_tasks = kcalloc(prod, sizeof(*prod_tasks), GFP_KERNEL);
	cons_tasks = kcalloc(cons, sizeof(*cons_tasks), GFP_KERNEL);
	if (!prod_tasks || !cons_tasks) {
		ret = -ENOMEM;
		goto out_free_ring;
	}

	/* spawn producers */
	for (i = 0; i < prod; i++) {
		prod_tasks[i] = kthread_run(producer_fn, (void *)(long)i, "pc_prod/%d", i);
		if (IS_ERR(prod_tasks[i])) {
			ret = PTR_ERR(prod_tasks[i]);
			prod_tasks[i] = NULL;
			goto out_stop_started;
		}
	}

	/* spawn consumers */
	for (i = 0; i < cons; i++) {
		cons_tasks[i] = kthread_run(consumer_fn, (void *)(long)i, "pc_cons/%d", i);
		if (IS_ERR(cons_tasks[i])) {
			ret = PTR_ERR(cons_tasks[i]);
			cons_tasks[i] = NULL;
			goto out_stop_started;
		}
	}

	pr_info("producer_consumer loaded (Step 1). params: prod=%d cons=%d size=%d uid=%d\n",
		prod, cons, size, uid);
	return 0;

	/* ---- reverse-order cleanup paths ---- */
out_stop_started:
	for (i = 0; i < prod; i++) {
		if (prod_tasks && prod_tasks[i])
			kthread_stop(prod_tasks[i]);
	}
	for (i = 0; i < cons; i++) {
		if (cons_tasks && cons_tasks[i])
			kthread_stop(cons_tasks[i]);
	}
	kfree(cons_tasks);
	kfree(prod_tasks);
out_free_ring:
	kfree(ringbuf.buf);
	return ret;
}

static void __exit pc_exit(void)
{
	int i;

	/* stop all threads we created */
	if (prod_tasks) {
		for (i = 0; i < prod; i++) {
			if (prod_tasks[i])
				kthread_stop(prod_tasks[i]);
		}
	}
	if (cons_tasks) {
		for (i = 0; i < cons; i++) {
			if (cons_tasks[i])
				kthread_stop(cons_tasks[i]);
		}
	}

	/* free arrays and ring buffer */
	kfree(cons_tasks);
	kfree(prod_tasks);
	kfree(ringbuf.buf);

	pr_info("producer_consumer: totals produced=%lld consumed=%lld dropped=%lld\n",
		(long long)atomic64_read(&total_produced),
		(long long)atomic64_read(&total_consumed),
		(long long)atomic64_read(&total_dropped));

	pr_info("producer_consumer unloaded (Step 1)\n");
}

module_init(pc_init);
module_exit(pc_exit);
