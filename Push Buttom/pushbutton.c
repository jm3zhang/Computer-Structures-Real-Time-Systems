#include <LPC17xx.h>
#include <stdbool.h>
#include <stdio.h>

bool button_pushed = false;

void setup_INT0(void) {
	LPC_GPIOINT->IO2IntEnF |= (1 << 10); // falling-edge interrupt
	NVIC_EnableIRQ(EINT3_IRQn); // enable external interrupt 3 handler
}

void EINT3_IRQHandler(void) {
	button_pushed = true;
	LPC_GPIOINT->IO2IntClr |= (1 << 10); // clear pending interrupt
}

int main(void) {
	setup_INT0();

	while(true) {
		if(button_pushed) {
			printf("button pushed\n");
			button_pushed = false;
		}
	}
}
