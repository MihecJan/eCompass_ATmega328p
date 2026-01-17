//#include "../config.h"
//
//#include "timer.h"
//
//static volatile bool timer_flag = false;
//
//// ISR for Timer1 Compare A
//ISR(TIMER1_COMPA_vect)
//{
	//timer_flag = true;
//}
//
//void timer1Init(uint16_t period_ms)
//{
	//// Stop timer (clear prescaler bits)
	//TCCR1B = 0;
	//
	//// CTC mode
	//TCCR1A = 0;
	//TCCR1B |= (1 << WGM12);
//
	//// Compute OCR1A value
	//uint32_t ocr = ((uint32_t)F_CPU / (64UL * 1000UL)) * period_ms - 1;
	//if (ocr > 0xFFFF) ocr = 0xFFFF;
	//OCR1A = (uint16_t)ocr;
//
	//// Clear any pending interrupt flags
	//TIFR1 |= (1 << OCF1A);
//
	//// Enable Output Compare A Match interrupt
	//TIMSK1 |= (1 << OCIE1A);
//
	//// Start timer with prescaler = 64
	//TCCR1B |= (1 << CS11) | (1 << CS10);
//}
//
//void timer1Start(void)
//{
	//// Prescaler = 64
	//TCCR1B |= (1 << CS11) | (1 << CS10);
//}
//
//void timer1Stop(void)
//{
	//TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
//}
//
//bool timer1Flag(void)
//{
	//return timer_flag;
//}
//
//void timer1ClearFlag(void)
//{
	//timer_flag = false;
//}