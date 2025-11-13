#!/usr/bin/env python3
"""
Encode PHC-25 binary to avoid consecutive zeros.
Usage: python encode_phc25.py tetrice.bin tetrice_encoded.bin
"""

import sys


def find_good_mask(binary_data):
    """Find XOR mask that eliminates consecutive zeros."""
    for mask in range(1, 256):
        has_consecutive_zeros = False
        previous_byte = None

        for byte in binary_data:
            encoded_byte = byte ^ mask

            if encoded_byte == 0x00 and previous_byte == 0x00:
                has_consecutive_zeros = True
                break

            previous_byte = encoded_byte

        if not has_consecutive_zeros:
            return mask

    return None


def encode_phc25_binary(input_file, output_file):
    """
    Encode PHC-25 binary, skipping the decoder stub at the start.
    """
    # Read original binary
    with open(input_file, 'rb') as f:
        data = bytearray(f.read())

    print(f"Original file size: {len(data)} bytes")

    # The decoder stub is about 20-25 bytes
    # We'll skip the first bytes
    DECODER_SIZE = 20

    if len(data) <= DECODER_SIZE:
        print("ERROR: File too small!")
        return False

    # Split into decoder (unencoded) and rest (to encode)
    decoder_stub = data[:DECODER_SIZE]
    body_to_encode = data[DECODER_SIZE:]

    print(f"Decoder stub: {DECODER_SIZE} bytes (not encoded)")
    print(f"Body to encode: {len(body_to_encode)} bytes")

    # Find suitable mask
    mask = find_good_mask(body_to_encode)

    if mask is None:
        print("ERROR: Could not find suitable XOR mask!")
        return False

    print(f"Found mask: 0x{mask:02X} ({mask})")

    # Encode the body
    encoded_body = bytearray(byte ^ mask for byte in body_to_encode)

    # Verify no consecutive zeros
    for i in range(len(encoded_body) - 1):
        if encoded_body[i] == 0x00 and encoded_body[i+1] == 0x00:
            print("ERROR: Still has consecutive zeros!")
            return False

    # Write output: decoder + encoded body
    with open(output_file, 'wb') as f:
        f.write(decoder_stub)
        f.write(encoded_body)

    print(f"Success! Wrote {output_file}")
    print(f"\n*** IMPORTANT ***")
    print(f"Update crt0_phc25.asm:")
    print(f"    XOR_MASK_VALUE  EQU     0x{mask:02X}")
    print(f"Then recompile your program.")

    return True


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python encode_phc25.py input.bin output.bin")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    if encode_phc25_binary(input_file, output_file):
        sys.exit(0)
    else:
        sys.exit(1)
