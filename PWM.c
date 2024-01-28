/* 
 * File:   PWM.c
 * Author: Hussein Ahmed Fouad Hassan s5165612,
 *         Daniele Martino Parisi s4670964,
 *         Davide Leo Parisi s4329668,
 *         Sara Sgambato s4648592
 */

#include "PWM.h"
#include "xc.h"
void PWM_init (void)
 {
    // PWM setup
    PTCONbits.PTMOD = 0; // Set the mode to free run, is the default one so it can be omitted
    PTCONbits.PTCKPS = 0; // Set the prescaler at 1:1
    PWMCON1bits.PEN2H = 1; // use pin PWM2H
    PWMCON1bits.PEN3H = 1; // use pin PWM3H
    PWMCON1bits.PEN2L = 1; // 
    PWMCON1bits.PEN3L = 1; // 
    // set the dead time
    DTCON1bits.DTAPS  = 0; //Prescaler 1:1
    DTCON1bits.DTA = 6; // Amount time
    PTPER = 1842; // Set the value of ||PTPER = Fcy/(1*1000Hz)-1||;
    PTCONbits.PTEN = 1; // Enabling the PWM
 }
 
 
 void PWM_set_dutycycle(double RPM1, double RPM2)
 {
    double dutyCycle1 = 0.5  + RPM1/120;
    double dutyCycle2 = 0.5  + RPM2/120; 

    PDC2 = 2*PTPER*dutyCycle1; 
    PDC3 = 2*PTPER*dutyCycle2; 
    
 }
 

