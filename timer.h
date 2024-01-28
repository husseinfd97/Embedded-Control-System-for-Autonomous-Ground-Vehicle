/* 
 * File:   timer.h
 * Author: Hussein Ahmed Fouad Hassan s5165612,
 *         Daniele Martino Parisi s4670964,
 *         Davide Leo Parisi s4329668,
 *         Sara Sgambato s4648592
 */

#ifndef TIMER_H
#define	TIMER_H
#define TIMER1 1
#define TIMER2 2
#define TIMER3 3
#define TIMER4 4
#define TIMER5 5
#define fcy 1843200 

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void init_timer_1(void);
void init_timer_2(void);
void init_timer_4(void);
void choose_prescaler(int, int *, int *);
void tmr_setup_period(int, int);
void tmr_wait_period(int);
void tmr_wait_ms(int, int);

#endif	

