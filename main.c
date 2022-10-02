#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
unsigned char hours=0,minutes=0,seconds=0,flagSec=0,flagMin=0;

void setupTimer1(void){
	OCR1A=976;
	TCCR1A = (1<<FOC1A)| (1<<FOC1B);
	TCCR1B = (1<<CS12)|(1<<CS10)|(1<<WGM12);
	TCNT1=0;
	TIMSK |=(1<<OCIE1A);
	//	TCCR1B = (1<<WGM12) | (1<<CS11);
}

ISR(TIMER1_COMPA_vect)
{
	seconds%=60;
	if(seconds==59)
		minutes++;
	if(minutes==59 && seconds==59)
	{
		hours++;
		minutes=0;
	}
	if(seconds<59)
		seconds++;
	else
		seconds=0;
}

// reset stop watch
void INI0_Init(void){
	DDRD &=~(1<<2);
	PORTD|=(1<<2);
	MCUCR|=(1<<ISC01); // with falling edge
	MCUCR &=~(1<<ISC00);
	GICR |=(1<<INT0);
}

// stop counting stop watch
void INI1_Init(void){
	DDRD &=~(1<<3);
	MCUCR|=(1>>ISC01) |(1<<ISC00);// with raising edge
	GICR |=(1<<INT1);
}

// resume stop watch
void INI2_Init(void){
	DDRB &=~(1<<2);
	PORTB|=(1<<2);
	MCUCR|=(1>>ISC01); // with falling edge
	MCUCR &=~(1<<ISC00);
	GICR |=(1<<INT2);
}

// ISR for reset
ISR(INT0_vect){
	TIMSK|=(1<<OCIE1A);
	PORTA &=0x00;
	PORTA|=0xff;
	PORTC=0;
	seconds=0,minutes=0,hours=0;
}

// ISR stop
ISR(INT1_vect){
	TIMSK&=~(1<<OCIE1A); // disable clock pin
}

// ISR resume
ISR(INT2_vect){
	TIMSK|=(1<<OCIE1A); // enable clock  pin
}
int main ()
{
	SREG|=(1<<7); // enable I-global interrupt
	DDRC|=0x0f; // decoder
	DDRA |=0X3f; // enable 6-7 segment
	PORTA |=0x00;
	PORTC =0xC0;
	INI0_Init();
	INI1_Init();
	INI2_Init();
	setupTimer1();
	while(1){

		// displaying seconds
		PORTA &=0x00;
		PORTA |=0x20;
		PORTC=seconds%10;
		_delay_ms(2);

		PORTA &=0x00;
		PORTA|=0x10;
		PORTC=seconds/10;
		_delay_ms(2);

		// displaying minutes
		PORTA &=0x00;
		PORTA |=0x08;
		PORTC=minutes%10;
		_delay_ms(2);

		PORTA &=0x00;
		PORTA|=0x04;
		PORTC=minutes/10;
		_delay_ms(2);


		// displaying hours
		PORTA &=0x00;
		PORTA |=0x02;
		PORTC =hours%10;
		_delay_ms(2);

		PORTA &=0x00;
		PORTA |=0x01;
		PORTC=hours/10;
		_delay_ms(2);
	}
}
