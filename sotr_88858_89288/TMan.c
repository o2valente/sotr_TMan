/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include <xc.h>

/* Hardware specific includes. */
#include "ConfigPerformance.h"

#include <math.h>

#include "TMan.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

#include <semphr.h>

/* App includes */
#include "../UART/uart.h"


struct TMan_task tasks[6];
int maxTasks;
int taskIndex;
TickType_t TMan_tick;
QueueHandle_t xQueue_msg;


void TMAN_Init (int numOfTasks) {
    
    uartInit();
    
    /* Welcome message*/
    printf("\n\n*********************************************\n\r");
    printf("Initializing the TMAN Framework \n\r");
    printf("*********************************************\n\r");
    
    maxTasks = numOfTasks;
    taskIndex = 0;
    
    printf("Creating Tick Handler Task\n\r");
    xTaskCreate ( TMAN_getTics , ( const signed char * const ) "Tick Handler", configMINIMAL_STACK_SIZE, NULL, tics_PRIORITY, NULL);
    printf("Creating Printer Task\n\r");
    xTaskCreate(printTask , ( const signed char * const ) "Printer", configMINIMAL_STACK_SIZE, NULL, print_PRIORITY, NULL );

    xQueue_msg = xQueueCreate(maxTasks*5,sizeof(char)*80 );

    
}

void TMAN_TaskAdd(const char* name, int priority){
    if(taskIndex < maxTasks){
        tasks[taskIndex].name           = name;
        tasks[taskIndex].phase          = 0;
        tasks[taskIndex].current_a      = 0;
        tasks[taskIndex].deadline       = 0;   
        tasks[taskIndex].deadline_count = 0;
        tasks[taskIndex].next_a         = 0;
        tasks[taskIndex].priority       = tskIDLE_PRIORITY + priority;
        tasks[taskIndex].precedence     = "";
        tasks[taskIndex].state          = TASK_STATE_INITIAL; 
        xTaskCreate( BusyWait, ( const signed char * const ) name, configMINIMAL_STACK_SIZE,(void *) &tasks[taskIndex], tskIDLE_PRIORITY + priority, NULL );
        printf("Task %s created!\n\r", tasks[taskIndex].name);
        taskIndex++;
    }else{
        printf("Task can not be created, max number %d of tasks created\n\r", maxTasks);
    }
}

void TMAN_TaskRegisterAttributes (const char* name, int period, int phase, int deadline, int wcet, const char* precedence) {
    int index = getTaskIndex(name);
    tasks[index].period         = period;
    tasks[index].phase          = phase;
    tasks[index].current_a      = phase;
    tasks[index].deadline       = deadline + phase;   
    tasks[index].deadline_count = 0;
    tasks[index].next_a         = phase;
    tasks[index].precedence     = precedence;
    tasks[index].wcet           = wcet;
    printf("Attribute added to Task %s\n\r", tasks[index].name);
}

void TMAN_TaskWaitPeriod(void){
    const char* name = pcTaskGetName(xTaskGetCurrentTaskHandle());
    int index = getTaskIndex(name);
    if(TMan_tick>tasks[index].current_a+tasks[index].deadline){
        tasks[index].deadline_count++;
    }
    vTaskSuspend(NULL);
}

void BusyWait(void *pvParams){
    char msg[80];
    TMan_task * task = (TMan_task *) pvParams;
    int index = getTaskIndex(task->name);
    float x=200;
    for(;;){
        TMAN_TaskWaitPeriod();
        TickType_t tic = xTaskGetTickCount();
        sprintf(msg,"%s, %d \n\r", task->name, tic);
        if( xQueueSend(xQueue_msg, msg, 10) != pdPASS ) { }
        for(int i=0; i<IMAXCOUNT; i++){
            for(int j=0; j<JMAXCOUNT; j++){
              x=x/7;  
            }
        }
        tasks[index].state = TASK_STATE_BLOCKED; 
        tasks[index].conclusion = TMan_tick;
    }
}

