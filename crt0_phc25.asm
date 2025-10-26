; Minimal CRT0 for PHC25 Tetris using z88dk with XOR decoder

        ; Set origin to start of Program Work Area
        org     $C009

; ================================================================
; XOR Decoder Stub - THIS SECTION MUST NOT BE ENCODED!
; ================================================================

        SECTION code_decoder

decoder_start:
        LD      D, XOR_MASK_VALUE       ; Mask in D
        LD      HL, encoded_start       ; Start of encoded region
        LD      BC, ENCODED_LENGTH      ; Length to decode

decode_loop:
        LD      A, B                    ; Check if done
        OR      C
        JR      Z, start                ; Jump to program start when done
        
        LD      A, (HL)                 ; Load byte
        XOR     D                       ; Decode
        LD      (HL), A                 ; Store back
        
        INC     HL                      ; Next byte
        DEC     BC
        JR      decode_loop

; Configuration
XOR_MASK_VALUE  EQU     0x03            ; Set to actual mask after encoding
ENCODED_LENGTH  EQU     0x1EDF          ; Set to actual length after encoding

; ================================================================
; Main program code - THIS GETS ENCODED
; ================================================================

        SECTION code_compiler

encoded_start:                          ; Mark where encoding begins

start:
        call    _main

halt_loop:
        jr      halt_loop

        PUBLIC  start
        EXTERN  _main
