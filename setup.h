/*
 * setup.h
 *
 *  Created on: Mar 26, 2019
 *      Author: patrickhan
 */

#ifndef SETUP_H_
#define SETUP_H_

void button_setup(void); // Setup pushbuttons

void buzzer_setup(void); // Setup buzzer pins

void timer_setup(void); // Setup timers

void SPI_setup(void); // Setup SPI communication

void setup_temperature_sensor(void); // Setup the ADC10 to take input from the temperature sensor
void disable_temperature_sensor(void);

void wdt_setup(void); // Setup the Watchdog Timer
void wdt_disable(void); // Disable Watchdog Timer





#endif /* SETUP_H_ */
