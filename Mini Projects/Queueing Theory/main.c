#include <cmsis_os2.h>
#include <LPC17xx.h>
#include <stdlib.h>
#include "stdio.h"
#include "GLCD.h"
#include "uart.h"
#include "random.h"

//global variable
int msg_sent_q1 = 0;
int msg_sent_q2 = 0;
int msg_recv_q1 = 0;
int msg_recv_q2 = 0;
int overflow_1 = 0;
int overflow_2 = 0;
int msg;

osMessageQueueId_t q_1;
osMessageQueueId_t q_2;


//Read gyro, acc, and mag values from MPU9250 and stored in memory
void client(void *arg){
    int queue_counter = 0;
    osStatus_t status;
		int delay;
		int second = osKernelGetTickFreq();
	
    while(1){
//				//converting floating point to int (ticks) 
//        delay = next_event();//in second
//				delay *= second; //in ticks
//        delay /= 9; 
//        delay = delay >> 16; 
				
        osDelay((next_event()*osKernelGetTickFreq()/18)>> 16);
			
				//alternating between queues
        if(queue_counter % 2){
            status = osMessageQueuePut(q_1, &msg, 0, 0);
						//counting the overflows
						printf("%d", status);
						if(status == osOK){
							//osMessageQueuePut(q_1, &msg, 0, 0);
							msg_sent_q1 ++;
							//queue_counter ++;
							queue_counter ++;
						}
            if(status == osErrorResource){
                //dropped
								overflow_1 ++;
								queue_counter ++;
            }
        }
        else{
            status = osMessageQueuePut(q_2, &msg, 0, 0);
						//counting the overflows
						if(status == osOK){
							//osMessageQueuePut(q_2, &msg, 0, 0);
							msg_sent_q2 ++;
							queue_counter ++;
						}
            if(status == osErrorResource){
                //dropped
								overflow_2 ++;
								queue_counter ++;
            }
        }
				//queue_counter ++;
    }
}

//Server watching queue 1
void server_1(void *arg){
	int delay;
	int second = osKernelGetTickFreq();
	osStatus_t status;
	// Wait for random delay 
	while(1){
		//converting floating point to int (ticks) 
		//delay = next_event();//in second
		//delay *= second; //in ticks
//		delay /= 10; 
//		delay = delay >> 16; 
		
		 osDelay((next_event()*osKernelGetTickFreq()/10)>> 16);
		
		//sever grets message from queue
		status = osMessageQueueGet (q_1, &msg, NULL, osWaitForever);
		if(status == osOK){		
			//Increment recieve counter
			msg_recv_q1 ++;
		}
	}
}

//Server Watching queue 2
void server_2(void *arg){
	int delay;
	int second = osKernelGetTickFreq();
	osStatus_t status;
	// Wait for random delay 
	while(1){
		//converting floating point to int (ticks) 
		
		 osDelay((next_event()*osKernelGetTickFreq()/10)>> 16);
		
		//sever grets message from queue
		status = osMessageQueueGet (q_2, &msg, NULL, osWaitForever);
		if(status == osOK){		
			//Increment recieve counter
			msg_recv_q2 ++;
		}
	}
}

// 
void monitor (void *arg){
    // LCD display
		GLCD_Init();
    GLCD_SetBackColor(Black);
    GLCD_SetTextColor(Blue);
		int time = 0;
	  char time_str[30];
		char msg_sent_q1_str[30];
		char msg_sent_q2_str[30];
		char msg_recv_q1_str[30];
		char msg_recv_q2_str[30];
		char overflow_q1_str[30];
		char overflow_q2_str[30];
		GLCD_Clear(Black);
		int second = osKernelGetTickFreq();
	
    while(1){
				//GLCD_Clear(Black);
        // only for the first row
        GLCD_DisplayString(0, 5, 1, "q1     q2");
				GLCD_DisplayString(1, 0, 1, "sent");
				snprintf(msg_sent_q1_str, 30, "%d", msg_sent_q1);
				GLCD_DisplayString(1, 5, 1, (unsigned char*)msg_sent_q1_str);
				snprintf(msg_sent_q2_str, 30, "%d", msg_sent_q2);
				GLCD_DisplayString(1, 12, 1, (unsigned char*)msg_sent_q2_str);
				GLCD_DisplayString(2, 0, 1, "recv");
				snprintf(msg_recv_q1_str, 30, "%d", msg_recv_q1);
				GLCD_DisplayString(2, 5, 1, (unsigned char*)msg_recv_q1_str);
				snprintf(msg_recv_q2_str, 30, "%d", msg_recv_q2);
				GLCD_DisplayString(2, 12, 1, (unsigned char*)msg_recv_q2_str);
				GLCD_DisplayString(3, 0, 1, "over");
				snprintf(overflow_q1_str, 30, "%d", overflow_1);
				GLCD_DisplayString(3, 5, 1, (unsigned char*)overflow_q1_str);
				snprintf(overflow_q2_str, 30, "%d", overflow_2);
				GLCD_DisplayString(3, 12, 1, (unsigned char*)overflow_q2_str);
				GLCD_DisplayString(5, 0, 1, "time");
			  snprintf(time_str, 30, "%d", time);
				//sprintf(numberArray, "%d", time);
				GLCD_DisplayString(5, 5, 1, (unsigned char*)time_str);
				//GLCD_DisplayString(5, 5, 1, convert(time));
				time ++;
        osDelay(second);                                                         
    }
}

unsigned char * convert(int time){
	unsigned char *output;
	int temp = time;
	int count = 0;
	while(temp != 0){
		temp -= temp % 10; 
		temp /= 10;
		count ++;
	}
	for(int a = count; a > 0; a--){
		output[a] = time % 10;
		time -= time % 10;
		time /= 10;
	}
	return output;
}

int main( void )
{
  //initialization
	
	
	osKernelInitialize();
	q_1 = osMessageQueueNew(10, sizeof(int), NULL);
	q_2 = osMessageQueueNew(10, sizeof(int), NULL);
	
	osThreadNew(client, NULL, NULL);
  osThreadNew(server_1, NULL, NULL);
	osThreadNew(server_2, NULL, NULL);
	osThreadNew(monitor, NULL, NULL);
	osKernelStart();
	
	while(1);
}

