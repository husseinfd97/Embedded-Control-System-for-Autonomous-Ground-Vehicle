/* 
 * File:   main.c
 * Author: Hussein Ahmed Fouad Hassan s5165612,
 *         Daniele Martino Parisi s4670964,
 *         Davide Leo Parisi s4329668,
 *         Sara Sgambato s4648592
 */

// DSPIC30F4011 Configuration Bit Settings

// 'C' source line config statements

// FOSC
#pragma config FPR = XT                 // Primary Oscillator Mode (XT)
#pragma config FOS = PRI                // Oscillator Source (Primary Oscillator)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)

// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = BORV20          // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_ON          // PBOR Enable (Enabled)
#pragma config LPOL = PWMxL_ACT_HI      // Low-side PWM Output Polarity (Active High)
#pragma config HPOL = PWMxH_ACT_HI      // High-side PWM Output Polarity (Active High)
#pragma config PWMPIN = RST_IOPIN       // PWM Output Pin Reset (Control with PORT/TRIS regs)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include "xc.h"
#include "timer.h"
#include "ADC.h"
#include "helper_api.h"
#include "spi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UART.h"
#include "PWM.h"

#define MAX_TASKS 6 // define sched info size
#define STATE_DOLLAR (1) // in this state, we discard everything until a dollar is found
#define STATE_TYPE (2) // in this state, we are reading the type of msg until a comma is found
#define STATE_PAYLOAD (3) // in this state, we read the payload until a star is found
#define NEW_MESSAGE (1) // new message received and parsed completely
#define NO_MESSAGE (0) // no new messages
#define MSGTYPE_SIZE 6 // maximum size of the msg type
#define PAYLOAD_SIZE 100 // maximum payload size
#define ACK_SIZE 16 // size of ack_msg
#define FBK_SIZE 25 // size of fbk_msg
#define SPI_SIZE 15 // size of spi message
#define TEMPVAL_SIZE 16 // size of temp_msg
#define ALERT_SIZE 23 // size of alert_msg
#define TEMP_SIZE 10 // array dimension for the average of current and temperature values
#define OTHERWISE 0 // value for safe nor timeout mode
#define TIMEOUT 1 // value for timeout mode
#define SAFE 2 // value for safe mode

// Definition of global variables
volatile int status, s6_flag, msg_type, TIMEOUT_FLAG, ALERT_FLAG;
volatile double angular, linear, temperature, temp_sum, RPM_left, RPM_right, sat_left, sat_right;
char * vel[2];
volatile float temp_index;

// Definition of structs
typedef struct
{
    char *buffer;
    int size;
    int readIndex;
    int writeIndex;
} CircularBuffer;

typedef struct 
{
    int state;
    char msg_type[MSGTYPE_SIZE]; // type is 5 chars + string terminator
    char msg_payload[PAYLOAD_SIZE]; // assume payload cannot be longer than 100 chars
    int index_type;
    int index_payload;
} parser_state;

typedef struct
{
    int n; 
    int N; // task called every N periods
} heartbeat;

// Functions
void init_buffer(void);
void write_buffer(volatile CircularBuffer*, char);
int read_buffer(volatile CircularBuffer*, char*);
int parse_byte(parser_state*, char);
void reset_RPM(void);
void scheduler();
void task1();
void task2();
void task3();
void task4();
void task5();
void task6();

/////////*********start of circular buffer part*********/////////

volatile CircularBuffer cb_rx; // buffer for receiving
volatile CircularBuffer cb_tx; // buffer for transmitting

// Initialise the circular buffer indexes 
void init_buffer(void)
{
    cb_rx.writeIndex = 0;
    cb_rx.readIndex = 0;
    cb_tx.writeIndex = 0;
    cb_tx.readIndex = 0;
}

// Write data in the circular buffer
void write_buffer(volatile CircularBuffer* cb, char value)
{
    cb->buffer[cb->writeIndex] = value;
    cb->writeIndex++;
    if(cb->writeIndex == cb->size)
        cb->writeIndex=0;
}