void TMAN_getTics(void *pvParams){
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    int indexPrec;
    for(;;){            
        for(int i=0;i<maxTasks;i++){
            if(strcmp(tasks[i].precedence,"Z") != 0){
                indexPrec = getTaskIndex(tasks[i].precedence);
                if(tasks[indexPrec].state == TASK_STATE_BLOCKED && tasks[indexPrec].conclusion >= tasks[i].current_a){
                    updateTask(i, pdPASS);
                }              
            }else{
                if(TMan_tick>=tasks[i].next_a){ //pode executar
                  
                    if((int) TMan_tick <= (tasks[i].next_a+tasks[i].deadline)){
                        updateTask(i,pdFAIL);
                    }
                    else{
                       tasks[i].next_a=tasks[i].next_a+tasks[i].period;
                       tasks[i].current_a=tasks[i].next_a;
                       tasks[i].deadline_count++;
                    }
               }
            }   
        }  
        vTaskDelayUntil( &xLastWakeTime, TMan_PERIOD);
        TMan_tick++;
    }
}

void TMAN_Close(void){
    maxTasks = 0;
    taskIndex = 0;
    vTaskEndScheduler();
}

void TMAN_TaskStats(void){
    char msg[80];
    for(int i = 0; i < maxTasks; i++){
        sprintf(msg,"TASK %s: Deadline Misses %d , Number of Activations %d \n\r",tasks[i].name, tasks[i].deadline_count, tasks[i].activation_num);
        if( xQueueSend(xQueue_msg, msg, 10) != pdPASS ) { }
    }
    
}

int TMAN_Feasibility() {
    
    int f = 1;
    
    for (int i = 0; i<maxTasks; i++) {
        if(TMAN_Wcet(tasks[i])==0){
            f=0;
            break;
        }
    }
    return f;
}

int TMAN_Wcet(TMan_task task) {         //FIXE PRIORITY SLIDES : SLIDE 21
    
    int iteration = 0;
    double RWC = 0;
    double previousRWC = 0;
    
    uint8_t mesg[80];
    char msg[80];
    
    
    do {
        RWC=0;
        sprintf(msg,"RWC: %f, PRWC: %f , Inicio da Iteration: %d, \n\r", RWC, previousRWC, iteration);
        if( xQueueSend(xQueue_msg, msg, 10) != pdPASS ) { }
        //printf("RWC , PRWC : %f %f \n\n", RWC, previousRWC);
        for(int i=0;i<maxTasks;i++){
            if(tasks[i].priority>task.priority){
                if (iteration == 0) {
                    RWC = RWC+ (double) tasks[i].wcet;
                }
                else{
                    RWC = RWC + ceil ( previousRWC/(double) tasks[i].period)* (double) tasks[i].wcet;
                    
                }
            }
        }
        
        
        
        RWC = RWC + (double) task.wcet;
        sprintf(msg,"RWC: %f, Iteration: %d, \n\r", RWC, iteration);
        if( xQueueSend(xQueue_msg, msg, 10) != pdPASS ) { }
        
        
        if (RWC > (double) task.deadline) {
            sprintf(msg,"%s task, iteration %d",task.name,iteration);
            if( xQueueSend(xQueue_msg, msg, 10) != pdPASS ) { }
            return 0; //wrongg
        }
        if (previousRWC==RWC && iteration >0) {
            return 1; // converge
        }
        
        iteration++;
        
        previousRWC = (double) RWC;
        sprintf(msg,"RWC: %f, PRWC: %f , Fim da Iteration: %d, \n\r", RWC, previousRWC, iteration);
        if( xQueueSend(xQueue_msg, msg, 10) != pdPASS ) { }


    }while(RWC <=(double)  task.deadline);
}

void updateTask(int index, BaseType_t sporadic){
    TaskHandle_t handle = xTaskGetHandle(tasks[index].name);
    if(sporadic){
        tasks[index].current_a=TMan_tick;
    }else{
        tasks[index].current_a=tasks[index].next_a;
        tasks[index].next_a=tasks[index].next_a+tasks[index].period;
    }
    tasks[index].activation_num++;
    tasks[index].state = TASK_STATE_RUNNING; 
    vTaskResume(handle);
}

int getTaskIndex(const char *name){
    for(int i = 0; i < taskIndex; i++)
        if(strcmp(tasks[i].name,name) == 0)
            return i; 
}


void printTask(void *pvParam){
    char mesg[80];
    for(;;){
        if( xQueue_msg != NULL ){
            for(;;) {            
               if( xQueueReceive( xQueue_msg,mesg,portMAX_DELAY ) == pdPASS ){
                   PrintStr(mesg);
               }
            }        
        }
    }
}

void uartInit(void){
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
    AD1CON1bits.ON = 1; // Enable A/D module (This must be the **last instruction of configuration phase**)
}

