;********************************************************************************
; main.asm: Demonstration of drivers for leds and buttons in AVR assembly,
;           corresponding to structs in C.
;
;           A LED connected to pin 8 (PORTB0) is toggled via pressdown of
;           a button connected to pin 13 (PORTB5). 
;********************************************************************************

;********************************************************************************
; Interrupt vectors:
;********************************************************************************
.EQU RESET_vect  = 0x00 ; Reset vector, starting address of the program.
.EQU PCINT0_vect = 0x06 ; Interrupt vector for pin change interrupt on I/O port B.

;********************************************************************************
; .CSEG: Code segment, storage location of the machine code.
;********************************************************************************
.CSEG

;********************************************************************************
; RESET_vect: Reset vector and starting address of the program. A jump is made
;             to the main subroutine in order to start the program.
;********************************************************************************
.ORG RESET_vect
   RJMP main

;********************************************************************************
; PCINT0_vect: Interrupt vector for pin change interrupt on I/O port B.
;              A jump is made to the corresponding interrupt routine ISR_PCINT0
;              in order to handle the interrupt.
;********************************************************************************
.ORG PCINT0_vect
   RJMP ISR_PCINT0

;********************************************************************************
; Include directives:
;********************************************************************************
.include "led.inc"
.include "button.inc"

;********************************************************************************
; Macro definitions:
;********************************************************************************
.EQU LED1_ADDR = RAMEND + 1              ; Starting address for LED1.
.EQU BUTTON1_ADDR = LED1_ADDR + LED_SIZE ; Starting address for BUTTON1.

;********************************************************************************
; ISR_PCINT0: Interrupt routine for pin change interrupt on I/O port B, caused
;             by pressdown or release of BUTTON1. If BUTTON1 is pressed, LED1
;             is toggled.
;********************************************************************************
ISR_PCINT0:
   LDI R24, LOW(BUTTON1_ADDR)
   LDI R25, HIGH(BUTTON1_ADDR)
   RCALL button_is_pressed
   CPI R24, 0x00
   BREQ ISR_PCINT0_end
   LDI R24, low(LED1_ADDR)
   LDI R25, high(LED1_ADDR)
   CALL led_toggle
ISR_PCINT0_end:
   RETI

;********************************************************************************
; main: Connects LED1 to pin 8 (PORTB0) and BUTTON1 to pin 13 (PORTB5). 
;       Interrupt is enabled on the button pin so that a pressdown or release
;       causes an interrupt request.
;********************************************************************************
main:

;********************************************************************************
; init_stack: Initiates the stack pointer at start. This is done automatically
;             when using a modern microprocessor, but is recommended still so
;             that the stack pointer starts from the correct location after
;             a software reset.
;********************************************************************************
   LDI R16, low(RAMEND)
   LDI R17, high(RAMEND)
   OUT SPL, R16
   OUT SPH, R17

;********************************************************************************
; led1_init: Initiates LED1 at pin 8 (PORTB0), which corresponds to the 
;            following C code:
;
;            struct led led1;
;            led_init(&led1, 8);
;********************************************************************************
led1_init: 
   LDI R24, low(LED1_ADDR)
   LDI R25, high(LED1_ADDR)
   LDI R22, 8
   RCALL led_init

;********************************************************************************
; button1_init: Initiates BUTTON1 at pin 13 (PORTB5) and enables interrupt
;               on the button pin, which corresponds to the following C code:
;
;               struct button button1;
;               button_init(&button1, 13);
;               button_enable_interrupt(&button1);
;********************************************************************************
button1_init:
   LDI R24, low(BUTTON1_ADDR)
   LDI R25, high(BUTTON1_ADDR)
   MOVW Y, R24
   LDI R22, 13
   RCALL button_init
   MOVW R24, Y
   RCALL button_enable_interrupt

;********************************************************************************
; main_loop: Keeps the program running as long as voltage is supplied.
;********************************************************************************
main_loop:
   RJMP main_loop

