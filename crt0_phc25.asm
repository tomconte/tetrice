; Minimal CRT0 for PHC25 Tetris using z88dk
; Based on successful POC implementation

        SECTION code_compiler

        ; Set origin to start of Program Work Area
        ; PHC-25 has Program Work Area at $C000-$DFFF
        org     $C009

        ; Entry point - jump to C main function
start:
        call    _main

        ; Infinite loop after main returns (prevent crash)
halt_loop:
        jr      halt_loop

        ; Export the start symbol
        PUBLIC  start

        ; Import the main function from C code
        EXTERN  _main