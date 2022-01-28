/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include <xc.h>

/* Hardware specific includes. */
#include "ConfigPerformance.h"

#include "TMan.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* App includes */
#include "../UART/uart.h"

/* Priorities of the demo application tasks (high numb. -> high prio.) */
#define tsk_PRIORITY	( tskIDLE_PRIORITY + 1 )
#define tics_PRIORITY	( tskIDLE_PRIORITY + 3 )

#define TMan_PERIOD 10


struct TMan_task tasks[6];
int maxTasks;
int taskIndex;
TickType_t TMan_tick;


void TMAN_Init (int numOfTasks) {
    
     // Init UART and redirect stdin/stdot/stderr to UART
    if(UartInit(configPERIPHERAL_CLOCK_HZ, 115200) != UART_SUCCESS) {
        PORTAbits.RA3 = 1; // If Led active error initializing UART
        while(1);
    }

     __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/
    
    // Disable JTAG interface as it uses a few ADC ports
    DDPCONbits.JTAGEN = 0;
    
    // Initialize ADC module
    // Polling mode, AN0 as input
    // Generic part
    AD1CON1bits.SSRC = 7; // Internal counter ends sampling and starts conversion
    AD1CON1bits.CLRASAM = 1; //Stop conversion when 1st A/D converter interrupt is generated and clears ASAM bit automatically
    AD1CON1bits.FORM = 0; // Integer 16 bit output format
    AD1CON2bits.VCFG = 0; // VR+=AVdd; VR-=AVss
    AD1CON2bits.SMPI = 0; // Number (+1) of consecutive conversions, stored in ADC1BUF0...ADCBUF{SMPI}
    AD1CON3bits.ADRC = 1; // ADC uses internal RC clock
    AD1CON3bits.SAMC = 16; // Sample time is 16TAD ( TAD = 100ns)
    // Set AN0 as input
    AD1CHSbits.CH0SA = 0; // Select AN0 as input for A/D converter
    TRISBbits.TRISB0 = 1; // Set AN0 to input mode
    AD1PCFGbits.PCFG0 = 0; // Set AN0 to analog mode
    // Enable module
    AD1CON1bits.ON = 1; // Enable A/D module (This must be the ***last instruction of configuration phase***)
    
    /* Welcome message*/
    printf("\n\n*********************************************\n\r");
    printf("Initializing the TMAN Framework \n\r");
    printf("*********************************************\n\r");
    
    maxTasks = numOfTasks;
    taskIndex = 0;
    
    printf("Creating getTicks Task\n\r");
    xTaskCreate ( TMAN_getTicks , ( const signed char * const ) "Get Tics", configMINIMAL_STACK_SIZE, NULL, tics_PRIORITY, NULL);

    
}

void TMAN_TaskAdd(const char* name){
    if(taskIndex < maxTasks){
        tasks[taskIndex].name = name;
        xTaskCreate( BusyWait, ( const signed char * const ) name, configMINIMAL_STACK_SIZE,(void *) name, tsk_PRIORITY, NULL );
        printf("Task %c created!\n\r", tasks[taskIndex].name);
        taskIndex++;
        
    }else{
        printf("Task can not be created, max number %d of tasks created\n\r", maxTasks);
    }
}

void TMAN_TaskRegisterAttributes (const char* name, int period, int phase, int deadline) {
    for (int i=0; i < maxTasks; i++){
        if(tasks[i].name == name){
            tasks[i].period = period;
            tasks[i].phase = phase;
            tasks[i].deadline = deadline;    
            printf("Attributes period:%d , phase:%d , deadline:%d attributed to Task %c\n\r", tasks[i].period, tasks[i].phase, tasks[i].deadline, tasks[i].name);
        }
    
    }
    
}

void TMAN_getTicks(void *pvParams){
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    for(;;){
        vTaskDelayUntil( &xLastWakeTime, TMan_PERIOD);
        TMan_tick++; 
    }   
}

void TMAN_Close(void){
    vTaskEndScheduler();
}

void BusyWait(void *pvParams){
    uint8_t mesg[80];
    
    for(;;){
        TickType_t tic = xTaskGetTickCount();
        printf("%s, %d\n\r",(const char*) pvParams, tic);
    }
    
   // for(;;){
     //   TMAN_TaskWaitPeriod(args ?); // Add args if needed
       // GET_TICKS
       // print ?Task Name? and ?Ticks?
       // for(i=0; i<IMAXCOUNT; i++)
       // for(j=0; j<JMAXCOUNT; j++)
       // do_some_computation_to_consume_time;
       // OTHER_STUFF (if needed)
}
    

