/****************************************************************************
 * This code is copyright (c) 2017 by the University of Waterloo and may    *
 * not be redistributed without explicit written permission.                *
 ****************************************************************************/
 
#include <LPC17xx.h>
#include "stdio.h"
#include "uart.h"
#include "GLCD.h"
#include <cmsis_os.h>


int main( void ) 
{
	//LED
	LPC_GPIO1->FIOCLR |= 0xffffffff;
	LPC_GPIO2->FIOCLR |= 0xffffffff;
	
	LPC_GPIO1->FIODIR |= 0xB0000000;
	LPC_GPIO2->FIODIR |= 0x0000007C;
	
	int input = 123;
	int remainder; 
  int i = 0;
	int binary[8];
	binary[0] = 0;
	binary[1] = 0;
	binary[2] = 0;
	binary[3] = 0;
	binary[4] = 0;
	binary[5] = 0;
	binary[6] = 0;
	binary[7] = 0;
  
  while(input != 0) {
    remainder = input%2;
    input = input/2;
    binary[i] = remainder;
    i++;
  }
	
	if (binary[0] == 1){
		LPC_GPIO2->FIOSET |= 0x0000040;
	}
	if (binary[1] == 1){
		LPC_GPIO2->FIOSET |= 0x0000020;
	}
	if (binary[2] == 1){
		LPC_GPIO2->FIOSET |= 0x0000010;
	}
	if (binary[3] == 1){
		LPC_GPIO2->FIOSET |= 0x0000008;
	}
	if (binary[4] == 1){
		LPC_GPIO2->FIOSET |= 0x0000004;
	}
	if (binary[5] == 1){
		LPC_GPIO1->FIOSET |= 0x80000000;
	}
	if (binary[6] == 1){
		LPC_GPIO1->FIOSET |= 0x20000000;
	}
	if (binary[7] == 1){
		LPC_GPIO1->FIOSET |= 0x10000000;
	}
	
	//Joystick
	// while(1){
	// 	if ((LPC_GPIO1->FIOPIN & 0x00100000) != 0){
	// 		//not pressed 
	// 		if ((LPC_GPIO1->FIOPIN & 0x04000000) == 0){
	// 			printf("down, not pressed\n");
	// 		}
	// 		else if ((LPC_GPIO1->FIOPIN & 0x02000000) == 0){
	// 			printf("right, not pressed\n");
	// 		}
	// 		else if ((LPC_GPIO1->FIOPIN & 0x01000000) == 0){
	// 			printf("up, not pressed\n");
	// 		}
	// 		else if ((LPC_GPIO1->FIOPIN & 0x00800000) == 0){
	// 			printf("left, not pressed\n");
	// 		}
	// 	}
	// 	else if ((LPC_GPIO1->FIOPIN & 0x00100000) == 0){
	// 		//pressed 
	// 		if ((LPC_GPIO1->FIOPIN & 0x04000000) == 0){
	// 			printf("down, pressed\n");
	// 		}
	// 		else if ((LPC_GPIO1->FIOPIN & 0x02000000) == 0){
	// 			printf("right, pressed\n");
	// 		}
	// 		else if ((LPC_GPIO1->FIOPIN & 0x01000000) == 0){
	// 			printf("up, pressed\n");
	// 		}
	// 		else if ((LPC_GPIO1->FIOPIN & 0x00800000) == 0){
	// 			printf("left, pressed\n");
	// 		}
	// 	}
	// }

	//Potentiometer 
	// while(1){
	// 	LPC_PINCON->PINSEL1 &= ~(0x03<<18);
	// 	LPC_PINCON->PINSEL1 |= (0x01<<18);
		
	// 	//LPC_SC->PCONP &= ~(0x01<<12);
	// 	LPC_SC->PCONP |= (0x01<<12);
		
	// 	LPC_ADC->ADCR = (1 << 2) |  (4 << 8) | (1 << 21);    
		
	// 	LPC_ADC->ADCR |= (0x01<<24);
		
	// 	printf("Potential read: %d\n", (LPC_ADC->ADGDR & 0xFFF0));
	// }
	
	//LCD
	
	// GLCD_Init();
	// GLCD_SetBackColor(Blue);
	// for (int j = 0; j < 5; j++){
	// 	for (int i = 0; i < 20; i ++){
	// 		GLCD_DisplayString(j, i, 1, " ");
	// 	}
	// }
	// GLCD_DisplayString(5, 0, 1, " ");
	// GLCD_DisplayString(5, 1, 1, " ");
	// GLCD_DisplayString(5, 2, 1, "H");
	// GLCD_DisplayString(5, 3, 1, "e");
	// GLCD_DisplayString(5, 4, 1, "l");
	// GLCD_DisplayString(5, 5, 1, "l");
	// GLCD_DisplayString(5, 6, 1, "o");
	// GLCD_DisplayString(5, 7, 1, " ");
	// GLCD_DisplayString(5, 8, 1, "W");
	// GLCD_DisplayString(5, 9, 1, "o");
	// GLCD_DisplayString(5, 10, 1, "r");
	// GLCD_DisplayString(5, 11, 1, "l");
	// GLCD_DisplayString(5, 12, 1, "d");
	// GLCD_DisplayString(5, 13, 1, "!");
	// GLCD_DisplayString(5, 14, 1, " ");
	// GLCD_DisplayString(5, 15, 1, " ");
	// GLCD_DisplayString(5, 16, 1, " ");
	// GLCD_DisplayString(5, 17, 1, " ");
	// GLCD_DisplayString(5, 18, 1, " ");
	// GLCD_DisplayString(5, 19, 1, " ");
	// for (int j = 6; j < 10; j++){
	// 	for (int i = 0; i < 20; i ++){
	// 		GLCD_DisplayString(j, i, 1, " ");
	// 	}
	// }
	
	// osKernelInitialize();
	// osKernelStart();
	
	//osThreadCreate(osThread(potentiometer), NULL);
	//osThreadCreate(osThread(joystrick), NULL);
	//osThreadCreate(osThread(button), NULL);
	
	while(1);
}

