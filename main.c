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

#define green  	1
#define red  	2
#define blue  	3

#define green_led	LED4
#define red_led		LED5
#define blue_led	LED6


/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );

/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */
static void Manager_Task( void *pvParameters );
static void Blue_LED_User_Task( void *pvParameters );
static void Green_LED_User_Task( void *pvParameters );
static void Red_LED_User_Task( void *pvParameters );

static void DDS_Manager_Task( void *pvParameters );
static void Generate_DD_Task1( void *pvParameters );
static void Generate_DD_Task2( void *pvParameters );
static void Generate_DD_Task3( void *pvParameters );
static void Monitor_Task( void *pvParameters );

xQueueHandle xQueue_GeneratedTasks_handle = 0;


/*-----------------------------------------------------------*/
  // DD_TASK STRUCT
typedef enum task_type task_type;
enum task_type {PERIODIC, APERIODIC};

typedef struct dd_task {
//	TaskHandle_t t_handle;
	task_type type;
	uint32_t task_id;
	uint32_t release_time;
	uint32_t absolute_deadline;
	uint32_t completion_time;
} dd_task;

// DD_TASK LIST STRUCT
typedef struct dd_task_list {
	struct dd_task task;
	struct dd_task_list *next_task;
	struct dd_task_list *prev_task;
} dd_task_list;

//Task lists
struct dd_task_list active_task_list;
struct dd_task_list comp_task_list;
struct dd_task_list overdue_task_list;

/*-----------------------------------------------------------*/
// THIS IS WHERE YOU SET THE BENCHMARK VALUES
int t1_ET = 95;
int t1_P = 500;
int t2_ET = 150;
int t2_P = 500;
int t3_ET = 250;
int t3_P = 750;

int T1 = 1;
int T2 = 2;
int T3 = 3;

TaskHandle_t t1_handle;


/*-----------------------------------------------------------*/
  // Core Functions
static void create_and_add_to_list(int task_num) {

	int cur_time = xTaskGetTickCount() * (1/configTICK_RATE_HZ) * 1000;



	struct dd_task* new_dd_task = (struct dd_task*)malloc(sizeof(struct dd_task));

	if(task_num == 1){
		new_dd_task->absolute_deadline = cur_time + t1_P;
		//new_dd_task->completion_time = 0;
		new_dd_task->release_time = cur_time;
		//new_dd_task->t_handle =;
		new_dd_task->task_id = 1;
		new_dd_task->type = PERIODIC;
	}
	else if(task_num == 2){
		new_dd_task->absolute_deadline = cur_time + t2_P;
		//new_dd_task->completion_time = 0;
		new_dd_task->release_time = cur_time;
		//new_dd_task->t_handle =;
		new_dd_task->task_id = 2;
		new_dd_task->type = PERIODIC;
	}
	else if(task_num == 3){
		new_dd_task->absolute_deadline = cur_time + t3_P;
		//new_dd_task->completion_time = 0;
		new_dd_task->release_time = cur_time;
		//new_dd_task->t_handle =;
		new_dd_task->task_id = 3;
		new_dd_task->type = PERIODIC;
	}


	dd_task_list new_dd_task_list;
	new_dd_task_list.next_task = NULL;
	new_dd_task_list.prev_task = NULL;
	new_dd_task_list.task = *new_dd_task;



	if(&active_task_list.task != NULL){ //if empty list

		active_task_list.next_task = &active_task_list;
		active_task_list.prev_task = NULL;
		active_task_list.task = *new_dd_task;
	}
	else { //if none empty list

		struct dd_task_list current_task_list = active_task_list;
		while(&active_task_list.task != NULL){
			if (new_dd_task_list.task.absolute_deadline < current_task_list.task.absolute_deadline) {
				//insert task into task list

				new_dd_task_list.next_task = &current_task_list;
				new_dd_task_list.prev_task = current_task_list.prev_task;


				current_task_list.prev_task = &new_dd_task_list;
				new_dd_task_list.prev_task->next_task = &new_dd_task_list;

			} else {
				current_task_list = current_task_list->next_task;
			}

		}
	}




}


static int release_dd_task(){
	return 0;
}

static int complete_dd_task(){
	return 0;
}

void get_active_dd_task_list(){}

void get_completed_dd_task_list(){}

void get_overdue_dd_task_list(){}


/*-----------------------------------------------------------*/




int main(void)
{

	/* Initialize LEDs */

	STM_EVAL_LEDInit(green_led);
	STM_EVAL_LEDInit(red_led);
	STM_EVAL_LEDInit(blue_led);

	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */
	prvSetupHardware();


	/* Create the queues.
	http://www.freertos.org/a00116.html */
	xQueue_GeneratedTasks_handle = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
							sizeof( uint16_t ));	/* The size of each item the queue holds. */


	/* Add to the registry, for the benefit of kernel aware debugging. */
	vQueueAddToRegistry( xQueue_GeneratedTasks_handle, "Active_Task_Queue" );


	//xTaskCreate( Manager_Task, "Manager", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate( Blue_LED_User_Task, "Blue_LED", configMINIMAL_STACK_SIZE, NULL, 1, &t1_handle);
	//xTaskCreate( Red_LED_User_Task, "Red_LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	//xTaskCreate( Green_LED_User_Task, "Green_LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	xTaskCreate( DDS_Manager_Task, "DDS_Manager", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Monitor_Task, "Monitor_Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	xTaskCreate( Generate_DD_Task1, "Generate_DD_Task1", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
