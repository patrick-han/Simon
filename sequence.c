/*
 * sequence.c
 *
 *  Created on: Mar 24, 2019
 *      Author: patrickhan
 *      Description: Contains functions for playing array sequences
 */
#include <msp430.h>

// Sound frequencies for each LED
#define LED_1 //
#define LED_2 //
#define LED_3 //
#define LED_4 //


unsigned int i; // for-loop iterator

//void playnote(int note, const int dur){ // dur is in seconds
//
//    TA0CCR0 = note;
//    TA0CCR1 = TA0CCR0/2;
//    for (i = 0; i<= dur*2; i++){
//           __delay_cycles(1);}
//
//
//    TA0CCR0 = 0;
//    for (i = 0; i<= dur*2; i++){
//           __delay_cycles(1);}
//}



void playLED(int LED_n) { // Takes in a single integer corresponding to an LED/buzzer value
    // TODO: piezo buzzer/LED lightup stuff etc.
    if (LED_n == 0) {
        TA0CCR0 = LED_1; // some frequency
    }
    else if (LED_n == 1) {

    }
    else if (LED_n == 2) {

    }
    else if (LED_n == 3) {

    }

    TA0CCR1 = TA0CCR0/2; // Maximize volume & minimize distortion with 50% duty cycle
}

void playSequence(int *arr, int n) { // Accepts the array and size of the array to be played (allows us to play partition) (n includes 0 index, so may need to subtract 1)
    for (i = 0; i <= n; i++) {
        playLED(arr[i]);
    }
}


// CCIE 1 --> timer that controls the T (the timer's timer)
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


