import java.io.*;
import java.util.List;

import javax.smartcardio.Card;
import javax.smartcardio.CardChannel;
import javax.smartcardio.CardTerminal;
import javax.smartcardio.CommandAPDU;
import javax.smartcardio.ResponseAPDU;
import javax.smartcardio.TerminalFactory;

/**
 * This class is a client side application to send challenges to a simulated
 * arbiter PUF on a Funcard ATMEGA163. The messages come in the form of a text
 * file, separated by new lines for each challenge. After converting the input
 * into hex and padding it with the standard APDU padding, it is sent to the
 * Smart Card using built in libraries. The response is then output to a
 * specified output file. Requires BitZip.java.
 * 
 * This is for the alternate input for the arbiter PUF using a delay vector
 * 
 * @author Eric Li
 * @version 1.0
 * 
 */
public class CardComAlt {

	private static int SIZE = 128 ; // Challenge size, in bits
	private static CardChannel channel; // Channel object
	private static Card CardObj; // the card

	/**
	 * Converts a given string to byte array and inserts it into the APDU call
	 * 
	 * @param array
	 *            APDU array
	 * @param in
	 *            Input binary string
	 */
	private static void toByteArray(byte[] array, String in) {
		int start = 5;
		byte[] bval = BitZip.zip(in);
		reverse(bval);

		// Pad if not of size/8
		if (bval.length < SIZE / 8) {
			int extra = SIZE / 8 - bval.length;
			byte[] tmp = new byte[SIZE / 8];

			for (int i = 0; i < SIZE / 8; i++) {
				if (i < extra)
					tmp[i] = 0;
				else
					tmp[i] = bval[i - extra];
			}
			bval = tmp;
		}
//		 System.out.print("Hex value: ");
//		 for (byte b : bval) {
//		 System.out.printf("%02x", b);
//		 }
//		 System.out.println();
		for (int i = 0; i < SIZE / 8; i++) {
			array[start + i] = bval[i];
		}

//		 for (byte b : array) {
//		 System.out.printf("%02x", b);
//		 }
//		 System.out.println();
	}

	/**
	 * Reverses a given byte array
	 * 
	 * @param in
	 *            reverses the array given
	 */
	private static void reverse(byte[] in) {
		int left = 0;
		int right = in.length - 1;

		while (left < right) {
			// swap the values at the left and right indices
			byte temp = in[left];
			in[left] = in[right];
			in[right] = temp;

			// move the left and right index pointers in toward the center
			left++;
			right--;
		}
	}

	/**
	 * Creates a connection with the smart card
	 */
	private static void createConnection() {
		try {
			// Get terminal Factory
			TerminalFactory TermFacObj = javax.smartcardio.TerminalFactory
					.getDefault();
			// Get a list of available readers
			List<CardTerminal> CardTermList = TermFacObj.terminals().list();

			// Choose first reader
			CardTerminal CardTermObj = (CardTerminal) CardTermList.get(0);
			// Establish connection with card (T=1 Protocol)
			CardObj = CardTermObj.connect("T=1");
			// Connect to card
			channel = CardObj.getBasicChannel();

		} catch (Exception ex) {
			System.out.println("Exception : " + ex);

		}
	}

	/**
	 * Closes the connection with the card
	 */
	private static void closeConnection() {
		try {
			// Disconnect
			CardObj.disconnect(false);
		} catch (Exception ex) {
			System.out.println("Exception : " + ex);

		}
	}

	/**
	 * Send receive protocol with smart card
	 * 
	 * @param arr
	 *            APDU formatted byte array
	 * @return Response from card
	 */
	private static byte[] sendReceive(byte[] arr) {
		try {
			// Transmit APDU+ get Response APDU
			ResponseAPDU r = channel.transmit(new CommandAPDU(arr));

			// Build response
			return r.getData();
		} catch (Exception ex) {
			System.out.println("Exception : " + ex);

		}
		return null;

	}
	
	/**
	 * Sends delays to a card and receives the response APDU as a byte array
	 * @param arr APDU input
	 * @return response APDU
	 */
	private static byte[] sendReceiveDelays(byte[] arr) {
		try {
			// Transmit APDU+ get Response APDU
			ResponseAPDU r = channel.transmit(new CommandAPDU(arr));

			// Build response
			return r.getBytes();
		} catch (Exception ex) {
			System.out.println("Exception : " + ex);

		}
		return null;	
		
	}

