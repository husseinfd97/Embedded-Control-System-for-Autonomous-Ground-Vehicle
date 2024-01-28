/* 
 * File:   ADC.c
 * Author: Hussein Ahmed Fouad Hassan s5165612,
 *         Daniele Martino Parisi s4670964,
 *         Davide Leo Parisi s4329668,
 *         Sara Sgambato s4648592
 */

#include "ADC.h"
#include "xc.h"

void ADC_Init(void) 
{   
    /** ADC setup **/
    
    
    ADCON3bits.ADCS = 8; // set the TAD
    ADCON1bits.ASAM = 1; // bit for automatic start
    ADCON1bits.SSRC = 7; // bit for automatic end
    ADCON3bits.SAMC = 0b11111; // maximum autosample time value. To optimize the sampling process, for decreasing the sampling frequency
    
    // Set the channel CH0
    ADCON2bits.CHPS = 0; // Selecting channel 0
    ADCHSbits.CH0NA = 0; // Set CH0 to ground
    ADCHSbits.CH0SA = 3; // Set CH0 connected to AN3
    
    // Make AN3 as analog pin
    ADPCFG = 0xFFFF;
    ADPCFGbits.PCFG3 = 0;
    
    // turn ADC on
    ADCON1bits.ADON = 1;
}

double Temp_sensor_ADC_converter(void)
{
        // ADC for temp sensor 
        int t_bits = ADCBUF0;
        double t_volts = t_bits * 5.0 /1024.0;
        double temp = (t_volts - 0.75)*100.0 + 25;
        return temp;
}
