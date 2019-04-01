/*
 * setup.c
 *
 *  Created on: Mar 26, 2019
 *      Author: patrickhan
 *      Description: Contains functions for setting up and disabling MSP430 modules
 */
#include <setup.h>
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

    // ------ Enable interrupts
    P2IE  |= 0x1C;     // Enable interrupt for P2.0, 2.2, 2.3, and 2.4
    P2IES |= 0x1C;     // Enable edge select for the interrupt (high to low transition)
    P2IFG &= ~0x1C;    // Clearing the interrupt status of P2.0, 2.2, 2.3, and 2.4
}

void buzzer_setup(void) {
    P2DIR |= BIT1;                                // Make P2.1 an output pin (Piezo)
    P2SEL |= BIT1;                                // Select P2.1 as our PWM output
    P2SEL2 &= ~BIT1;

    P2DIR |= BIT5;                                // Make P2.5 an output pin (Piezo)
    P2OUT &= ~BIT5;                               // set to 0 output (ground)
}



void timer_setup(void) {
    // TA0
    TA0CCTL1 &= ~CCIE;                            // Disable interrupts for TA0
    TA0CCTL1 |= OUTMOD_7;                         // Governs the high/low behavior related to CCR0, CCR1
    TA0CTL = TASSEL_2 + MC_1;                     // Select SMCLK as source clock, up count mode
    TA0CCR0 = 0;

    // TA1
    TA1CCTL1 &= ~CCIE;                            // Disable interrupts for TA1
    TA1CCTL1 |= OUTMOD_7;                         // Governs the high/low behavior related to CCR0, CCR1
    TA1CTL = TASSEL_2 + MC_1;                     // Select SMCLK as source clock, up count mode
    TA1CCR0 = 0;
}

void SPI_setup(void) {
    UCA0CTL1 |= UCSWRST;                          // Enable SPI

    UCA0CTL0 |= UCCKPH + UCSYNC + UCMST + UCMSB;  // Clock phase sync mode, Synchronous  (SPI), Master Mode on, MSB first direction for the RX/TX shifter register
    UCA0CTL1 |= UCSSEL_2;                         // Select SMCLK as source

    UCA0BR1 |= 0;
    UCA0BR0 |= 0x02;                              // Baud rate settings, clock divide by 2
    UCA0CTL1 &= ~UCSWRST;                         // Enable SPI
}

void setup_temperature_sensor(void) {
    ADC10CTL0 &= ~ENC;                                                           // Disable ENC (Enable-Conversion) bit by default
    // TODO: Verify these ports/clock selections are correct!!!
    ADC10AE0 |= BIT0;
    ADC10CTL0 |= REFON + SREF_1 + REF2_5V + ADC10SHT_3 + ADC10ON + ADC10IE;      // Reference on, select ref voltage, Sample and hold time, ADC10 ON, enable ADC interrupt enable,
    ADC10CTL1 |= ADC10DIV_0 + ADC10SSEL_3 + INCH_0;                              // Divide by 1, select SMCLK, Select temp sensor as channel

    ADC10CTL0 |= ENC + ADC10SC;                                                  // Start conversion
}

void disable_temperature_sensor(void) {
    ADC10CTL0 &= ~ENC;                        // Disable ENC (Enable-Conversion) bit
}


void setup_wdt(void) {

    // TODO: obviously need to change this
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}


void disable_wdt(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}
