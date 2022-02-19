#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <string.h>

void LCD_Init()
{
	DDRD = 0xFF;  // set data direction as output
	DDRC = 0x03;
	PORTC = 0x00; //port direction input
	PORTD = 0x00; //port direction input
	CommandLine_Place(0x3E); // make display off
	CommandLine_Place(0x0E);  // make display ON, cursor ON
	CommandLine_Place(0x06); // make increment in cursor
	CommandLine_Place(0x01);// Clear screen
	LCD_Write("Glove Control");
	CommandLine_Place(0xC3);
	LCD_Write("Car");
	_delay_ms(2000);
}

void LCD_Write(char *s)
{
	uint8_t l,i;
	l = strlen(s);				// get the length of string
	for(i=0;i<l;i++)
	{
		Send_Data(*s);			// write every char one by one
		s++;
	}
}

void Send_Data(unsigned char data)
{
	_delay_ms(2);
	PORTC |= 0x01; // pin0 = output - RS
	PORTD=data;  // dATA PORT 
	PORTC |= 0x02; // pin1=output - E
	PORTC &= 0xFD; // data
}
void CommandLine_Place(unsigned char cmd)
{
	_delay_ms(2);
	PORTC &= 0xFE; // set pin1 as input
	PORTD=cmd; // save command
	PORTC |= 0x02; // set pin1 as output - enable
	PORTC &= 0xFD; // 
}



void ADC_Init()
{
	DDRA=0x0; // set data direction as input 
	ADCSRA |= (1<<ADEN) ; // ADC Enable
	ADMUX = (1<<REFS0);				// select voltage reference for ADC
	ADMUX = (0<<REFS1);	
	ADCSRA |= (0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	// select clock s to 8 bit mode ( PRESCALAR DIVISION FACTOR =  8 )
}
unsigned int ADC_Convert()
{
	unsigned int tmp1,tmp2,tmp3,t;
	tmp1 = (ADCL & 0x0F);
	tmp2 = (ADCL >> 4) & 0x0F;
	tmp3 = (ADCH & 0x0F);
	tmp2 = tmp2*16;
	tmp3 = tmp3*256;
	t = tmp1+tmp2+tmp3;
	return t;
}

int main()
{
	unsigned int adc_value,ch_count=0;
	unsigned int fwd_flag=0,rev_flag=0,right_flag=0,left_flag=0;
	unsigned int msg1_flag=0,msg1_count=0;
	unsigned int msg2_flag=0,msg2_count=0;
	unsigned int msg3_flag=0,msg3_count=0;
	unsigned int msg4_flag=0,msg4_count=0;
	unsigned int msg5_count=0;
	
	DDRA = 0x00; // Data Direction IP.
	PORTA = 0xFF; // Pull up
	
	DDRB = 0xFF; // Data Direction OP.
	PORTB = 0x00; // PORTB Input on LEDS
	
	TCCR0=0x6B; // Timer 0
	
	LCD_Init();
	ADC_Init();
	while(1)
	{
		ch_count++;
		if(ch_count==1) ADMUX = 0x40 ; // ADMUX Selects channel
		else if (ch_count==2) ADMUX = 0x41 ;
		else if (ch_count==3) ADMUX = 0x42;
		else if (ch_count==4) ADMUX = 0x43;
		ADCSRA |= (1<<ADEN) | (1<<ADSC); // ADC Enable - ADC Start Conversion
		while(!(ADCSRA & (1<<ADIF)));
		ADCSRA |= (1<<ADIF); // ADC Interrupt Flag
		adc_value = ADC_Convert();	// Convert input voltages from sensors into hex values then into decimal values
		int ADCAvgVal = 820;
				// if first Flex Sensor

		if(ch_count==1)
		{
			if(adc_value>ADCAvgVal)
			{
				PORTB = 0x15; //Blink LED Num 1
				fwd_flag=1;
				msg1_flag = 1;
			}
			else
			{
				fwd_flag=0;
				msg1_count=0;
				msg1_flag=0;
			}
		}
				   // if Second Flex Sensor

		else if(ch_count==2)
		{
			if(adc_value>ADCAvgVal)
			{
				PORTB = 0x21; //Blink LED Num 2
				left_flag=1;
				msg2_flag=1;
			}
			else {
				left_flag=0;
				msg2_count=0;
				msg2_flag=0;
				}
		}
				  // if third Flex Sensor.

		else if(ch_count==3)
		{
			if(adc_value>ADCAvgVal)
			{
				PORTB = 0x44; //Blink LED Num 3
				right_flag=1;
				msg3_flag=1;
			}
			else {
				right_flag=0;
				msg3_count=0;
				msg3_flag=0;
				}
		}
				  // if fourth Flex Sensor.

		else if(ch_count==4)
		{
			if(adc_value>ADCAvgVal)
			{
				PORTB = 0x8A; //Blink LED Num 4
				rev_flag=1;
				msg4_flag=1;
			}
			else {
				rev_flag=0;
				msg4_count=0;
				msg4_flag=0;
			}
			ch_count=0;
		}
				  // if all flags equals 0 means no flex sensor is bending.

		if((fwd_flag==0) && (rev_flag==0) && (left_flag==0) && (right_flag==0))
		{
			PORTB = 0x00;
			msg1_flag=0;
			msg2_flag=0;
			msg3_flag=0;
			msg4_flag=0;
		}
		
				  // if msg1 flag = 1 means eno f forward direction

		if(msg1_flag==1)
		{
			//Forward
			if(msg1_count==0)
			{
				{
					// Print ll direction bta3o 3la el LCD
					
					CommandLine_Place(0x01);
					LCD_Write("Car");
					CommandLine_Place(0xC0);
					LCD_Write("moving forward");
					
					//msg1_flag=0;
					msg1_count++; // to break el if cond
					msg5_count=0; // go to Status "STOP"
				}
			}
		}
						  // if msg2 flag = 1 means eno f Left direction

		else if(msg2_flag==1)
		{
			//Left
			if(msg2_count==0)
			{
				{
					// Print ll direction bta3o 3la el LCD

					CommandLine_Place(0x01);
					LCD_Write("Car");
					CommandLine_Place(0xC0);
					LCD_Write("turning left");
					
					//msg2_flag=0;
					msg2_count++; // to break el if cond
					msg5_count=0; // go to Status "STOP"
				}
			}
		}
						  // if msg3 flag = 1 means eno f right direction

		else if(msg3_flag==1)
		{
			//Right
			if(msg3_count==0)
			{
				{
					// Print ll direction bta3o 3la el LCD

					CommandLine_Place(0x01);
					LCD_Write("Car");
					CommandLine_Place(0xC0);
					LCD_Write("turning right");
					
					//msg3_flag=0;
					msg3_count++; // to break el if cond
					msg5_count=0; // go to Status "STOP"
				}
			}
		}
						  // if msg4 flag = 1 means eno f backward direction

		else if(msg4_flag==1)
		{
			//Backward
			if(msg4_count==0)
			{
				{
					// Print ll direction bta3o 3la el LCD

					CommandLine_Place(0x01);
					LCD_Write("Car");
					CommandLine_Place(0xC0);
					LCD_Write("moving backward");
					
					//msg4_flag=0;
					msg4_count++; // to break el if cond
					msg5_count=0; // go to Status "STOP"
				}
			}
		}
		else
		{
			// car not moving
			if(msg5_count==0)
			{
				// Print ll direction bta3o 3la el LCD

				CommandLine_Place(0x01);
				LCD_Write("Car");
				CommandLine_Place(0xC0);
				LCD_Write("is not moving");
				
				msg5_count++; // to break el if cond
			}
		}
	}
}

