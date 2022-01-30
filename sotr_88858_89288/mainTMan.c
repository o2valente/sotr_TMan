/*
 * Pedro Valente, Jan/2022
 * Dário Matos, Jan/2022
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

#define OUTPUT_PRIORITY (tskIDLE_PRIORITY + 1)
#define PROCESSING_PRIORITY (tskIDLE_PRIORITY + 2)
#define ACQUISTION_PRIORITY (tskIDLE_PRIORITY + 3)

/* Priorities of the demo application tasks (high numb. -> high prio.) */
#define tsk_PRIORITY	( tskIDLE_PRIORITY + 1 )
#define tics_PRIORITY	( tskIDLE_PRIORITY + 3 )



/*
 * Create the demo tasks then start the scheduler.
 */
int mainTMan( void *pvParam)
{
    
//    const char *names[]={"A","B","C","D","E","F"};
////    xTaskCreate(printTask , ( const signed char * const ) "Printer", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
//    printf("%s %d \n",names[2],sizeof(names));
//    for(int i = 0; i < sizeof(names); i++){
//        xTaskCreate(BusyWait , ( const signed char * const ) names[i], configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
//        TMAN_TaskAdd(names[i]);
//    }
    
    xTaskCreate(printTask , ( const signed char * const ) "Printer", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    
    TMAN_Init(1);
   
    
    
    TMAN_TaskAdd("A");
//    TMAN_TaskAdd("B");   
//    TMAN_TaskAdd("C");   
//    TMAN_TaskAdd("D");   
            
    TMAN_TaskRegisterAttributes("A",5,0,3,0);
//    TMAN_TaskRegisterAttributes("B",4,0,10,0);
//    TMAN_TaskRegisterAttributes("C",8,0,10,0);
//    TMAN_TaskRegisterAttributes("D",10,3,10,0);
    
//    TMAN_TaskRegisterAttributes("C",4,2,10,0);
//    TMAN_TaskRegisterAttributes("D",4,2,10,0);
//    TMAN_TaskRegisterAttributes("E",4,2,10,0);
//    TMAN_TaskRegisterAttributes("F",6,2,10,0);
    

        
    vTaskStartScheduler();
    
   
	return 0;
}
