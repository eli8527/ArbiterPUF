/***************************************************************************
 *                                                                         *
 * Arbiter PUF Implementation on a Microcontroller                         *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************/
/*!
    \file        functions.c

    \brief       Main functions for the Arbiter PUF Spoofer

    \author      Eric Li

    \version     1.0

    \date        29-07-2014

*/
/***************************************************************************/

#include "functions.h"
#include "arbiterPUF.h"


/*
**  do_PUF will take an input bitstring (64 or 128 bit length) and run a maximal length LFSR
*   		on it and will send the output to the Arbiter PUF method (both versions are implemented here)
*/
void do_PUF (command_APDU * com_APDU, response_APDU * resp_APDU)
{

  /* key must be set when this function is called for the first time */
  unsigned char ind, answerLength;  // Index and answer length
	
	unsigned char size = (*com_APDU).P1; // get byte size (either 8 bytes for 64 bit or 16 bytes for 128 bit)
 	unsigned char response[size]; // Response array

  if ((*com_APDU).LC != size) {  /* if challenge size isn't the given */

    /* Wrong length, send error code */
    (*resp_APDU).LEN = 2;    /* overall length of APDU   */
    (*resp_APDU).LE = 0;    /* user data length of APDU(for internal use only) */
    (*resp_APDU).SW1 = 0x64;  /* SW1 and SW2 of message (error code) */
    (*resp_APDU).SW2 = 0x00;
    return;
  }



  /* ELSE: */
  	
  /*read data  */
  for (ind = 0; ind < size; ind++) {
    response[ind] = (*com_APDU).data_field[ind];
   }
  	
	unsigned char bin[8*size]; // binary array for PUF method
	unsigned char out[size]; // output array
//	signed char transformed[8*size+1]; // used for the alternate PUF implementation in creating challenge vector

	// Initial run through before clocking LFSR

	toBinary(response,bin,size); // Converts input to binary
//	out[0] = transform(bin, transformed, size); // Alternate transforms the challenge and performs alternate PUF
	out[0] = doPUF(bin); // Performs PUF operations

	unsigned char count = 0x01; // resets every 8 for output array
	unsigned char idx = 0x00; // current index in output array


	// Running of LFSR + PUF for n length
	for (unsigned char i = 1; i < bitLength; i++) {
	
	// Compiler commands
	#if bitLength == 64
		LFSR64(response);	// Clocks LFSR 64 times and modifies response array
		
	#elif bitLength == 128

		LFSR128(response); // Clocks LFSR 64 times and modifies response array
	    LFSR128(response); // Clocks LFSR another 64 times and modifies response array. This does not work as it
							// takes the card too long to respond

	#endif

	
	
	toBinary(response,bin,size); // Converts to binary

	//	out[idx] = transform(bin, transformed, size); // Uncomment for alt PUF
		out[idx] = ((out[idx]<<1) | doPUF(bin)); // Left shifts and ORs with the output to save space
		count = count + 0x01;

		// Resets
		if (count >= 0x08) {
			count = 0x00;
			idx++;
			out[idx] = 0x00;
		}
		
	
	}

    answerLength = size;   


 	
//	toHex(out,response,size);

	// copies back over to response
	for (int i = 0; i < size; i++) {
		(*resp_APDU).data_field[i] = out[i];
	}

	  // send Ack
  (*resp_APDU).LEN = answerLength + 2;  /* response length +Ack */
  (*resp_APDU).LE = answerLength;  /* answer length is set by client */
  (*resp_APDU).SW1 = 0x90;    /* Ack: task accomplished */
  (*resp_APDU).SW2 = size;

}



/*
** do_set_Delay sets the delay values
*/
void do_set_Delay (command_APDU * com_APDU, response_APDU * resp_APDU)
{

	// Sets the 0 delay array starting from a given index P2
	if((*com_APDU).P1 == 0x00) {
	
		setDelays((*com_APDU).data_field, 0x00, (*com_APDU).P2);
	} 
	// Sets the 1 delay array starting from a given index P2
	else if ((*com_APDU).P1 == 0x01) {

		setDelays((*com_APDU).data_field, 0x01, (*com_APDU).P2);
	}
	 else {
			return;
	}

	// Sets alternate delay values starting at index P2 (this takes n+1 space)
//	setAltDelay((*com_APDU).data_field, (*com_APDU).P2);


	// Testing method that returns zeros
	unsigned char response[16];
	zeroDelays(response);
/*write data to resp_APDU */
  for (int ind = 0; ind < 16; ind++) {
    (*resp_APDU).data_field[ind] = response[ind];
  }


  /** send Ack */
  (*resp_APDU).LEN = 16+2;
  (*resp_APDU).LE = 16;      /* needed by the sending routine (answer length) */
  (*resp_APDU).SW1 = 0x90;    /* Ack: task accomplished */
  (*resp_APDU).SW2 = 0x00;
}

/***************************************************************************/



/* 
** Main command Handler processing incoming APDUs
*/
void command_Handler (command_APDU * com_APDU, response_APDU * resp_APDU)
{
  (*resp_APDU).NAD = (*com_APDU).NAD;
  (*resp_APDU).PCB = (*com_APDU).PCB;

  if ((*com_APDU).PCB == 0xC1) {  /* S-Block Handling */

    (*resp_APDU).NAD = (*com_APDU).NAD;
    (*resp_APDU).PCB = 0xE1;
    (*resp_APDU).LEN = 1;
    (*resp_APDU).data_field[0] = (*com_APDU).CLA;
  }
  else {            /* I-Block Handling */

  switch ((*com_APDU).CLA) {
    case 0x80: {
      switch ((*com_APDU).INS) {
        case 0x02:
          do_set_Delay (com_APDU, resp_APDU); // sets delays
          break;
        case 0x40:
          do_PUF (com_APDU, resp_APDU); // the key
          break;
        default:
          (*resp_APDU).LEN = 2;
          (*resp_APDU).LE = 0;
          (*resp_APDU).SW1 = 0x68;  /* instruction not supported */
          (*resp_APDU).SW2 = 0x00;
          break;
        }
      break;
      }
    default:
      {
      (*resp_APDU).LEN = 2;
      (*resp_APDU).LE = 0;
      (*resp_APDU).SW1 = 0x6e;  /* class not supported */
      (*resp_APDU).SW2 = 0x00;
      break;
      }
    }
  }
}

/***************************************************************************
 * 6. END                                                                  *
 ***************************************************************************/
