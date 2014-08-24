/***************************************************************************
 *                                                                         *
 * Arbiter PUF Implementation on a Microcontroller                         *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************/
/*!
    \file        arbiterPUF.c

    \brief       Main functions for the Arbiter PUF implementation

    \author      Eric Li

    \version     1.0

    \date        29-07-2014

*/
/***************************************************************************/

#include "arbiterPUF.h"

// Delay values for PUF implementatoin
short delayZero[bitLength]; 
short delayOne[bitLength];

//short delayVector[bitLength+1]; // n+1 implementation of alt delays

// Converts the input hex to its binary equivalent
void toBinary(unsigned char *in, unsigned char* bin, unsigned char size) {
	unsigned char hex; // temp hex variable

	// for loop that expands hex to binary
	for (unsigned char i = 0; i < size; i++) {
		hex = in[i];
		int idx = i*8;
		// conversion code
		for (int currBit = 0; currBit < 8; currBit++) {
			bin[7-currBit+idx] = hex&1;
			hex>>=1;
		}
	}	
}

// Converts back to hex
//void toHex(unsigned char*in, unsigned char *hex, unsigned char size) {
//	unsigned char bin[8];
//	for (int i =0; i < size; i++) {
//		int index = i*8;
//		// Recovers the binary
//		for (int j = 0; j < 8; j++) {
//			bin[j] = in[index+j];
//		}
//
//		unsigned char idx = 128*bin[0]+64*bin[1]+32*bin[2]+16*bin[3]+8*bin[4]+4*bin[5]+2*bin[6]+bin[7];
//		 
//		hex[i] = idx;
//	}
//}

// Hex to Int
void hexToInt(unsigned char *in, short *delays, unsigned char size, int start) {
	char byte[2]; // scratch array
	for (unsigned char i = 0; i < size; i++) {
			byte[0] = in[i*2];
			byte[1] = in[i*2+1];	
		delays[start+i] = ((byte[0] << 8) | byte[1]);	
	}
}

// Sets the delays array
void setDelays(unsigned char *in, char delay, char count) {	
			if (delay == 0x00) {
				hexToInt(in, delayZero, 16, 16*count);
			}
			else {
			hexToInt(in, delayOne, 16, 16*count);	
			}
}

// Tester method to see that delays are being saved correctly
void zeroDelays(unsigned char *resp) {
	for (int i = 0; i < 8; i++) {
		resp[2*i] =  ((delayZero[i] & 0xFF00) >> 8);
		resp[2*i+1] = (delayZero[i] & 0x00FF);
	}
}

// An arbiter PUF
unsigned char doPUF(unsigned char *challenge) {
	long currDelay = 0; // uses long to prevent overflow
	unsigned char currBit;

	// runs PUF
	for (unsigned char i = 0; i < bitLength; i++) {
	 currBit = challenge[i];

		if (currBit == 0x01) {
			currDelay = -currDelay+delayOne[i];			
		}
		else {
			currDelay = currDelay+delayZero[i];
		}
	}
	
	return (currDelay > 0);
}

// LFSR64
void LFSR64(unsigned char *in) {

	// clocks 64 times
	for (int i = 0; i < 64; i++) {
	    unsigned char bit = ((in[7] >> 0) ^ (in[7] >> 1) ^ (in[7] >> 3) ^ (in[7] >> 4)) & 1;
    
	    in[7] = (in[7] >> 1) | (in[6] << 7);
	    in[6] = (in[6] >> 1) | (in[5] << 7);
	    in[5] = (in[5] >> 1) | (in[4] << 7);
	    in[4] = (in[4] >> 1) | (in[3] << 7);
	    in[3] = (in[3] >> 1) | (in[2] << 7);
	    in[2] = (in[2] >> 1) | (in[1] << 7);
	    in[1] = (in[1] >> 1) | (in[0] << 7);
	    in[0] = (in[0] >> 1) | (bit << 7);
	}
}

// LFSR128
void LFSR128(unsigned char *in) {
    unsigned char bit;

	// clocks 64 times
	for (unsigned char i = 0; i < 64; i++) {
	    bit = ((in[15] >> 0) ^ (in[15] >> 2) ^ (in[12] >> 3) ^ (in[12] >> 5)) & 1;    
     
	    in[15] = (in[15] >> 1) | (in[14] << 7);
	    in[14] = (in[14] >> 1) | (in[13] << 7);
	    in[13] = (in[13] >> 1) | (in[12] << 7);
	    in[12] = (in[12] >> 1) | (in[11] << 7);
	    in[11] = (in[11] >> 1) | (in[10] << 7);
	    in[10] = (in[10] >> 1) | (in[9] << 7);
	    in[9] = (in[9] >> 1) | (in[8] << 7);
	    in[8] = (in[8] >> 1) | (in[7] << 7);
	    in[7] = (in[7] >> 1) | (in[6] << 7);
	    in[6] = (in[6] >> 1) | (in[5] << 7);
	    in[5] = (in[5] >> 1) | (in[4] << 7);
	    in[4] = (in[4] >> 1) | (in[3] << 7);
	    in[3] = (in[3] >> 1) | (in[2] << 7);
	    in[2] = (in[2] >> 1) | (in[1] << 7);
	    in[1] = (in[1] >> 1) | (in[0] << 7);
	    in[0] = (in[0] >> 1) | (bit << 7);
		
	}




	
}

//void setAltDelay(unsigned char *in, unsigned char count) {
//	char byte[2];
//	int startIdx = 16*count;
//	if (count < bitLength/16)
//	{
//	for (int i = 0; i < 16; i++) {
//			byte[0] = in[i*2];
//			byte[1] = in[i*2+1];	
//	//	delayVector[startIdx+i] = ((byte[0] << 8) | byte[1]);	
//	}
//	}
//	else {
//	//	delayVector[bitLength] = ((in[0] << 8) | in[1]);
//	}
//
//}

// transforms the char AND computes the alternate puf at the same time
//unsigned char transform(unsigned char *bin, signed char *transform, unsigned char size) {	
//	transform[bitLength] = 1;
//	long delay = delayVector[bitLength]*transform[bitLength];
//	for (int i = bitLength-1; i >=0; i--) {
//		if (bin[i] == 0x00)
//			transform[i] = transform[i+1];	
//		else
//			transform[i] = -transform[i+1];
//	
//		//delay = delay + (delayVector[i]*transform[i]);
//	}
//
//	return (delay > 0);
//}
