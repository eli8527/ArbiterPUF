/**
 * Zips (compresses) bit strings to byte arrays and unzips (decompresses)
 * byte arrays to bit strings.
 *
 * @author ryan
 *
 */
public class BitZip {

  private static final byte[] BIT_MASKS = new byte[] {1, 2, 4, 8, 16, 32, 64, -128};
  private static final int BITS_PER_BYTE = 8;
  private static final int MAX_BIT_INDEX_IN_BYTE = BITS_PER_BYTE - 1;

  /**
   * Decompress the specified byte array to a string.
   * <p>
   * This function does not pad with zeros for any bit-string result
   * with a length indivisible by 8.
   *
   * @param bytes The bytes to convert into a string of bits, with byte[0]
   *              consisting of the least significant bits in the byte array.
   * @return The string of bits representing the byte array.
   */
  public static final String unzip(final byte[] bytes) {
    int byteCount = bytes.length;
    int bitCount = byteCount * BITS_PER_BYTE;

    char[] bits = new char[bitCount];
    {
      int bytesIndex = 0;
      int iLeft = Math.max(bitCount - BITS_PER_BYTE, 0);
      while (bytesIndex < byteCount) {
        byte value = bytes[bytesIndex];
        for (int b = MAX_BIT_INDEX_IN_BYTE; b >= 0; --b) {
          bits[iLeft + b] = ((value % 2) == 0 ? '0' : '1');
          value >>= 1;
        }
        iLeft = Math.max(iLeft - BITS_PER_BYTE, 0);
        ++bytesIndex;
      }
    }
    return new String(bits).replaceFirst("^0+(?!$)", "");
  }

  /**
   * Compresses the specified bit string to a byte array, ignoring trailing
   * zeros past the most significant set bit.
   *
   * @param bits The string of bits (composed strictly of '0' and '1' characters)
   *             to convert into an array of bytes.
   * @return The bits, as a byte array with byte[0] containing the least
   *         significant bits.
   */
  public static final byte[] zip(final String bits) {
    if ((bits == null) || bits.isEmpty()) {
      // No observations -- return nothing.
      return new byte[0];
    }
    char[] bitChars = bits.toCharArray();

    int bitCount = bitChars.length;
    int left;

    for (left = 0; left < bitCount; ++left) {
      // Ignore leading zeros.
      if (bitChars[left] == '1') {
        break;
      }
    }
    if (bitCount == left) {
      // Only '0's in the string.
      return new byte[] {0};
    }
    int cBits = bitCount - left;
    byte[] bytes = new byte[((cBits) / BITS_PER_BYTE) + (((cBits % BITS_PER_BYTE) > 0) ? 1 : 0)];
    {
      int iRight = bitCount - 1;
      int iLeft = Math.max(bitCount - BITS_PER_BYTE, left);
      int bytesIndex = 0;
      byte _byte = 0;

      while (bytesIndex < bytes.length) {
        while (iLeft <= iRight) {
          if (bitChars[iLeft] == '1') {
            _byte |= BIT_MASKS[iRight - iLeft];
          }
          ++iLeft;
        }
        bytes[bytesIndex++] = _byte;
        iRight = Math.max(iRight - BITS_PER_BYTE, left);
        iLeft = Math.max((1 + iRight) - BITS_PER_BYTE, left);
        _byte = 0;
      }
    }
    return bytes;
  }
}