// Read data from the circular buffer
int read_buffer(volatile CircularBuffer* cb, char* value)
{
    if (cb->readIndex == cb->writeIndex)
        return 0;
    
    *value = cb->buffer[cb->readIndex];
    cb->readIndex++;
    if(cb->readIndex == cb->size)
        cb->readIndex = 0;
    return 1;
}
/////////*********end of circular buffer part*********/////////

/////////*********start of the scheduling part*********/////////

parser_state pstate; // pointer to the structure 
heartbeat schedInfo[MAX_TASKS]; // define the heartbeat

int parse_byte(parser_state* ps, char byte) {
    switch (ps->state) {
        case STATE_DOLLAR:
            if (byte == '$') 
            {
                ps->state = STATE_TYPE;
                ps->index_type = 0;
            }
            break;
        case STATE_TYPE:
            if (byte == ',') 
            {
                ps->state = STATE_PAYLOAD;
                ps->msg_type[ps->index_type] = '\0';
                ps->index_payload = 0; // initialize properly the index
            } else if (ps->index_type == 6) 
            { // error! 
                ps->state = STATE_DOLLAR;
                ps->index_type = 0;
			} else if (byte == '*') 
            {
				ps->state = STATE_DOLLAR; // get ready for a new message
                ps->msg_type[ps->index_type] = '\0';
				ps->msg_payload[0] = '\0'; // no payload
                return NEW_MESSAGE;
            } else {
                ps->msg_type[ps->index_type] = byte; // ok!
                ps->index_type++; // increment for the next time;
            }
            break;
        case STATE_PAYLOAD:
            if (byte == '*') {
                ps->state = STATE_DOLLAR; // get ready for a new message
                ps->msg_payload[ps->index_payload] = '\0';
                return NEW_MESSAGE;
            } else if (ps->index_payload == 100) { // error
                ps->state = STATE_DOLLAR;
                ps->index_payload = 0;
            } else {
                ps->msg_payload[ps->index_payload] = byte; // ok!
                ps->index_payload++; // increment for the next time;
            }
            break;
    }
    return NO_MESSAGE;
}

void reset_RPM()
{
    angular = 0.0;
    linear = 0.0;
    RPM_left = 0.0;
    RPM_right = 0.0;
}

void scheduler()
{
    int i;
    for (i=0; i<=MAX_TASKS; i++)
    {
        schedInfo[i].n++;
        if(schedInfo[i].n >= schedInfo[i].N)
        {
            switch(i)
            {
                case 0:
                    task1();
                    break;
                case 1:
                    task2();
                    break;
                case 2:
                    task3();
                    break;
                case 3:
                    task4();
                    break;
                case 4:
                    task5();
                    break;
                case 5:
                    task6();
                    break;
                default:
                    break;                    
            }
            schedInfo[i].n = 0;
        }
    }
}

