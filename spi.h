/* 
 * File:   spi.h
 * Author: Hussein Ahmed Fouad Hassan s5165612,
 *         Daniele Martino Parisi s4670964,
 *         Davide Leo Parisi s4329668,
 *         Sara Sgambato s4648592
 */

#ifndef SPI_H_
#define SPI_H_

#include "std_types.h"
#include "common_macros.h"

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void SPI_initMaster(void); 
void SPI_initSlave(void);
void SPI_sendByte(const char);
uint8 SPI_recieveByte(void);
void SPI_sendString(const char *);
void SPI_receiveString(char *);
void move_cursor_first_row(uint8);
void move_cursor_second_row(uint8);
void clear_LCD(int);
void clear_chars(int, int);

#endif