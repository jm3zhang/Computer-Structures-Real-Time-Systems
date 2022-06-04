#include <cmsis_os2.h>
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdio.h>

void setupLEDs( void ) {
	LPC_GPIO1->FIODIR |= 0xB0000000;
	LPC_GPIO2->FIODIR |= 0x0000007C;
}

void t1_2(void *arg) {
	uint32_t period = (uint32_t)arg;
	bool on = true;
	while(true) {
		if(period == 1) {
			if(on) LPC_GPIO1->FIOCLR |= (1UL << 28);
			else LPC_GPIO1->FIOSET |= (1UL << 28);
		} else {
			if(on) LPC_GPIO2->FIOCLR |= (1UL << 6);
			else LPC_GPIO2->FIOSET |= (1UL << 6);
		}
		on = !on;
		osDelay(osKernelGetTickFreq()*period);
	}
}

#define BUTTON_FLAG 1UL
osEventFlagsId_t eventFlags;

void EINT3_IRQHandler(void) {
	LPC_GPIOINT->IO2IntClr |= (1 << 10); // clear pending interrupt
	osEventFlagsSet(eventFlags, BUTTON_FLAG);
}

void setupPushButton(void) {
	LPC_GPIOINT->IO2IntEnF |= (1 << 10); // falling-edge interrupt
	NVIC_EnableIRQ(EINT3_IRQn); // enable external interrupt 3 handler
}

void t3(void *arg) {
	eventFlags = osEventFlagsNew(NULL);
	setupPushButton();
	while(true) {
		osEventFlagsWait(eventFlags, BUTTON_FLAG, osFlagsWaitAny, osWaitForever);
		printf("button pushed\n");

		// debounce button
		osDelay(osKernelGetTickFreq()/20);
		osEventFlagsClear(eventFlags, BUTTON_FLAG);
	}
}

int main(void) {
	setupLEDs();
	osKernelInitialize();
	osThreadNew(t1_2, (void *)1, NULL);
	osThreadNew(t1_2, (void *)2, NULL);
	osThreadNew(t3, NULL, NULL);
	osKernelStart();
	for( ; ; ) ;
}
