#include <msp430.h> 
// test push :D
#include <sequence.h>
#include <setup.h>
#include <gameStart.h>

// Flags/States
#define startUp 1
#define playGame 2
#define gameWon 3
#define gameLost 4

unsigned int state = startUp;

//unsigned int playerCanPress = 0; // Flag that indicates whether player input is allowed/valid (i.e. only during the valid 1-2 sec time window)

int gameOverWin[] = {0,1,2,3,0,1,2,3,99}; // TODO: Program this sequence
int gameOverLose[] = {3,2,1,0,3,2,1,0,99}; // TODO: Program this sequence
int startSequence[] = {0,1,2,3,0,1,2,3,99}; // TODO: Program this sequence

//int startSequence[] = {0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,99,
//                       0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,99};

//#include <rand.h>
//int test_rand;

int main(void)
{
//    setup_temperature_sensor(); // Setup temperature sensor
    timer_setup();
    buzzer_setup();
    SPI_setup();
    wdt_setup();
    button_setup();
    __bis_SR_register(GIE);                   // Enable general interrupts

    while(1)
    {
//    __bis_SR_register(LPM0);
    switch (state)
        {
            case startUp:
//                srand(731);
//                while(1) {
//                    test_rand = rand();
//                }
                playSequence(startSequence, sizeof(startSequence)/sizeof(int)-1, 0);
                state = playGame;
                break;
            case playGame:
                state = gameStart();
                break;
            case gameWon:
                playSequence(gameOverWin, sizeof(gameOverWin)/sizeof(int) - 1, 0);
                state = startUp;
                break;
            case gameLost:
                playSequence(gameOverLose, sizeof(gameOverLose)/sizeof(int) - 1, 0);
                state = startUp;
                break;
        }
    }
}





// ADC interrupt service routine (For the temp sensor)
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







