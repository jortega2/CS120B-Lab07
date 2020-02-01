/*	Author: jorte057
 *  Partner(s) Name: Duke Pham dpham073@ucr.edu
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
    DDRC = 0xFF; PORTC = 0x00; //LCD data lines
    DDRD = 0x0FF; PORTD = 0x00; //LCD control lines

    LCD_init();

    LCD_DisplayString(1, "Hello World");

    while (1) {
	continue;
    }
}
