/*
 * SegmentLCD_example.c
 *
 * Created: 2018-10-09 오전 12:34:10
 * Author : minhy
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

#define CS (1<<PA1)
#define MOSI (1<<PB5)
#define MISO (1<<PB6)
#define SCK (1<<PB7)
#define SS (1<<PB4)
#define CS_DDR DDRA
#define CS_ENABLE() (PORTA&= ~CS)
#define CS_DISABLE() (PORTA|= CS)
#define SYS_EN 0x01
#define BIAS_13_4COM 0x29
#define LCD_ON 0x03
#define LCD_OFF 0x02
#define TONE_OFF 0x08
#define TONE_ON 0x09

void SPI_Init()
{
	CS_DDR |= CS; // SDcard circuit select as output
	PORTA|=CS;
	DDRB = MOSI|SCK|SS; // MOSI and SCK as outputs
	//PORTB|=SS;
	//PORTB |= MISO; // pullup in MISO, might not be needed
	//PORTB&=~MISO;
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1);
}

void SPI_write(unsigned char ch) {
	//_delay_ms(2);
	SPDR = ch;
	while(!(SPSR & (1<<SPIF)));
	//return SPDR;
}

void LCD_command(unsigned char data)
{
	unsigned char temp[2];
	
	temp[0]=0x80|(data&0xf8)>>3;
	temp[1]=data<<5;
	
	CS_ENABLE();
	SPI_write(temp[0]);
	SPI_write(temp[1]);
	CS_DISABLE();
}

unsigned char LCD_data(unsigned int address, unsigned char data)
{
	unsigned char temp[3];
	unsigned int tempint;

	if(address>=8) return 1;
	else
	{
		tempint=0xa000|address<<7|(unsigned int)(data<<3);
		temp[0]=(unsigned char)((tempint&0xff00)>>8);
		temp[1]=(unsigned char)(tempint&0x00ff);
		/*
		if(address>=1)
		{
			temp[0]=0xa0|(1<<(address-1));
			temp[1]=data>>1;
		}
		else
		{
			temp[0]=0xa0;
			temp[1]=0x80|(data>>1);
		}
		*/
	}
	
	CS_ENABLE();
	SPI_write(temp[0]);
	SPI_write(temp[1]);
	CS_DISABLE();
	
	return 0;
}

unsigned char *convert_number(unsigned char ch, unsigned char *buffer)
{
	switch(ch)
	{
		case 0:
		buffer[0]=0x0b;
		buffer[1]=0x0e;
		break;
		
		case 1:
		buffer[0]=0x00;
		buffer[1]=0x06;
		break;
		
		case 2:
		buffer[0]=0x07;
		buffer[1]=0x0c;
		break;
		
		case 3:
		buffer[0]=0x05;
		buffer[1]=0x0e;
		break;
		
		case 4:
		buffer[0]=0x0c;
		buffer[1]=0x06;
		break;
		
		case 5:
		buffer[0]=0x0d;
		buffer[1]=0x0a;
		break;
		
		case 6:
		buffer[0]=0x0f;
		buffer[1]=0x0a;
		break;
		
		case 7:
		buffer[0]=0x08;
		buffer[1]=0x0e;
		break;
		
		case 8:
		buffer[0]=0x0f;
		buffer[1]=0x0e;
		break;
		
		case 9:
		buffer[0]=0x0d;
		buffer[1]=0x0e;
		break;
	}
	return buffer;
}

unsigned char LCD_int(unsigned int i)
{
	volatile unsigned char a, b, c, d;
	if(i>9999) return 1;

	a=i/1000;
	b=(i%1000)/100;
	c=(i%100)/10;
	d=i%10;

	unsigned char temp[2];
	convert_number(a,temp);
	LCD_data(0, temp[0]);
	LCD_data(1,temp[1]);
	convert_number(b,temp);
	LCD_data(2, temp[0]);
	LCD_data(3,temp[1]);
	convert_number(c,temp);
	LCD_data(4, temp[0]);
	LCD_data(5,temp[1]);
	convert_number(d,temp);
	LCD_data(6, temp[0]);
	LCD_data(7,temp[1]);
	
	return 0;
}

void LCD_Init()
{
	LCD_command(SYS_EN);
	LCD_command(BIAS_13_4COM);
	LCD_command(LCD_ON);
	LCD_command(TONE_OFF);
}

int main(void)
{
	int a=0;
	DDRA=0x01;
	PORTA=0x70;
	SPI_Init();
	LCD_Init();
    /* Replace with your application code */
    while (1) 
    {
		switch(PINA&0x70)
		{
			case 0x60:
			if(a<9999) a++;
			else a=0;
			PORTA|=(1<<0);
			LCD_int(a);
			_delay_ms(5);
			break;
			
			case 0x50:
			if(a>0) a--;
			else a=9999;
			PORTA|=(1<<0);
			LCD_int(a);
			_delay_ms(5);		
				
			default:
			PORTA&=~(1<<0);
			break;
		}
    }
}