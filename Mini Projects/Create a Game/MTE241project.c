#include <cmsis_os2.h>
#include "random.h"
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdio.h>
#include "GLCD.h"
#include "led.h"
#include <stdlib.h>
#include <time.h>


typedef enum {CENTER, DOWN, UP, RIGHT, LEFT}direction_t;

//Global Variables
direction_t snake_direction = RIGHT;
bool powerUp = 0;
int score = 0;
int energy = 0;
int state = 0; //state 0 - start screen, state 1 - play game, state 2 - end screen
int last_tail_x = 0;
int last_tail_y = 0;
int apple_x, apple_y = NULL;
bool is_apple = false;
osMutexId_t mutex_1; 
osMutexId_t mutex_2; 

//create a linked list that keeps track of the positions
typedef struct positionNode{
    unsigned int positionX, positionY;
    struct positionNode *next; 
    struct positionNode *prev;
}positionNode_t;

//struct of snake that stores the length and position
typedef struct snake{
    int length;
    positionNode_t *head;
    positionNode_t *tail;
}snake_t;

//Initialize snake 
snake_t snake = {3, NULL, NULL};


//Reads output from the joystick
direction_t joystick_read(){
    direction_t direction = CENTER;
    if((LPC_GPIO1->FIOPIN & 0x01000000) == 0){
        direction = RIGHT;
    }
    else if((LPC_GPIO1->FIOPIN & 0x02000000) == 0){
        direction = UP;
    }
    else if((LPC_GPIO1->FIOPIN & 0x04000000) == 0){
        direction = LEFT;
    }
    else if((LPC_GPIO1->FIOPIN & 0x00800000) == 0){
        direction = DOWN;
    }
    else{
        direction = CENTER;
    }

    return direction;
}

//traverse function to check if input = snake position 
bool traverse(int x, int y){
	positionNode_t *temp = snake.tail;
	if(x == temp ->positionX && y == temp -> positionY){
		return true;
	}
	while(temp -> next != NULL){
		temp = temp -> next;
		if(x == temp ->positionX && y == temp -> positionY){
			return true;
		}
	}
	return false;
}

void LCD_display(void *arg){
	while(1){
			int second = osKernelGetTickFreq();
			char score_str[30];

			GLCD_Clear(Black);
			GLCD_SetBackColor(Black);
			GLCD_SetTextColor(White);

			//display start screeen 
			while(state == 0){
					GLCD_DisplayString(3, 5, 1, "Snake Game");
					GLCD_DisplayString(7, 1, 1, "Press INT0 to start");
			}
			//count down to start
			GLCD_Clear(Blue);
			GLCD_SetBackColor(Blue);
			GLCD_DisplayString(5, 10, 1, "3");
			osDelay(second);
			GLCD_DisplayString(5, 10, 1, "2");
			osDelay(second);
			GLCD_DisplayString(5, 10, 1, "1");
			osDelay(second);
			
			//putting initial snake (3 tiles) on screen
			GLCD_SetBackColor(White);
			//tail
			GLCD_DisplayString(5, 9, 1, " ");
			GLCD_DisplayString(5, 10, 1, " ");
			//head
			GLCD_DisplayString(5, 11, 1, " ");
			
			
			while(state == 1){
				//drawing apple
					GLCD_SetBackColor(Red);
					GLCD_DisplayString(apple_y, apple_x, 1, " ");
				//removing tail tile
					GLCD_SetBackColor(Blue);
					GLCD_DisplayString(last_tail_y, last_tail_x, 1, " ");
				//drawing new head tile
					GLCD_SetBackColor(White);
					GLCD_DisplayString(snake.head -> positionY, snake.head -> positionX, 1, " ");
				
				//speed slower if powerup
					if (powerUp){
						osDelay(second);
					}
					else{
						osDelay(second/4);
					}
			}
			GLCD_Clear(Black);
			
			//display score after end game 
			while(state == 2){
					GLCD_SetBackColor(Black);
					snprintf(score_str, 30, "Your Score: %d", score);
					GLCD_DisplayString(3, 2, 1, (unsigned char*)score_str);
					GLCD_DisplayString(7, 3, 1, "Press RESET to");
					GLCD_DisplayString(8, 2, 1, "Restart the Game");
					//printf("%d\n", state);
			}	
		}
}

