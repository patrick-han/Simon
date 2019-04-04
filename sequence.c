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
// TODO!!!!!!!!!


#define LED_0 2551 // G_4
#define LED_1 2273 // A_4
#define LED_2 1911 // C_5
#define LED_3 1703 // D_5

//#define LED_0 0
//#define LED_1 0
//#define LED_2 0
//#define LED_3 0

#define LED_99 0   // OFF
//#define note_len 500000 // How long to play each note (0.5 seconds)
#define note_len 62500 // ID_3 for the Timer clocks (divide by 8)


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

int off_frame[4] = {0xFF, 0x00, 0x00, 0x00};

unsigned int i; // for loop iterator
unsigned int k; // for loop iterator for main sequence loop
unsigned int z;

extern int debouncing;

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
//        __bis_SR_register(LPM0_bits + GIE);
        while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
            UCA0TXBUF = 0x00;
    }
}

void sendEndFrame(void) { // Sends end frame bits for LED
    for (i = 0; i <= 3; i++) {
//        __bis_SR_register(LPM0_bits + GIE);
        while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
            UCA0TXBUF = 0xFF;
    }
}

void sendOffFrame(void) {
    for (i = 0; i <= 3; i++) {
//        __bis_SR_register(LPM0_bits + GIE);
        while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
            UCA0TXBUF = off_frame[i];
    }
}

void playLED(int LED_n) { // Takes in a single integer corresponding to an LED/buzzer value
    // TODO: /LED lightup stuff etc.
//    IE1 |= WDTIE;                                 // Enable WDT interrupt

//    IE2 |= UCA0TXIE;                            // Enable transmit interrupt
//    IFG2 &= ~UCA0TXIFG;                         // Clear interrupt status before transmitting anything
    if (LED_n == 0) {
        // Buzzer Code
        TA1CCR0 = LED_0; // TA1 controls the frequency of our note
        P2OUT |= BIT1;   // Start outputting sound from P2.1 (Piezo)

        // LED Code
        //TODO idk if this is a good implementation

        sendStartFrame();
//        while(1);
        for (i = 0; i <= 3; i++) { // Send Color Frame
//            __bis_SR_register(LPM0_bits + GIE);
            while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
                UCA0TXBUF = LED_0_frame[i];
        }
        sendOffFrame(); // Other LEDs are off
        sendOffFrame();
        sendOffFrame();

        sendEndFrame();
    }
    else if (LED_n == 1) {
        TA1CCR0 = LED_1;
        P2OUT |= BIT1;
        sendStartFrame();

        sendOffFrame();

        for (i = 0; i <= 3; i++) { // Send Color Frame
//            __bis_SR_register(LPM0_bits + GIE);
            while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
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


        for (i = 0; i <= 3; i++) { // Send Color Frame
//            __bis_SR_register(LPM0_bits + GIE);
            while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
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


        for (i = 0; i <= 3; i++) { // Send Color Frame
//            __bis_SR_register(LPM0_bits + GIE);
            while(!(IFG2 & UCA0TXIFG)); // Buffer ready?
                UCA0TXBUF = LED_3_frame[i];
        }
        sendEndFrame();
    }
    else if (LED_n == 99) {
        TA1CCR0 = LED_99;
        P2OUT &= ~BIT1;
        sendStartFrame();

        sendOffFrame();
        sendOffFrame();
        sendOffFrame();
        sendOffFrame();

        sendEndFrame();
    }
    TA1CCR1 = TA1CCR0 >> 2; // Maximize volume & minimize distortion with 50% duty cycle

}

void playSequence(int arr[], int n, int press) {  // Accepts the array and size of the array to be played (allows us to play partition) (n includes 0 index, so may need to subtract 1)
                                                  // Third argument 'press' is whether or not the playSequence() call is attributed to a button press or just a sequence
    P2DIR |= BIT1;                                // Make P2.1 an output pin (Piezo)
    P2DIR |= BIT5;                                // Make P2.5 an output pin (Piezo)

    for (k = 0; k <= n; k++) {
//        TA0CCTL1 &= ~CCIE;            // Disable interrupts for TA0
//        IE2 &= ~UCA0TXIE;             // Disable transmit interrupt (LEDs)

        playLED(arr[k]);                // Start playing our tone/note
        P2DIR |= BIT1;                                // Make P2.1 an output pin (Piezo)
        P2DIR |= BIT5;                                // Make P2.5 an output pin (Piezo)
//        TA0CCR0 = 0;                  // Set period as 0 (for safety)
//        TA0CCR0 = note_len;           // Set period to desired note length
//
//        TA0CCTL1 |= CCIE;             // Enable interrupts for TA0

        if (press == 0) {               // If this call was the result of needing a sequence (not a button press)
            __delay_cycles(500000);
//            P2OUT &= ~BIT1;           // Shut off P2.1 (Piezo PWM output)

            P2DIR &= ~BIT1;             // Disable Piezo (making it an input)
            P2DIR &= ~BIT5;             // Disable Piezo (making it an input)

            TA0CCTL1 &= ~CCIE;          // Disable interrupts for TA0
            IE1 &= ~WDTIE;              // Disable WDT interrupt

        }
        else {
              //TODO
        }
//        __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 until the TA0CCR0 runs out



//        P2OUT &= ~BIT1;               // Shut off P2.1 (Piezo PWM output)
//
//        TA0CCTL1 &= ~CCIE;            // Disable interrupts for TA0
//
//        IE1 &= ~WDTIE;                // Disable WDT interrupt
//


//        IE2 &= ~UCA0TXIE;             // Disable transmit interrupt (LEDs)
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
    else {                  // For the normal case where we are using TA1 to control note frequency
        TA1CCTL1 &= ~CCIE;
    }

//    __bic_SR_register_on_exit(LPM0_bits);
//    __bic_SR_register_on_exit(LPM0_bits + GIE);  // On exit from the Timer A1 ISR, take the MSP430 out of low power mode 0, clear general interrupt
}

// TA0 interrupt service routine, used for controlling the duration of a note
//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector=TIMER0_A1_VECTOR
//__interrupt void Timer_A0 (void)
//#elif defined(__GNUC__)
//void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) Timer_A0 (void)
//#else
//#error Compiler not supported!
//#endif
//{
//    TA0CCTL1 &= ~CCIE;
//    __bic_SR_register_on_exit(LPM0_bits);
////    __bic_SR_register_on_exit(LPM0_bits + GIE);  // On exit from the Timer A0 ISR, take the MSP430 out of low power mode 0, clear general interrupt
//}



//// USCI_A0 Data ISR
//#pragma vector = USCIAB0TX_VECTOR
//__interrupt void USCIA0TX_ISR(void)
//{
//    IFG2 &= ~UCA0TXIFG; // Clear interrupt status after byte is transmitted
//    // So apparently, clearing LPM within this ISR doesn't work, in fact the WDT is doing the clearing
////    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM1 after ISR completes
//}

// Watchdog Timer interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__) // Pre-compilers checks for compiler compatibility
#pragma vector=WDT_VECTOR // Treat following code as part of the interrupt vector
__interrupt void watchdog_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0 after ISR completes
}



