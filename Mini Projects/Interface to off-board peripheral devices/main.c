#include <LPC17xx.h>
#include "led.h"
#include "stdio.h"
#include "uart.h"
#include <cmsis_os2.h>
#include "MPU9250.h"
#include "sensor_fusion.h"


osMutexId_t mutex_1;  
osMutexId_t mutex_2;  

//Read gyro, acc, and mag values from MPU9250 and stored in memory
void read(void *arg){
	while(1){
		// Lock the Mutex
		osMutexAcquire(mutex_1, osWaitForever);
		MPU9250_read_gyro();
		MPU9250_read_acc();
		MPU9250_read_mag();
		//release the lock
		osMutexRelease(mutex_1);
		osThreadYield();
	}
}

//calls fusion function from sensor_fusion.h using values from read
void fusion(void *arg){
	while(1){
		osMutexAcquire(mutex_1, osWaitForever);
		osMutexAcquire(mutex_2, osWaitForever);
		sensor_fusion_update(MPU9250_gyro_data[0],MPU9250_gyro_data[1],MPU9250_gyro_data[2],MPU9250_accel_data[0],MPU9250_accel_data[1],MPU9250_accel_data[2],MPU9250_mag_data[0],MPU9250_mag_data[1],MPU9250_mag_data[2]);
		osMutexRelease(mutex_2);
		osMutexRelease(mutex_1);
		osThreadYield();
	}
}

//Sending 
void send(void *arg){
	while(1){
		osMutexAcquire(mutex_2, osWaitForever);
		printf("%f,%f,%f\n", sensor_fusion_getRoll(), sensor_fusion_getPitch(), sensor_fusion_getYaw());
		osMutexRelease(mutex_2);
		osThreadYield();
	}
}

int main( void )
{
//initialization
	LED_setup();
	sensor_fusion_init();
	sensor_fusion_begin(250);
	MPU9250_init(1,1);
	
	
	osKernelInitialize();
	
	osThreadNew(read, NULL, NULL);
	osThreadNew(fusion, NULL, NULL);
	osThreadNew(send, NULL, NULL);
	mutex_1 = osMutexNew(NULL);
	mutex_2 = osMutexNew(NULL);
	osKernelStart();
	
	while(1);
}