//	xTaskCreate( Generate_DD_Task2, "Generate_DD_Task2", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
//	xTaskCreate( Generate_DD_Task3, "Generate_DD_Task3", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}


/*-----------------------------------------------------------*/
// DDS TASK
/*-----------------------------------------------------------*/
static void DDS_Manager_Task( void *pvParameters )
{
	// IN THIS TASK WE WILL HAVE:
	// DD task lists (Active, completed, & overdue)
	// Timer
	// Call DD_functions
	// Hold the queues

	int num;


	if(xQueueReceive(xQueue_GeneratedTasks_handle, &num, 500)){
		//create t1 struct, add to active list
		create_and_add_to_list(num);

	}



	uint16_t released_task_info = release_dd_task();

	uint16_t completed_task_info = complete_dd_task();

	get_active_dd_task_list();

	get_completed_dd_task_list();

	get_overdue_dd_task_list();

	while(1){

	}

}


/*-----------------------------------------------------------*/
// DD TASK GENERATOR TASK
/*-----------------------------------------------------------*/
//void vTimerOneCallback ( TimerHandle_t xTimer )
//{
//	// Create Task 1
//	STM_EVAL_LEDOn(red_led);
//
//	// add it to the DDS manager queue (as created task message)
//
// }

static void Generate_DD_Task1( void *pvParameters)
{
	while(1){

//		struct dd_task dd_task_instance;
//		dd_task_instance.type = PERIODIC;
//		dd_task_instance.t_handle = t1_handle;
//		dd_task_instance.task_id = 1;



		if( xQueueSend(xQueue_Active_handle,&T1,1000)) {
			vTaskDelay(t1_P);
		}
	}

}

//static void Generate_DD_Task2( void *pvParameters)
//{
//	while(1){
//
//		struct dd_task dd_task_instance;
//		dd_task_instance.type = PERIODIC;
//
//
//		if( xQueueSend(xQueue_handle,&dd_task_instance,1000)) {
//			vTaskDelay(P);
//		}
//	}
//
//}
//
//static void Generate_DD_Task3( void *pvParameters)
//{
//	while(1){
//
//		struct dd_task dd_task_instance;
//		dd_task_instance.type = PERIODIC;
//
//
//		if( xQueueSend(xQueue_handle,&dd_task_instance,1000)) {
//			vTaskDelay(P);
//		}
//	}
//
//}


/*-----------------------------------------------------------*/
// MONITOR TASK
/*-----------------------------------------------------------*/
static void Monitor_Task( void *pvParameters )
{
	// IN THIS TASK WE WILL HAVE:

	while(1){

	}

}


/*-----------------------------------------------------------*/
//static void Manager_Task( void *pvParameters )
//{
//	uint16_t tx_data = green;
//
//
//	while(1)
//	{
//
//		if(tx_data == green)
//			STM_EVAL_LEDOn(green_led);
//		if(tx_data == red)
//			STM_EVAL_LEDOn(red_led);
//		if(tx_data == blue)
//			STM_EVAL_LEDOn(blue_led);
//
//		if( xQueueSend(xQueue_handle,&tx_data,1000))
//		{
//			printf("Manager: %u ON!\n", tx_data);
//			if(++tx_data == 4)
//				tx_data = 1;
//			vTaskDelay(1000);
//		}
//		else
//		{
//			printf("Manager Failed!\n");
//		}
//	}
//}


/*-----------------------------------------------------------*/
// USER TASKS
/*-----------------------------------------------------------*/
static void Blue_LED_User_Task( void *pvParameters )
{
	while(1)
	{
		STM_EVAL_LEDOn(blue_led);
		vTaskDelay(100);
		STM_EVAL_LEDOff(blue_led);
		vTaskDelay(600);
	}
}


/*-----------------------------------------------------------*/
//static void Green_LED_User_Task( void *pvParameters )
//{
//	uint16_t rx_data;
//	while(1)
//	{
//		if(xQueueReceive(xQueue_handle, &rx_data, 500))
//		{
//			if(rx_data == green)
//			{
//				vTaskDelay(250);
//				STM_EVAL_LEDOff(green_led);
//				printf("Green Off.\n");
//			}
//			else
//			{
//				if( xQueueSend(xQueue_handle,&rx_data,1000))
//					{
//						printf("GreenTask GRP (%u).\n", rx_data); // Got wrong Package
//						vTaskDelay(500);
//					}
//			}
//		}
//	}
//}
//
///*-----------------------------------------------------------*/
//static void Red_LED_User_Task( void *pvParameters )
//{
//	uint16_t rx_data;
//	while(1)
//	{
//		if(xQueueReceive(xQueue_handle, &rx_data, 500))
//		{
//			if(rx_data == red)
//			{
//				vTaskDelay(250);
//				STM_EVAL_LEDOff(red_led);
//				printf("Red off.\n");
//			}
//			else
//			{
//				if( xQueueSend(xQueue_handle,&rx_data,1000))
//					{
//						printf("RedTask GRP (%u).\n", rx_data); // Got wrong Package
//						vTaskDelay(500);
//					}
//			}
//		}
//	}
//}


/*-----------------------------------------------------------*/
// MISCELLANEOUS TASKS
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
}
