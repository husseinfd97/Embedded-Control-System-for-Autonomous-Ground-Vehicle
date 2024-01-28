/* 
 * File:   UART.c
 * Author: Hussein Ahmed Fouad Hassan s5165612,
 *         Daniele Martino Parisi s4670964,
 *         Davide Leo Parisi s4329668,
 *         Sara Sgambato s4648592
 */

#include "xc.h"
#include "UART.h"

// Initiliase transmitter and receiver of UART2
void UART_transmitter_init(void)
{
    U2BRG = 23; // baude rate
    U2MODEbits.UARTEN = 1; // enable UART2
    U2STAbits.UTXEN = 1;  // enable transmission 
    U2STAbits.UTXISEL = 1; // TRANSMISSION INTERRUPT 
}
void UART_receiver_init(void)
{
    U2BRG = 23; // baude rate
    U2MODEbits.UARTEN = 1; // enable UART2
    U2STAbits.URXISEL = 01; // receive interrupt mode selection bit 
}

// Function to send one byte from UART2
void UART_sendByte(const char data) 
{   
    U2STAbits.TRMT = 0; // notify that there is an ongoing transmission
    U2TXREG = data; // send data from UART2
}

// Function to send a string from UART2
void UART_sendString(const char *Str)
{
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
}

// Function to get data received by UART2
uint8 UART_Receiver(void)
{   
    uint8 received;
    while (U2STAbits.URXDA == 0){} // until the receiver buffer is empty do nothing
    received = U2RXREG; // then read from the receiver buffer
    return received;
}




