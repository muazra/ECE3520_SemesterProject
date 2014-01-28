//Simon LED Game
//Muaz Rahman

//Clock 
#define F_CPU 368600

//Libraries
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

//Declarations
#define SPKR	 0x20
#define START  	    0
#define Level1 		1
#define Level2 		2
#define Level3 		3
#define Level4 		4
#define Level5 		5
#define WIN	   		6
#define Wait   		7

//Arrays
unsigned char LEDs[] = {0x8F, 0x4F, 0x2F, 0x1F};									//In order of board
unsigned char Buttons[] = {0x07, 0x0B, 0x0D, 0x0E};									//In order of board

//Program Memory Tables
unsigned char ButtonSounds[] PROGMEM = {237, 228, 241, 246};						//(Btn1, Btn2, Btn3, Btn4)

//Harry Potter Theme Music Melody
unsigned char StartGameSound[] PROGMEM = {241.423732, 234.1577101, 237.634646, 236.542652, 234.1577101, 241.423732, 236.542652, 234.1577101, 237.634646, 
										  236.542652, 231.487849, 235.4308036, 241.423732, 241.423732, 234.1577101, 237.634646, 236.542652, 234.1577101, 
										  241.423732, 231.487849, 231.487849, 228.4800506, 239.6381392, 228.4800506, 241.423732, 239.6381392, 239.6381392,
										  237.634646, 234.1577101, 237.634646, 241.423732, 237.634646, 241.423732, 237.634646, 228.4800506, 241.423732,
										  240.5510327, 236.542652, 237.634646, 241.423732, 240.5510327, 240.5510327, 241.423732, 241.423732, 237.634646,
										  241.423732, 237.634646, 241.423732, 237.634646, 231.487849, 231.487849, 228.4800506, 239.6381392, 228.4800506,
										  241.423732, 239.6381392, 240.5510327, 237.634646, 234.1841856};	//59 notes 
//Imperial March Melody 
unsigned char LoseRoundSound[] PROGMEM = {219.2689173, 219.2689173, 219.2689173, 209.7215228, 240.5564678, 219.2689173,
										  232.86, 240.5564678, 219.2689173, 231.4849276, 231.4849276, 231.4849276, 
										  232.8608358, 240.5564678, 217.084673, 209.7215228, 240.5564678, 219.2689173, 
										  219.2689173, 219.2689173, 219.2689173, 209.7215228, 240.5564678, 219.2689173,
										  232.86, 240.5564678, 219.2689173, 231.4849276, 231.4849276, 231.4849276, 
										  232.8608358, 240.5564678, 217.084673, 209.7215228, 240.5564678, 219.2689173, 
										  219.2689173, 219.2689173, 219.2689173, 209.7215228, 240.5564678};	//41 notes

//Payphone Maroon 5 Melody
unsigned char WinRoundSound[] PROGMEM = {231.487849, 234.1577101, 236.542652, 234.1577101, 230.0287924, 241.423732, 241.423732, 239.6381392, 235.3836806,
										 234.1577101, 241.423732, 234.1577101, 234.1577101, 231.487849, 241.423732, 234.1577101, 238.6650163, 231.487849,
										 230.0287924, 231.487849, 230.0287924, 231.487849, 234.1577101, 236.542652, 234.1178761, 230.0287924, 241.423732,
										 241.423732, 239.6381392, 236.542652, 234.1577101, 241.423732, 234.1577101};	//33 notes
//Castelvania Bloody Tears Melody
unsigned char WinGameSound[] PROGMEM = {241.423732, 235.3836806, 235.3836806, 234.1577101, 235.3836806, 231.487849,
										235.3836806, 228.4800506, 235.3836806, 231.487849, 235.3836806, 228.4800506, 
										241.423732, 235.3836806, 235.3836806, 228.4800506, 235.3836806, 231.487849,
										235.3836806, 234.1577101, 235.3836806, 231.487849, 235.3836806, 235.3836806,
										228.4800506, 241.423732, 235.3836806, 228.4800506, 241.423732, 235.3836806,
										235.3836806, 235.3836806, 235.3836806, 235.3836806, 234.1577101, 235.3836806,
										231.487849, 241.423732, 228.4800506, 235.3836806, 231.487849, 235.3836806, 
										228.4800506, 235.3836806, 235.3836806, 235.3836806, 228.4800506, 235.3836806,
										231.487849, 235.3836806, 234.1577101, 241.423732, 231.487849, 235.3836806,
										228.4800506, 235.3836806, 228.4800506};	//57 notes