void task1()
{    
    char cl_msg_type[MSGTYPE_SIZE];
    int ret = 0;
    char byte;
    int counter = 0;
    double * RPM;
    char ack_msg[ACK_SIZE]; 
    char tx_buf;
    
    // enable the interrupt for transmitting
    IEC1bits.U2TXIE = 1;
    
    // while there is something to read, read it!
    while (read_buffer(&cb_rx, &byte) == 1)
    {           
       ret = parse_byte(&pstate, byte);
      
       if (ret == NEW_MESSAGE)
       {
           // copy the message type in another array
           strcpy(cl_msg_type, pstate.msg_type);
           // check if the message received was MCSEN
           cl_msg_type[strlen(cl_msg_type)-1] = 0; // save the msg type in another array to modify it 
           // control if we are not in timeout mode and receive a reference message
           if(!strcmp(pstate.msg_type, "HLREF") && status != SAFE) 
           {
                // Extract the first token
                char * token = strtok(pstate.msg_payload, ",");
                // loop through the string to extract all other tokens
                while( token != NULL ) 
                {
                  vel[counter] = token; // printing each token
                  token = strtok(NULL, ",");
                  counter++;
                }
                tmr_setup_period(TIMER2,5000);
                TIMEOUT_FLAG = 0;
                LATBbits.LATB1 = 0;
                status = OTHERWISE;
           } 
           
           // control if we are in safe mode and receive an enable message
           if (!strcmp(pstate.msg_type, "HLENA") && status != OTHERWISE && status != TIMEOUT )
           {
                sprintf(ack_msg, "$MCACK,ENA,1*");
                for(int i=0; ack_msg[i]!='\0'; i++)
                {
                    write_buffer(&cb_tx, ack_msg[i]);
                }

                IEC1bits.U2TXIE = 0;
                while(U2STAbits.UTXBF == 0)
                {
                    if (read_buffer(&cb_tx, &tx_buf) == 1)
                    {
                        U2TXREG = tx_buf;
                    }
                    else
                    {
                        break;
                    }
                }
                IEC1bits.U2TXIE = 1;
                status = OTHERWISE;
                strcpy(vel[0], "0.0");
                strcpy(vel[1], "0.0");
           }
       }
    }
    
    if (status == OTHERWISE)
    {
        // get linear and angular velocities
        angular = atof(vel[0]);
        linear = atof(vel[1]);
        
        // get RPMs values
        RPM = speed_to_RPM(linear, angular, 0.5, 0.2);
        RPM_left = *RPM;
        RPM_right = *(RPM+1);
        
        // control if values exceed the maximum allowed values
        if (RPM_left < -50 || RPM_left > 50 || RPM_right < -50 || RPM_right > 50)
        {
            sat_left = RPM_left;
            sat_right = RPM_right;
            ALERT_FLAG = 1;
        }
        // if values are under the limits
        if (RPM_left > -50 && RPM_left < 50 && RPM_right > -50 && RPM_right < 50)
        {
            ALERT_FLAG = 0;
        }
        // Check the RPM values and saturate to the maximum/minimum value if necessary
        if (RPM_left < -50) 
        {
            RPM_left = -50;
        }
        if (RPM_left > 50) 
        {
            RPM_left = 50;
        }
        if (RPM_right < -50) 
        {
            RPM_right = -50;
        }
        if (RPM_right > 50) 
        {
            RPM_right = 50;
        }
        // set duty cycle
        PWM_set_dutycycle(RPM_left, RPM_right);
    }
    
    // If in timeout mode reset velocities and rpms to zero.
    if (TIMEOUT_FLAG == 1)
    {
        ALERT_FLAG = 0;
        reset_RPM();
        PWM_set_dutycycle(RPM_left, RPM_right);
    }
    
    // ADC conversion
    while (ADCON1bits.DONE == 0);
    ADCON1bits.DONE = 0;

    temperature = Temp_sensor_ADC_converter();
    
    // compute the average with 10 values of the temperature
    if(temp_index < TEMP_SIZE)
    {        
        temp_sum = temp_sum + temperature;
        temp_index++;
    }
    
    if(temp_index == TEMP_SIZE)
    {
        temperature = temp_sum/temp_index;        
        temp_sum = 0.0;
        temp_index = 0.0;
    }
}

void task2()
{
    char fbk_msg[FBK_SIZE]; 
    char tx_buf;
    
    // toggle the led D4 if in timeout mode
    if(TIMEOUT_FLAG == 1)
    {
        LATBbits.LATB1 = !LATBbits.LATB1;
    }
    
    // create the feedback message to send to the UART
    sprintf(fbk_msg, "$MCFBK,%.1f,%.1f,%d*", RPM_left, RPM_right, status);
    for(int i=0; fbk_msg[i]!='\0'; i++)
    {
        write_buffer(&cb_tx, fbk_msg[i]);
    }

    IEC1bits.U2TXIE = 0;
    while(U2STAbits.UTXBF == 0)
    {
        if (read_buffer(&cb_tx, &tx_buf) == 1)
        {
            U2TXREG = tx_buf;
        }
        else
        {
            break;
        }
    }
    IEC1bits.U2TXIE = 1;
}

// Write on the LCD task
void task3()
{
    char msg[SPI_SIZE]; 
    // write status message in the first row to the LCD
    move_cursor_first_row(8);
    if (status == OTHERWISE)
    {
        SPI_sendByte('C');
    }
    else if (status == TIMEOUT)
    {
        SPI_sendByte('T');
    }
    else if (status == SAFE)
    {
        SPI_sendByte('H');
    }
    // write velocities or RPMs in the second row to the LCD
    if (s6_flag == 0)
    {
        clear_chars(1,3);
        move_cursor_second_row(3);
        sprintf(msg, "%.1f; %.1f", RPM_left, RPM_right);
        SPI_sendString(msg);       
    }
    else if (s6_flag == 1)
    {
        clear_chars(1,3);
        move_cursor_second_row(3);
        sprintf(msg, "%.1f; %.1f", angular, linear);
        SPI_sendString(msg);
    }
}

