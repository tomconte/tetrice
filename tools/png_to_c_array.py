#!/usr/bin/env python3
"""
Convert PNG image to C byte array for monochrome Mode 12 graphics.
Each byte represents 8 horizontal pixels (MSB = leftmost pixel).
Optional RLE compression to avoid long runs of identical bytes.
"""

from PIL import Image
import sys
import os
import argparse


def pixel_is_set(pixel):
    """Determine if a pixel should be considered 'on' (1) or 'off' (0)."""
    # Handle both RGB and RGBA formats
    if isinstance(pixel, (tuple, list)):
        # Use simple threshold on red channel (assuming grayscale-ish)
        # Pixel is 'on' if it's bright enough
        return pixel[0] > 128
    else:
        # Single value (grayscale or palette index)
        # For palette mode, non-zero values are 'on'
        return pixel != 0


def convert_png_to_bytes(img):
    """Convert entire PNG image to byte array."""
    width, height = img.size

    # Validate width is multiple of 8
    if width % 8 != 0:
        raise ValueError(f"Image width ({width}) must be multiple of 8")

    bytes_per_row = width // 8
    data = []

    for y in range(height):
        for byte_x in range(bytes_per_row):
            byte_value = 0
            for bit_x in range(8):
                pixel_x = byte_x * 8 + bit_x
                pixel = img.getpixel((pixel_x, y))
                if pixel_is_set(pixel):
                    # Set bit (MSB is leftmost pixel)
                    bit_pos = 7 - bit_x
                    byte_value |= (1 << bit_pos)
            data.append(byte_value)

    return data


def compress_rle(data, escape_byte=0x01, min_run=4):
    """
    Simple RLE compression using escape byte.
    Format: escape_byte <count> <value> = repeat <value> <count> times
    Only compresses runs of min_run or more identical bytes.
    Literal escape_byte encoded as: escape_byte 0x01 escape_byte
    """
    if not data:
        return []

    compressed = []
    i = 0

    while i < len(data):
        current_byte = data[i]
        run_length = 1

        # Count consecutive identical bytes
        while i + run_length < len(data) and data[i + run_length] == current_byte:
            run_length += 1
            if run_length >= 255:  # Max run length (uint8_t)
                break

        # Decide whether to compress this run
        if run_length >= min_run:
            # Compress the run
            compressed.append(escape_byte)
            compressed.append(run_length)
            compressed.append(current_byte)
            i += run_length
        elif current_byte == escape_byte:
            # Literal escape byte: encode as escape_byte 0x01 escape_byte
            compressed.append(escape_byte)
            compressed.append(0x01)
            compressed.append(escape_byte)
            i += 1
        else:
            # Pass through as-is
            compressed.append(current_byte)
            i += 1

    return compressed


def generate_header_file(data, width, height, var_name, input_filename, is_compressed=False, uncompressed_size=None):
    """Generate C header file content."""
    guard_name = f"{var_name.upper()}_H"
    bytes_per_row = width // 8
    total_bytes = len(data)

    lines = []
    lines.append(
        f"/* Generated from {os.path.basename(input_filename)} ({width}x{height}) */")
    if is_compressed:
        lines.append(
            f"/* RLE compressed: {total_bytes} bytes (was {uncompressed_size} bytes, {100*total_bytes//uncompressed_size}% of original) */")
    else:
        lines.append(
            f"/* Total size: {total_bytes} bytes ({bytes_per_row} bytes per row) */")
    lines.append("")
    lines.append(f"#ifndef {guard_name}")
    lines.append(f"#define {guard_name}")
    lines.append("")
    lines.append("#include <stdint.h>")
    lines.append("")
    lines.append(f"#define {var_name.upper()}_WIDTH {width}")
    lines.append(f"#define {var_name.upper()}_HEIGHT {height}")
    lines.append(f"#define {var_name.upper()}_BYTES_PER_ROW {bytes_per_row}")
    lines.append(f"#define {var_name.upper()}_SIZE {total_bytes}")
    if is_compressed:
        lines.append(
            f"#define {var_name.upper()}_UNCOMPRESSED_SIZE {uncompressed_size}")
    lines.append("")
    lines.append(f"const uint8_t {var_name}[{total_bytes}] = {{")

    # Format data as hex bytes, 16 per line
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        hex_bytes = ', '.join(f'0x{b:02X}' for b in chunk)
        if i + 16 < len(data):
            lines.append(f"    {hex_bytes},")
        else:
            lines.append(f"    {hex_bytes}")

    lines.append("};")
    lines.append("")
    lines.append(f"#endif /* {guard_name} */")
    lines.append("")

    return '\n'.join(lines)


def main():
    parser = argparse.ArgumentParser(
        description='Convert PNG to C byte array with optional RLE compression')
    parser.add_argument('input_png', help='Input PNG file')
    parser.add_argument('output_h', help='Output C header file')
    parser.add_argument('var_name', nargs='?',
                        help='Variable name (default: derived from output filename)')
    parser.add_argument('--rle', action='store_true',
                        help='Enable RLE compression')
    parser.add_argument('--escape-byte', type=lambda x: int(x, 0), default=0x01,
                        help='RLE escape byte (default: 0x01)')
    parser.add_argument('--min-run', type=int, default=4,
                        help='Minimum run length to compress (default: 4)')

    args = parser.parse_args()

    # Auto-generate variable name from output filename if not provided
    if args.var_name:
        var_name = args.var_name
    else:
        var_name = os.path.splitext(os.path.basename(args.output_h))[0]

    print(f"Converting {args.input_png} to {args.output_h}...")

    # Load the PNG
    img = Image.open(args.input_png)
    width, height = img.size
    print(f"Image size: {width}x{height}")

    # Convert to byte array
    data = convert_png_to_bytes(img)
    uncompressed_size = len(data)
    print(f"Generated {uncompressed_size} bytes")

    # Apply RLE compression if requested
    is_compressed = False
    if args.rle:
        compressed_data = compress_rle(
            data, escape_byte=args.escape_byte, min_run=args.min_run)
        print(
            f"RLE compressed: {len(compressed_data)} bytes ({100*len(compressed_data)//uncompressed_size}% of original)")
        data = compressed_data
        is_compressed = True

    # Generate header file
    header_content = generate_header_file(data, width, height, var_name, args.input_png,
                                          is_compressed=is_compressed,
                                          uncompressed_size=uncompressed_size if is_compressed else None)

    # Write to file
    with open(args.output_h, 'w') as f:
        f.write(header_content)

    print(f"Successfully wrote {args.output_h}")


if __name__ == '__main__':
    main()
