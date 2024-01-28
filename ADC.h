/* 
 * File:   ADC.h
 * Author: Hussein Ahmed Fouad Hassan s5165612,
 *         Daniele Martino Parisi s4670964,
 *         Davide Leo Parisi s4329668,
 *         Sara Sgambato s4648592
 */

#ifndef ADC_H
#define	ADC_H

void ADC_Init(void);
double Potentiometer_ADC_converter(void);
double Temp_sensor_ADC_converter(void);

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

