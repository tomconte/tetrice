Certainly! Here is a focused user manual for **as0.c**, the main assembler program in the as68 suite of the EtchedPixels/CC6303 repository:

---

# as68 Assembler (as0.c) — User Manual

## Overview

**as0.c** is the main entry point for the as68 assembler, part of the CC6303 C compiler suite. It assembles source code written for 6800-family processors into object code, handling all the necessary passes and file management.

## Usage

Command line syntax:
```
as [options] source.s
```

- `source.s` — The assembly source file to assemble (required).
- `[options]` — Optional flags to control output and behavior.

## Main Options

- `-o <object.o>`  
  Specify the output object file name.  
  If omitted, the output file name is derived by replacing the source file’s extension with `.o`.

- `-l <listing.lst>`  
  Generate a human-readable listing file.

## How It Works

1. **File Handling:**  
   as0.c opens the specified source file, prepares the object and listing files (if requested), and manages file extensions.

2. **Pass Management:**  
   Performs several passes over the source to resolve symbols, generate code, and optionally produce listings.

3. **Assembly:**  
   Reads each line of the assembly file, processes instructions, directives, and labels, and writes the result to the object file.

4. **Listing File:**  
   If requested, produces a listing with addresses, object bytes, and source lines for debugging/documentation.

## Error Handling

- Prints errors if source, object, or listing files cannot be opened.
- Reports “Out of memory” if resource limits are exceeded.
- Removes incomplete object files if errors prevent successful assembly.

## Output Files

- **Object file:** Contains assembled machine code, suitable for linking or execution.
- **Listing file:** Human-readable file showing addresses, opcodes, and source lines (if `-l` is specified).

## Example Commands

Basic assembly:
```
as hello.s
```

Specify output object and listing files:
```
as -o hello6800.o -l hello.lst hello.s
```

## Notes

- The assembler expects the source file to have a valid extension (usually `.s`).
- The program uses Unix-style command line parsing; more options may be available in future versions.
- Only one source file can be processed per invocation.

## Troubleshooting

- Ensure the input file exists and is readable.
- Check write permissions for output and listing files.
- Verify system memory if encountering “Out of memory” errors.
