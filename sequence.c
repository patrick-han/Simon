/*
 * sequence.c
 *
 *  Created on: Mar 24, 2019
 *      Author: patrickhan
 *      Description: Contains functions for playing array sequences
 */
#include <sequence.h>
#include <msp430.h>

// Sound frequencies for each LED, running on SMCLK = 1 MHz
// --> ClockSpeed (Hz) / Desired Frequency (Hz)
// TODO!!!!!!!!!
#define LED_0 2551 // G_4
#define LED_1 2273 // A_4
#define LED_2 1911 // C_5
#define LED_3 1703 // D_5
#define note_len 500000 // How long to play each note (0.5 seconds)


// TODO: define these
//static int frame[] =    // frame contains LED colors corresponding to different buttons
//{     //B     G     R
//0x00, 0x00, 0x00, 0x00, // Start Frame
//0xFF, 0x00, 0x00, 0x00, // LED 1
//0xFF, 0x00, 0x00, 0x00,
//0xFF, 0x00, 0x00, 0x00,
//0xFF, 0x00, 0x00, 0x00,
//0xFF, 0xFF, 0xFF, 0xFF  // End frame
//};

//                            B     G     R
int LED_0_frame[4] = {0xFF, 0x00, 0x00, 0xFF};
int LED_1_frame[4] = {0xFF, 0x00, 0xFF, 0x00};
int LED_2_frame[4] = {0xFF, 0xFF, 0x00, 0x00};
int LED_3_frame[4] = {0xFF, 0x42, 0xF1, 0xF4}; // Yellow

unsigned int i; // for loop iterator

/*
 * LED's are daisy chained: 1 -> 2 -> 3 -> 4 (corresponding to respective buttons)
 *
 *
 * Data Line: P1.2 (UCA0TXD/UCA0SIMO)
 * CLK line: P1.4 (UCA0CLK)
 */

void sendStartFrame(void) { // Sends start frame bits for LED
    for (i = 0; i <= 3; i++) {
        UCA0TXBUF = 0x00;
    }
}

void sendEndFrame(void) { // Sends end frame bits for LED
    for (i = 0; i <= 3; i++) {
        UCA0TXBUF = 0xFF;
    }
}

void playLED(int LED_n) { // Takes in a single integer corresponding to an LED/buzzer value
    // TODO: /LED lightup stuff etc.
    IE2 |= UCA0TXIE;     // Enable transmit interrupt

//    UCA0TXBUF = frame[i];                     // Write frame elements to buffer
    if (LED_n == 0) {
        // Buzzer Code
        TA1CCR0 = LED_0; // TA1 controls the frequency of our note
        P2OUT |= BIT1;   // Start outputting sound from P2.1 (Piezo)

        // LED Code
        //TODO idk if this is a good implementation
        sendStartFrame();
        for (i = 0; i <= 3; i++) { // Send Color Frame
            UCA0TXBUF = LED_0_frame[i];
        }
        sendEndFrame();
    }
    else if (LED_n == 1) {
        TA1CCR0 = LED_1;
        P2OUT |= BIT1;

        sendStartFrame();
        for (i = 0; i <= 3; i++) { // Send Color Frame
            UCA0TXBUF = LED_1_frame[i];
        }
        sendEndFrame();
    }
    else if (LED_n == 2) {
        TA1CCR0 = LED_2;
        P2OUT |= BIT1;

        sendStartFrame();
        for (i = 0; i <= 3; i++) { // Send Color Frame
            UCA0TXBUF = LED_2_frame[i];
        }
        sendEndFrame();
    }
    else if (LED_n == 3) {
        TA1CCR0 = LED_3;
        P2OUT |= BIT1;

        sendStartFrame();
        for (i = 0; i <= 3; i++) { // Send Color Frame
            UCA0TXBUF = LED_3_frame[i];
        }
        sendEndFrame();
    }
    TA1CCR1 = TA1CCR0 >> 2; // Maximize volume & minimize distortion with 50% duty cycle
}

void playSequence(int arr[], int n) {  // Accepts the array and size of the array to be played (allows us to play partition) (n includes 0 index, so may need to subtract 1)
    for (i = 0; i <= n; i++) {
        TA0CCTL1 &= ~CCIE;            // Disable interrupts for TA0
        IE2 &= ~UCA0TXIE;             // Disable transmit interrupt (LEDs)

        playLED(arr[i]);              // Start playing our tone/note
        TA0CCR0 = 0;                  // Set period as 0 (for safety)
        TA0CCR0 = note_len;           // Set period to desired note length

        TA0CCTL1 |= CCIE;             // Enable interrupts for TA0
        __bis_SR_register(LPM0_bits); // Enter LPM0 until the TA0CCR0 runs out
        P2OUT &= ~BIT1;               // Shut off P2.1 (Piezo PWM output)

        TA0CCTL1 &= ~CCIE;            // Disable interrupts for TA0
        IE2 &= ~UCA0TXIE;             // Disable transmit interrupt (LEDs)
    }
}


// buzzer uses P2.1/TA1.1 and P2.5/TA1.2, just choose one of them as ground.

// CCIE 1 --> timer that controls the T (the timer's timer) (TA0)
// CCR0 1 --> CCR0 of timer that controls the T
//
// CCIE 1 disabled --> disable the interrupt
// playsound()     --> start playing the sound (using the other timer to set the frequency etc)
// CCR0 1 = 0      --> timer's timer period = 0
// CCR0 1 = T      --> timer's timer period = some amt of seconds
// CCIE 1 = 1      --> enable the interrupt for the timer's timer
// bis lpm0        --> go into low power mode (msp will get stuck here until end of timer's timer ISR, and then jump out of LPM)
// pin out for buzzer = 0 --> stop the buzzer sound
// CCIE 1 disabled --> disable interrupt again

// TA0 interrupt service routine, used for controlling the duration of a note
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) Timer_A0 (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(LPM0_bits);  // On exit from the Timer A0 ISR, take the MSP430 out of low power mode 0
}

// TA1 interrupt service routine, used for controlling the frequency of a note
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer_A1 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) Timer_A1 (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(LPM0_bits);  // On exit from the Timer A1 ISR, take the MSP430 out of low power mode 0
}



