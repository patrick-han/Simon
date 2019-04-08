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



// Sequencing related variables
int seq0[] = {0};
int seq1[] = {1};
int seq2[] = {2};
int seq3[] = {3};
int seq99[] = {99};
int seq98[] = {98};

#define M 4// The length of our gameSequence (INDEXED FROM 0), needs to be changed for different sequence sizes/difficulty
int gameSequence[] = {0, 0, 0, 0, 0}; // length-M pattern (will be affected/built-out by the seed)


// Debouncing and Button Variables
const int freeze_amt = 1000; // Number of cycles we will freeze for debouncing
extern int debouncing;
int button_pressed = 0;

// Game logic variables
int nextLED; // Next LED in our gameSequence
int button; // Which button was pressed?
unsigned int start = 0; // Flag that is set when button is pressed to start game
unsigned int n = 0; // Keeps track of what part of M we are on.
int i; // Iterator variable
int b; // Another iterator variable
int random_seed; // Used for generating a random pattern

// Timeout variables
long int timeout_limit = 125000; // By default, Medium
unsigned long int timeout_count = 0;
extern int timeout; // flag

void reset_game(void) { // Helper function that resets game variables
    start = 0;
    n = 0;
    button = 99;
    button_pressed = 0;
    timeout_count = 0;
    timeout = 0;
}


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
    start = 0;           // Ensure if the player accidentally hits some buttons during one of the game lost/win/startup sequences, it doesn't set the difficulty yet
    while (start == 0) { // Before we start the game, wait for a player to press a button to begin

        playSequence(seq98, 0, 0);
    }
    ADC10CTL0 |= ENC + ADC10SC;     // Start conversion
    __bis_SR_register(LPM0_bits);
    random_seed = ADC10MEM;
    ADC10CTL0 &= ~ENC;

    srand(random_seed);            // Generate an initial random seed based on temperature sensor
//    srand(731); // Placeholder for testing
    // disable_temperature_sensor();

    for (i = 0 ; i <= M ; i++) { // Build out the levels
        nextLED = rand();
        gameSequence[i] = nextLED;
    }

    if (start == 1) {                            // If a button has been pressed to start the game
        __delay_cycles(500000);                  // Just a small delay after starting the game with a button press

        while (n <= M) {                         // While the player has not reached the game end
            playSequence(gameSequence, n, 0);    // Play the current sequence partition (including the newly generated LED)
            // while (wait for press & counter < timeout_limit) {
            //    delay(10 microseconds)
            //    counter++
            // if (counter > timeout_limit)
            //    fail
            // else if (button = correct button)
            //    win
            // else
            //    fail
            // TODO: yeah this FAILS
            timeout = 1;
            for (b = 0; b <= n; b++) { // For each button/stage to check in the level/sequence

                timeout_count = 0; // Reset timer for each stage of the level to check
                while ((button_pressed == 0) & (timeout_count < timeout_limit)) { // Waiting for the next button press
                    timeout_count++; // Iterate the timeout counter
                }



                if (timeout_count >= timeout_limit) {    // If we exceed the timeout, we lose
                    reset_game();
                    return 4; // Lose!
                }
                else if (button != gameSequence[b]) {    // If the player presses the wrong button
                    button_pressed = 0;
                    button = 99;
                    reset_game();
                    return 4; // Lose!
                }
                else if (button == gameSequence[b]) {    // If the player presses the right button
                    button_pressed = 0;
                    button = 99;
                }




            }
            n++;



        }
    }
    // Reach here if n exceeds M (i.e. we pressed all the right buttons
    reset_game();
    return 3; // Game won!

}

// ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__) // Pre-compilers checks for compiler compatibility
#pragma vector=ADC10_VECTOR // Treat following code as part of the interrupt vector
__interrupt void ADC10_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0 after ISR completes
}

// Watchdog Timer interrupt service routine
//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__) // Pre-compilers checks for compiler compatibility
//#pragma vector=WDT_VECTOR // Treat following code as part of the interrupt vector
//__interrupt void watchdog_timer(void)
//#elif defined(__GNUC__)
//void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
//#else
//#error Compiler not supported!
//#endif
//{
//    if (timeout < timeout_limit) {
//        timeout++;
//    }
//    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0 after ISR completes
//}



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
    button_pressed = 1;
    P2IE  &= ~(BIT0 + BIT2 + BIT3 + BIT4);   // Disable interrupt for P2.x
    TA1CCTL1 &= ~CCIE;                       // Disable interrupts for TA1

    P2DIR &= ~BIT1;                          // Disable Piezo (making it an input)
    P2DIR &= ~BIT5;                          // Disable Piezo (making it an input)

    button = checkButton();                  // Get the button we pressed


    if (start == 0) {                        // Difficulty selector code
        if (button == 0) {                   // Easy
            timeout_limit = 125000;
        }
        else if (button == 1) {              // Medium
            timeout_limit = 70000;
        }
        else if (button == 2) {              // Hard
            timeout_limit = 30000;
        }
        else if (button == 3) {              // Very Hard
            timeout_limit = 10000;
        }
    }
    start = 1;

    P2IFG &= ~(BIT0 + BIT2 + BIT3 + BIT4);   // Clear interrupt status flags for P2.x so it's ready for another button press

    // Setup and play sound pins
    // Setup and show LEDs
    if (button == 0) {
        playSequence(seq0, 0, 1);
        while(!(P2IN & BIT0));
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
    if (timeout != 1) {
        button = 99;
        button_pressed = 0;
    }
    __bis_SR_register_on_exit(LPM0_bits); // Will freeze here until TA1 ISR is called and finished (ISR located in sequence.c)
    // TODO: Maybe a temporary fix for the button pressing delay before the sequence is played again
    __delay_cycles(25000);
}