//Task that updates the position of the snake 
void Snake(void *arg){
	while(1){
		int second = osKernelGetTickFreq();
	
		// start game
		while(state == 0){}
			
		//initialize snake nodes 
		snake.head = malloc(sizeof(positionNode_t));
		snake.tail = malloc(sizeof(positionNode_t));
		snake.head -> positionX = 11;
		snake.head -> positionY = 5;
		snake.head -> next = NULL;
		snake.head -> prev = malloc(sizeof(positionNode_t));
		snake.head -> prev -> positionX = 10;
		snake.head -> prev -> positionY = 5;
		snake.head -> prev -> next = snake.head;
		snake.head -> prev -> prev = snake.tail;
		snake.tail -> positionX = 9;
		snake.tail -> positionY = 5;
		snake.tail -> next = snake.head -> prev;
		snake.tail -> prev = NULL;
		
		//to sycronize with countdown
		osDelay(second * 4);
			
		while(state == 1){	
			//Lock the mutex
			osMutexAcquire(mutex_1, osWaitForever);
			int positionX_next = snake.head -> positionX;
			int positionY_next = snake.head -> positionY;
			
			//Calculates the position based off the direction
			if(snake_direction == UP){
					positionY_next += 1;
			}
			else if(snake_direction == DOWN){
					positionY_next -= 1;
			}
			else if(snake_direction == RIGHT){
					positionX_next += 1;
			}
			else{
					positionX_next -= 1;
			}
			
			//end game detection (edge)
			if(positionX_next > 19 || positionX_next < 0 || positionY_next < 0 || positionY_next > 9){
				state = 2;
				break;
			}
			//end game detection (eat itself)
			if(traverse(positionX_next,positionY_next)){
				state = 2;
				break;
			}
			
			//add to snake if 'eats' apple 
			if(positionX_next == apple_x && positionY_next == apple_y){
				positionNode_t *temp = NULL;
				temp = malloc(sizeof(positionNode_t)); //allocate memory using malloc;
				temp -> positionX = positionX_next;
				temp -> positionY = positionY_next;
				temp -> prev = snake.head;
				temp -> prev -> next = temp;
				temp -> next = NULL;
				snake.head = temp;
				snake.length ++;
				osMutexAcquire(mutex_2, osWaitForever);
				is_apple = false;
				energy ++;
				osMutexRelease(mutex_2);
				
				//score increase
				if (powerUp){
					osDelay(second);
					score += 200;
				}
				else{
					osDelay(second/4);
					score += 100;
				}
			}else{ //move snake by deleting tail and adding to head
				//Put the newest position at the head of snake
				positionNode_t *temp = NULL;
				temp = malloc(sizeof(positionNode_t)); //allocate memory using malloc;
				temp -> positionX = positionX_next;
				temp -> positionY = positionY_next;
				temp -> prev = snake.head;
				temp -> prev -> next = temp;
				temp -> next = NULL;
				snake.head = temp;
				
				//delete the tail of snake  
				//copy x y variables of tail position
				last_tail_x = snake.tail ->positionX;
				last_tail_y = snake.tail ->positionY;
				snake.tail = snake.tail -> next;
				free(snake.tail -> prev);
				snake.tail -> prev = NULL; 
				if (powerUp){
					osDelay(second);
				}
				else{
					osDelay(second/4);
				}
			}
			
			//Release the mutex
		  osMutexRelease(mutex_1);
			osThreadYield();
		}
		while(state == 2){
			energy = 0; 
			powerUp = 0;
			is_apple = 0;
			positionNode_t *current = snake.tail;
			positionNode_t *next;
			
			while (current != NULL)  
			{ 
				 next = current->next; 
				 free(current); 
				 current = next; 
			} 
			
			 snake.head = NULL; 
		}
	}
}

//Task that controls apples
void Apple(void *arg){
	while(1){
		int second = osKernelGetTickFreq();
	
	
		int i = 0;
	
		// start game
		while(state == 0){
			i ++;
		}
		//delay for sycronization
		osDelay(second * 4);
		
		while(state == 1){
			//generates random number for apple position
			int rand1 = (rand() + (i*i - 100))%20;
			int rand2 = (rand() + (i*i - 100))%10;
			if(rand1 < 0){
				rand1 *= -1;
			}
			if(rand2 < 0){
				rand2 *= -1;
			}
			//aquire mutex 
			osMutexAcquire(mutex_2, osWaitForever);
			//check for only one apple on the screen
			if (is_apple == false){
				apple_x = rand1;
				apple_y = rand2;
				//check if apple is on snake
				if (traverse(apple_x, apple_y)){
					continue;
				}
				is_apple = true;
			}
			//release mutex
			osMutexRelease(mutex_2);
		}
		while(state == 2){}
	}
}

