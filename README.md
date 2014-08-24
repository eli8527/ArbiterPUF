Arbiter PUF Implementation on an ATMEGA163 Funcard
==========

#### README
This repository contains all the files you will need to simulate either a 64 bit or 128 bit Arbiter PUF on an ATMEGA163 Funcard. The 'Client files' folder includes the Java classes necessary for communicating with the smartcard and is intended to be run from a computer. It also includes many test cases and reference solutions, though the ones that will run correctly with the current code are located in `Client files/64/pseudorandom`. The `Matlab files` folder includes the MATLAB code needed to generate more delay values, challenge bits, and reference solutions. Finally, the `Smartcard files` folder contains all necessary C files to generate a .hex file that can be flashed to the smartcard.
	
For a step-by-step guide on how to get this running, as well as how to generate more data, see below. Most questions should be answered within this document.

In addition, the paper that summarizes the theory and execution behind this work is included as `ArbiterPUF Paper.pdf`.
	
#### CONTACT
If you have any questions, problems, or thoughts about this implementation, please contact me at either of the following email address:
>Eric Li
>ericyoungli@gmail.com

#### REQUIREMENTS

In order to use these files, the following programs (or similar) must be installed on the computer:
- MATLAB 
- Java Development Environment
- Eclipse (or similar Java IDE)
- AVR Studio 4 (or similar C IDE)
- CAS Interface Studio
- Windows Powershell
	
In addition, the following hardware components will be needed:
	
- ATMEGA163 Funcard
- Smartcard Reader
- Smartcard Programmer (Dynamite Plus Smartcard Programmer from Duolabs)
- USB A Cable
		
#### USAGE

Once the above programs have be installed, importing the files into the programs should be relatively straightforward and therefore not included in this Readme. However, I will document the typical usage of these codes in the following pipeline.
	
1. Generate delay values, challenge bits, and challenge solutions in MATLAB and save to a .txt file
2. Run `java CardCom delayVectors.txt challengeInput.txt` in command prompt
3. Compare results (written to `challengeInput.txt-output`) in Notepad++ or similar and timing
	
For specifics, see below.

#### DATA GENERATION

This section details the specifics in the MATLAB code. The MATLAB code contains two files:`XOR_Arbiter_PUF_Simulation.m` and `LFSR.m`. You don't have to worry about LFSR.m too much as that is just the LFSR code. The main functions are all contained within the former document.
	
Though this script was written by Georg Becker, there were several modifications made in order to suite the nature of this project. Several key sections will be pointed out here:
	
- Lines 7-9: These are the main parameters of the script. In general, `numOfStages` should either be 128 or 64, `numOfXors` should always be 1, and `numOfInputs` can be whatever you want it to be. Keep in mind that the more inputs generated, the longer execution time will take.

- Lines 14-23: LFSR functions

- Lines 58-76: Delay value calculation. Uncomment lines 61 and 63 in order to generate new delay values for a specified bitlength and comment out lines 64 and 75. In general, I usually generate new delay values once when changing bitlengths and then leave the values constant so that all generated input files have the same delay values (this is more convenient). To do this, I do the following:
		
	1. Uncomment lines 61 and 63, and comment out lines 64 and 75.
	2. Run once.
	3. Perform the following commands in the console:
		`save('delayArray,'delayArray')`				`save('delayVector','delayVector')`
	4. Uncomment 64 and 75 whilst commenting out 61 and 63.
	5. View `delayVector` and `delayArray` in MATLAB and copy each to its own .txt file
	6. For `delayVector`, replace all `\t` with `\n`

- Lines 87-114: Standard implementation of Arbiter PUF

- Lines 122-139: Alternate Implementation of Arbiter PUF

- Lines 147-148: This saves the challenge bits and the output array to the current folder. These files can then be modified using Notepad++ or a similar program in order to suit the input for the Java client. To do so, follow the following steps:
		
	**`challenges.csv`**
	
	1. Remove all `,`
	2. Rename to `challengeN.txt`
				
	**`reference.csv`**
	
	1. Remove all `,`			
	2. Rename to `referenceN.txt`
	3. Run the following command in Powershell:
	``(gc referenceN.txt) -replace ".{l}" , "$&`r`n" |sc referenceN.txt``			
	
	Replacing N with the input size and l the bitlength (64 or 128)
				
#### CLIENT OPERATION

The Java code is relatively straight forward. There are two main Java files and one helper file. `BitZip.java` contains functions for converting from binary to hexadecimal and vice versa. `CardCom.java` is the main file for the Arbiter PUF implementation whereas `CardComAlt.java` is the main file for the alternate implementation of the Arbiter PUF. The difference is in the delay value input.

The only variable that needs to be changed is the `SIZE` variable on line 25 of `CardCom.java` and line 27 of `CardComAlt.java` depending on if the bitlength is 64 or 128. Keep in mind that it is advisable to stick with the standard implementation of the PUF as it is faster. 

Nevertheless, documentation for both is included here.
	
- To compile run:	
	`javac CardCom.java`
	`javac CardComAlt.java`
		
- To run:	
	`java CardCom delayArray.txt challengeInput.txt`
	`java CardComAlt delayVector.txt challengeInput.txt`
	
Timing will be included in the command prompt output in ms whereas the response from the smartcard will be stored in `challengeInput.txt-output`.
	
Make sure that when running, the smartcard is plugged into the smartcard reader and not the writer and that it is running software compiled for the correct bitlength.

#### PROGRAMMER

To program the card, simply open 'CAS Interface Studio' and connect the programmer without the card. Once it loads up, click 'Smart Card' then 'Card Programmer' and insert the card. 

Make sure to select 'FunCard ATmega163' and then load the correct .hex file in the 'Flash memory' input field. Finally, click 'Write' on the bottom left and wait until the popup button says 'Done'.

#### SMARTCARD OS

The code for the smartcard OS was originally taken from the Smartcard Practicum class at Ruhr Universitat Bochum and adapted to suit the project. The code is very well commented and should be straight forward. The key files to pay attention to are `functions.c` which contains the main function and `arbiterPUF.h/arbiterPUF.c` which contains all the Arbiter PUF functionality.

To get going, just create a new project in AVR Studio and import the files. Compile and program.
	
To use the alternate PUF implementation, comment all methods and variables needed for the standard PUF and uncomment all methods and variables needed for the anternate PUF, making sure to only uncomment the very left `//`. Note that the pseudorandom LFSR has only been verified to be working with the standard implementation and untested with the alternate implementation.
	
To change the bitlength, go to `arbiterPUF.h` and change the value of `bitLength` on line 25.

#### KNOWN ISSUES & DIRECTIONS

Pseudorandom LFSR computation will not work for bit length challenges of size 128. This is due to a high latency in the APDU protocol. Look into ways of optimizing this such as writing Assembly.
	
