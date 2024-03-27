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
#include <stdlib.h> //for the free
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"



/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 1000


#define blue  	1
#define green  	2
#define red  	3

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

static void Blue_LED_User_Task( void *pvParameters );
static void Green_LED_User_Task( void *pvParameters );
static void Red_LED_User_Task( void *pvParameters );

static void DDS_Manager_Task( void *pvParameters );
static void Generate_DD_Task1( void *pvParameters );
static void Generate_DD_Task2( void *pvParameters );
static void Generate_DD_Task3( void *pvParameters );
static void Monitor_Task( void *pvParameters );

xQueueHandle xQueue_GeneratedTasks_handle = 0;
xQueueHandle xQueue_DisplayTask_handle = 0;
xQueueHandle xQueue_CompTimeTask_handle = 0;

/*-----------------------------------------------------------*/
// THIS IS WHERE YOU SET THE BENCHMARK VALUES
int t1_ET = 95;
int t1_P = 500;
int t2_ET = 150;
int t2_P = 500;
int t3_ET = 250;
int t3_P = 750;


int global_cur_time = 0;

int dd_task_count = 0;
int dd_task_comp_count = 0;
int dd_task_overdue_count = 0;

int time_tester = 0;

//testing
int test_if_received = 0;


TaskHandle_t t1_handle;
TaskHandle_t t2_handle;
TaskHandle_t t3_handle;
TaskHandle_t monitor_handle;

/*-----------------------------------------------------------*/
// THIS IS WHERE WE HAVE THE LNKED LIST CODE
// https://www.geeksforgeeks.org/generic-linked-list-in-c-2/

typedef struct Node {
	uint32_t task_id;
	uint32_t release_time;
	uint32_t absolute_deadline;
	uint32_t completion_time;
    struct Node* next;
} Node;

typedef struct List {
    int size;
    Node* head;
} List;


//Task lists
List* active_task_list;
List* comp_task_list;
List* overdue_task_list;

List* create_list() {
    List* new_list = (List*)malloc(sizeof(List));
    new_list->size = 0;
    new_list->head = NULL;
    return new_list;
}

/*Function to swap the nodes */
struct Node* swap(struct Node* ptr1, struct Node* ptr2)
{
    struct Node* tmp = ptr2->next;
    ptr2->next = ptr1;
    ptr1->next = tmp;
    return ptr2;
}

/* Function to sort the list */
void sortByDeadline(struct Node** head, int count)
{
    struct Node** h;
    int i, j, swapped;

    for (i = 0; i <= count; i++) {

        h = head;
        swapped = 0;

        for (j = 0; j < count - i - 1; j++) {

            struct Node* p1 = *h;
            struct Node* p2 = p1->next;

            if (p1->absolute_deadline > p2->absolute_deadline) {

                /* update the link after swapping */
                *h = swap(p1, p2);
                swapped = 1;
            }

            h = &(*h)->next;
        }

        /* break if the loop ended without any swap */
        if (swapped == 0)
            break;
    }
}

void add_to_list(List* list, uint32_t task_id, uint32_t release_time, int period) {
    Node* new_node = (Node*)malloc(sizeof(Node));

    new_node->task_id = task_id;
    new_node->release_time = release_time;
    new_node->absolute_deadline = release_time + period;
    new_node->completion_time = 0;

    new_node->next = list->head;
    list->head = new_node;
    list->size++;

    sortByDeadline(&list->head, list->size);

}

void add_to_list_comp(List* list, uint32_t task_id, uint32_t release_time, int period, int completion_time) {
    Node* new_node = (Node*)malloc(sizeof(Node));

    new_node->task_id = task_id;
    new_node->release_time = release_time;
    new_node->absolute_deadline = release_time + period;
    new_node->completion_time = completion_time;

    new_node->next = list->head;
    list->head = new_node;
    list->size++;

    //sortByDeadline(&list->head, list->size);

}


void remove_from_list(List* list) {
    if (list->size > 0) {
		Node* node_to_remove = list->head;
		list->head = node_to_remove->next;
		free(node_to_remove);
		list->size--;
    }

}


/*-----------------------------------------------------------*/
  // Core Functions


