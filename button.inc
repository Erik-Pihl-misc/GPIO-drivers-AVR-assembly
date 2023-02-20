;********************************************************************************
; button.inc: Contains drivers for buttons via objects, implemented as shown below:
;
;            struct button
;            {
;               volatile uint8_t* port_reg;
;               volatile uint8_t* pin_reg;
;               volatile uint8_t* pcmsk_reg;
;               uint8_t pcie_bit;
;               uint8_t pin;
;            };
;********************************************************************************
.IFNDEF BUTTON_INC_
.EQU BUTTON_INC_ = 0

;********************************************************************************
; Include directives:
;********************************************************************************
.INCLUDE "misc.inc"

.EQU BUTTON_PORT_REG_PTR_OFFSET  = 0 ; Offset for port register pointer.
.EQU BUTTON_PIN_REG_PTR_OFFSET   = 2 ; Offset for pin register pointer.
.EQU BUTTON_PCMSK_REG_PTR_OFFSET = 4 ; Offset for pin change interrupt mask register pointer.
.EQU BUTTON_PCIE_BIT_OFFSET      = 6 ; Offset for pin change interrupt enable bit in PCICR.
.EQU BUTTON_PIN_OFFSET           = 7 ; Offset for button pin number.
.EQU BUTTON_SIZE                 = 8 ; Size of button object in bytes.

;********************************************************************************
; button_init: Initiates button on specified I/O port B or D. If an invalid pin 
;              number is specified, no initiation is done and error code 1 is
;              returned, else success code 0 is returned after initiation.
;
;              - [R24:R25]: Reference to the button.
;              - R22      : Button pin number on Arduino Uno (0 - 13).
;********************************************************************************
button_init:
   MOVW Z, R24
   CPI R22, 8
   BRLO button_init_portd
   CPI R22, 14
   BRLO button_init_portb
   LDI R24, 0x01
   RET
button_init_portb:
   SUBI R22, 8
   LDI R20, PCIE0
   LDI XL, low(PINB + 0x20)
   LDI XH, high(PINB + 0x20)
   LDI R18, low(PCMSK0)
   LDI R19, high(PCMSK0)
   RJMP button_init_members
button_init_portd:
   LDI R20, PCIE2
   LDI XL, low(PIND + 0x20)
   LDI XH, high(PIND + 0x20)
   LDI R18, low(PCMSK2)
   LDI R19, high(PCMSK2)
button_init_members:
   STD Z + BUTTON_PIN_OFFSET, R22
   STD Z + BUTTON_PCIE_BIT_OFFSET, R20
   STD Z + BUTTON_PIN_REG_PTR_OFFSET, XL
   STD Z + BUTTON_PIN_REG_PTR_OFFSET + 1, XH
   ADIW X, 2
   STD Z + BUTTON_PORT_REG_PTR_OFFSET, XL 
   STD Z + BUTTON_PORT_REG_PTR_OFFSET + 1, XH
   MOVW X, R18
   STD Z + BUTTON_PCMSK_REG_PTR_OFFSET, XL
   STD Z + BUTTON_PCMSK_REG_PTR_OFFSET + 1, XH
button_init_ports:
   MOV R24, R22
   RCALL bit_shift
   LDD XL, Z + BUTTON_PORT_REG_PTR_OFFSET
   LDD XH, Z + BUTTON_PORT_REG_PTR_OFFSET + 1
   LD R16, X
   OR R16, R24
   ST X, R16
   CLR R24
   RET

;********************************************************************************
; button_clear: Clears referenced button and disables connected port.
;
;               - [R24:R25]: Reference to the button.
;********************************************************************************
button_clear:
   MOVW Z, R24
   LDD R24, Z + BUTTON_PIN_OFFSET
   RCALL bit_shift
button_clear_disable_pin:
   LDD XL, Z + BUTTON_PORT_REG_PTR_OFFSET
   LDD XH, Z + BUTTON_PORT_REG_PTR_OFFSET + 1
   LD R16, X
   AND R16, R24
   ST X, R16
   LDD XL, Z + BUTTON_PCMSK_REG_PTR_OFFSET
   LDD XH, Z + BUTTON_PCMSK_REG_PTR_OFFSET + 1
   LD R16, X
   AND R16, R24
   ST X, R16
button_clear_data:
   CLR R24
   CLR R16
button_clear_data_loop:
   CPI R16, BUTTON_SIZE
   BREQ button_clear_end
   ST Z+, R24
   INC R16
   RJMP button_clear_data_loop
button_clear_end:
   RET

;********************************************************************************
; button_is_pressed: Indicates if referenced button is pressed by returning
;                    1 (true) or 0 (false) via R24.
;
;                    - [R24:R25]: Reference to the button.
;********************************************************************************
button_is_pressed:
   MOVW Z, R24
   LDD R24, Z + BUTTON_PIN_OFFSET
   RCALL bit_shift
   LDD XL, Z + BUTTON_PIN_REG_PTR_OFFSET
   LDD XH, Z + BUTTON_PIN_REG_PTR_OFFSET + 1
   LD R16, X
   AND R16, R24
   BRNE button_is_pressed_return_1
