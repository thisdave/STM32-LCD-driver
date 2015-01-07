#include "lcd.h"

volatile uint32_t TickValue;

/*void SysTick_Handler (void)
{
	TickValue--;
}*/

void LCDclear(){
	unsigned i;
	/*for(i=0;i<16;i++){
			LCDpos(i,0);
			LCDstring(" ");
			LCDpos(i,1);
			LCDstring(" ");
		}*/
	LCDpos(0,0);
	LCDstring("                ");
	LCDpos(0,1);
	LCDstring("                ");
}

void LCDshow(char*a,char*b) {

	int k=0;

	CLEAR_RS();  // Initialize RS=0 for selecting instruction Send
	CLEAR_RW();  // Select RW=0 to write Instruction/data on LCD
	SET_EN();    // EN=1 for unlatch. (used at initial condition)

	delay_ms(COIN*5);
	LCD_DATA        (0x0); // Move cursor to beginning of first row
	delay_ms(COIN);
	s_latch(); 			   //Latch the above instruction
	delay_ms(COIN);
	LCD_DATA		(0x2);
	delay_ms(COIN);
			   //cursor back to the  beginning
	s_latch(); //Latch the above instruction
	delay_ms(COIN);

	GPIOC->BSRR=RS;  // Initialize RS=1 for selecting Data Send
	GPIOC->BRR=RW;  // Select RW=0 to write Instruction/data on LCD
	GPIOC->BSRR=EN; // EN=1 for unlatch. (used at initial condition)

	for(k=0;a[k];k++)
		{
			LCD_DATA		(a[k] >> 4); //It will send a[0]='P' as = '0x0050' on Port A.
			delay_us_100();
			s_latch(); //Latch the above instruction only once. Or it will clone each character twice if you latch twice.
			delay_us_100();
			LCD_DATA (a[k]); //It will send a[0]='P' as = '0x0050' on Port A.
			delay_us_100();
			s_latch(); //Latch the above instruction only once. Or it will clone each character twice if you latch twice.
			delay_us_100();
		}

	GPIOC->BRR  = RS; //Initialize RS=0 for selecting instruction Send
	GPIOC->BRR  = RW; // Select RW=0 to write Instruction/data on LCD
	GPIOC->BSRR = EN; // EN=1 for unlatch. (used at initial condition)

	delay_ms(COIN);
	LCD_DATA(0xC); // Move cursor to beginning of second row
	delay_us_100();
	s_latch(); //Latch the above instruction
	delay_ms(COIN);//
	LCD_DATA		(0x0);
	delay_us_100();
	s_latch();
	delay_ms(COIN);//
	GPIOC->BSRR=RS;  // Initialize RS=1 for selecting Data Send
	GPIOC->BRR=RW;  // Select RW=0 to write Instruction/data on LCD
	GPIOC->BSRR=EN; // EN=1 for unlatch. (used at initial condition)

	for(k=0;b[k];k++)
	{
		delay_us_100();
		LCD_DATA		((b[k]>>4)); //It will send b[0]='E' as = '0x0044' on Port A.
		delay_us_100();
		s_latch();//Latch the above instruction only once. Or it will clone each character twice if you latch twice.
		delay_us_100();
		LCD_DATA		((b[k])); //It will send b[0]='E' as = '0x0044' on Port A.
		delay_us_100();
		s_latch();//Latch the above instruction only once. Or it will clone each character twice if you latch twice.
		delay_us_100();
	}
	//s_init();
}



void LCDstring(const char *msg)
{
	/*****************************************************************

	This function Writes a given string to lcd at the current cursor
	location.

	Arguments:
	msg: a null terminated string to print


	*****************************************************************/
 while(*msg!='\0')
 {
	 delay_ms(COIN);
	LCD_DATA(*msg >> 4);
	s_latch();
	delay_ms(COIN);
	LCD_DATA(*msg);
	s_latch();
	delay_ms(COIN);
	msg++;
 }
}

