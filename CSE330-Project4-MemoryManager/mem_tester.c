#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>

static volatile int g_init = 0x12345678;
static volatile int g_bss;

static void dummy_func(void) {
	asm volatile("" ::: "memory");
}

static void touch(void *p) {
	volatile char *c = (volatile char *)p;
	*c = (char)(*c + 1);
}

int main(void) {
	printf("mem_tester PID: %d\n", getpid());
	volatile char stack_buf[4096];
	memset((void *)stack_buf, 0, sizeof(stack_buf));
	touch((void *)stack_buf);

	void *heap = malloc(4096);
	if (!heap) {
		perror("malloc");
		return 1;
	}

	memset(heap, 0xAB, 4096);
	touch(heap);

	void *map = mmap(NULL, 4096,PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if (map == MAP_FAILED) {
		perror("mmap");
		return 1;
	}

	memset(map, 0xCD, 4096);
	touch(map);

	printf("\nPick one of these addresses and feed it to your module (as hex):\n");
	printf("  stack   : %p\n", (void *)stack_buf);
	printf("  stack   : %p\n", heap);
	printf("  mmap    : %p\n", map);
	printf("  global(.data): %p\n", (void *)&g_init);
	printf("  global(.bss) : %p\n", (void *)&g_bss);
	printf("  code    : %p (dummy_func)\n", (void *)&dummy_func);

	printf("\nKeep this process running while you test.\n");
	printf("Press Ctrl+C to quit when you're done.\n");

	for (;;) {
		touch(heap);
		touch(map);
		touch((void *)stack_buf);
		struct timespec ts = { .tv_sec = 1, .tv_nsec = 0 };
		nanosleep(&ts, NULL);
	}
}