	/**
	 * Sends and receives challenge and response pairs from a given input file
	 * 
	 * @param fileName
	 *            Filename. Output appends -output.txt to the input
	 */
	private static void sendChallenges(String fileName) {
		// Reads challenges
		try {
			BufferedReader reader = new BufferedReader(new FileReader(fileName));
			BufferedWriter writer = new BufferedWriter(new FileWriter(fileName
					+ "-output.alt.txt"));

			String input = "";
			// Creates the skeleton byte array
			int byteSize = SIZE / 8;

			byte[] c1 = new byte[6 + byteSize];
			c1[0] = (byte) 0x80; // Command
			c1[1] = (byte) 0x40; // Instruction

			// P1 - challenge size
			c1[2] = (byte) (byte) new Integer(byteSize).byteValue();
			c1[3] = (byte) 0x00; // P2
			// Number of the following input bytes
			c1[4] = (byte) new Integer(byteSize).byteValue();

			// Expected response
			//c1[c1.length - 1] = (byte) new Integer(byteSize).byteValue();
			c1[c1.length - 1] = 0x01;
			
			input = reader.readLine();

			// Reads each challenge
			while (input != null) {
		//		System.out.println("Challenge: " + input);
				if (input.length() != SIZE)
					throw new IllegalArgumentException(
							"Invalid challenge size!");
				toByteArray(c1, input);
				// System.out.println(c1);
				byte[] rawResponse = sendReceive(c1);

				reverse(rawResponse);
				//System.out.print("Response: ");
//				 for (int i = 0; i < rawResponse.length; i++) {
//				 System.out.printf("%02x", rawResponse[i]);
//				 }
//				 System.out.println("\n");

				String response = BitZip.unzip(rawResponse);

				// Padding
				if (response.length() < SIZE) {
					int extra = SIZE - response.length();

					char padding[] = new char[extra];

					for (int i = 0; i < extra; i++) {
						padding[i] = '0';
					}

					StringBuilder sb = new StringBuilder();
					sb.append(padding);
					sb.append(response);
					response = sb.toString();
				}
		//		System.out.println("Response:  " + response);
			//	System.out.println();

				// Write to output
				writer.write(response);
				writer.newLine();

				// Next challenge
				input = reader.readLine();
				
				
			}
			
			writer.close();
			reader.close();
			
		} catch (IOException e) {
			System.out.println(e);
		}

	}

	/**
	 * Sends the list of delays to the microcontroller
	 * 
	 * @param delayFile
	 *            Filename of delays
	 */
	private static void sendDelays(String delayFile) {
		// Reads challenges
		try {
			BufferedReader reader = new BufferedReader(
					new FileReader(delayFile));

			// Creates the skeleton byte array
			
			int idx = 5;
			byte[] c0 = new byte[5 + 32]; // One Delay value APDU Call

			c0[0] = (byte) 0x80;
			c0[1] = (byte) 0x02;
			// P1 - challenge size
			c0[2] = (byte) 0x00; // zero delay
			// P2 count
			c0[3] = (byte) 0x00;
			// Number of the following input bytes
			c0[4] = (byte) new Integer(32).byteValue();
			// Expected response
			c0[c0.length - 1] = 0x02;
			// Reads file
			String input = reader.readLine();
			
			while (input != null) {
				short delayVal = Short.parseShort(input);

				// Shorts to bytes
				byte[] tmp = shortToByteArray(delayVal);

				for (int i = 0; i < 2; i++) {
					c0[idx + i] = tmp[i];
				}

				// Next pair
				input = reader.readLine();
				idx += 2;

				if (idx >= 36) {
					idx = 5;
					
//					for (byte b : c0) {
//						System.out.printf("%02x", b);
//					}
//					System.out.println();
//					
					
					byte[] send0 = sendReceiveDelays(c0);
					
//					for (byte b : c1) {
//						System.out.printf("%02x", b);
//					}
//					System.out.println();

					
					c0[3]++;
//					System.out.println(send0.length + " " + send1.length);
//											
//						System.out.printf("%02x %02x", send0[0],send0[1]);
//						System.out.print("\t");
//						System.out.printf("%02x %02x", send1[0],send1[1]);
//
//					
//					System.out.println();


				}
				// n+1
				if (c0[3] >=  SIZE/16) {
					sendReceiveDelays(c0);
				}
			}
			reader.close();

			System.out.println("Delay Transmission Completed!");
		} catch (IOException e) {
			System.out.println(e);
		}
	}

	/**
	 * Converts a short to a two byte array
	 * @param s Short
	 * @return Two byte hexadecimal representation of the short
	 */
	private static byte[] shortToByteArray(short s) {
		return new byte[] { (byte) ((s & 0xFF00) >> 8), (byte) (s & 0x00FF) };
	}

	/**
	 * Communicates with the card
	 * 
	 * @param args
	 *            0 - delay timings; 1 - input file name
	 */
	public static void main(String[] args) {
		String delayList = args[0]; // Delay timings
		String fileName = args[1]; // Input file name
		createConnection();
		sendDelays(delayList);
		System.out.println("Sending Challenges...");
		long start = System.currentTimeMillis(); 
		sendChallenges(fileName);
		long end = System.currentTimeMillis();
		closeConnection();
		System.out.println("Done!\n"
				+ "Elapsed time: " + (end-start));

	}
}