void task4()
{
    // if an overflow occurs 
    if(U2STAbits.OERR == 1)
    {
        U2STAbits.OERR = 0;
        LATBbits.LATB0 = 0;
    }
    else
    { 
        LATBbits.LATB0 = !LATBbits.LATB0;// Toggle the led D3 every 500 ms if everython is ok
    }
}

void task5()
{   
    char temp_msg[TEMPVAL_SIZE]; 
    char tx_buf;
    
    // create the message to send to the UART for the temperature average
    sprintf(temp_msg, "$MCTEM,%.1f*", temperature);
    for(int i=0; temp_msg[i]!='\0'; i++)
    {
        write_buffer(&cb_tx, temp_msg[i]);
    }

    IEC1bits.U2TXIE = 0;
    while(U2STAbits.UTXBF == 0)
    {
        if (read_buffer(&cb_tx, &tx_buf) == 1)
        {
            U2TXREG = tx_buf;
        }
        else
        {
            break;
        }
    }
    IEC1bits.U2TXIE = 1;
}

void task6()
{
    char alert_msg[ALERT_SIZE];
    char tx_buf;
     
     // if there is saturation, create the message to send to the UART 
    if (ALERT_FLAG == 1)
    {
        sprintf(alert_msg, "$MCALE,%.1f,%.1f*", sat_left, sat_right);
        for(int i=0; alert_msg[i]!='\0'; i++)
        {
            write_buffer(&cb_tx, alert_msg[i]);
        }

        IEC1bits.U2TXIE = 0;
        while(U2STAbits.UTXBF == 0)
        {
            if (read_buffer(&cb_tx, &tx_buf) == 1)
            {
                U2TXREG = tx_buf;
            }
            else
            {
                break;
            }
        }
        IEC1bits.U2TXIE = 1;
    }
}

/////////*********end of scheduling part*********/////////

// UART2 interrupt routines
void __attribute__ (( __interrupt__ , __auto_psv__ )) _U2RXInterrupt() 
{
    IFS1bits.U2RXIF = 0;
    char val = U2RXREG;
    write_buffer(&cb_rx, val);
}

void __attribute__ (( __interrupt__ , __auto_psv__ )) _U2TXInterrupt() 
{
    IFS1bits.U2TXIF = 0;
    char tx_byte;
    // read values from the circular buffer and save them in the transmitter register
    while(U2STAbits.UTXBF == 0)
    {
        if (read_buffer(&cb_tx, &tx_byte) == 1)
        {
            U2TXREG = tx_byte;
        }
        else
        {
            break;
        }
    }    
}

// Timeout routine: enter timeout mode
void __attribute__ (( __interrupt__ , __auto_psv__ )) _T2Interrupt()
{
    IFS0bits.T2IF = 0;
    T2CONbits.TON = 0; // stop T2   
    status = TIMEOUT;
    TIMEOUT_FLAG = 1;
}

/* Button S5 interrupt routine: enter safe mode */
void __attribute__ (( __interrupt__ , __auto_psv__ )) _INT0Interrupt()
{
    IFS0bits.INT0IF = 0;
    IEC0bits.INT0IE = 0; // disable interrupt for INT0
    tmr_setup_period(TIMER4, 30);
}

void __attribute__ (( __interrupt__ , __auto_psv__ )) _T4Interrupt()
{
    IFS1bits.T4IF = 0;
    T4CONbits.TON = 0; // stop T4
    T2CONbits.TON = 0; // stop T2
    IFS0bits.INT0IF = 0; // interrupt flag set to zero
    IEC0bits.INT0IE = 1; // enable interrupt for INT0
    if(PORTDbits.RD0 == 1)
    {
        status = SAFE;
        TIMEOUT_FLAG = 0;
        ALERT_FLAG = 0;
        LATBbits.LATB1 = 0;
        reset_RPM();
        PWM_set_dutycycle(RPM_left, RPM_right);
    }    
}

