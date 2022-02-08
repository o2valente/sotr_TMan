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


/*
 * Create the demo tasks then start the scheduler.
 */
int mainTMan( void *pvParam)
{

    TMAN_Init(6);
   
    /*Task Name and Task Priority*/
    TMAN_TaskAdd("A",4);
    TMAN_TaskAdd("B",4);   
    TMAN_TaskAdd("C",3); 
    TMAN_TaskAdd("D",3); 
    TMAN_TaskAdd("E",2); 
    TMAN_TaskAdd("F",2); 
    
    /*Task Name, Period, Phase, Deadline, WCET and Precedence*/
    /*Use the letter Z to assume no Precedence*/
    TMAN_TaskRegisterAttributes("A",1,0,1,1,"Z");
    TMAN_TaskRegisterAttributes("B",1,0,1,1,"F");   
    TMAN_TaskRegisterAttributes("C",3,0,3,1,"Z");
    TMAN_TaskRegisterAttributes("D",3,1,3,0,"Z");
    TMAN_TaskRegisterAttributes("E",4,0,4,0,"Z");
    TMAN_TaskRegisterAttributes("F",4,2,4,0,"Z");

        
    //if(TMAN_Feasibility()==1)
    vTaskStartScheduler();
    
   
	return 0;
}