void Game_Control(void *arg){
	while(1){
		int second = osKernelGetTickFreq();
		// if button push move to play state 
		while(state == 0){
			if (!(LPC_GPIO2->FIOPIN & (1<<10))){
				state = 1;
			}
		}
		//sycronization
		osDelay(second * 4);
		while(state == 1){
			//Lock the mutex
			osMutexAcquire(mutex_1, osWaitForever);
			
			//constanly polling the joystick position
			direction_t direction = joystick_read();
			//only update position if pushed or not backwards
			if(direction != CENTER && !(snake_direction == UP && direction == DOWN) && !(snake_direction == DOWN && direction == UP) && !(snake_direction == RIGHT && direction == LEFT) && !(snake_direction == LEFT && direction == RIGHT)){
					snake_direction = direction;
			}

			//Release the mutex
			osMutexRelease(mutex_1);	
			
			//polling to detect powerup boutton push 
			if (!(LPC_GPIO2->FIOPIN & (1<<10)) && energy >= 8){
				powerUp = 1;
			}
		}
		
		while(state == 2){}
	}
}
	
//Task that contols energy bar (LEDS)
void Energy(void *arg){
	while(1){	
		int second = osKernelGetTickFreq();
	
		// start game
		while(state == 0){
			LPC_GPIO1->FIOCLR |= 0xffffffff;
			LPC_GPIO2->FIOCLR |= 0xffffffff;
			
			LPC_GPIO1->FIODIR |= 0xB0000000;
			LPC_GPIO2->FIODIR |= 0x0000007C;
		}
		osDelay(second * 4);
		while(state == 1){
			//display on LED for different energy levels 
			if (energy == 8){
				LPC_GPIO2->FIOSET |= 0x0000040;
			}
			if (energy == 7){
				LPC_GPIO2->FIOSET |= 0x0000020;
			}
			if (energy == 6){
				LPC_GPIO2->FIOSET |= 0x0000010;
			}
			if (energy == 5){
				LPC_GPIO2->FIOSET |= 0x0000008;
			}
			if (energy == 4){
				LPC_GPIO2->FIOSET |= 0x0000004;
			}
			if (energy == 3){
				LPC_GPIO1->FIOSET |= 0x80000000;
			}
			if (energy == 2){
				LPC_GPIO1->FIOSET |= 0x20000000;
			}
			if (energy == 1){
				LPC_GPIO1->FIOSET |= 0x10000000;
			}
			//if powerup used decrease back to 0 
			if(powerUp == 1){
				osDelay(second * 3);
				LPC_GPIO2->FIOCLR |= 0x0000040;
				osDelay(second * 3);
				LPC_GPIO2->FIOCLR |= 0x0000020;
				osDelay(second * 3);
				LPC_GPIO2->FIOCLR |= 0x0000010;
				osDelay(second * 3);
				LPC_GPIO2->FIOCLR |= 0x0000008;
				osDelay(second * 3);
				LPC_GPIO2->FIOCLR |= 0x0000004;
				osDelay(second * 3);
				LPC_GPIO1->FIOCLR |= 0x80000000;
				osDelay(second * 3);
				LPC_GPIO1->FIOCLR |= 0x20000000;
				osDelay(second * 3);
				LPC_GPIO1->FIOCLR |= 0x10000000;
				//reset energy and powerup to 0 
				powerUp = 0;
				energy = 0;
			}
		}
		while(state == 2){}
  
	}
}

int main( void )
{
  //initialization
	GLCD_Init();
	osKernelInitialize();
	
	osThreadNew(Snake, NULL, NULL);
  osThreadNew(LCD_display, NULL, NULL);
	osThreadNew(Apple, NULL, NULL);
	osThreadNew(Energy, NULL, NULL);
	osThreadNew(Game_Control, NULL, NULL);
	mutex_1 = osMutexNew(NULL);
	osKernelStart();
	
	while(1);
}

