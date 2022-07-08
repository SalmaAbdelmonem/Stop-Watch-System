#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

unsigned char sec_ones = 0, sec_tens = 0, min_ones = 0, min_tens = 0, hour_ones = 0, hour_tens = 0;
unsigned char interrupt_flag = 0, reset_flag = 0, pause_flag = 0, resume_flag = 0;

void Timer1_CTC_mode_Init(void)
{
	sei(); //enable I-Bit
	TCNT1 = 0; //initial value of timer1
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); //set timer1 to compare mode & prescaler to 1024
	OCR1A = 1000; //compare value
	TIMSK |= (1<<OCIE1A); //output compare A match interrupt enable
	TCCR1A |= (1<<FOC1A); //non PWM mode
}

ISR(TIMER1_COMPA_vect)
{
	interrupt_flag = 1;
}

void INT0_reset_Init(void)
{
	DDRD &= ~(1<<PD2); //set pin PD2 as input
	PORTD |= (1<<PD2); //enable internal pull up res
	MCUCR |= (1<<ISC01); //assign falling edge
	GICR |= (1<<INT0); //enable interrupt
}

ISR(INT0_vect)
{
	sec_ones = 0, sec_tens = 0, min_ones = 0, min_tens = 0, hour_ones = 0, hour_tens = 0;
	reset_flag = 0;
}

void INT1_pause_Init(void)
{
	DDRD &= ~(1<<PD3);
	MCUCR = (1<<ISC10) | (1<<ISC11) ;
	GICR |= (1<<INT1);
}

ISR(INT1_vect)
{
	TCCR1B = (1<<WGM12); //timer is off
}

void INT2_resume_Init(void)
{
	DDRD &= ~(1<<PB2);
	PORTD |= (1<<PB2);
	MCUCR &= ~(1<<ISC2);
	GICR |= (1<<INT2);
}

ISR(INT2_vect)
{
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); //set timer to compare mode again
}

void display(void)
{
	PORTA &= 0xC0; //clear port A
	PORTA |= (1<<5); //enable 7-segment
	PORTC = (PORTC & 0xF0)|(sec_ones & 0x0F); //assign value to 7-segment
	_delay_ms(5);

	PORTA &= 0xC0;
	PORTA |= (1<<4);
	PORTC = (PORTC & 0xF0)|(sec_tens & 0x0F);
	_delay_ms(5);

	PORTA &= 0xC0;
	PORTA |= (1<<3);
	PORTC = (PORTC & 0xF0)|(min_ones & 0x0F);
	_delay_ms(5);

	PORTA &= 0xC0;
	PORTA |= (1<<2);
	PORTC = (PORTC & 0xF0)|(min_tens & 0x0F);
	_delay_ms(5);


	PORTA &= 0xC0;
	PORTA |= (1<<1);
	PORTC = (PORTC & 0xF0)|(hour_ones & 0x0F);
	_delay_ms(5);

	PORTA &= 0xC0;
	PORTA |= (1<<0);
	PORTC = (PORTC & 0xF0)|(hour_tens & 0x0F);
	_delay_ms(5);
}

int main()
{
	DDRA |= 0x3F;
	PORTA |= 0x3F;
	DDRC |= 0x0F;
	PORTC &= 0xF0;
	SREG |= (1<<7);

	Timer1_CTC_mode_Init();
	INT0_reset_Init();
	INT1_pause_Init();
	INT2_resume_Init();

	while(1)
	{
		if(interrupt_flag == 1)
		{
			sec_ones ++;

			if(sec_ones == 10)
			{
				sec_tens ++;
				sec_ones = 0;
			}

			if(sec_ones == 0 && sec_tens == 6)
			{
				sec_tens = 0;
				min_ones ++;
			}

			if(min_ones == 10)
			{
				min_tens ++;
				min_ones = 0;
			}

			if(min_ones == 0 && min_tens == 6)
			{
				min_tens = 0;
				hour_ones ++;
			}

			if(hour_ones == 10)
			{
				hour_tens ++;
				hour_ones = 0;
			}
			interrupt_flag = 0;
		}

		/*-----------------------------------------------------------*/
		/* display function */

		display();
	}
}

