/*
 * gameStart.c
 *
 *  Created on: Mar 27, 2019
 *      Author: patrickhan
 *      Description: Contains all the logic for the main Simon game
 */
#include <msp430.h>
#include <rand.h>
#include <sequence.h>
#include <setup.h>

#define M 4// The length of our gameSequence (INDEXED FROM 0), needs to be changed for different sequence sizes/difficulty
int gameSequence[] = {0, 0, 0, 0, 0}; // length-M pattern (will be affected by the seed)

int testSequence[] = {0,1,2,3,0,1,2,3,99};

int nextLED; // Next LED in our gameSequence
unsigned int start = 0; // Flag that is set when button is pressed to start game

unsigned int n = 0; // Keeps track of what part of M we are on.

int checkButton(void) {
    if (P2IFG & BIT0) { // If P2.0 button is pressed
        playLED(0);
//        start = 1;
        return 0;
    }
    else if (P2IFG & BIT2) { // If P2.2 button is pressed
        playLED(1);
//        start = 1;
        return 1;
    }
    else if (P2IFG & BIT3) { // If P2.3 button is pressed
        playLED(2);
//        start = 1;
        return 2;
    }
    else if (P2IFG & BIT4) { // If P2.4 button is pressed
        playLED(3);
//        start = 1;
        return 3;
    }
}


int gameStart(void) {
    while (start == 0) { // Before we start the game, wait for a player to press a button to begin
    }

//    srand(ADC10MEM);            // Generate an initial random seed based on temperature sensor
    srand(640); // Placeholder for testing
    // disable_temperature_sensor();
    if (start == 1) {                           // If a button has been pressed to start the game
        playSequence(testSequence, 8);


        while (n < M) {                         // While the player has not reached the game end

            if (n == M) {                       // If the player has reached the game end
                return 1;                       // Game as been won!
            }
            else {
                nextLED = rand();               // Generate the next LED in our sequence
                gameSequence[n] = nextLED;      // Add the new generated LED to the sequence
                playSequence(gameSequence, n);  // Play the current sequence partition (including the newly generated LED)
            }

            // TODO: start the watchdog/timer module countdown


            // TODO: Button logic, (port interrupt)

    // This code should go in watchdog timer stuff probably
    //      if (/* TODO: Watchdog timer interrupt triggered */) { // If player waits too long to press the button;
    //          playSequence(gameOverLose, sizeof(gameOverLose)/sizeof(int) - 1); // Play game over lose sequence
    //          // TODO: player startup anim -> clear gameSequence?
    //      }

            n++; // Move on to the next LED in the random sequence
        }
    }
//    start = 0;
}


// TODO: This ISR

/* BUTTON LAYOUT */
/* (P2.4) S4 --------- S3 (P2.3) */
/*         |            |        */
/*         |            |        */
/* (P2.0) S1 --------- S2 (P2.2) */


// Port 2 interrupt service routine

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
    checkButton();
    P2IE  &= ~(BIT0 + BIT2 + BIT3 + BIT4);   // Disable interrupt for P2.x
    P2IFG &= ~(BIT0 + BIT2 + BIT3 + BIT4);   // Clear interrupt status for P2.x so it's ready for another button press
    P2IE  |= BIT0 + BIT2 + BIT3 + BIT4;    // Re-enable interrupt for P2.x
//  __bic_SR_register_on_exit(LPM0_bits);  // On exit low power mode 0
//  __bis_SR_register_on_exit(LPM0_bits + GIE); // On exit from being put to "sleep", enter low power mode 4 and re-enable interrupt
}


