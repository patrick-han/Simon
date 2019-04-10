/*
 * sequence.c
 *
 *  Created on: Mar 24, 2019
 *      Author: patrickhan
 *      Description: Contains functions for playing array sequences
 */
#include <sequence.h>
#include <msp430.h>
#include <global.h>

// Sound frequencies for each LED, running on SMCLK = 1 MHz
// --> ClockSpeed (Hz) / Desired Frequency (Hz)
// Timer Capture/Control Register is 16-bits (65535 max value)
#define LED_0 2551 // G_4
#define LED_1 2273 // A_4
#define LED_2 1911 // C_5
#define LED_3 1703 // D_5

#define LED_99 0   // OFF
//#define note_len 62500 // ID_3 for the Timer clocks (divide by 8)

// LED frames for normal play
//                            B     G     R
int LED_0_frame[4] = {0xFF, 0x00, 0x00, 0xFF};
int LED_1_frame[4] = {0xFF, 0x00, 0xFF, 0x00};
int LED_2_frame[4] = {0xFF, 0xFF, 0x00, 0x00};
int LED_3_frame[4] = {0xFF, 0x00, 0xA3, 0xFF}; // Orange

int off_frame[4] = {0xFF, 0x00, 0x00, 0x00};

unsigned int i; // for loop iterator
unsigned int k; // for loop iterator for main sequence loop
unsigned int z;

extern int debouncing;
extern int timeout;




// Difficulty-chooser frames
//       B     G     R
int difficulty_frame[] =
{
0x00, 0x00, 0x00, 0x00, // Start Frame
0xFF, 0x00, 0xFF, 0x00, // Easy: Green
0xFF, 0x00, 0xFF, 0xFF, // Medium: Yellow
0xFF, 0x00, 0x00, 0xFF, // Hard: Red
0xFF, 0xD3, 0x00, 0x94, // Very Hard: Purple
0xFF, 0xFF, 0xFF, 0xFF  // End frame
};


/*
 * LED's are daisy chained: 1 -> 2 -> 3 -> 4 (corresponding to respective buttons)
 *
 *
 * Data Line: P1.2 (UCA0TXD/UCA0SIMO)
 * CLK line: P1.4 (UCA0CLK)
 */

/* LED LAYOUT */
/* 4 --------- 3 */
/* |           | */
/* |           | */
/* 1 --------- 2 */

void sendStartFrame(void) { // Sends start frame bits for LED
    for (i = 0; i <= 3; i++) {
        while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
            UCA0TXBUF = 0x00;
    }
}

void sendEndFrame(void) { // Sends end frame bits for LED
    for (i = 0; i <= 3; i++) {
        while(!(IFG2 & UCA0TXIFG));
            UCA0TXBUF = 0xFF;
    }
}

void sendOffFrame(void) { // Sends empty frame bits for LED (clears the LED)
    for (i = 0; i <= 3; i++) {
        while(!(IFG2 & UCA0TXIFG));
            UCA0TXBUF = off_frame[i];
    }
}