//MT
	void potentiometer(void const *arg){
		while(1){
			LPC_PINCON->PINSEL1 &= ~(0x03<<18);
			LPC_PINCON->PINSEL1 |= (0x01<<18);
			
			//LPC_SC->PCONP &= ~(0x01<<12);
			LPC_SC->PCONP |= (0x01<<12);
			
			LPC_ADC->ADCR = (1 << 2) |  (4 << 8) | (1 << 21);    
			
			LPC_ADC->ADCR |= (0x01<<24);
			
			printf("Potential read: %d\n", (LPC_ADC->ADGDR & 0xFFF0));
		}
	}

	void joystrick(void const *arg){
		while(1){
			if ((LPC_GPIO1->FIOPIN & 0x00100000) != 0){
				//not pressed 
				if ((LPC_GPIO1->FIOPIN & 0x04000000) == 0){
					printf("down, not pressed\n");
				}
				else if ((LPC_GPIO1->FIOPIN & 0x02000000) == 0){
					printf("right, not pressed\n");
				}
				else if ((LPC_GPIO1->FIOPIN & 0x01000000) == 0){
					printf("up, not pressed\n");
				}
				else if ((LPC_GPIO1->FIOPIN & 0x00800000) == 0){
					printf("left, not pressed\n");
				}
			}
			else if ((LPC_GPIO1->FIOPIN & 0x00100000) == 0){
				//pressed 
				if ((LPC_GPIO1->FIOPIN & 0x04000000) == 0){
					printf("down, pressed\n");
				}
				else if ((LPC_GPIO1->FIOPIN & 0x02000000) == 0){
					printf("right, pressed\n");
				}
				else if ((LPC_GPIO1->FIOPIN & 0x01000000) == 0){
					printf("up, pressed\n");
				}
				else if ((LPC_GPIO1->FIOPIN & 0x00800000) == 0){
					printf("left, pressed\n");
				}
			}
		}
	}
	
	void button(void const *arg){
		
	}
