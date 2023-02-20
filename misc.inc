;********************************************************************************
; misc.inc: Contains miscellaneous functions and macros.
;********************************************************************************
.IFNDEF MISC_INC_
.EQU MISC_INC_ = 0

;********************************************************************************
; Macro definitions:
;********************************************************************************
.EQU EXTERNAL_RAM_START = RAMEND + 1 ; Starting address for external RAM memory.

;********************************************************************************
; bit_shift: Returns value bit shifted specified number of times via R24.
;            For instance, if specified number of shifts is 5, the value
;            (1 << 5) = 0010 0000 is returned.
;
;            - R24 - Number of bit shifts.
;********************************************************************************
bit_shift:
   LDI R16, 0x01
   LDI R17, 0x00
bit_shift_loop:
   CP R17, R24
   BREQ bit_shift_end
   LSL R16
   INC R17
   RJMP bit_shift_loop
bit_shift_end:
   MOV R24, R16
   RET

;********************************************************************************
; bit_shift_inverted: Returns value bit shifted specified number of times and 
;                     inverted via R24. For instance, if specified number of 
;                     shifts is 5, the value ~(1 << 5) = 1101 1111 is returned.
;
;                     - R24 - Number of bit shifts.
;********************************************************************************
bit_shift_inverted:
   LDI R16, 0xFE
   LDI R17, 0x00
bit_shift_inverted_loop:
   CP R17, R24
   BREQ bit_shift_end
   LSL R16
   ORI R16, 0x01
   INC R17
   RJMP bit_shift_loop
bit_shift_inverted_end:
   MOV R24, R16
   RET

.ENDIF ; MISC_INC_
