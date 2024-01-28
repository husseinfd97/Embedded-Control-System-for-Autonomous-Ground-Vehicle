/* 
 * File:   timer.c
 * Author: Hussein Ahmed Fouad Hassan s5165612,
 *         Daniele Martino Parisi s4670964,
 *         Davide Leo Parisi s4329668,
 *         Sara Sgambato s4648592
 */

#include "xc.h"
#include "timer.h"

// Functions to initialise the timers
void init_timer_1(void)
{
    T1CONbits.TON = 0;
    TMR1 = 0; // reset timer counter   
}
void init_timer_2(void)
{
    T2CONbits.TON = 0;
    TMR2 = 0; // reset timer counter 
    T2CONbits.T32 = 0;   
}
void init_timer_3(void)
{
    T3CONbits.TON = 0;
    TMR3 = 0; // reset timer counter
}
void init_timer_4(void)
{
    T4CONbits.TON = 0;
    TMR4 = 0; // reset timer counter
    T4CONbits.T32 = 0;    
}
void init_timer_5(void)
{
    T5CONbits.TON = 0;
    TMR5 = 0; // reset timer counter
}

// Function to choose which prescaler to use
void choose_prescaler(int ms, int *tckps, int *pr)
{
    float steps = fcy*0.001*ms;

    if (steps < 65535)
    {
        *tckps = 0;
        *pr = steps;
        return;
    }
    
    steps = steps/8;
    if (steps < 65535)
    {
        *tckps = 1;
        *pr = steps;
        return;
    }
    
    steps = steps/8;
    if (steps < 65535)
    {
        *tckps = 2;
        *pr = steps;
        return;
    }
    
    steps = steps/4;
    if (steps < 65535)
    {
        *tckps = 3;
        *pr = steps;
        return;
    }
}

// Function to setup the period of the timer
void tmr_setup_period(int timer, int ms)
{
    int tckps, pr;

    if (timer==TIMER1)
    {
        init_timer_1();
        choose_prescaler(ms, &tckps, &pr);
        T1CONbits.TCKPS = tckps; // prescaler choice
        PR1 = pr; // steps to reach the count
        T1CONbits.TON = 1; // START THE TIMER 
    }
    if (timer==TIMER2)
    {
        init_timer_2();
        choose_prescaler(ms, &tckps, &pr);
        T2CONbits.TCKPS = tckps; // prescaler choice
        PR2 = pr; // steps to reach the count
        T2CONbits.TON = 1; // START THE TIMER
    }
    if (timer == TIMER3)
    {
        init_timer_3();
        choose_prescaler(ms, &tckps, &pr);
        T3CONbits.TCKPS = tckps; // prescaler choice
        PR3 = pr; // steps to reach the count
        T3CONbits.TON = 1; // START THE TIMER
    }
    if (timer == TIMER4)
    {
        init_timer_4();
        choose_prescaler(ms, &tckps, &pr);
        T4CONbits.TCKPS = tckps; // prescaler choice
        PR4 = pr; // steps to reach the count
        T4CONbits.TON = 1; // START THE TIMER
    }
    if (timer == TIMER5)
    {
        init_timer_5();
        choose_prescaler(ms, &tckps, &pr);
        T5CONbits.TCKPS = tckps; // prescaler choice
        PR5 = pr; // steps to reach the count
        T5CONbits.TON = 1; // START THE TIMER
    }
}

// Function to wait for the timer period to expire
void tmr_wait_period(int timer)
{   
    if (timer == TIMER1)
    {
        while(IFS0bits.T1IF == 0){} // do nothing till the flag of comparator finishes
        IFS0bits.T1IF = 0;
    }
    if (timer == TIMER2)
    {
        while (IFS0bits.T2IF == 0){}
        IFS0bits.T2IF = 0;
    }
    if (timer == TIMER3)
    {
        while (IFS0bits.T3IF == 0){}
        IFS0bits.T3IF = 0;
    }
    if (timer == TIMER4)
    {
        while (IFS1bits.T4IF == 0){}
        IFS1bits.T4IF = 0;
    }
    if (timer == TIMER5)
    {
        while (IFS1bits.T5IF == 0){}
        IFS1bits.T5IF = 0;
    }
}

// Function to wait for the ms to expire
void tmr_wait_ms(int timer, int ms)
{   
    if (timer == TIMER1)
    {
        IFS0bits.T1IF = 0;
        tmr_setup_period(timer, ms);
        while(IFS0bits.T1IF == 0){} // do nothing till the flag of comparator finishes
        IFS0bits.T1IF = 0;
        T1CONbits.TON = 0;
    }
    if (timer == TIMER2)
    {
        IFS0bits.T2IF = 0;
        tmr_setup_period(timer, ms);
        while(IFS0bits.T2IF == 0){} // do nothing till the flag of comparator finishes
        IFS0bits.T2IF = 0;
        T2CONbits.TON = 0;
    }
    if (timer == TIMER3)
    {
        IFS0bits.T3IF = 0;
        tmr_setup_period(timer, ms);
        while(IFS0bits.T3IF == 0){} // do nothing till the flag of comparator finishes
        IFS0bits.T3IF = 0;
        T3CONbits.TON = 0;
    }
    if (timer == TIMER4)
    {
        IFS1bits.T4IF = 0;
        tmr_setup_period(timer, ms);
        while(IFS1bits.T4IF == 0){} // do nothing till the flag of comparator finishes
        IFS1bits.T4IF = 0;
        T4CONbits.TON = 0;
    }
    if (timer == TIMER5)
    {
        IFS1bits.T5IF = 0;
        tmr_setup_period(timer, ms);
        while(IFS1bits.T5IF == 0){} // do nothing till the flag of comparator finishes
        IFS1bits.T5IF = 0;
        T5CONbits.TON = 0;
    }
}
