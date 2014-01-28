// generate C4 major scale (C4,D4,E4,F4,G4,A4,B4,C5) 
//#define F_CPU 3868000

#define C4				146
#define D4				158
#define E4				169
#define F4				173.5
#define G4				182.5
#define A4				190.5
#define B4				198
#define C5				201

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define SPKR 0x20

unsigned char C_major_scale[] PROGMEM = {C4,D4,E4,F4,G4,A4,B4,C5};
volatile unsigned char tone;
volatile unsigned char ctr;

// Function prototype
unsigned char decode_sw(unsigned char);

int main(void)
{
  unsigned char x,x_old=0xFF;

  // initialize speaker port
  DDRC = 0xFF;  // set as output
  PORTC = ~SPKR;  // turn off speaker

  // initialize switch/LED port
  DDRA = 0xF0;   // set PA0-3 as input
  PORTA = 0x8F;  // pull-up switch ports

  // initialize Timer0 interrupts
  TCCR0 = 0x03; // prescaler = 64
  TIMSK = 0x02; // initialize TOV0 interrupt
  sei();
  
  while (1)
  {
    x = PINA|0xF0;

		if((x!=0xFF)&&(x!=x_old))//if switch is pressed
		{
			_delay_ms(10);
				if(PINA!=0xFF)
					{
				    tone = decode_sw(x); 

					for(ctr = 0; ctr < 8; ctr++)
						 _delay_ms(500);
					}
		}
     x_old = x;
     //PORTC = ~SPKR;  // turn off sound
  }
  return 0;
}

ISR(TIMER0_OVF_vect)
{
  if (tone==0)
     PORTC = ~SPKR;  // turn off sound

  else if(tone==1)
  {	
     // toggle speaker port
     PORTC ^= SPKR;

     // reload timer value (read from table)
	 TCNT0 = pgm_read_byte(&C_major_scale[ctr]);
  }
}

unsigned char decode_sw(unsigned char x)
{
  switch (x)
  {
    case 0xFE:
       return 1;
       break;
    case 0xFD:
       return 2;
       break;
    case 0xFB:
       return 3;
       break;
    case 0xF7:
       return 4;
       break;
    default:
       return 0;
  }
  return x;
}
