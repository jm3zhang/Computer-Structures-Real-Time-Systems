#include <cmsis_os2.h>
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

osMutexId_t printMutex;

typedef struct {
	osMutexId_t mutex;
	osSemaphoreId_t turnstile1, turnstile2;
	uint32_t count, n;
} barrier_t;

void init(barrier_t *b, uint32_t n) {
	//osMutexAttr_t attr = { .attr_bits = osMutexPrioInherit | osMutexRobust };
	//b->mutex = osMutexNew(&attr);
	b->mutex = osMutexNew(NULL);
	b->turnstile1 = osSemaphoreNew(n, 0, NULL);
	b->turnstile2 = osSemaphoreNew(n, 1, NULL);
	b->count = 0;
	b->n = n;
}

void sync(barrier_t * b) {
	osMutexAcquire(b->mutex, osWaitForever); {
		b->count++;
		if(b->count == b->n) {
			osSemaphoreAcquire(b->turnstile2, osWaitForever);
			osSemaphoreRelease(b->turnstile1);
		}
	} osMutexRelease(b->mutex);

	osSemaphoreAcquire(b->turnstile1, osWaitForever);
	osSemaphoreRelease(b->turnstile1);

	osMutexAcquire(b->mutex, osWaitForever); {
		b->count--;
		if(b->count == 0) {
			osSemaphoreAcquire(b->turnstile1, osWaitForever);
			osSemaphoreRelease(b->turnstile2);

			osMutexAcquire(printMutex, osWaitForever);
			printf("...\n");
			osMutexRelease(printMutex);
		}
	} osMutexRelease(b->mutex);

	osSemaphoreAcquire(b->turnstile2, osWaitForever);
	osSemaphoreRelease(b->turnstile2);
}

barrier_t b;
void task(void *args) {
	while(true) {
		osMutexAcquire(printMutex, osWaitForever);
		printf("%s\n", (char const *)args);
		osMutexRelease(printMutex);

		osDelay(osKernelGetTickFreq()*(rand()%3 + 1));
		sync(&b);
	}
}

int main(void) {
	srand(2000000001);
	osKernelInitialize();
	printMutex = osMutexNew(NULL);
	init(&b, 3);
	osThreadNew(task, "t1", NULL);
	osThreadNew(task, "t2", NULL);
	osThreadNew(task, "t3", NULL);
	osKernelStart();
	for( ; ; ) ;
}
