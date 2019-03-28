/*
 * setup.h
 *
 *  Created on: Mar 26, 2019
 *      Author: patrickhan
 */

#ifndef SETUP_H_
#define SETUP_H_

void button_setup(void); // Setup pushbuttons

void setup_temperature_sensor(void); // Setup the ADC10 to take input from the temperature sensor
void disable_temperature_sensor(void);

void setup_wdt(void); // Setup the Watchdog Timer
void disable_wdt(void); // Disable Watchdog Timer





#endif /* SETUP_H_ */
