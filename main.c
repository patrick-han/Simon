#include <msp430.h> 
#include <sequence.h>
#include <setup.h>
#include <gameStart.h>

// Flags/States
#define startUp 1
#define playGame 2
#define gameWon 3
#define gameLost 4

unsigned int state = startUp;

int gameOverWin[] = {0,1,2,3,0,1,2,3,99}; // TODO: Program this sequence
int gameOverLose[] = {3,2,1,0,3,2,1,0,99}; // TODO: Program this sequence
int startSequence[] = {0,2,1,3,0,2,1,3,99}; // TODO: Program this sequence

int main(void)
{
    timer_setup();
    buzzer_setup();
    SPI_setup();
    wdt_disable();
    button_setup();
    setup_temperature_sensor();
    __bis_SR_register(GIE);                   // Enable general interrupts

    while(1)
    {
    switch (state)
        {
            case startUp:

                P2DIR |= 0x1D;                           // Make P2.0, 2.2, 2.3, and 2.4 output pins (disabled)
                playSequence(startSequence, sizeof(startSequence)/sizeof(int)-1, 0);
                P2DIR &= ~0x1D;                           // Make P2.0, 2.2, 2.3, and 2.4 input pins

                state = playGame;
                break;
            case playGame:
                state = gameStart();
                break;
            case gameWon:

                P2DIR |= 0x1D;
                playSequence(gameOverWin, sizeof(gameOverWin)/sizeof(int) - 1, 0);
                P2DIR &= ~0x1D;

                state = startUp;
                break;
            case gameLost:

                P2DIR |= 0x1D;
                playSequence(gameOverLose, sizeof(gameOverLose)/sizeof(int) - 1, 0);
                P2DIR &= ~0x1D;

                state = startUp;
                break;
        }
    }
}







