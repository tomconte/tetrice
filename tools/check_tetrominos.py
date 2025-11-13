#!/usr/bin/env python3
"""
Parse tetromino.h and verify SIDE flags are correct
"""
import re

# Parse tetromino.h


def parse_tetromino_header(filename):
    """Parse tetromino definitions from C header file"""
    with open(filename, 'r') as f:
        content = f.read()

    # Extract constants
    side_flags = {}
    for match in re.finditer(r'#define\s+(SIDE_\w+)\s+(0x[0-9A-Fa-f]+)', content):
        side_flags[match.group(1)] = int(match.group(2), 16)

    # Extract tetromino array
    array_match = re.search(
        r'packed_tetromino all_tetrominos\[\]\s*=\s*\{(.*?)\};', content, re.DOTALL)
    if not array_match:
        raise ValueError("Could not find all_tetrominos array")

    array_content = array_match.group(1)

    # Parse each tetromino shape
    shapes = []
    # Find all shapes (enclosed in { })
    shape_pattern = r'\{\s*PACK_BLOCK\([^}]+\)\s*\}'

    for shape_match in re.finditer(shape_pattern, array_content):
        shape_text = shape_match.group(0)
        blocks = []

        # Parse each PACK_BLOCK
        block_pattern = r'PACK_BLOCK\((\d+),\s*(\d+),\s*([^)]+)\)'
        for block_match in re.finditer(block_pattern, shape_text):
            x = int(block_match.group(1))
            y = int(block_match.group(2))
            sides_str = block_match.group(3).strip()

            # Parse sides
            sides = []
            if sides_str == '0':
                pass  # No sides
            else:
                for side in ['SIDE_LEFT', 'SIDE_RIGHT', 'SIDE_BOTTOM']:
                    if side in sides_str:
                        sides.append(side)

            blocks.append((x, y, sides))

        shapes.append(blocks)

    # Parse piece names from comments
    piece_names = []
    name_pattern = r'//\s*(\w+\s+piece)\s*\((\d+)\s+rotations?\)'
    for match in re.finditer(name_pattern, array_content):
        piece_names.append((match.group(1), int(match.group(2))))

    # Parse offsets
    offsets = []
    offset_match = re.search(
        r'uint8_t tetromino_offsets\[\]\s*=\s*\{([^}]+)\}', content)
    if offset_match:
        offsets = [int(x.strip()) for x in offset_match.group(1).split(',')]

    return side_flags, shapes, piece_names, offsets


def check_tetromino(name, blocks):
    """Check if a single tetromino rotation has correct SIDE flags"""
    # Build grid
    grid = {(x, y): sides for x, y, sides in blocks}

    errors = []
    for x, y, sides in blocks:
        # Check SIDE_BOTTOM: should be set if there's no block below
        should_have_bottom = (x, y + 1) not in grid
        has_bottom = 'SIDE_BOTTOM' in sides

        if should_have_bottom and not has_bottom:
            errors.append(f"Block ({x},{y}) is missing SIDE_BOTTOM")
        elif not should_have_bottom and has_bottom:
            errors.append(f"Block ({x},{y}) should NOT have SIDE_BOTTOM")

        # Check SIDE_LEFT: should be set if there's no block to the left
        should_have_left = (x - 1, y) not in grid
        has_left = 'SIDE_LEFT' in sides

        if should_have_left and not has_left:
            errors.append(f"Block ({x},{y}) is missing SIDE_LEFT")
        elif not should_have_left and has_left:
            errors.append(f"Block ({x},{y}) should NOT have SIDE_LEFT")

        # Check SIDE_RIGHT: should be set if there's no block to the right
        should_have_right = (x + 1, y) not in grid
        has_right = 'SIDE_RIGHT' in sides

        if should_have_right and not has_right:
            errors.append(f"Block ({x},{y}) is missing SIDE_RIGHT")
        elif not should_have_right and has_right:
            errors.append(f"Block ({x},{y}) should NOT have SIDE_RIGHT")

    return errors


def visualize_shape(blocks):
    """Create ASCII visualization of a shape"""
    if not blocks:
        return ""

    max_x = max(b[0] for b in blocks)
    max_y = max(b[1] for b in blocks)

    grid = {(x, y): sides for x, y, sides in blocks}
    expected_bottom = {(x, y) for x, y, _ in blocks if (x, y + 1) not in grid}

    lines = []
    for y in range(max_y + 1):
        row = ""
        for x in range(max_x + 1):
            if (x, y) in grid:
                sides = grid[(x, y)]
                has_bottom = 'SIDE_BOTTOM' in sides
                should_have_bottom = (x, y) in expected_bottom

                if has_bottom and not should_have_bottom:
                    marker = "X"  # Has BOTTOM but shouldn't
                elif not has_bottom and should_have_bottom:
                    marker = "!"  # Missing BOTTOM
                else:
                    marker = "#"  # Correct
                row += f" {marker} "
            else:
                row += "   "
        lines.append(row)

    return "\n".join(lines)


def main():
    """Main function"""
    import sys

    filename = 'tetromino.h'
    if len(sys.argv) > 1:
        filename = sys.argv[1]

    print("TETROMINO SIDE FLAG VERIFICATION")
    print("=" * 70)
    print(f"Parsing: {filename}\n")

    try:
        side_flags, shapes, piece_names, offsets = parse_tetromino_header(
            filename)
    except Exception as e:
        print(f"ERROR: Failed to parse file: {e}")
        return 1

    print(f"Found {len(shapes)} tetromino shapes")
    print(f"Found {len(piece_names)} piece types\n")

    print("Legend:")
    print("  # = Correct block")
    print("  ! = Missing SIDE_BOTTOM flag")
    print("  X = Has SIDE_BOTTOM but shouldn't\n")

    total_errors = 0
    shape_idx = 0

    for piece_name, num_rotations in piece_names:
        print("=" * 70)
        print(f"{piece_name.upper()}")
        print("=" * 70)

        for rot in range(num_rotations):
            if shape_idx >= len(shapes):
                print(f"WARNING: Not enough shapes defined for {piece_name}")
                break

            blocks = shapes[shape_idx]
            shape_idx += 1

            print(f"\nRotation {rot}:")
            print(visualize_shape(blocks))

            errors = check_tetromino(piece_name, blocks)

            if errors:
                print(f"\n*** ERRORS FOUND:")
                for error in errors:
                    print(f"    {error}")
                    total_errors += 1
            else:
                print("\n    All blocks OK")

            # Print block details
            print("\n    Block coordinates and sides:")
            for x, y, sides in sorted(blocks, key=lambda b: (b[1], b[0])):
                sides_str = ", ".join(sides) if sides else "none"
                print(f"      ({x},{y}): {sides_str}")

        print()

    print("=" * 70)
    if total_errors > 0:
        print(f"*** {total_errors} ERROR(S) FOUND IN TETROMINO DEFINITIONS! ***")
        return 1
    else:
        print("All tetromino definitions are CORRECT!")
        return 0


if __name__ == "__main__":
    import sys
    sys.exit(main())