//Variables
volatile unsigned char state = 0;
volatile unsigned char startISR = 0; 
volatile unsigned char x = 0x0F;
volatile unsigned char ctr;
volatile unsigned char var;
volatile unsigned char var1 = 0;
volatile unsigned char x_old = 0x0F;

//Level Answers Memory Tables - Buttons
unsigned char Answer1[] PROGMEM = {0x07};
unsigned char Answer2[] PROGMEM = {0x07, 0x0B};
unsigned char Answer3[] PROGMEM = {0x07, 0x0D, 0x0B};
unsigned char Answer4[] PROGMEM = {0x0B, 0x0E, 0x07, 0x0D};										
unsigned char Answer5[] PROGMEM = {0x07, 0x0E, 0x0B, 0x0D, 0x07};										

//Level Questions Memory Tables - LEDs
unsigned char Question1[] PROGMEM = {0x8F};
unsigned char Question2[] PROGMEM = {0x8F, 0xCF};
unsigned char Question3[] PROGMEM = {0x8F, 0xAF, 0xEF};
unsigned char Question4[] PROGMEM = {0x4F, 0x5F, 0xDF, 0xFF};
unsigned char Question5[] PROGMEM = {0x8F, 0x9F, 0xDF, 0xFF, 0x8F};																			

//Prototype Functions
void initialize();
void delayMethod(unsigned char);
void winMethod(unsigned char);
void loseMethod(unsigned char);
void playNOTE(unsigned char, unsigned char);
void winGameSequence();
void reset();


