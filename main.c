/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

// PROJECT TRAFFIC LIGHT


/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"




/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 100


/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );

/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */
static void Traffic_Flow_Adjustment_Task( void *pvParameters );
static void Traffic_Generator_Task( void *pvParameters );
static void Traffic_Light_State_Task( void *pvParameters );
static void System_Display_Task( void *pvParameters );

xQueueHandle xQueue_handle = 0;

//mutexes for global variables
xMutexPotValue = xSemaphoreCreateMutex(uint8_t);
xMutexTrafficLightState = xSemaphoreCreateMutex();
xMutexNewCar = xSemaphoreCreateMutex();

xSemaphoreGive(xMutexPotValue);
xSemaphoreGive(xMutexTrafficLightState);
xSemaphoreGive(xMutexNewCar);

// create timers - TODO
greenLightTime = xTimerCreate();
redLightTime = xTimerCreate();



// Global Variables
uint16_t pot_val = 0;
uint16_t new_car = 0; // next start of first queue
uint16_t last_1 = 0; // next start of second queue
uint16_t last_2 = 0; // next start of third queue
uint16_t traffic_light_state = 1; // 0 is green, 1 is yellow, 2 is red
car_queue_1 = xQueueCreate(8, sizeof( uint16_t )); //stopping queue
car_queue_2 = xQueueCreate(3, sizeof( uint16_t )); // continue on yellow
car_queue_3 = xQueueCreate(6, sizeof( uint16_t )); // after lights

//^^ these are actually x queues


/*-----------------------------------------------------------*/

int main(void)
{

	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */
	prvSetupHardware();

	// Test
//	GPIO_SetBits(GPIOB,GPIO_Pin_3); //Red
//	GPIO_SetBits(GPIOB,GPIO_Pin_4); //Green
//	GPIO_SetBits(GPIOB,GPIO_Pin_5); //Yellow

	GPIO_Write(GPIOC,0b111); //SR write
	GPIO_SetBits(GPIOC,GPIO_Pin_9); //SR set bits
	GPIO_SetBits(GPIOC,GPIO_Pin_8); //SR clock set
	GPIO_ResetBits(GPIOC,GPIO_Pin_8); //SR clock reset


	/* Create the queue used by the queue send and queue receive tasks.
	http://www.freertos.org/a00116.html */
	xQueue_handle = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
							sizeof( uint16_t ) );	/* The size of each item the queue holds. */

	/* Add to the registry, for the benefit of kernel aware debugging. */
	vQueueAddToRegistry( xQueue_handle, "MainQueue" );

	xTaskCreate( Traffic_Flow_Adjustment_Task, "TFA", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Traffic_Generator_Task, "TFG", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Traffic_Light_State_Task, "TLS", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( System_Display_Task, "SD", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

//
//	traffic_light_state = 0;
//	sleep(4);
//	traffic_light_state = 1;
//	sleep(4);
//	traffic_light_state = 2;
//	sleep(4);


	return 0;
}



/*-----------------------------------------------------------*/

static void Traffic_Flow_Adjustment_Task( void *pvParameters )
{
	//Read value from Pot and update the global variable for traffic flow rate
	while(1)
	{
		if (xSemaphoreTake(xMutexPotValue)) {
			//Read from the Pot (values range from 0 to 61)
			ADC_SoftwareStartConv(ADC1);
			ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC);
			pot_val = ADC_GetConversionValue(ADC1);

			//make sure that the pot value is greater than zero
			if(0 == pot_val){
				pot_val = 1;
			}

			printf("pot value: %d\n",pot_val);
			vTaskDelay(250);
			xSemaphoreGive(xMutexPotValue);
		}
	}

}


/*-----------------------------------------------------------*/

static void Traffic_Generator_Task( void *pvParameters )
{
	// Based on the value changed by the Traffic Flow Adjustment Task
	// create a chance of generating a car

	while(1)
	{
		if (xSemaphoreTake(xMutexPotValue) && xSemaphoreTake(xMutexNewCar)) {
			// Generate a random value between 0 and 90
			uint16_t rand_val = 0;
			rand_val = rand() % 90;

			// If the random value is less than the pot create a new car
			// NOTE: the greater the pot_value the heavier the traffic flow
			if( rand_val <= pot_val){
				new_car = 1;
			} else {
				new_car = 0;
			}

			vTaskDelay(250);
			xSemaphoreGive(xMutexPotValue);
			xSemaphoreGive(xMutexNewCar);
		}
	}
}

/*-----------------------------------------------------------*/

