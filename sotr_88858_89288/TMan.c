/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Hardware specific includes. */
#include "ConfigPerformance.h"

#include "TMan.h"

#include <stdio.h>
#include <string.h>

/* App includes */
#include "../UART/uart.h"


struct TMan_task tasks[6];

void TMAN_Init () {
    
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
    printf("\n\n *********************************************\n\r");
    printf("Initializing the TMAN Framework \n\r");
    printf("*********************************************\n\r");
    
    char *names[6] = {"A","B","C","D","E","F"};
    for(int i=0; i<6; i++){
        tasks[i].name = names[i];
        tasks[i].deadline = 1; //TODO
        tasks[i].period = 1; //TODO
        tasks[i].phase = 1; //TODO
    }
    
    for(int i=0; i<6;i++){
        printf("%s",tasks[i].name);
    }
    
}