

#define  F_CPU 11059200UL
#include "LCD16x2_4bit.h"
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#define  is_bit_set(byte,bit) (byte & (1<<bit))
#define  is_bit_clr(byte,bit) (!(byte & (1<<bit)))
#define BUAD	57600
#define BRC		((F_CPU/16/BUAD) - 1)
#define ee_address 0;

char password[4]="8051";
volatile char enter[4];
int check;
////////////////////////	KEYAPD	/////////////////////////////
#define KEY_PRTr 	PORTD
#define KEY_DDRr	DDRD
#define KEY_PINr	PIND

#define KEY_PRTc 	PORTB
#define KEY_DDRc	DDRB
#define KEY_PINc	PINB

unsigned char keypad[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}};


	unsigned char colloc, rowloc;
unsigned int keypadint[4][4] = {
	{1,2,3,10},
	{4,5,6,11},
	{7,8,9,12},
	{13,0,14,15}};

	unsigned char colloc, rowloc;

	int keyfindint()
	{
		// so lower nibble col upper nibble rows
		while(1)
		{
			//KEY_DDR = 0xF0;           /* set port direction as input-output rows out cols input */
			//KEY_PRT = 0xFF; //0xff original
			KEY_DDRr |= (1<<7) | (1<<6) | (1<<5) | (1<<4); //rows
			
			KEY_DDRc &= ~ (1<<3) | (1<<2) | (1<<1) | (1<<0); // cols
			
			KEY_PRTr &= ~ ((1<<7) | (1<<6) | (1<<5) | (1<<4)); //rows
			KEY_PRTc |=  (1<<3) | (1<<2) | (1<<1) | (1<<0); // cols
			do
			{
				//KEY_PRTr = 0x00; //rows
				KEY_PRTc &= 0x0F;      /* mask PORT for column read only */
				asm("NOP");
				colloc = (KEY_PINc & 0x0F); /* read status of column will be like 0000-1101 this will be second col*/
			}while(colloc != 0x0F);
			
			do
			{
				do
				{
					_delay_ms(20);             /* 20ms key debounce time */
					colloc = (KEY_PINc & 0x0F); /* read status of column */
					}while(colloc == 0x0F);        /* check for any key press */
					
					_delay_ms (40);	            /* 20 ms key debounce time */
					colloc = (KEY_PINc & 0x0F);
				}while(colloc == 0x0F);
				/* now check for rows */
				//KEY_PRT = 0xEF;            /* check for pressed key in 1st row */
				KEY_PRTr |= (1<<7) | (1<<6) | (1<<5) ; //rows
				KEY_PRTr &= ~(1<<4);
				KEY_PRTc |= (1<<3) | (1<<2) | (1<<1)| (1<<0);//cols
				asm("NOP");
				colloc = (KEY_PINc & 0x0F);
				if(colloc != 0x0F)
				{
					rowloc = 0;
					break;
				}

				//KEY_PRT = 0xDF;		/* check for pressed key in 2nd row */
				KEY_PRTr |= (1<<7) | (1<<6) | (1<<4) ; //rows
				KEY_PRTr &= ~(1<<5);
				KEY_PRTc |= (1<<3) | (1<<2) | (1<<1)| (1<<0);//cols
				asm("NOP");
				colloc = (KEY_PINc & 0x0F);
				if(colloc != 0x0F)
				{
					rowloc = 1;
					break;
				}
				
				//KEY_PRT = 0xBF;		/* check for pressed key in 3rd row */
				KEY_PRTr |= (1<<7) | (1<<5) | (1<<4) ; //rows
				KEY_PRTr &= ~(1<<6);
				KEY_PRTc |= (1<<3) | (1<<2) | (1<<1)| (1<<0);//cols
				asm("NOP");
				colloc = (KEY_PINc & 0x0F);
				if(colloc != 0x0F)
				{
					rowloc = 2;
					break;
				}

				//KEY_PRT = 0x7F;		/* check for pressed key in 4th row */
				KEY_PRTr |= (1<<6) | (1<<5) | (1<<4) ; //rows
				KEY_PRTr &= ~(1<<7);
				KEY_PRTc |= (1<<3) | (1<<2) | (1<<1)| (1<<0);//cols
				asm("NOP");
				colloc = (KEY_PINc & 0x0F);
				if(colloc != 0x0F)
				{
					rowloc = 3;
					break;
				}
			}

			if(colloc == 0x0E)
			return(keypadint[rowloc][0]);
			else if(colloc == 0x0D)
			return(keypadint[rowloc][1]);
			else if(colloc == 0x0B)
			return(keypadint[rowloc][2]);
			else
			return(keypadint[rowloc][3]);
		}
		////////////////////////////////////////////////////
		
		char int_to_char(int x)
		{
			switch(x)
			{
				case 0:
				return '0';
				break;
				case 1:
				return '1';
				break;
				case 2:
				return '2';
				break;
				case 3:
				return '3';
				break;
				case 4:
				return '4';
				break;
				case 5:
				return '5';
				break;
				case 6:
				return '6';
				break;
				case 7:
				return '7';
				break;
				case 8:
				return '8';
				break;
				case 9:
				return '9';
				break;
				default:
				return 'x';
			}
		}