//Main Method Begin
int main(void)
{
  initialize();

	while(1)
	{
		switch(state)
		{
			case 0:							//Start of Game
				
				x = PINA;
				//Checking to see if button is pressed
				if(x!=0x0F)
				{
					_delay_ms(10);
					
					if(PINA!=0x0F)
					{
						startISR = 1;
				
						for(ctr = 0; ctr < 59; ctr++)	//Playing Start Game Sound	
							_delay_ms(2000);
						
						startISR = 0;
						state = 1;
					}
				}

				break;
			
			case 1:
				
				playNOTE(0,1);
				//Checking to see if correct button(s) are pressed
				while(1)
				{
					if(PINA!=0x0F)
					{
						if(PINA == pgm_read_byte(&Answer1[0]))
						{	
							winMethod(2);
							state = 2;
							break;
						}
						else if(PINA != pgm_read_byte(&Answer1[0]))
						{
							loseMethod(3);
							break;
						}	
					}
				}
				break;

			case 2:

				reset();
				playNOTE(0,1);
				playNOTE(1,2);

				_delay_ms(500);
				
				//Checking to see if correct button(s) are pressed
				while(1)
				{
					if((PINA != 0x0F) && (PINA != x_old))
					{	
						if(PINA == pgm_read_byte(&Answer2[var]))
						{
							PORTA = pgm_read_byte(&Question2[var]);
							var1++;
							if(var1 == 2)
							{
								winMethod(4);
								state = 3; 
								break;
							}
							else
							{
								x_old = PINA;
								var++;
								_delay_ms(500);
							}
						}
						else if((PINA != pgm_read_byte(&Answer2[var])) && (PINA != 0x0F) && (PINA != x_old))
						{
							loseMethod(3);
							break;
						}
					}
				}
				break;
			
			case 3:
				
				reset();
				playNOTE(0,1);
				playNOTE(2,2);
				playNOTE(1,3);

				_delay_ms(500);
				
				//Checking to see if correct button(s) are pressed
				while(1)
				{
					if((PINA != 0x0F) && (PINA != x_old))
					{
						_delay_ms(500);
						if(PINA == pgm_read_byte(&Answer3[var]))
						{
							PORTA = pgm_read_byte(&Question3[var]);
							var1++;
							if(var1 == 3)
							{
								winMethod(5);
								state = 4; 
								break;
							}
							else
							{
								x_old = PINA;
								var++;
								_delay_ms(500);
							}
							
						}
						else if((PINA != pgm_read_byte(&Answer3[var])) && (PINA != 0x0F) && (PINA != x_old))
						{
							loseMethod(4);
							break;
						}
					}
				}
				break;
		
			case 4:
				
				reset();
				playNOTE(1,1);
				playNOTE(3,2);
				playNOTE(0,3);
				playNOTE(2,4);

				_delay_ms(500);
				
				//Checking to see if correct button(s) are pressed
				while(1)
				{
					if((PINA != 0x0F) && (PINA != x_old))
					{
						if(PINA == pgm_read_byte(&Answer4[var]))
						{
							PORTA = pgm_read_byte(&Question4[var]);
							var1++;
							if(var1 == 4)
							{
								winMethod(6);
								state = 5; 
								break;
							}
							else
							{
								x_old = PINA;
								var++;
								_delay_ms(500);
							}
							
						}
						else if((PINA != pgm_read_byte(&Answer3[var])) && (PINA != 0x0F) && (PINA != x_old))
						{
							loseMethod(5);
							break;
						}
					}
				}
				break;
		
			case 5:
				
				reset();
				playNOTE(0,1);
				playNOTE(3,2);
				playNOTE(1,3);
				playNOTE(2,4);
				playNOTE(0,5);	
				
				_delay_ms(500);			
				
				while(1)
				{
					if((PINA != 0x0F) && (PINA != x_old))
					{
						if(PINA == pgm_read_byte(&Answer5[var]))
						{
							PORTA = pgm_read_byte(&Question5[var]);
							var1++;
							if(var1 == 5)
							{
								state = 6; 
								break;
							}
							else
							{
								x_old = PINA;
								var++;
								_delay_ms(500);
							}
							
						}
						else if((PINA != pgm_read_byte(&Answer3[var])) && (PINA != 0x0F) && (PINA != x_old))
						{
							loseMethod(6);
							break;
						}
					}
				}
				break;
		
			case 6:							//End of Game
						
				winGameSequence();
				state = 7;

				break;
		
			case 7:							//To restart game

				PORTA = 0x0F;
				if(PINA == 0x07)
				{
					_delay_ms(10);
					if(PINA == 0x07)
						state = 0;
				}
				else if(PINA != 0x07)
				{
					state = 7;
				}
				break;
		}
	}
}
					
