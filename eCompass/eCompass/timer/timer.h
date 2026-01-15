#ifndef TIMER_H_
#define TIMER_H_


#include <stdint.h>
#include <stdbool.h>

#include <avr/interrupt.h>

void timer1Init(uint16_t period_ms);
void timer1Start(void);
void timer1Stop(void);
bool timer1Flag(void);
void timer1ClearFlag(void);


#endif /* TIMER_H_ */