//////////////////////////////////////////////////////
	char keyfind()
	{
		// so lower nibble col upper nibble rows
		while(1)
		{
			//KEY_DDR = 0xF0;           /* set port direction as input-output rows out cols input */
			//KEY_PRT = 0xFF; //0xff original
			KEY_DDRr |= (1<<7) | (1<<6) | (1<<5) | (1<<4); //rows
			
			KEY_DDRc &= ~ (1<<3) | (1<<2) | (1<<1) | (1<<0); // cols
			
			KEY_PRTr &= ~ ((1<<7) | (1<<6) | (1<<5) | (1<<4)); //rows
			KEY_PRTc |=  (1<<3) | (1<<2) | (1<<1) | (1<<0); // cols
			do
			{
				//KEY_PRTr = 0x00; //rows
				KEY_PRTc &= 0x0F;      /* mask PORT for column read only */
				asm("NOP");
				colloc = (KEY_PINc & 0x0F); /* read status of column will be like 0000-1101 this will be second col*/
			}while(colloc != 0x0F);
			
			do
			{
				do
				{
					_delay_ms(20);             /* 20ms key debounce time */
					colloc = (KEY_PINc & 0x0F); /* read status of column */
					}while(colloc == 0x0F);        /* check for any key press */
					
					_delay_ms (40);	            /* 20 ms key debounce time */
					colloc = (KEY_PINc & 0x0F);
				}while(colloc == 0x0F);
				/* now check for rows */
				//KEY_PRT = 0xEF;            /* check for pressed key in 1st row */
				KEY_PRTr |= (1<<7) | (1<<6) | (1<<5) ; //rows
				KEY_PRTr &= ~(1<<4);
				KEY_PRTc |= (1<<3) | (1<<2) | (1<<1)| (1<<0);//cols
				asm("NOP");
				colloc = (KEY_PINc & 0x0F);
				if(colloc != 0x0F)
				{
					rowloc = 0;
					break;
				}

				//KEY_PRT = 0xDF;		/* check for pressed key in 2nd row */
				KEY_PRTr |= (1<<7) | (1<<6) | (1<<4) ; //rows
				KEY_PRTr &= ~(1<<5);
				KEY_PRTc |= (1<<3) | (1<<2) | (1<<1)| (1<<0);//cols
				asm("NOP");
				colloc = (KEY_PINc & 0x0F);
				if(colloc != 0x0F)
				{
					rowloc = 1;
					break;
				}
				
				//KEY_PRT = 0xBF;		/* check for pressed key in 3rd row */
				KEY_PRTr |= (1<<7) | (1<<5) | (1<<4) ; //rows
				KEY_PRTr &= ~(1<<6);
				KEY_PRTc |= (1<<3) | (1<<2) | (1<<1)| (1<<0);//cols
				asm("NOP");
				colloc = (KEY_PINc & 0x0F);
				if(colloc != 0x0F)
				{
					rowloc = 2;
					break;
				}

				//KEY_PRT = 0x7F;		/* check for pressed key in 4th row */
				KEY_PRTr |= (1<<6) | (1<<5) | (1<<4) ; //rows
				KEY_PRTr &= ~(1<<7);
				KEY_PRTc |= (1<<3) | (1<<2) | (1<<1)| (1<<0);//cols
				asm("NOP");
				colloc = (KEY_PINc & 0x0F);
				if(colloc != 0x0F)
				{
					rowloc = 3;
					break;
				}
			}

			if(colloc == 0x0E)
			return(keypad[rowloc][0]);
			else if(colloc == 0x0D)
			return(keypad[rowloc][1]);
			else if(colloc == 0x0B)
			return(keypad[rowloc][2]);
			else
			return(keypad[rowloc][3]);
		}
///////////////////////////////////////////////////////////////////

///////////// password writing comparing changing code///////////////////
		int match_pass()
		{
			int x=0;
			for(int i=0 ; i<4 ; i++)
			{
				if ( (*(password+i)==*(enter+i)) )
				x++;
				
			}
			if (x==4)
			return 1;
			else
			return 0;
		}
		void pass_enter()
		{
			unsigned int i=0;
			
			for (i=0 ; i<4 ; i++)
			{
				enter[i] = keyfind();
				LCD_Char(enter[i]);;
			}
		}
		void pass_change()
		{
			unsigned char x;
			for (int i=0;i<4;i++)
			{
				x=keyfind();
				*(password+i)=x;
				eeprom_write_byte((uint8_t*)0+i,x);
				LCD_Char(x);
			}
		}
		void pass_init()
		{
			for (int i=0;i<4;i++)
			*(password+i)=eeprom_read_byte((uint8_t*)0+i);
		}
