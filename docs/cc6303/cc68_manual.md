## CC6303 User Manual

### Overview
CC6303 is a C compiler for 6800 series processors (6800, 6803, 6303). It is invoked via the command line and accepts a variety of options for controlling compilation, code generation, output, warnings, and more.

---

### Usage

```
cc68 [options] file
```

---

### Short Options

| Option         | Description                                                  |
|----------------|-------------------------------------------------------------|
| `-Cl`          | Make local variables static                                  |
| `-Dsym[=defn]` | Define a symbol (with optional value)                        |
| `-E`           | Stop after the preprocessing stage                           |
| `-I dir`       | Set an include directory search path                         |
| `-O`           | Optimize code                                                |
| `-Oi`          | Optimize code, inline more code                              |
| `-Or`          | Enable register variables                                    |
| `-Os`          | Inline some standard functions                               |
| `-T`           | Include source as comment in output                          |
| `-V`           | Print the compiler version number                            |
| `-W warning,..`| Suppress warnings                                            |
| `-d`           | Debug mode                                                   |
| `-g`           | Add debug info to object file                                |
| `-h`           | Help (prints usage info)                                     |
| `-j`           | Default characters are signed                                |
| `-mm model`    | Set the memory model                                         |
| `-o name`      | Name the output file                                         |
| `-r`           | Enable register variables                                    |
| `-v`           | Increase verbosity                                           |

---

### Long Options

| Option                   | Description                                              |
|--------------------------|---------------------------------------------------------|
| `--add-source`           | Include source as comment                               |
| `--bss-name seg`         | Set the name of the BSS segment                         |
| `--check-stack`          | Generate stack overflow checks                          |
| `--code-name seg`        | Set the name of the CODE segment                        |
| `--codesize x`           | Accept larger code by factor x%                         |
| `--cpu type`             | Set CPU type (`6800`, `6803`, `6303`)                   |
| `--create-dep name`      | Create a make dependency file                           |
| `--create-full-dep name` | Create a full make dependency file                      |
| `--data-name seg`        | Set the name of the DATA segment                        |
| `--debug`                | Debug mode                                              |
| `--dep-target target`    | Use this dependency target                              |
| `--eagerly-inline-funcs` | Eagerly inline some known functions                     |
| `--help`                 | Print help (usage info)                                 |
| `--include-dir dir`      | Set an include directory search path                    |
| `--inline-stdfuncs`      | Inline some standard functions                          |
| `--list-warnings`        | List available warning types for `-W`                   |
| `--local-strings`        | Emit string literals immediately                        |
| `--register-space b`     | Set space available for register variables              |
| `--register-vars`        | Enable register variables                               |
| `--rodata-name seg`      | Set the name of the RODATA segment                      |
| `--signed-chars`         | Default characters are signed                           |
| `--standard std`         | Language standard (`c89`, `c99`, `cc68`)                |
| `--static-locals`        | Make local variables static                             |
| `--verbose`              | Increase verbosity                                      |
| `--version`              | Print compiler version                                  |
| `--writable-strings`     | Make string literals writable                           |

---

### Parameters/Arguments

- **file**: The C source file to compile (required).
- **-o name**: Name the output file (optional).
- **-I dir or --include-dir dir**: Specify include directories.
- **-mm model**: Specify memory model.
- **--cpu type**: Specify target CPU.

---

### Typical Usage Example

```
cc68 -O -mm small --cpu 6303 -I include/ -o output.asm source.c
```

---

### Output

- Generates assembler output for the specified CPU.
- Output file can be named using `-o` or defaults automatically.
- Warnings and errors will be printed to stderr.

---

### Help and Version

- `-h` or `--help`: Show usage help.
- `-V` or `--version`: Print the compiler version.

---

### Advanced Features

- Register variables and space control.
- Segment name customization.
- Code size factor adjustment.
- Dependency file generation for make.
- Debug info and verbose output.

---

### Error Handling

- If no input file is specified, the compiler aborts with an error.
- Unknown or malformed options result in an error message.
- Invalid arguments for options (like segment names or code size) abort compilation.
