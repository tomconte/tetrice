#!/usr/bin/env python3
"""Inspect PNG pixel values to understand the format."""

from PIL import Image
import sys

if len(sys.argv) < 2:
    print("Usage: python inspect_png.py <input.png>")
    sys.exit(1)

img = Image.open(sys.argv[1])
print(f"Mode: {img.mode}")
print(f"Size: {img.size}")
print(f"\nSample pixels from first block (0-7, 0-7):")

for y in range(8):
    row = []
    for x in range(8):
        pixel = img.getpixel((x, y))
        row.append(str(pixel))
    print(f"Row {y}: {', '.join(row)}")