////////////////////////////////////////////////////////////////////////////////
		
		///////////////////////// low level func for uart////////////////////////////////
uint8_t * rxbytes(int x)
{
	static uint8_t y[15];
	for(int i=0 ; i < x ;i++)
	{
		while(is_bit_clr(UCSRA,RXC));
		y[i]=UDR;	
	}
	
	return y;
}


void txbytes(uint8_t *x, int length)
{
	for(int i= 0 ;  i < length; i++)
	{
		UDR = *x++;
		while(is_bit_clr(UCSRA,UDRE));
	}
}
/////////////////////////////////////////////////////////////////////////////


////////////////////////fingerprint cmds//////////////////////
int VfyPwd()
{
	/*uint8_t x[15];
	for(int i=0 ; i<15 ;i++)
	{
		while(is_bit_clr(UCSR0A,RXC0));
		x[i]=UDR0;
	}*/
	
	
	uint8_t y[] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1B};
	txbytes(y,sizeof(y));
	//PORTB |= (1<<2);
	uint8_t *data =rxbytes(12);
	//PORTB &= ~(1<<2);
	if(*(data+9)==0x00)
	{
	return 1;
	}
	else
	return 0;
	
}
int GenImg()
{
	
	uint8_t y[] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};
	txbytes(y,sizeof(y));
	//PORTB |= (1<<2);
	uint8_t *data =rxbytes(12);
	//PORTB &= ~(1<<2);
	if(*(data+9)==0x00)
	{
	return 0;
	}
	else
	return 1;
	
}
int img2Tz1()
{
	
	uint8_t y[] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08};
	txbytes(y,sizeof(y));
	uint8_t *data =rxbytes(12);
	if(*(data+9)==0x00)
	{
		return 1;
	}
	else
	return 0;
	
}
int img2Tz2()
{
	
	
	uint8_t y[] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x02,0x00,0x09};
	txbytes(y,sizeof(y));
	uint8_t *data =rxbytes(12);
	if(*(data+6)==0x01)
	{
		return 1;
	}
	else
	return 0;
	}
int search()
{
	
	
	
	uint8_t y[] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x01,0x00,0xFF,0x01,0x0E};
	txbytes(y,sizeof(y));
	
	uint8_t *data =rxbytes(16);
	if(*(data+9)==0x00)
	{
		return 1;
	}
	else
	return 0;
}

int RegModel()
{
	
	
	uint8_t y[] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x05,0x00,0x09};
	txbytes(y,sizeof(y));
	uint8_t *data =rxbytes(12);
	if(*(data+9)==0x00)
	{
		return 1;
	}
	else
	return 0;
	
}
int Store(int x)
{
	uint8_t y[] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x06,0x06,0x02,0x00,x,0x00,0x0F+x};//edit check sum
	txbytes(y,sizeof(y));
	
	uint8_t *data =rxbytes(12);
	if(*(data+9)==0x00)
	{
		return 1;
	}
	else
	return 0;
}

void fingerprint_search()
{
	int x;
	LCD_String("Place Finger :)");
	while(GenImg());
	LCD_Clear();
		PORTB &= ~(1<<0);
		if(img2Tz1())
		PORTB |= (1<<0);
		x=search();
		if(x==1)
		{
			PORTB |= (1<<1);
			LCD_Clear();
			LCD_String_xy(1,0,"Door Unlocked :)");
		}
		if(x==0)
		{
			LCD_Clear();
			PORTB &= ~(1<<1);
			LCD_String_xy(1,0,"Wrong finger");
			LCD_String_xy(2,0,"Try again");
		}
		
		
}
int enterid()
{
	int char1;
	int char2;
	int result;
	LCD_String("Enter two digit");
	LCD_String_xy(2,0,"id:");
	char1=keyfindint();
	LCD_Char(int_to_char(char1));
	char2=keyfindint();
	LCD_Char(int_to_char(char2));
	_delay_ms(500);
	result = char1*10 + char2;
	return result;
}
void fingerprint_change()
{
	int id;
	id=enterid();
	LCD_Clear();
	LCD_String("Place finger");
	while(GenImg());
	LCD_Clear();
	if (img2Tz1()==1)
	LCD_String("finger stored 1");
	
	_delay_ms(2000);
	
	LCD_Clear();
	LCD_String("Place same ");
	LCD_String_xy(2,0,"finger again");
	while(GenImg());
	LCD_Clear();
	if (img2Tz2()==1)
	LCD_String("finger stored 2");
	LCD_Clear();
	if(RegModel()==1)
	{
		LCD_String("Got finger");
		PORTB |= (1<<1);
		_delay_ms(500);
		LCD_Clear();
		if(Store(id)==1)
		{
			LCD_String("finger saved :)");
			PORTB |= (1<<2);
		}
		else
		{
			LCD_String("finger not saved :(");
			PORTB &= ~(1<<2);
		}
		
	}
	else
	{
		LCD_String("finger is not");
		LCD_String_xy(2,0,"same :(");
		PORTB &= ~(1<<1);
		_delay_ms(500);
	}
	
	//LCD_Clear();
	
	/*if(Store(id)==1)
	{
		LCD_String("finger saved :)");
		PORTB |= (1<<2);
	}
	else
	{
		LCD_String("finger not saved :(");
		PORTB &= ~(1<<2);
	}*/
	//_delay_ms(1000);
}

