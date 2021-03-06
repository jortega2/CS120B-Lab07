/*  Juvenal Ortega jorte057 
 *  Partner(s) Name & E-mail: Duke Pham dpham073@ucr.edu
 * Lab Section: 022
 * Assignment: Lab 6  Exercise 2
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template 
 * or example code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms. 
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks 
unsigned int i = 0;
unsigned int tmpC = 0;
enum states {init, interphase, inc, incHold, dec, decHold, reset} state; 

void TimerOn(){
// AVR timer/counter controller register TCCR1
TCCR1B = 0x0B; // bit3 = 0: CTC mode (clear timer on compare)
	       // bit2bit1bit0=011: pre-scaler /64
	       // 00001011: 0x0B
	       // So, 8 Mhz clock or 8,000,000 /64 = 125,000 ticks/s
	       // Thus, TCNT1 register will count at 125,000 ticks/s

// AVR output compare register OCR1A.
OCR1A = 125; // Timer interrupt will be generated when TCNT1 == OCR1A
	     // We want a 1 ms tick. 0.001s * 125,0000 ticks/s = 125
	     // So when TCNT1 register equals 125,
	     // 1 ms has passed. Thus, we compare to 125.

// AVR timer interrupt mask register
TIMSK1 = 0x02; //bit1: OCIE1A -- enables compare match interrupt 

// Initialize avr counter
TCNT1 = 0;

_avr_timer_cntcurr = _avr_timer_M;
// TimerISR will be called every _avr_timer_cntcurr milliseconds 

// Enable  global interrupts  
SREG |= 0x80; // 0x80: 1000000
}

void TimerOff(){
	TCCR1B = 0x00; // bit3bit1bit0: timer off

}

void TimerISR(){
	TimerFlag = 1;
}

void TickSM(){
	switch (state) {
		case init:
			tmpC = 7;
			i = 0;
			LCD_ClearScreen();
			LCD_WriteData(tmpC + '0');
			state = interphase;
			break;
		case interphase:
			i = 0;
			if ((~PINA & 0x03) == 0x01){
				state = inc;
			} else if ((~PINA & 0x03) == 0x02){
				state = dec;
			} else if ((~PINA & 0x03) == 0x03){
				state = reset;
			} else {
				state = interphase;
			}			
			break;
		case inc:
			if ((~PINA & 0x03) == 0x01){
				state = incHold;
			} else {
				state = interphase;
			}
			break;
		case incHold:
			i++;
			if ((i > 10) && ((~PINA & 0x03)==0x01)){
				i = 0;
				state = inc;
			} else if ((i <= 10) && ((~PINA & 0x03)==0x01)){
				state = incHold;
			} else {
				i = 0;
				state = interphase;
			}
			break;
		case dec:
			if ((~PINA & 0x03) == 0x02){
				state = decHold;
			} else {
				state = interphase;
			}
			break;
		case decHold:
			i++;
			if ((i > 10) && ((~PINA & 0x03) == 0x02)){
				i = 0;
                                state = dec;
                        } else if (( i <= 10) && ((~PINA & 0x03) == 0x02)){
				state = decHold;
			} else {
				i = 0;
				state = interphase;
			}
			break;
		case reset:
			if ((~PINA & 0x03) == 0x00){
                                state = interphase;
                        } else {
                                state = reset;
                        }
                        break;
		default:
			break;
	} //transitions
	switch (state) {
		case init:
			LCD_ClearScreen();
			break;
		case interphase:
			break;
		case inc:
			if (tmpC < 9) {
				tmpC = (tmpC + 1);
				LCD_ClearScreen();
				LCD_WriteData (tmpC + '0');
			}
			break;	
		case incHold:
			break;
		case dec: 
			if (tmpC > 0) {
                                tmpC = (tmpC - 1);
                                LCD_ClearScreen();
                                LCD_WriteData (tmpC + '0');
                        }
			break;
		case decHold:
			break;
		case reset:
			tmpC = 0;
			LCD_ClearScreen();
			LCD_WriteData (tmpC + '0');
			break;
		default:
			break;
	} //state logic 
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect){
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerON settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if(_avr_timer_cntcurr == 0){ //results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	
	}
}
	// Set TimerISR() to tick every M ms
	void TimerSet(unsigned long M){
		_avr_timer_M = M;
		_avr_timer_cntcurr = _avr_timer_M;
	}

int main(void){
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; // Set port B to output
	PORTC = 0x00; // Init port B to 0s
	DDRA = 0x00; PORTA = 0xFF;
	TimerSet(10);
	TimerOn();
	state = init;
	LCD_init();
	//unsigned char tmpB = 0x00;
	while(1){
		// User code (i.e. synchSM calls)
		TickSM();
		while(!TimerFlag); // Wait 1 sec
		TimerFlag = 0;
		// Note: FOr the above a better style woulde use a synchSM with TickSM()
		// This example just illustrates the use of the ISR and flag
	
	}
	return 1;
}


