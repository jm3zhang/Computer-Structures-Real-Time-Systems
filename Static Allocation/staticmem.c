#include <cmsis_os2.h>
#include <LPC17xx.h>
#include <rtx_os.h>
#include <stdio.h>

#define STACK_SIZE 2048 // 2kB

uint32_t mutex_cb[osRtxMutexCbSize/4];  // 4B aligned
uint32_t thread_cb[osRtxThreadCbSize/4];
uint64_t stack[STACK_SIZE/8]; // 8B aligned

osMutexId_t mutex;

void worker(void *args) {
	osMutexAcquire(mutex, osWaitForever);
	printf("hello\n");
	osMutexRelease(mutex);
}

int main(void) {
	osKernelInitialize();

	osMutexAttr_t mAttr = { .cb_mem=mutex_cb, .cb_size=osRtxMutexCbSize };
	mutex = osMutexNew(&mAttr);

	osThreadAttr_t tAttr = { .cb_mem=thread_cb, .cb_size=osRtxThreadCbSize,
		.stack_mem=stack, .stack_size=STACK_SIZE };
	osThreadNew(worker, NULL, &tAttr);

	osKernelStart();
	for( ; ; ) ;
}
