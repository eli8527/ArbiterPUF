/***************************************************************************
 *                                                                         *
 * Arbiter PUF Implementation on a Microcontroller                         *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************/
/*!
    \file        arbiterPUF.h

    \brief       Main functions for the Arbiter PUF implementation

    \author      Eric Li

    \version     1.0

    \date        29-07-2014

*/
/***************************************************************************/

#ifndef arbiterPUF
#define arbiterPUF

#define bitLength 64 //This is the bit length of the challenge and should be changed accordingly

#include "global.h"

// Converts hex array to binary
// in - input array of hex
// bin - binary output array
// size - the size in bytes. aka bitLength/8
void toBinary(unsigned char *in, unsigned char*bin, unsigned char size);

// Converts binary array to hex - no longer needed, but kept in case
// in - input binary array
// hex - output hex array
// size - size in bytes, aka bitLength/8
//void toHex(unsigned char*in, unsigned char *hex, unsigned char size);

// Converts a hex to a short from a starting index and stores the delays
// in - input hex array
// delays - delays array
// size - size in bytes
// start - starting index
void hexToInt(unsigned char *in, short *delays, unsigned char size, int start);

// Sets the delay values
// in - input hex array of values
// delay - chooses the delay array (0 or 1)
// count - current count of inputs
void setDelays(unsigned char *in, char delay, char count);

// Tester method that gives the last 8 delay values (used for response APDU)
// resp - response APDU
void zeroDelays(unsigned char *resp);

// Implements an arbiter PUF
// challenge - the input challenge in binary (binary array)
//
// returns - 0 or 1 depending on PUF output
unsigned char doPUF(unsigned char *challenge);

// LFSR64 - clocks 64 times
// in - input hex array
void LFSR64(unsigned char *in);

// LFSR128 - clocks 64 times
// in - input hex array
void LFSR128(unsigned char *in);

// Alt implementation of arbiter PUF below. Uncomment as needed (slower)
// in - input hex array
// count - current count of inputs
//void setAltDelay(unsigned char *in, unsigned char count);

// transforms and implements alt PUF (above method unnecessary)
// bin - binary input of challenge
// transform - transform array for challenge vector
// size - size in bytes
// 
// returns - 0 or 1 depending on PUF output
//unsigned char transform(unsigned char *bin, signed char *transform, unsigned char size);

#endif


