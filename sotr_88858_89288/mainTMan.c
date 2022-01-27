/*
 * Pedro Valente, Dec/2021
 * Dário Matos, Dec/2021
 *
 *
 * Environment:
 * - MPLAB X IDE v5.45
 * - XC32 V2.50
 * - FreeRTOS V202107.00
 *
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

#include <xc.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "TMan.h"

/* App includes */
#include "../UART/uart.h"
#include "semphr.h"

/* Set the tasks' period (in system ticks) */
#define LED_FLASH_PERIOD_MS 	( 250 / portTICK_RATE_MS ) // 
#define INTERF_PERIOD_MS 	( 3000 / portTICK_RATE_MS )

//Task Acq is executed every 100 ms
#define ACQ_PERIOD_MS 	( 100 / portTICK_RATE_MS )

/* Control the load task execution time (# of iterations)*/
/* Each unit corresponds to approx 50 ms*/
#define INTERF_WORKLOAD          ( 20)

/* Priorities of the demo application tasks (high numb. -> high prio.) */
// #define LED_FLASH_PRIORITY	( tskIDLE_PRIORITY + 1 )
// #define INTERF_PRIORITY	    ( tskIDLE_PRIORITY + 2 )

#define OUTPUT_PRIORITY (tskIDLE_PRIORITY + 1)
#define PROCESSING_PRIORITY (tskIDLE_PRIORITY + 2)
#define ACQUISTION_PRIORITY (tskIDLE_PRIORITY + 3)

int pot = 0;
int avg = 0;
int arr[5] = {0};

SemaphoreHandle_t xSem1;
SemaphoreHandle_t xSem2;

/*
 * Prototypes and tasks
 */

void pvAcq(void *pvParam){
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    uint8_t msg[80];
    
    for(;;){
        vTaskDelayUntil(&xLastWakeTime, ACQ_PERIOD_MS);
        
        IFS1bits.AD1IF = 0; // Reset interrupt flag
        AD1CON1bits.ASAM = 1; // Start conversion
        while (IFS1bits.AD1IF == 0); // Wait fo EOC
        
        int v = ((int *)(&ADC1BUF0))[0];
        
        pot = (v*100 + 511)/1023;
        
        sprintf(msg,"Read Value %d\n\r",pot);
        PrintStr(msg);
        xSemaphoreGive(xSem1);
    }
}


void pvProc(void *pvParam){
    uint8_t msg[80];
    
    for(;;){
        if(xSemaphoreTake(xSem1, (TickType_t) 10) == pdTRUE){
            
            arr[0] = arr[1];
            arr[1] = arr[2];
            arr[2] = arr[3];
            arr[3] = arr[4];
            arr[4] = pot;
            
            sprintf(msg,"Array: \n\r",arr[1]);
            PrintStr(msg);
            for(int i = 0; i < 5; i++){
                sprintf(msg,"Index %d,  %d\n\r",i,arr[i]);
                PrintStr(msg);
            }
            
          
            avg = (arr[0]+arr[1]+arr[2]+arr[2]+arr[3]+arr[4])/5;
            
            xSemaphoreGive(xSem2);
        }
    }
}

void pvOut(void *pvParam){
    uint8_t msg[80];
    
    for(;;){
        if(xSemaphoreTake(xSem2, ( TickType_t ) 10 ) == pdTRUE){
            sprintf(msg,"Average %d\n\r",avg);
            PrintStr(msg);
        }
    }
}

TMan_task tasks_global[6];

/*
 * Create the demo tasks then start the scheduler.
 */
int mainTMan( void )
{
    
    TMAN_Init();
   
    
    //xSem1 = xSemaphoreCreateBinary();
    //xSem2 = xSemaphoreCreateBinary();
        
      
    /* Create the tasks defined within this file. */
	//xTaskCreate( pvAcq, ( const signed char * const ) "ACQ", configMINIMAL_STACK_SIZE, NULL, ACQUISTION_PRIORITY, NULL );
    //xTaskCreate( pvProc, ( const signed char * const ) "PROC", configMINIMAL_STACK_SIZE, NULL, PROCESSING_PRIORITY, NULL );
    //xTaskCreate( pvOut, ( const signed char * const ) "OUT", configMINIMAL_STACK_SIZE, NULL, OUTPUT_PRIORITY, NULL );

    /* Finally start the scheduler. */
	//vTaskStartScheduler();

	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
	return 0;
}