static void Traffic_Light_State_Task( void *pvParameters )
{

	//Cycle through light states based on a timer.
	// Update light state Global variable

	uint16_t wait_time_val = 0;
	uint16_t wait_time_scalar = 0;
	printf("test %d", traffic_light_state);

	if(traffic_light_state == 2){
		//Here if the light is currently red

		//wait red light time (TODO use redlight timer)
		wait_time_scalar = pot_val / 10; // will result in a scalar from 0 to 6
		wait_time_scalar = wait_time_scalar * 100; // convert into seconds
		wait_time_scalar = 600 - wait_time_scalar; // take the inverse (opposite of green light)
		wait_time_val = wait_time_scalar + 200; // add 2 seconds so the wait is at least that
		vTaskDelay(wait_time_val);

		//update light to green
		traffic_light_state = 0;

	} else if(traffic_light_state == 1){
		//Here if the light is currently yellow

		//wait constant time
		vTaskDelay(300); // 3 seconds

		//update light to red
		traffic_light_state = 2;

	} else {
		//Here if the light is currently green

		//wait green light time (TODO use greenlight timer)
		wait_time_scalar = pot_val / 10; // will result in a scalar from 0 to 6
		wait_time_scalar = wait_time_scalar * 100; // convert into seconds
		wait_time_val = wait_time_scalar + 200; // add 2 seconds so the wait is at least that
		vTaskDelay(wait_time_val);

		//update light to yellow
		traffic_light_state = 1;

	}


}


/*-----------------------------------------------------------*/

static void System_Display_Task( void *pvParameters )
{
	static void shift_queue(queue, first_bit){
		for(int i = length(queue) - 1; i > 0; i--){
			queue[i] = queue[i-1];
		}
		queue[0] = first_bit;

	}


	while(1)
	{
		if (traffic_light_state == 0) { //green
			GPIO_ResetBits(GPIOB,GPIO_Pin_3); //Red
			GPIO_SetBits(GPIOB,GPIO_Pin_4); //Green
			GPIO_ResetBits(GPIOB,GPIO_Pin_5); //Yellow

			shift_queue(car_queue_1, new_car); //shift queue 1
			shift_queue(car_queue_2, last_1); //shift queue 2
			shift_queue(car_queue_3, last_2); //shift queue 3

		}
		else if (traffic_light_state == 1) { //yellow
			GPIO_ResetBits(GPIOB,GPIO_Pin_3); //Red
			GPIO_ResetBits(GPIOB,GPIO_Pin_4); //Green
			GPIO_SetBits(GPIOB,GPIO_Pin_5); //Yellow
			shift_queue(car_queue_2, last_1); //shift queue 2
			shift_queue(car_queue_3, last_2); //shift queue 3
		}
		else if (traffic_light_state == 2) { //red
			GPIO_SetBits(GPIOB,GPIO_Pin_3); //Red
			GPIO_ResetBits(GPIOB,GPIO_Pin_4); //Green
			GPIO_ResetBits(GPIOB,GPIO_Pin_5); //Yellow
			shift_queue(car_queue_3, last_2); //shift queue 3
		}
	}
}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */


	/*
	 *     GPIO and ADC Setup
	 */


	// GPIO SETUP FOR TRAFFIC LIGHTS
		// Enable Clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // AHB High speed bus for GPIOB ports
		// GPIO Init (configurations)
	GPIO_InitTypeDef TrafficLight_GPIO_InitStruct;
	TrafficLight_GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;  // Pins for Red, Yellow, & Green Lights
	TrafficLight_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; // Set Out bc LEDS are output
	TrafficLight_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // High is pushed to VCC, low is pulled to GND
	TrafficLight_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; // No switch
	TrafficLight_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // High speed for LEDs
		// GPIO Init (call)
	GPIO_Init(GPIOB, &TrafficLight_GPIO_InitStruct);


	// GPIO SETUP FOR SHIFT REG
		// Enable Clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		// GPIO Init (configurations)
	GPIO_InitTypeDef ShiftReg_GPIO_InitStruct;
	ShiftReg_GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;  // Data Pin
	ShiftReg_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; // Set Out bc shift registers are output
	ShiftReg_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // High is pushed to VCC, low is pulled to GND
	ShiftReg_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; // No switch
	ShiftReg_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // High speed for shift registers
		// GPIO Init (call)
	GPIO_Init(GPIOC, &ShiftReg_GPIO_InitStruct);

	// ADC SETUP FOR POTENTIOMETER
		// Enable Clocks (GPIO and ADC)
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // High Speed Bus for ports
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // Peripheral Bus for ADC
		// GPIO Pin configuration (8.3.12)
	GPIO_InitTypeDef ADC_Pin_GPIO_InitStruct;
	ADC_Pin_GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	ADC_Pin_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN; // Set this pin for analog data
		// ADC_Init (configurations)
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; // Conv done without extrenal triggers
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right; // Data align (assume right? ADC_DataAlign_Right)
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_6b; // Lower resolution = fewer clk cycles (12, 10, 8, or 6)
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;  // Only need to scan one channel
	ADC_InitStruct.ADC_ExternalTrigConv = DISABLE; // Dont need bc ContinuousConvMode is on
		// GPIO & ADC_Init (calls)
	GPIO_Init(GPIOA, &ADC_Pin_GPIO_InitStruct);
	ADC_Init(ADC1, &ADC_InitStruct);
		// ADC_Cmd and Channel Config
	ADC_Cmd(ADC1, ENABLE);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_3Cycles); // NOTE: can test dif sample times

}