////////////////////////////////////////////////////////////////////////////


///////////////////////////		MAIN	///////////////////////////
		int main(void)
		{
			int pass_count=0;
			
			pass_init();//to read previous pass from eeprom
			
			unsigned char x;
			unsigned char x1;
			DDRC =(1<<5);
			//DDRB |= (1<<0) | (1<<1) | (1<<2);
			LCD_Init();
			UBRRH = (BRC >> 8);
			UBRRL =  BRC;
			//UCSRA &= ~(1<<U2X);
			UCSRB |= (1<<TXEN) | (1<<RXEN);
			//LCD_Command(0xc0);  //move to second row
			DDRD=0xFC;
			
			 if (VfyPwd())
				  PORTB |= (1<<0);
			  
			while(1)
			{
				LCD_String_xy(1,0,"1 to change password");
				LCD_String_xy(2,0,"2 to Continue...");
				x=keyfind();
				LCD_Clear();
				if (x=='1')////changing routine//
				{
					LCD_String_xy(1,0,"1 to change password");
					LCD_String_xy(2,0,"2 to change prints");
					x1=keyfind();
					LCD_Clear();
					if (x1=='1')
					{
					LCD_Command(0x01); //to clear
					LCD_String_xy(1,0,"Enter old pass:");
					LCD_String_xy(2,0,"");
					pass_enter();
					if (match_pass()==1)
					{
						LCD_Command(0x01); //to clear
						LCD_String_xy(1,0,"Enter new pass:");
						LCD_String_xy(2,0,"");
						pass_change();
						LCD_Command(0x01); //to clear
						LCD_String_xy(1,0,"Pass changed :)");
					}
					else
					{
						LCD_Command(0x01); //to clear
						LCD_String_xy(1,0,"Wrong password ");
						
					}
					}
					
					
					if (x1=='2')
					{
						LCD_Command(0x01); //to clear
						LCD_String_xy(1,0,"Enter old pass:");
						LCD_String_xy(2,0,"");
						pass_enter();
						if (match_pass()==1)
						{
							LCD_Clear();
							fingerprint_change();
						}
						else
						{
							LCD_Command(0x01); //to clear
							LCD_String_xy(1,0,"Wrong password ");
							
						}
						
					}
					
					
					_delay_ms(1000);
				}
				///////////////////////////////////
				
				///////// Lock password checking code//////////
				if(x=='2')
				{
					LCD_Command(0x01); //to clear
					LCD_String_xy(1,0,"1 to write pass");
					LCD_String_xy(2,0,"2 to use Fingerprint");
					x=keyfind();
					if (x=='1')
					{
					LCD_Command(0x01); //to clear
					LCD_String_xy(1,0,"Password:");
					pass_enter();
					//LCD_Char(keyfind());       /* Display which key is pressed */
					LCD_Command(0x01); //to clear
					check=match_pass();
					if (check==1)
					{
						LCD_String("Correct pass :))");
						LCD_String_xy(2,0,"Door unlocked");
						PORTC |= (1<<5);
						pass_count=0;
					}
					
					else
					{
						LCD_String("Wrong Pass :(");
						LCD_String_xy(2,0,"Try again");
						PORTC &= ~(1<<5);
						pass_count += 1;
					}
					
					if(pass_count==4)
					{
						LCD_String("Wait for 5 sec");
						_delay_ms(5000);
						pass_count=0;
						LCD_Command(0x01); //to clear
						
					}
					}
					if (x=='2')
					{
						LCD_Command(0x01); //to clear
					
						DDRD=0xFC;
						fingerprint_search();
					}
					_delay_ms(1000);
					LCD_Command(0x01); //to clear
				}
			}
		}
