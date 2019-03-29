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
#include <setup.c>

#define M 5; // The length of our gameSequence, needs to be changed for different sequence sizes/difficulty
int gameSequence[] = {0, 0, 0, 0, 0}; // length-M pattern (will be affected by the seed)

int nextLED; // Next LED in our gameSequence

unsigned int n = 0; // Keeps track of what part of M we are on.


int gameStart(void) {
//    srand(ADC10MEM);            // Generate an initial random seed based on temperature sensor
    srand(640); // Placeholder for testing
    // disable_temperature_sensor();

    while (n < sizeof(gameSequence)/sizeof(int)) { // While the player has not reached the game end
        if (n == M)
        {
            return 1; // game won!
        }
        else
        {
            nextLED = rand(); // Generate the next LED in our sequence
            gameSequence[n] = nextLED; // Add the new generated LED to the sequence
            playSequence(*gameSequence, n); // Play the current sequence partition (including the newly generated LED)
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


// TODO: lol this stuff below is garbage
// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
  P1IE  &= ~BIT2;   // Disable interrupt for P1.2
  P1IFG &= ~BIT2;   // Clear interrupt status for P1.2 so it's ready for another button press
  if (asleep) {
    asleep = 0;
    P1IE  |= BIT2;
    __bic_SR_register_on_exit(LPM4_bits);  // On exit from being "asleep", exit low power mode 4 (Turn on CPU)
  }
  else {
    P1OUT &= ~0xC0;  // CLear P1 (LEDs off)
    P2OUT &= ~0xFF;  // CLear P2 (LEDs off)
    asleep = 1;
    P1IE  |= BIT2;
    __bis_SR_register_on_exit(LPM4_bits + GIE); // On exit from being put to "sleep", enter low power mode 4 and re-enable interrupt
  }
}