static void create_and_add_to_list(List* active_task_list, int task_num) {

	global_cur_time = xTaskGetTickCount();// * (1/configTICK_RATE_HZ) * 1000;

	// Adds task of given number to active list (will be put in order too)
	if(task_num == 1){
		add_to_list(active_task_list, 1, global_cur_time, t1_P);
	} else if(task_num == 2){
		add_to_list(active_task_list, 2, global_cur_time, t2_P);
	} else if(task_num == 3){
		add_to_list(active_task_list, 3, global_cur_time, t3_P);
	}



}



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

	xQueue_DisplayTask_handle = xQueueCreate( 	mainQUEUE_LENGTH, sizeof( uint16_t ));
	xQueue_CompTimeTask_handle = xQueueCreate( 	mainQUEUE_LENGTH, sizeof( uint16_t ));


	/* Add to the registry, for the benefit of kernel aware debugging. */
	vQueueAddToRegistry( xQueue_GeneratedTasks_handle, "Generated_Tasks_Queue" );
	vQueueAddToRegistry( xQueue_DisplayTask_handle, "Display_Tasks_Queue" );
	vQueueAddToRegistry( xQueue_CompTimeTask_handle, "Completion_Time_Tasks_Queue" );

	xTaskCreate( Blue_LED_User_Task, "Blue_LED", configMINIMAL_STACK_SIZE, NULL, 3, &t1_handle);
	xTaskCreate( Green_LED_User_Task, "Green_LED", configMINIMAL_STACK_SIZE, NULL, 3, &t2_handle);
	xTaskCreate( Red_LED_User_Task, "Red_LED", configMINIMAL_STACK_SIZE, NULL, 3, &t3_handle);

	xTaskCreate( DDS_Manager_Task, "DDS_Manager", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate( Monitor_Task, "Monitor_Task", configMINIMAL_STACK_SIZE, NULL, 3, &monitor_handle);

	xTaskCreate( Generate_DD_Task1, "Generate_DD_Task1", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
	xTaskCreate( Generate_DD_Task2, "Generate_DD_Task2", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
	xTaskCreate( Generate_DD_Task3, "Generate_DD_Task3", configMINIMAL_STACK_SIZE, NULL, 4, NULL);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}


/*-----------------------------------------------------------*/
// DDS TASK
/*-----------------------------------------------------------*/
static void DDS_Manager_Task( void *pvParameters )
{

	uint16_t num;

	active_task_list = create_list();
	comp_task_list = create_list();
	overdue_task_list = create_list();

	//vTaskDelay(10);

	while(1){

		if(xQueueReceive(xQueue_GeneratedTasks_handle, &num, 5000)){
			//creates a dd_task struct, add adds it to the active list

			create_and_add_to_list(active_task_list, num);
			dd_task_count++;

			vTaskResume(monitor_handle); //allow task monitor to do one update cycle

		}

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
	uint16_t num = 1;

	while(1){
		if( xQueueSend(xQueue_GeneratedTasks_handle,&num,1000)) {
			vTaskDelay(t1_P);
			//vTaskDelay(3000); //test
		}
	}

}

static void Generate_DD_Task2( void *pvParameters)
{
	uint16_t num = 2;

	while(1){
		if( xQueueSend(xQueue_GeneratedTasks_handle,&num,1000)) {
			vTaskDelay(t2_P);
			//vTaskDelay(3000); //test
		}
	}

}

static void Generate_DD_Task3( void *pvParameters)
{
	uint16_t num = 3;

	while(1){
		if( xQueueSend(xQueue_GeneratedTasks_handle,&num,1000)) {
			vTaskDelay(t3_P);
			//vTaskDelay(3000); //test
		}
	}

}


/*-----------------------------------------------------------*/
// MONITOR TASK
/*-----------------------------------------------------------*/
static void Monitor_Task( void *pvParameters )
{

		uint16_t current_task;
		uint16_t current_p;
		uint16_t comp_time;



	while(1){

		current_task = active_task_list->head->task_id;

		if (current_task == 1) {
			vTaskResume(t1_handle);
			current_p = t1_P;
		} else if (current_task == 2) {
			vTaskResume(t2_handle);
			current_p = t2_P;
		} else if (current_task == 3){
			vTaskResume(t3_handle);
			current_p = t3_P;
		}


		//send the first task to the task display queue
		//xQueueSend(xQueue_DisplayTask_handle,&current_task,1000);

		vTaskSuspend(NULL);

		//wait for the response from the task finished queue
		xQueueReceive(xQueue_CompTimeTask_handle, &comp_time, 1000);

		if(comp_time <= active_task_list->head->absolute_deadline){
			// add to comp and add the task execution time to the finished task
			add_to_list_comp(comp_task_list, active_task_list->head->task_id, active_task_list->head->release_time, current_p, comp_time);
			dd_task_comp_count++;

			// remove from active
			remove_from_list(active_task_list);

		} else {
			// add to overdue and add the task execution time to the finished task
			add_to_list_comp(overdue_task_list, active_task_list->head->task_id, active_task_list->head->release_time, current_p, comp_time);
			dd_task_overdue_count++;

			// remove from active
			remove_from_list(active_task_list);

		}

		vTaskSuspend(NULL);
	}

}


/*-----------------------------------------------------------*/
// USER TASKS
/*-----------------------------------------------------------*/
static void Blue_LED_User_Task( void *pvParameters )
{
	int task_num = 0;
	int comp_time = 0;

	while(1)
	{
		vTaskSuspend(NULL);

		STM_EVAL_LEDOn(blue_led);
		vTaskDelay(t1_ET);
		STM_EVAL_LEDOff(blue_led);

		//once the task is finished, sent the comp time to comp queue
		comp_time = xTaskGetTickCount();
		//comp_time = dd_task_comp_count * t1_ET;

		xQueueSend(xQueue_CompTimeTask_handle,&comp_time,1000);
		vTaskResume(monitor_handle);

		printf("size of active list: %d\n", active_task_list->size);
		printf("size of comp list: %d\n", comp_task_list->size);
		printf("size of overdue list: %d\n", overdue_task_list->size);


//		//wait for new task in the display queue
//		if(xQueueReceive(xQueue_DisplayTask_handle,&task_num,1000)){
//
//			test_if_received = 10000;
//
//			if(task_num == 1){
//				STM_EVAL_LEDOn(blue_led);
//				vTaskDelay(t1_ET);
//				STM_EVAL_LEDOff(blue_led);
//
//				//once the task is finished, sent the comp time to comp queue
//				comp_time = xTaskGetTickCount();
//
//				xQueueSend(xQueue_CompTimeTask_handle,&comp_time,1000);
//				vTaskResume(monitor_handle);
//			} else {
//				xQueueSend(xQueue_DisplayTask_handle,&task_num,1000);
//			}


//		}




	}
}


static void Green_LED_User_Task( void *pvParameters )
{
	int task_num = 0;
	int comp_time = 0;

	while(1)
	{
		vTaskSuspend(NULL);

		STM_EVAL_LEDOn(green_led);
		vTaskDelay(t2_ET);
		STM_EVAL_LEDOff(green_led);

		//once the task is finished, sent the comp time to comp queue
		comp_time = xTaskGetTickCount();
		//comp_time = dd_task_comp_count * t2_ET;

		xQueueSend(xQueue_CompTimeTask_handle,&comp_time,1000);
		vTaskResume(monitor_handle);

//		//wait for new task in the display queue
//		if(xQueueReceive(xQueue_DisplayTask_handle,&task_num,1000)){
//
//			test_if_received = 10000;
//
//			if(task_num == 2){
//				STM_EVAL_LEDOn(green_led);
//				vTaskDelay(t2_ET);
//				STM_EVAL_LEDOff(green_led);
//
//				//once the task is finished, sent the comp time to comp queue
//				comp_time = xTaskGetTickCount();
//
//				xQueueSend(xQueue_CompTimeTask_handle,&comp_time,1000);
//				vTaskResume(monitor_handle);
//			} else {
//				xQueueSend(xQueue_DisplayTask_handle,&task_num,1000);
//			}
//
//
//
//		}




	}
}

static void Red_LED_User_Task( void *pvParameters )
{
	int task_num = 0;
	int comp_time = 0;

	while(1)
	{
		vTaskSuspend(NULL);

		STM_EVAL_LEDOn(red_led);
		vTaskDelay(t3_ET);
		STM_EVAL_LEDOff(red_led);

		//once the task is finished, sent the comp time to comp queue
		comp_time = xTaskGetTickCount();
		//comp_time = dd_task_comp_count * t3_ET;

		xQueueSend(xQueue_CompTimeTask_handle,&comp_time,1000);
		vTaskResume(monitor_handle);
//		//wait for new task in the display queue
//		if(xQueueReceive(xQueue_DisplayTask_handle,&task_num,1000)){
//
//			test_if_received = 10000;
//
//			if(task_num == 3){
//				STM_EVAL_LEDOn(red_led);
//				vTaskDelay(t3_ET);
//				STM_EVAL_LEDOff(red_led);
//
//				//once the task is finished, sent the comp time to comp queue
//				comp_time = xTaskGetTickCount();
//
//				xQueueSend(xQueue_CompTimeTask_handle,&comp_time,1000);
//				vTaskResume(monitor_handle);
//			} else {
//				xQueueSend(xQueue_DisplayTask_handle,&task_num,1000);
//			}
//
//		}




	}
}


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
