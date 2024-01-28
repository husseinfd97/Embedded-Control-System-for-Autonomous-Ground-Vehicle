/* 
 * File:   spi.c
 * Author: Hussein Ahmed Fouad Hassan s5165612,
 *         Daniele Martino Parisi s4670964,
 *         Davide Leo Parisi s4329668,
 *         Sara Sgambato s4648592
 */

#include "xc.h"
#include "spi.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void SPI_initMaster(void) 
{
	/******** Configure SPI Master Pins *********
	 * SS(PB2)   --> Output
	 * SD01(PF3) --> Output 
	 * SD1(PF2) --> Input
	 * SCK(PF6) --> Output
	 ********************************************/
    
    // master mode
    SPI1CONbits.MSTEN = 1; 
    // 8 bit mode
    SPI1CONbits.MODE16 = 0; 
    // 1:1 primary pre-scaler
    SPI1CONbits.PPRE = 2;
    // 5:1 secondary pre-scaler
    SPI1CONbits.SPRE = 7; 
    // enable SPI
    SPI1STATbits.SPIEN = 1; 
}

void SPI_initSlave(void)
{ 
	/******** Configure SPI Slave Pins *********
	 * SS(PB4)   --> Input
	 * MOSI(PB5) --> Input
	 * MISO(PB6) --> Output
	 * SCK(PB7) --> Input
	 ********************************************/
    
    // master mode
    SPI1CONbits.MSTEN = 0; 
    // 8 bit mode
    SPI1CONbits.MODE16 = 0; 
    // 1:1 primary pre-scaler
    SPI1CONbits.PPRE = 3;
    // 5:1 secondary pre-scaler
    SPI1CONbits.SPRE = 3; 
    // enable SPI
    SPI1STATbits.SPIEN = 1; 
}

// Function to send byte from SPI 
void SPI_sendByte(const char data) 
{   
	while(SPI1STATbits.SPITBF == 1){} //wait until SPI interrupt flag=1 (data is sent correctly)
    SPI1BUF = data; //send data by SPI
}

// Function to receive byte from SPI
uint8 SPI_recieveByte(void)
{
   while(SPI1STATbits.SPIRBF == 1){} // wait until SPI interrupt flag=1(data is receive correctly)
   return SPI1BUF; // return the received byte from SPI data register
}

// Function to move cursor to a wanted position in the first row 
void move_cursor_first_row(uint8 counter)
{
    // if the arg is greater than the number of column of the LCD put it equal to 0
    if (counter > 16)
    {
        counter = 0;
    } 
    SPI_sendByte(0x80+counter);
}

// Function to move cursor to a wanted position in the second row 
void move_cursor_second_row(uint8 counter)
{
    // if the arg is greater than the number of column of the LCD put it equal to 0
    if (counter > 16)
    {
        counter = 0;
    }
    SPI_sendByte(0xC0+counter);
}

// Function to clear the first or both rows of the LCD depending on the flag 
void clear_LCD(int flag)
{
    move_cursor_first_row(0);
    int i=0;
    for (i=0;i<16;i++)
    {
       SPI_sendByte(' '); 
    }
    if (flag == 1)
    {
        move_cursor_second_row(0);
        int j=0;
        for (j=0;j<16;j++)
        {
           SPI_sendByte(' '); 
        }
    }
}

void clear_chars(int flag, int pose)
{
    if (flag == 0)
    {
        move_cursor_first_row(pose);
        for (int i = pose; i < 16; i++)
        {
           SPI_sendByte(' '); 
        }
    }
    
    if (flag == 1)
    {
        move_cursor_second_row(pose);
        for (int j = pose; j < 16; j++)
        {
           SPI_sendByte(' '); 
        }
    }
}

// Function to send string from SPI
void SPI_sendString(const char *Str)
{
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		SPI_sendByte(Str[i]);
		i++;
	}
}

// Function to receive string from SPI
void SPI_receiveString(char *Str)
{
	unsigned char i = 0;
	Str[i] = SPI_recieveByte();
	while(Str[i] != '#')
	{
		i++;
		Str[i] = SPI_recieveByte();
	}
	Str[i] = '\0';
}