void playLED(int LED_n) {                         // Takes in a single integer corresponding to an LED/buzzer value
    if (LED_n == 0) {
        // Buzzer Code
        TA1CCR0 = LED_0; // TA1 controls the frequency of our note
        P2OUT |= BIT1;   // Start outputting sound from P2.1 (Piezo)

        // LED Code
        sendStartFrame();
        for (i = 0; i <= 3; i++) { // Send Color Frame
            while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
                UCA0TXBUF = LED_0_frame[i];
        }
        sendOffFrame(); // Other LEDs are off, send empty "off" frames
        sendOffFrame();
        sendOffFrame();
        sendEndFrame();
    }
    else if (LED_n == 1) {
        TA1CCR0 = LED_1;
        P2OUT |= BIT1;

        sendStartFrame();
        sendOffFrame();
        for (i = 0; i <= 3; i++) {
            while(!(IFG2 & UCA0TXIFG));
                UCA0TXBUF = LED_1_frame[i];
        }
        sendOffFrame();
        sendOffFrame();
        sendEndFrame();
    }
    else if (LED_n == 2) {
        TA1CCR0 = LED_2;
        P2OUT |= BIT1;

        sendStartFrame();
        sendOffFrame();
        sendOffFrame();
        for (i = 0; i <= 3; i++) {
            while(!(IFG2 & UCA0TXIFG));
                UCA0TXBUF = LED_2_frame[i];
        }
        sendOffFrame();
        sendEndFrame();
    }
    else if (LED_n == 3) {
        TA1CCR0 = LED_3;
        P2OUT |= BIT1;

        sendStartFrame();
        sendOffFrame();
        sendOffFrame();
        sendOffFrame();
        for (i = 0; i <= 3; i++) {
            while(!(IFG2 & UCA0TXIFG));
                UCA0TXBUF = LED_3_frame[i];
        }
        sendEndFrame();
    }
    else if (LED_n == 99) { // For clearing the LEDs
        TA1CCR0 = LED_99; // No Sound
        P2OUT &= ~BIT1;

        sendStartFrame();
        sendOffFrame();
        sendOffFrame();
        sendOffFrame();
        sendOffFrame();
        sendEndFrame();
    }
    else if (LED_n == 98) { // Frame for showing the difficulty select
        TA1CCR0 = LED_99; // No sound
        P2OUT &= ~BIT1;

        for (i = 0; i <= 23; i++) {
            while(!(IFG2 & UCA0TXIFG));
                UCA0TXBUF = difficulty_frame[i];
        }
    }
    else if (LED_n == 100) { // Play difficulty select EASY
        TA1CCR0 = LED_99; // No sound
        P2OUT &= ~BIT1;

        sendStartFrame();
        for (i = 4; i <= 7; i++) {
            while(!(IFG2 & UCA0TXIFG));
                UCA0TXBUF = difficulty_frame[i];
        }
        sendOffFrame();
        sendOffFrame();
        sendOffFrame();
        sendEndFrame();
    }
    else if (LED_n == 101) { // Play difficulty select MEDIUM
        TA1CCR0 = LED_99; // No sound
        P2OUT &= ~BIT1;

        sendStartFrame();
        sendOffFrame();
        for (i = 8; i <= 11; i++) {
            while(!(IFG2 & UCA0TXIFG));
                UCA0TXBUF = difficulty_frame[i];
        }
        sendOffFrame();
        sendOffFrame();
        sendEndFrame();
    }
    else if (LED_n == 102) { // Play difficulty select HARD
        TA1CCR0 = LED_99; // No sound
        P2OUT &= ~BIT1;

        sendStartFrame();
        sendOffFrame();
        sendOffFrame();
        for (i = 12; i <= 15; i++) {
            while(!(IFG2 & UCA0TXIFG));
                UCA0TXBUF = difficulty_frame[i];
        }
        sendOffFrame();
        sendEndFrame();
    }
    else if (LED_n == 103) { // Play difficulty select VERY HARD
        TA1CCR0 = LED_99; // No sound
        P2OUT &= ~BIT1;

        sendStartFrame();
        sendOffFrame();
        sendOffFrame();
        sendOffFrame();
        for (i = 16; i <= 19; i++) {
            while(!(IFG2 & UCA0TXIFG));
                UCA0TXBUF = difficulty_frame[i];
        }
        sendEndFrame();
    }
    TA1CCR1 = TA1CCR0 >> 2; // Maximize volume & minimize distortion with 50% duty cycle
}

void playSequence(int arr[], int n, int press) {  // Accepts the array and size of the array to be played (allows us to play partition) (n includes 0 index, so may need to subtract 1)
                                                  // Third argument 'press' is whether or not the playSequence() call is attributed to a button press (1) or just a sequence (0)
                                                  // or a sequence involving the difficulty select (-1)

    P2DIR |= BIT1;                                // Make P2.1 an output pin (Piezo)
    P2DIR |= BIT5;                                // Make P2.5 an output pin (Piezo)

    for (k = 0; k <= n; k++) {

        playLED(arr[k]);                // Start playing our tone/note
        P2DIR |= BIT1;                                // Make P2.1 an output pin (Piezo)
        P2DIR |= BIT5;                                // Make P2.5 an output pin (Piezo)

        if (press == 0) {               // If this call was the result of needing a sequence (not a button press)
            __delay_cycles(500000);
            playLED(99);
            P2DIR &= ~BIT1;             // Disable Piezo (making it an input)
            P2DIR &= ~BIT5;             // Disable Piezo (making it an input)

        }
        else if (press == -1) {
            __delay_cycles(100000);
//            P2OUT &= ~BIT1;           // Shut off P2.1 (Piezo PWM output)
            playLED(99);
            P2DIR &= ~BIT1;             // Disable Piezo (making it an input)
            P2DIR &= ~BIT5;             // Disable Piezo (making it an input)
        }
    }
//    P2DIR &= ~BIT1;                   // Disable Piezo (making it an input)
//    P2DIR &= ~BIT5;                   // Disable Piezo (making it an input)
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
    if (debouncing == 1) {  // For when we are using TA1 as a freeze for button debouncing
        P2IE  |= BIT0 + BIT2 + BIT3 + BIT4;      // Re-enable interrupt for P2.x
        P2IFG &= ~(BIT0 + BIT2 + BIT3 + BIT4);   // Clear interrupt status flags for P2.x so it's ready for another button press (Safety)
        debouncing = 0;
        __bic_SR_register_on_exit(LPM0_bits);
    }
    else if (timeout == 3) { // TODO: JANK
        TA1CCTL1 &= ~CCIE;                       // Disable interrupts for TA1
        TA1CCTL1 |= CCIE;                     // Re-enable TA1 interrupt for the purpose of freeze time
        __bic_SR_register_on_exit(LPM0_bits);    // Exit when timeout timer runs out
    }
    else {                  // For the normal case where we are using TA1 to control note frequency
        TA1CCTL1 &= ~CCIE;
    }
}

