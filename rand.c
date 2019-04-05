/*
 * rand.c
 *
 *  Created on: Mar 19, 2019
 *      Author: patrickhan
 *      Description: Contains functions for generating random numbers
 */

#include <rand.h>
// rand module implementations

#define RAND_MAX 4 // Since we have 4 possible LEDs

unsigned int lfsr; // Our pseudo-random register


// Returns a pseudo-random number in the range of 0 to RAND_MAX.
int rand(void)
{

    int bit;                    /* Must be 16-bit to allow bit<<15 later in the code */

    /* taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) /* & 1u */;
    lfsr = (lfsr >> 1) | (bit << 15);
//    return lfsr >> 14;
    return lfsr % RAND_MAX; // Restricts the output to 0-3
}


// Init random number generator
void srand(int seed){
    lfsr = seed;
}




