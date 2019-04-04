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
#include <gameStart.h>
#include <global.h>

#define M 4// The length of our gameSequence (INDEXED FROM 0), needs to be changed for different sequence sizes/difficulty
int gameSequence[] = {0, 0, 0, 0, 0}; // length-M pattern (will be affected by the seed)

int freeze_amt = 1000; // Number of cycles we will freeze for debouncing
extern int debouncing;

int testSequence[] = {0,1,2,3,0,1,2,3,99};
int seq0[] = {0};
int seq1[] = {1};
int seq2[] = {2};
int seq3[] = {3};
int seq99[] = {99};


int nextLED; // Next LED in our gameSequence
int button; // WHich button was pressed?
int test_flag;
unsigned int start = 0; // Flag that is set when button is pressed to start game

unsigned int n = 0; // Keeps track of what part of M we are on.

int checkButton(void) {
    if (P2IFG & BIT0) { // If P2.0 button is pressed

//        start = 1;
        return 0;
    }
    else if (P2IFG & BIT2) { // If P2.2 button is pressed

//        start = 1;
        return 1;
    }
    else if (P2IFG & BIT3) { // If P2.3 button is pressed

//        start = 1;
        return 2;
    }
    else if (P2IFG & BIT4) { // If P2.4 button is pressed

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
        playSequence(testSequence, 8, 0);


        while (n < M) {                         // While the player has not reached the game end

            if (n == M) {                       // If the player has reached the game end
                return 1;                       // Game as been won!
            }
            else {
                nextLED = rand();               // Generate the next LED in our sequence
                gameSequence[n] = nextLED;      // Add the new generated LED to the sequence
                playSequence(gameSequence, n, 0);  // Play the current sequence partition (including the newly generated LED)
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
    P2IE  &= ~(BIT0 + BIT2 + BIT3 + BIT4);   // Disable interrupt for P2.x
    TA1CCTL1 &= ~CCIE;                       // Disable interrupts for TA1

    P2DIR &= ~BIT1;                          // Disable Piezo (making it an input)
    P2DIR &= ~BIT5;                          // Disable Piezo (making it an input)

    button = checkButton();                  // Get the button we pressed
    P2IFG &= ~(BIT0 + BIT2 + BIT3 + BIT4);   // Clear interrupt status flags for P2.x so it's ready for another button press
    //setup sound pins
    //setup LEDs
    if (button == 0) {
        playSequence(seq0, 0, 1);
        while(!(P2IN & BIT0));
        test_flag = 123;
    }
    else if (button == 1) {
        playSequence(seq1, 0, 1);
        while(!(P2IN & BIT2));
    }
    else if (button == 2) {
        playSequence(seq2, 0, 1);
        while(!(P2IN & BIT3));
    }
    else if (button == 3) {
        playSequence(seq3, 0, 1);
        while(!(P2IN & BIT4));
    }
    // When the user lets go...
    P2DIR &= ~BIT1;                       // Disable Piezo (making it an input)
    P2DIR &= ~BIT5;                       // Disable Piezo (making it an input)
    playLED(99);                          // Clear LEDs

    TA1CCR0 = freeze_amt;                 // Set timer register for # of cycles to debounce for (freeze)
    debouncing = 1;                       // We are now debouncing...  (global flag)
    TA1CCTL1 |= CCIE;                     // Re-enable TA1 interrupt for the purpose of freeze time
    __bis_SR_register_on_exit(LPM0_bits); // Will freeze here until TA1 ISR is called and finished (ISR located in sequence.c)


//    P2IE  |= BIT0 + BIT2 + BIT3 + BIT4;    // Re-enable interrupt for P2.x
//  __bic_SR_register_on_exit(LPM0_bits);  // On exit low power mode 0
//  __bis_SR_register_on_exit(LPM0_bits + GIE); // On exit from being put to "sleep", enter low power mode 4 and re-enable interrupt
}