void LCDint(int val, unsigned int field_length)
{
	/***************************************************************
	This function writes a integer type value to LCD module

	Arguments:
	1)int val	: Value to print

	2)unsigned int field_length :total length of field in which the value is printed
	must be between 1-5 if it is -1 the field length is no of digits in the val

	****************************************************************/

	char str[5]= {0,0,0,0,0};
	int i=4,j=0;
	while(val)
	{
	str[i]=val%10;
	val=val/10;
	i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
		j=5-field_length;

	if(val<0) {
				LCD_DATA('-'>>4);
				s_latch();
				delay_ms(COIN);
				LCD_DATA('-');
				s_latch();
				delay_ms(COIN);
				}
	for(i=j;i<5;i++)
	{
	LCD_DATA((48+str[i])>>4);
	s_latch();
	delay_ms(COIN);
	LCD_DATA((48+str[i]));
	s_latch();
	delay_ms(COIN);
	}
}

void LCD_init()
{
	GPIOC->BRR=RS; //Initialize RS=0 for selecting instruction Send
	GPIOC->BRR=RW; // Select RW=0 to write Instruction/data on LCD
	GPIOC->BRR=EN; // EN=1 for unlatch. strobe (used at initial condition)
	//After power on Wait for LCD to Initialize
	delay_ms(30);

	delay1per3us();//tAS  most: 0.8us
	SET_EN();
	LCD_DATA(0x2);
	//mérés szerint 1.6 us:
	delay1per3us();
	delay1per3us();
	CLEAR_EN();
	//mérés szerint 1.6 us:
	delay1per3us();
	delay1per3us();

	//wait for the LCD to execute functionset command:
	delay_ms(20);

	//display ON: kurzor, mutató be
	LCD_DATA(0x0);
	s_latch();
	LCD_DATA(0xF);
	s_latch();

	//function set 4 bit, 2 line, 5x7 dot format
	LCD_DATA(0x2);
	s_latch();
	LCD_DATA(0x8);
	s_latch();

	//képernyõ törlés:
	LCD_DATA(0x0);
	s_latch();
	LCD_DATA(0x1);
	s_latch();

	//Cursor To Home:
	LCD_DATA(0x0);
	s_latch();
	LCD_DATA(0x2);
	s_latch();

	/*//Set RS
	GPIOC->BSRR=RS;
	LCD_DATA('A'>>4);
	s_latch();
	LCD_DATA('A');
	s_latch();*/

}

void LCDpos(uint8_t x,uint8_t y)
{
	GPIOC->BRR=RS; //Initialize RS=0 for selecting instruction Send
	GPIOC->BRR=RW; // Select RW=0 to write Instruction/data on LCD
	GPIOC->BRR=EN; // EN=1 for unlatch. strobe (used at initial condition)

 if(x<40)
 {
  if(y) x|=0b01000000;
  x|=0b10000000;
  LCD_DATA(x>>4);
  s_latch();
  delay_ms(COIN);
  LCD_DATA(x);
  s_latch();
  delay_ms(COIN);
  }

 	GPIOC->BSRR=RS; //Initialize RS=0 for selecting Data Send
	GPIOC->BRR=RW; // Select RW=0 to write Instruction/data on LCD
	GPIOC->BRR=EN; // EN=1 for unlatch. strobe (used at initial condition)
}

char Hexa4ToChar(char digit){
	switch(digit)
		{
			case  0 : return '0'; break;		case 1 : return '1'; break;
			case  2 : return '2'; break;		case 3 : return '3'; break;
			case  4 : return '4'; break;		case 5 : return '5'; break;
			case  6 : return '6'; break;        case 7 : return '7'; break;
			case  8 : return '8'; break;    	case 9 : return '9'; break;
			case 10 : return 'A'; break; 		case 11 : return 'B'; break;
			case 12 : return 'C'; break;		case 13 : return 'D'; break;
			case 14 : return 'E'; break;		case 15 : return 'F'; break;
			default: return 0;
		}
	return 0;
}

void LCD_Hexa32(uint32_t num32,const char * endianness)
{
	LCDpos(0,1);
	//endianness= LE var is Little Endien
	uint32_t inv_num;
	char buffer [9];
	uint32_t i;
	if ( !strcmp(endianness,"LITTLE_E") )
		{

		//4 bajt megforditasa:
		for(i=0;i<4;i++)
			( (char*)(&inv_num) )[i]=( (char*)(&num32) )[3-i];
			num32 = inv_num;
		}

		/*for(j=0; j <4; j++)
		LCDWriteInt( (uint_fast16_t)(( (char*)(&num32) )[j] ),3);*/

		for(i=0;i<4;i++){
			buffer[i*2]= Hexa4ToChar(
										(char)(   ( (char*)(&num32) )[i]  )  >> 4 & 0x0F
									);
			buffer[i*2 + 1]= Hexa4ToChar(
										(char)(   ( (char*)(&num32) )[i]  )       & 0x0F
									);
						}

		buffer[8] = '\0';

	LCDstring(buffer);
}
/*
void SHOWREG(uint_fast16_t regname)
{
	LCDpos(0,1);
	LCD_Hexa32( (uint32_t) (  ( (uint32_t) 0x0000FFFF ) & (uint16_t) (regname) ),"LITTLE_E");
}*/

/*void SHOW_Uval16(uint_fast16_t val){
	LCDpos(0,1);
	LCD_Hexa32( (uint32_t) (  ( (uint32_t) 0x00000000 ) | (uint16_t) (val) ),"LITTLE_E");
}

*/
