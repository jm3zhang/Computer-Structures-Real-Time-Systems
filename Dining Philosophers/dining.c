/*
 * A simple solution to the dining philosphor's problem.
 * @author Andrew Morton, 2018
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmsis_os2.h>
#include <LPC17xx.h>

#define N_PHIL 5
uint32_t phil_id[N_PHIL];
typedef enum { THINKING, EATING } action_t;

void phil(void *id_p);
void printHelper(uint32_t id, action_t action);

osMutexId_t fork[N_PHIL];
osMutexId_t printMutex;

int main(void) {
	osKernelInitialize();
	printMutex = osMutexNew(NULL);
	for(uint32_t i=0; i<N_PHIL; i++) {
		phil_id[i] = i;
		fork[i] = osMutexNew(NULL);
		osThreadNew(phil, &phil_id[i], NULL);
	}
	osKernelStart();
	while(true) {}
}


void phil(void *id_p) {
	uint32_t id = *(uint32_t *)id_p;

	while(true) {
		// think for a while - (0,5] s
		printHelper(id, THINKING);
		uint32_t ticks = rand() % osKernelGetTickFreq()*5 + 1;
		osDelay(ticks);

		// pick up forks
		uint32_t first = (id == 4) ? 0 : id;
		uint32_t second = (id == 4) ? 4 : id + 1;
		osMutexAcquire(fork[first], osWaitForever);
		osMutexAcquire(fork[second], osWaitForever);
		
		// eat for a while 
		printHelper(id, EATING);
		ticks = rand() % osKernelGetTickFreq()*5 + 1;
		osDelay(ticks);

		// put down forks
		osMutexRelease(fork[first]);
		osMutexRelease(fork[second]);
	}

}

void printHelper(uint32_t id, action_t action) {
	osMutexAcquire(printMutex, osWaitForever);
	// printf("phil %d %s\n", id, action==THINKING ? "think" : "eat");
	// printf above hung up so I resorted to the following which worked
	switch(id) {
		case 0: printf("phil 0"); break;
		case 1: printf("phil 1"); break;
		case 2: printf("phil 2"); break;
		case 3: printf("phil 3"); break;
		case 4: printf("phil 4"); break;
		default: break;
	}
	if(action == THINKING) printf(" think\n");
	else printf(" eat\n");
	osMutexRelease(printMutex);
}
