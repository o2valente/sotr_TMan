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


/*
 * Create the demo tasks then start the scheduler.
 */
int mainTMan( void )
{
    
    TMAN_Init(6);
   
    TMAN_TaskAdd('A');
   
	return 0;
}
