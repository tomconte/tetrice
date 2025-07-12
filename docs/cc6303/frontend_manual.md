# cc68 User Manual

## NAME

`cc68` – C compiler driver for the 6800 series processors (6800, 6803, 6303), targeting Fuzix, FLEX, MC-10, and other systems.

---

## SYNOPSIS

```
cc68 [options] file...
```

---

## DESCRIPTION

`cc68` coordinates the compilation, assembly, and linking of C programs for the Motorola 6800 family of CPUs. It processes input files through several stages (preprocessing, compilation, assembly, linking), automatically handling temporary and output files unless otherwise specified.

Typical input files:
- `.c`   — C source file
- `.S`   — Assembly source file with preprocessor
- `.s`   — Assembly source file
- `.o`   — Object file
- `.a`   — Library archive

Output is, by default, `a.out` unless overridden with `-o`.

---

## STAGES

1. **Preprocessing** (`-E`): Convert `.c` or `.S` to preprocessed source.
2. **Compilation** (`-S`): Compile `.c` to assembly (`.s`).
3. **Assembly** (`-c`): Assemble `.s` to object (`.o`).
4. **Linking** (default): Link all `.o` and `.a` files to produce final binary.

---

## OPTIONS

### General
- `-c`  
  Compile or assemble, but do not link. Produces `.o` files.

- `-S`  
  Compile, but do not assemble or link. Produces `.s` files.

- `-E`  
  Preprocess only. Output to stdout or temporary file. Only one input file permitted.

- `-o <file>`  
  Set output filename (default: `a.out`).

- `-X`  
  Keep temporary files (do not delete intermediates).

- `-s`  
  Build as standalone binary (omit startup code and libraries).

- `-M`  
  Generate a map file alongside the output binary.

### Target CPU and OS
- `-m<cpu>`  
  Select CPU: `6800`, `6803`, or `6303`.  
  Example: `-m6303`

- `-t<type>`  
  Set target system:
    - `fuzix` / `fuzixrel1` / `fuzixrel2` (Fuzix variants)
    - `mc10`   (Tandy MC-10)
    - `flex`   (FLEX OS)

### Libraries and Includes
- `-l<name>`  
  Link against library `lib<name>.a`. Can be specified multiple times.

- `-L<path>`  
  Add library search path.

- `-I<path>`  
  Add include file search path.

### Preprocessor
- `-D<macro>`  
  Define macro for preprocessor.

### Other
- `--<option>`  
  Pass long-form options to the compiler (see below for supported options).

---

## LONG OPTIONS

Long options are passed using `--option` syntax. Some options require an argument:

| Option                  | Argument Required | Description                    |
|-------------------------|------------------|--------------------------------|
| bss-name                | Yes              | Name for BSS section           |
| check-stack             | No               | Enable stack checking          |
| code-name               | Yes              | Name for code section          |
| data-name               | Yes              | Name for data section          |
| debug                   | No               | Enable debug information       |
| inline-stdfuncs         | No               | Inline standard functions      |
| register-space          | Yes              | Register space size            |
| register-vars           | No               | Register variables             |
| rodata-name             | Yes              | Name for read-only data        |
| signed-char             | No               | Use signed chars               |
| standard                | Yes              | Specify language standard      |
| verbose                 | No               | Verbose output                 |
| writable-strings        | No               | Writable string literals       |

Example:
```
cc68 --debug --standard c89 file.c
```

---

## FILE TYPES

- `.c`   — C source (preprocessed and compiled)
- `.S`   — Assembly source with preprocessor (preprocessed, then assembled)
- `.s`   — Assembly source (direct to assembler)
- `.o`   — Object file (linked)
- `.a`   — Archive/library (linked)

---

## EXAMPLES

### Compile and link a C program for the 6303 CPU targeting FLEX

```
cc68 -m6303 -tflex -o myprog file.c
```

### Compile only, do not link

```
cc68 -c file.c
```

### Preprocess only

```
cc68 -E file.c
```

### Link with additional libraries and include paths

```
cc68 -Iinclude/ -Llibs/ -lfoo -lbar file.c
```

---

## ENVIRONMENT

The program expects binaries and libraries in configured `BINPATH`, `LIBPATH`, and `INCPATH`. These are set at compile-time.

---

## EXIT STATUS

- `0` — Success
- Non-zero — Failure (error messages printed to stderr)

---

## SEE ALSO

- `as68`, `cc68`, `copt`, `ld68` — toolchain components

---

## AUTHOR

EtchedPixels (GitHub: [EtchedPixels/CC6303](https://github.com/EtchedPixels/CC6303))

---

## NOTES

- Only one input file permitted with `-E`.
- Temporary files are removed unless `-X` is specified.
- Map file generated with `-M`.
- For supported target systems and CPUs, see the options above.
- Warnings are issued for unused files.

---

## BUGS

- Some options are not fully implemented (see TODOs in source).
- Platform-specific behavior may change.
