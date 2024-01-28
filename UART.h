/* 
 * File:   UART.h
 * Author: Hussein Ahmed Fouad Hassan s5165612,
 *         Daniele Martino Parisi s4670964,
 *         Davide Leo Parisi s4329668,
 *         Sara Sgambato s4648592
 */

#ifndef UART_H_
#define UART_H_

#include "std_types.h"
#include "common_macros.h"
#define BUFFERRX_SIZE 50
#define BUFFERTX_SIZE 38

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void UART_receiver_init(void);
uint8 UART_Receiver(void);
void UART_transmitter_init(void);
void UART_sendByte(const char);
void UART_sendString(const char *);

#endif