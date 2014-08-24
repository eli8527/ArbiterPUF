/***************************************************************************
 *                                                                         *
 * Arbiter PUF Implementation on a Microcontroller                         *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************/
/*!
    \file        functions.h

    \brief       Main functions for the Arbiter PUF Spoofer

    \author      Eric Li

    \version     1.0

    \date        29-07-2014

*/
/***************************************************************************/

#ifndef OS_functions
#define OS_functions
#include "global.h"


/** do_PUF will take an input bitstring (64 or 128 bit length) and run a maximal length LFSR
   		on it and will send the output to the Arbiter PUF method (both versions are implemented here

 \param[in] com_APDU pointer to received command APDU containing the plaintext and the expected answer length
 \param[out] resp_APDU pointer to new response APDU to which the expected number of ciphertext bytes is written
 */
void do_PUF(
    command_APDU * com_APDU,
    response_APDU * resp_APDU);

/** sets the delay values

 \param[in] com_APDU pointer to received command APDU containing the key
 \param[out] resp_APDU pointer to new response APDU which only consists of an error code (trailer)
 */
void do_set_Delay(
    command_APDU * com_APDU,
    response_APDU * resp_APDU);


/** Internal OS routine for calling the implemented functions

 The command handler checks the class (CLA) byte and finds and calls the corresponding function
 for the instruction (INS) byte. If one of the parameters is wrong or not set, it returns an APDU
 with the appropiate error code

 \param[in] com_APDU pointer to received command APDU to be processed
 \param[out] resp_APDU pointer to response APDU with processed data or appropiate error code
 */
void command_Handler(
    command_APDU * com_APDU,
    response_APDU * resp_APDU);

#endif
