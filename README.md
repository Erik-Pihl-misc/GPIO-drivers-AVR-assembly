# GPIO drivers for ATmega328P in AVR assembly
GPIO drivers for leds and buttons via structures in AVR assembly, implemented on microcontroller
ATmega328P, located on microcontroller board Arduino Uno.

The GPIO drivers are tested via a program, where a LED connected to pin 8 (PORTB0) 
is toggled via pressdown of a button connected to pin 13 (PORTB5). Pin change interrupt
is enabled so that the LED is only toggled at pressdown (rising edge of the input signal).

The file "led.inc" contains drivers for LEDs.
The file "button.inc" contains drivers for buttons, both for polling and interrupts. 
The file "main.asm" contains the program where the GPIO drivers are tested.
The file "misc.inc" contains subroutines for shifting bits.