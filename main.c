/*	Author: jorte057 - Juvenal Ortega
 *  Partner(s) Name: Duke Pham dpham073@ucr.edu
 *	Lab Section: 022
 	*	Assignment: Lab 5 Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
// init- start of sm, set PORTC to 0x07
// interphase - state that leads to either increment, decrement or reset states depending on inputs PA0 and PA1
// inc - sets PORTC = PORTC + 1 if PORTC is less than 9. Leads to incWait state
// dec - sets PORTC = PORTC - 1 if PORTC is greater than 0. Leads to decWait state
// incWait/decWait - wait for user to release PA0 or PA1, then goes back to interphase. Also goes to reset if both A0 or A1 are held down
// reset - sets PORTC to 0x00, then waits for both PA0 and PA1 to be released to head back to interphase
enum add_states { init, interphase,  inc, /*incWait,*/ dec, /*decWait,*/ reset } addsm;
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned int i = 0;
void tick();

void TimerOn(){
	TCCR1B = 0X0B;
	OCR1A = 125;
	TIMSK1 = 0X02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}


unsigned int tmpC = 0;

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr ==0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
	void TimerSet(unsigned long M){
		_avr_timer_M = M;
		_avr_timer_cntcurr = _avr_timer_M;
	}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	addsm = init;
	TimerSet(100);
	TimerOn();
	LCD_init();
    /* Insert your solution below */
    while (1) {
	tick();
	while(!TimerFlag);
	TimerFlag = 0;
    }
}

void tick(){
	switch(addsm) {
		case init:
			tmpC = 7;
			LCD_ClearScreen();
			LCD_WriteData(tmpC + '0');
			addsm = interphase;
			break;
		case interphase:
			//PA0 & !PA1 go to inc
			if ((~PINA & 0x03) == 0x01){
				addsm = inc;
			}
			//PA1 & !PA0 go to dec
			else if ((~PINA & 0x03) == 0x02){
				addsm = dec;
			}
			//!PA0 and !PA1 go to reset
			else if ((~PINA & 0x03) == 0x03){
				addsm = reset;
			} else {// stay in interphase for anyrthing else
				addsm = interphase;
			}
			break;
		case inc:
			if ((~PINA & 0x03) == 0x01) {
				addsm = inc;
			} else { 
				addsm = interphase;
			}
			break;
		/*case incWait:
			//wait for button release 
			if ((~PINA & 0x03) == 0x01){
				addsm = incWait;
			}else if ((~PINA & 0x03) == 0x03){
				addsm = reset;
			}else{
				addsm = interphase;
			}
			break;*/
		case dec:
			if ((~PINA & 0x03) == 0x02) {
				addsm = dec;
			}else{
				addsm = interphase;
			}
			break;
		/*case decWait:
			// wait for button release
                        if ((~PINA & 0x03) == 0x02){
                                addsm = decWait;
			} else if ((~PINA & 0x03) == 0x03){
                                addsm = reset;
                        }else{
                                addsm = interphase;
                        }

			break;*/
		case reset:
			if ((~PINA & 0x03) == 0x00){
				addsm = interphase;
			} else {
				addsm = reset;
			}
			break;
		default:
			addsm = interphase;
			break;
	}//transitions
	
	//state actions
	switch(addsm) {
                case init:
                        break;
                case interphase:
			break;
                case inc:
			// Add 1 to PORTC but doesn't surpass 9
			if (tmpC < 9){
				tmpC = (tmpC + 1);
				LCD_ClearScreen();
				LCD_WriteData(tmpC + '0');
			}	
                        break;
		/*case incWait:
			break;*/
                case dec:
			//Subtract 1 to PORTC but doesn't go below 0
			if (tmpC > 0){
                        	tmpC = (tmpC - 1);
				LCD_ClearScreen();
				LCD_WriteData (tmpC + '0');
			}
                        break;
		/*case decWait:
			break;*/
                case reset:
			tmpC = 0;
			LCD_ClearScreen();
			LCD_WriteData (tmpC + '0');
                        break;
                default:
                        break;
        }//state actins
}