button_is_pressed_return_0:
   CLR R24
   RET
button_is_pressed_return_1:
   LDI R24, 0x01
   RET

;********************************************************************************
; button_interrupt_enabled: Indicates if pin change interrupt is enabled on
;                           referenced button by returning 1 (true) or 0 (false) 
;                           via R24.
;
;                           - [R24:R25]: Reference to the button.
;********************************************************************************
button_interrupt_enabled:
   MOVW Z, R24
   LDD R24, Z + BUTTON_PIN_OFFSET
   RCALL bit_shift
   LDD XL, Z + BUTTON_PCMSK_REG_PTR_OFFSET
   LDD XH, Z + BUTTON_PCMSK_REG_PTR_OFFSET + 1
   LD R16, X
   AND R16, R24
   BRNE button_interrupt_enabled_return_1
button_interrupt_enabled_return_0:
   CLR R24
   RET
button_interrupt_enabled_return_1:
   LDI R24, 0x01
   RET

;********************************************************************************
; button_enable_interrupt: Enables pin change interrupt on referenced button.
;
;                          - [R24:R25]: Reference to the button.
;********************************************************************************
button_enable_interrupt:
   MOVW Z, R24
   LDD R24, Z + BUTTON_PCIE_BIT_OFFSET
   RCALL bit_shift
   LDS R16, PCICR
   OR R16, R24
   STS PCICR, R16
button_enable_interrupt_bit_shift:
   LDD R24, Z + BUTTON_PIN_OFFSET
   RCALL bit_shift
   LDD XL, Z + BUTTON_PCMSK_REG_PTR_OFFSET
   LDD XH, Z + BUTTON_PCMSK_REG_PTR_OFFSET + 1
   LD R16, X
   OR R16, R24
   ST X, R16
   SEI
   RET

;********************************************************************************
; button_disable_interrupt: Disables pin change interrupt on referenced button.
;
;                           - [R24:R25]: Reference to the button.
;********************************************************************************
button_disable_interrupt:
   MOVW Z, R24
   LDD R24, Z + BUTTON_PIN_OFFSET
   RCALL bit_shift_inverted
   LDD XL, Z + BUTTON_PCMSK_REG_PTR_OFFSET
   LDD XH, Z + BUTTON_PCMSK_REG_PTR_OFFSET + 1
   LD R16, X
   AND R16, R24
   ST X, R16
   RET

;********************************************************************************
; button_toggle_interrupt: Toggles pin change interrupt on referenced button.
;
;                          - [R24:R25]: Reference to the button.
;********************************************************************************
button_toggle_interrupt:
   PUSH R24
   PUSH R25
   RCALL button_interrupt_enabled
   MOV R16, R24
   POP R25
   POP R24
   CPI R16, 0x00
   BREQ button_toggle_interrupt_on
button_toggle_interrupt_off:
   RCALL button_disable_interrupt
   RET
button_toggle_interrupt_on:
   RCALL button_enable_interrupt
   RET

;********************************************************************************
; button_get_pin: Returns the pin number of referenced button via R24.
;
;              - [R24:R25]: Reference to the button.
;********************************************************************************
button_get_pin:
   MOVW Z, R24
   LDD R24, Z + BUTTON_PIN_OFFSET
   RET

;********************************************************************************
; button_get_pcie_bit: Returns the pin change interrupt enable bit of
;                      referenced button via R24.
;
;                      - [R24:R25]: Reference to the button.
;********************************************************************************
button_get_pcie_bit:
   MOVW Z, R24
   LDD R24, Z + BUTTON_PCIE_BIT_OFFSET
   LDD R25, Z + BUTTON_PCIE_BIT_OFFSET + 1
   RET

;********************************************************************************
; button_get_port_reg: Returns a reference to the port register of referenced 
;                      button via [R24:R25].
;
;                      - [R24:R25]: Reference to the button.
;********************************************************************************
button_get_port_reg:
   MOVW Z, R24
   LDD R24, Z + BUTTON_PORT_REG_PTR_OFFSET
   LDD R25, Z + BUTTON_PORT_REG_PTR_OFFSET + 1
   RET

;********************************************************************************
; button_get_pin_reg: Returns a reference to the pin register of referenced 
;                     button via [R24:R25].
;
;                     - [R24:R25]: Reference to the button.
;********************************************************************************
button_get_pin_reg:
   MOVW Z, R24
   LDD R24, Z + BUTTON_PIN_REG_PTR_OFFSET
   LDD R25, Z + BUTTON_PIN_REG_PTR_OFFSET + 1
   RET

;********************************************************************************
; button_get_pci_mask_reg: Returns a reference to the pin change interrupt mask 
;                          register of referenced button via [R24:R25].
;
;                          - [R24:R25]: Reference to the button.
;********************************************************************************
button_get_pci_mask_reg:
   MOVW Z, R24
   LDD R24, Z + BUTTON_PCMSK_REG_PTR_OFFSET
   LDD R25, Z + BUTTON_PCMSK_REG_PTR_OFFSET + 1
   RET

.ENDIF ; BUTTON_INC_
