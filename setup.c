/*
 * setup.c
 *
 *  Created on: Mar 26, 2019
 *      Author: patrickhan
 *      Description: Contains functions for setting up and disabling MSP430 modules
 */
#include <msp430.h>

void button_setup(void) {
    /* BUTTON LAYOUT */
    /* (P2.4) S4 --------- S3 (P2.3) */
    /*         |            |        */
    /*         |            |        */
    /* (P2.0) S1 --------- S2 (P2.2) */

    // ------ Setup pins for P1 and P2 as outputs (For the LEDS) and clear them
    P2DIR &= ~0x1C;                           // Make P2.0, 2.2, 2.3, and 2.4 input pins
    P2OUT &= ~0x1C;                           // Init P2 pins to 0

    // ------ Enable interrupt
    P2IE  |= 0x1C;     // Enable interrupt for P2.0, 2.2, 2.3, and 2.4
    P2IES |= 0x1C;     // Enable edge select for the interrupt (high to low transition)
    P2IFG &= ~0x1C;    // Clearing the interrupt status of P2.0, 2.2, 2.3, and 2.4
}



void setup_temperature_sensor(void) {
    ADC10CTL0 &= ~ENC;                        // Disable ENC (Enable-Conversion) bit by default
    // TODO: Verify these ports/clock selections are correct
    ADC10AE0 |= BIT0;
    ADC10CTL0 |= REFON + SREF_1 + REF2_5V + ADC10SHT_3 + ADC10ON + ADC10IE;      // Reference on, select ref voltage, Sample and hold time, ADC10 ON, enable ADC interrupt enable,
    ADC10CTL1 |= ADC10DIV_0 + ADC10SSEL_1 + INCH_0;  // Divide by 1, select ACLK, Select temp sensor as channel

    ADC10CTL0 |= ENC + ADC10SC;                 // start conversion
}

void disable_temperature_sensor(void) {
    ADC10CTL0 &= ~ENC;                        // Disable ENC (Enable-Conversion) bit
}


void setup_wdt(void) {

    // TODO: obv need to change this
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}


void disable_wdt(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}
