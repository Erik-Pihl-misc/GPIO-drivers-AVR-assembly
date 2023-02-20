;********************************************************************************
; LED.inc: Contains drivers for LEDs via objects, implemented as shown below:
;
;          struct led
;          {
;             volatile uint8_t* port_reg;
;             volatile uint8_t* pin_reg;
;             volatile uint8_t* dir_reg;
;             uint8_t pin;
;          };
;********************************************************************************
.IFNDEF LED_INC_
.EQU LED_INC_ = 0

;********************************************************************************
; Include directives:
;********************************************************************************
.INCLUDE "misc.inc"

;********************************************************************************
; Macro definitions:
;********************************************************************************
.EQU LED_PORT_REG_PTR_OFFSET = 0 ; Offset for port register pointer.
.EQU LED_PIN_REG_PTR_OFFSET  = 2 ; Offset for pin register pointer.
.EQU LED_DIR_REG_PTR_OFFSET  = 4 ; Offset for data direction register pointer.
.EQU LED_PIN_OFFSET          = 6 ; Offset for pin number.
.EQU LED_SIZE                = 7 ; Size of LED object in bytes.

;********************************************************************************
; led_init: Initiates LED on specified I/O port B or D. If an invalid pin 
;           number is specified, no initiation is done and error code 1 is
;           returned, else success code 0 is returned after initiation.
;
;         - [R24:R25]: Reference to the LED.
;         - R22      : LED pin number on Arduino Uno (0 - 13).
;********************************************************************************
led_init:
   MOVW Z, R24
   CPI R22, 8
   BRLO led_init_portD
   CPI R22, 14
   BRLO led_init_portB
   LDI R24, 0x01
   RET
led_init_portB:
   SUBI R22, 8
   LDI XL, low(PINB + 0x20)
   LDI XH, high(PINB + 20)
   RJMP led_init_members
led_init_portD:
   LDI XL, low(PIND + 0x20)
   LDI XH, high(PIND + 0x20)
led_init_members:
   STD Z + LED_PIN_OFFSET, R22
   STD Z + LED_PIN_REG_PTR_OFFSET, XL
   STD Z + LED_PIN_REG_PTR_OFFSET + 1, XH
   ADIW X, 1
   STD Z + LED_DIR_REG_PTR_OFFSET, XL
   STD Z + LED_DIR_REG_PTR_OFFSET + 1, XH
   ADIW X, 1
   STD Z + LED_PORT_REG_PTR_OFFSET, XL
   STD Z + LED_PORT_REG_PTR_OFFSET + 1, XH
led_init_ports:
   MOV R24, R22
   RCALL bit_shift
   LDD XL, Z + LED_DIR_REG_PTR_OFFSET
   LDD XH, Z + LED_DIR_REG_PTR_OFFSET + 1
   LD R16, X
   OR R16, R24
   ST X, R16
   CLR R24
   RET

;********************************************************************************
; led_clear: Clears referenced LED and disables connected port.
;
;            - [R24:R25]: Reference to the LED.
;********************************************************************************
led_clear:
   MOVW Z, R24
   LDD XL, Z + LED_DIR_REG_PTR_OFFSET
   LDD XH, Z + LED_DIR_REG_PTR_OFFSET + 1
   LDD R24, Z + LED_PIN_OFFSET
   RCALL bit_shift_inverted
   LD R16, X
   AND R16, R24
   ST X, R16
   LDD XL, Z + LED_PORT_REG_PTR_OFFSET
   LDD XH, Z + LED_PORT_REG_PTR_OFFSET + 1
   LD R16, X
   AND R16, R24
   ST X, R16
led_clear_data:
   CLR R24
   CLR R16
led_clear_data_loop:
   CPI R16, LED_SIZE
   BREQ led_clear_end
   ST Z+, R24
   INC R16
   RJMP led_clear_data_loop
led_clear_end:
   RET

;********************************************************************************
; led_on: Enables referenced LED.
;
;         - [R24:R25]: Reference to the LED.
;********************************************************************************
led_on:
   MOVW Z, R24
   LDD XL, Z + LED_PORT_REG_PTR_OFFSET
   LDD XH, Z + LED_PORT_REG_PTR_OFFSET + 1
   LDD R24, Z + LED_PIN_OFFSET
   RCALL bit_shift
   LD R16, X
   OR R16, R24
   ST X, R16
   RET

;********************************************************************************
; led_off: Disables referenced LED.
;
;          - [R24:R25]: Reference to the LED.
;********************************************************************************
led_off:
   MOVW Z, R24
   LDD XL, Z + LED_PORT_REG_PTR_OFFSET
   LDD XH, Z + LED_PORT_REG_PTR_OFFSET + 1
   LDD R24, Z + LED_PIN_OFFSET
   RCALL bit_shift_inverted
   LD R16, X
   AND R16, R24
   ST X, R16
   RET

;********************************************************************************
; led_toggle: Toggles referenced LED.
;
;             - [R24:R25]: Reference to the LED.
;********************************************************************************
led_toggle:
   MOVW Z, R24
   LDD R24, Z + LED_PIN_OFFSET
   RCALL bit_shift
   LDD XL, Z + LED_PIN_REG_PTR_OFFSET
   LDD XH, Z + LED_PIN_REG_PTR_OFFSET + 1
   ST X, R24
   RET

;********************************************************************************
; led_enabled: Indicates if referenced LED is enabled by returning 1 (true) 
;              or 0 (false) via R24.
;
;              - [R24:R25]: Reference to the LED.
;********************************************************************************
led_enabled:
   MOVW Z, R24
   LDD XL, Z + LED_PIN_REG_PTR_OFFSET
   LDD XH, Z + LED_PIN_REG_PTR_OFFSET + 1
   LDD R24, Z + LED_PIN_OFFSET
   RCALL bit_shift
   LD R16, X
   AND R16, R24
   BRNE led_enabled_return_1
led_enabled_return_0:
   CLR R24
   RET
led_enabled_return_1:
   LDI R24, 0x01
   RET

;********************************************************************************
; led_get_pin: Returns the pin number of referenced LED via R24.
;
;              - [R24:R25]: Reference to the LED.
;********************************************************************************
led_get_pin:
   MOVW Z, R24
   LDD R24, Z + LED_PIN_OFFSET
   RET

;********************************************************************************
; led_get_port_reg: Returns a reference to the port register of referenced LED
;                   via [R24:R25].
;
;                   - [R24:R25]: Reference to the LED.
;********************************************************************************
led_get_port_reg:
   MOVW Z, R24
   LDD R24, Z + LED_PORT_REG_PTR_OFFSET
   LDD R25, Z + LED_PORT_REG_PTR_OFFSET + 1
   RET

;********************************************************************************
; led_get_pin_reg: Returns a reference to the pin register of referenced LED
;                  via [R24:R25].
;
;                  - [R24:R25]: Reference to the LED.
;********************************************************************************
led_get_pin_reg:
   MOVW Z, R24
   LDD R24, Z + LED_PIN_REG_PTR_OFFSET
   LDD R25, Z + LED_PIN_REG_PTR_OFFSET + 1

;********************************************************************************
; led_get_dir_reg: Returns a reference to the data direction register of 
;                  referenced LED via [R24:R25].
;
;                  - [R24:R25]: Reference to the LED.
;********************************************************************************
led_get_dir_reg:
   MOVW Z, R24
   LDD R24, Z + LED_DIR_REG_PTR_OFFSET
   LDD R25, Z + LED_DIR_REG_PTR_OFFSET + 1
   RET

.ENDIF ; LED_INC_