ISR(TIMER0_OVF_vect)
{
  switch(state)
  {
	case 0:

		if(startISR == 1)
		{
			// toggle speaker port
     		PORTC ^= SPKR;

     		// reload timer value (read from table)
	 		TCNT0 = pgm_read_byte(&StartGameSound[ctr]);
		}
		break;

	case 1:
		
		if(startISR == 1)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}
		
		if(startISR == 2)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&WinRoundSound[ctr]);	
			break;
		}

		if(startISR == 3)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&LoseRoundSound[ctr]);	
			break;
		}
		
		break;
	
	case 2:
	
		if(startISR == 1)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}
		
		if(startISR == 2)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}
		
		if(startISR == 3)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&LoseRoundSound[ctr]);
			break;
		}

		if(startISR == 4)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&WinRoundSound[ctr]);	
			break;
		}

		break;
	
	case 3:

		if(startISR == 1)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}
		
		if(startISR == 2)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}
		
		if(startISR == 3)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}

		if(startISR == 4)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&LoseRoundSound[ctr]);	
			break;
		}

		if(startISR == 5)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&WinRoundSound[ctr]);	
			break;
		}

		break;
	
	case 4:

		if(startISR == 1)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}
		
		if(startISR == 2)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}
		
		if(startISR == 3)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}

		if(startISR == 4)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);	
			break;
		}

		if(startISR == 5)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&LoseRoundSound[ctr]);	
			break;
		}

		if(startISR == 6)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&WinRoundSound[ctr]);	
			break;
		}

		break;
	
	case 5:

		if(startISR == 1)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}
		
		if(startISR == 2)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}
		
		if(startISR == 3)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);
			break;
		}

		if(startISR == 4)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);	
			break;
		}

		if(startISR == 5)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&ButtonSounds[ctr]);	
			break;
		}
	
		if(startISR == 6)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&LoseRoundSound[ctr]);	
			break;
		}

		if(startISR == 7)
		{
			PORTC ^= SPKR;
			TCNT0 = pgm_read_byte(&WinRoundSound[ctr]);	
			break;
		}

		break;

	case 6:

		if(startISR == 1)
		{
			// toggle speaker port
     		PORTC ^= SPKR;

     		// reload timer value (read from table)
	 		TCNT0 = pgm_read_byte(&WinGameSound[ctr]);
		}
		break;
		
	}

}

void reset()
{
	var1 = 0;
	var = 0;
	x_old = 0x0F;
}

void playNOTE(unsigned char x, unsigned char isr)
{

	delayMethod(2);
	PORTA = LEDs[x];	
	
	startISR = isr;
				
	for(ctr = x; ctr < x+1; ctr++)
		_delay_ms(1000);

	_delay_ms(1000);
	PORTA = 0x0F;

	startISR = 0;
				
}


void initialize()
{
	// initialize speaker port
 	DDRC = 0xFF;  // set as output
  	PORTC = ~SPKR;  // turn off speaker

  	// initialize switch/LED port
  	DDRA = 0xF0;   // set PA0-3 as input
  	PORTA = 0x0F;  // pull-up switch ports

  	// initialize Timer0 interrupts
  	TCCR0 = 0x04; // prescaler = 256
  	TIMSK = 0x02; // initialize TOV0 interrupt
  	sei();
}

void delayMethod(unsigned char x)
{
	unsigned char y = 0;
	for(y = 0; y < x; y++)
		_delay_ms(1000);
}

void winMethod(unsigned char isr)
{		
	startISR = isr;
							
	for(ctr = 0; ctr < 33; ctr++)	//Playing Win Round Sound
	{ 	
		PORTA = LEDs[0];
		_delay_ms(500);
		PORTA = LEDs[1];
		_delay_ms(750);
		PORTA = LEDs[2];
		_delay_ms(750);
		PORTA = LEDs[3];
		_delay_ms(500);
	}
	
	PORTA = 0x0F;
	startISR = 0;
}

void loseMethod(unsigned char isr)
{
	
	startISR = isr;

	for(ctr = 0; ctr < 41; ctr++)	//Playing Lose Round Sound 
	{	
			PORTA = 0x9F;
			_delay_ms(1500);
			PORTA = 0x6F;
			_delay_ms(1500);
	}
	
	
	PORTA = 0x0F;		
	startISR = 0;
	state = 0;
}

void winGameSequence()
{		
	startISR = 1;			
	for(ctr = 0; ctr < 57; ctr++)	//Playing End Game Sound	
	{	
		PORTA = LEDs[0];
		_delay_ms(750);
		PORTA = LEDs[1];
		_delay_ms(1000);
		PORTA = LEDs[2];
		_delay_ms(1000);
		PORTA = LEDs[3];
		_delay_ms(750);;					
	}
	startISR = 0;

	startISR = 1;		
	for(ctr = 0; ctr < 57; ctr++)	//Playing End Game Sound	
	{		
			PORTA = 0x9F;
			_delay_ms(1500);
			PORTA = 0x6F;
			_delay_ms(1500);
	}					
	startISR = 0;

	PORTA = 0x0F;
}
