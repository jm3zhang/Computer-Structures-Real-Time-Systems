#include <LPC17xx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

uint32_t msTicks = 0;

void SysTick_Handler(void) {
	msTicks++;
}

bool INT0_pushed = false;

void setup_INT0(void) {
	LPC_GPIOINT->IO2IntEnF |= (1 << 10); // falling-edge interrupt
	NVIC_EnableIRQ(EINT3_IRQn); // enable external interrupt 3 handler
}

void EINT3_IRQHandler(void) {
	INT0_pushed = true;
	LPC_GPIOINT->IO2IntClr |= (1 << 10); // clear pending interrupt
}

void t1(void) {
	printf("t1\n");
}

void t2(void) {
	printf("t2\n");
}

int main(void) {
	uint32_t t1Prev = 0;
	uint32_t t1Period = 1000;
	uint32_t t2Prev = 0;
	uint32_t t2Period = 2000;

	setup_INT0();
	SysTick_Config(SystemCoreClock/1000);

	printf("Starting...\n\n");

	while(true) {
		if((uint32_t)(msTicks - t1Prev) >= t1Period) {
			t1();
			t1Prev += t1Period;
		}
		if((uint32_t)(msTicks - t2Prev) >= t2Period) {
			t2();
			t2Prev += t2Period;
		}
		if(INT0_pushed) {
			printf("INT0 pushed\n");
			INT0_pushed = false;
		}
	}
}