/* Button S6 interrupt routine: change LCD display*/
void __attribute__ (( __interrupt__ , __auto_psv__ )) _INT1Interrupt()
{
    IEC1bits.INT1IE = 0; // disable interrupt for INT1
    tmr_setup_period(TIMER3, 30);
}

void __attribute__ (( __interrupt__ , __auto_psv__ )) _T3Interrupt()
{
    IFS0bits.T3IF = 0;
    T3CONbits.TON = 0; // stop T3
    IFS1bits.INT1IF = 0; // interrupt flag set to zero
    IEC1bits.INT1IE = 1; // enable interrupt for INT1
    if(PORTEbits.RE8 == 1)
    {
        s6_flag = !s6_flag; // toggle the s6_flag
    }
}

int main(void) 
{
    const char str1[] = "STATUS: ";
    const char str2[] = "R: ";
    
    // initialize the circular buffers
    init_buffer();
    // intitalize the UART for receiving
    UART_receiver_init();
    // initialize the UART for transmission
    UART_transmitter_init();
    // ADC initialization 
    ADC_Init();
    // PWM initialization
    PWM_init();
    // SPI Master intializer
    SPI_initMaster();
    // enable interrupt for receiving
    IEC1bits.U2RXIE = 1;
    
    // here we should initialize all the parameters for the scheduling
    schedInfo[0].n = 0;
    schedInfo[1].n = -1;
    schedInfo[2].n = 0;
    schedInfo[3].n = 0;
    schedInfo[4].n = -2;
    schedInfo[5].n = 0;
    schedInfo[0].N = 1;
    schedInfo[1].N = 2;
    schedInfo[2].N = 3;
    schedInfo[3].N = 5;
    schedInfo[4].N = 10;
    schedInfo[5].N = 10;
    
    /* variables intialization */
    // create two circula buffers, one for transmitting and one for receiving
    char buffer_tx[BUFFERTX_SIZE];
    cb_tx.buffer = buffer_tx;
    cb_tx.size = BUFFERTX_SIZE;
    char buffer_rx[BUFFERRX_SIZE];
    cb_rx.buffer = buffer_rx;
    cb_rx.size = BUFFERRX_SIZE;
    
    /* buttons, timers and interrupts enabling */
    TRISDbits.TRISD0 = 1; // set button S6 as an input
    TRISEbits.TRISE8 = 1; // set button S5 as an input
    IEC0bits.INT0IE = 1; // enable interrupt for button S5
    IEC1bits.INT1IE = 1; // enable interrupt for button S6
    IEC0bits.T2IE = 1; // enable the interrupt for timer T2
    IEC0bits.T3IE = 1; // enable the interrupt for timer T3
    IEC1bits.T4IE = 1; // enable the interrupt for timer T4
    TRISBbits.TRISB0 = 0; // set led D3 as an output 
    TRISBbits.TRISB1 = 0; // set led D4 as an output 
    
    // protocol intialization: start from state dollar and initialize all the indexes to zero
    pstate.state = STATE_DOLLAR;
    pstate.index_type = 0;
    pstate.index_payload = 0;
    
    // timer setup for the control loop
    tmr_setup_period(TIMER1, 100);
    
    // wait one second for the LCD configuration
    tmr_wait_ms(TIMER5, 1000);
    
    // initialization of all global variables 
    status = OTHERWISE;
    angular = 0.0;
    linear = 0.0;
    RPM_left = 0.0;
    RPM_right = 0.0;
    sat_left = 0.0;
    sat_right = 0.0;
    temperature = 0.0;
    temp_sum = 0.0;
    temp_index = 0.0;
    s6_flag = 0;
    TIMEOUT_FLAG = 0;
    ALERT_FLAG = 0;
    
    // write constant msg on the LCD
    move_cursor_first_row(0);
    SPI_sendString(str1);
    move_cursor_second_row(0);
    SPI_sendString(str2);
    
    while(1)
    {
       scheduler();
       tmr_wait_period(TIMER1);
    }
    return 0;
}