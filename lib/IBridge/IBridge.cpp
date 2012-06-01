/* -*- mode: c++ -*- */
#include <Arduino.h>
#include <avr/pgmspace.h>
#include "Font.h"
extern unsigned char font6_8[92][6];

int IBridge_Column_Pin0 = 7;
int IBridge_Column_Pin1 = 6;
int IBridge_Column_Pin2 = 5;
int IBridge_Column_Pin3 = 4;

int IBridge_Row_Pin0 = 3;
int IBridge_Row_Pin1 = 2;
int IBridge_Row_Pin2 = 18;
int IBridge_Row_Pin3 = 19;

int IBridge_LCD5110_SCK_PIN = 8;
int IBridge_LCD5110_MO_PIN = 9;
int IBridge_LCD5110_DC_PIN = 10;
int IBridge_LCD5110_RST_PIN = 11;
int IBridge_LCD5110_CS_PIN = 12;

//Define the LCD Operation function
void IBridge_LCD5110_LCD_write_byte(unsigned char dat,unsigned char IBridge_LCD5110_Mode);


//Define the hardware operation function
void IBridge_GPIO_Config(void);
void IBridge_LCD5110_SCK(unsigned char temp);
void IBridge_LCD5110_MO(unsigned char temp);
void IBridge_LCD5110_CS(unsigned char temp);
void IBridge_LCD5110_RST(unsigned char temp);
void IBridge_LCD5110_DC(unsigned char temp);

void IBridge_LCD5110_clear();

void IBridge_init()
{
	IBridge_GPIO_Config();

	IBridge_LCD5110_DC(1);//LCD_DC = 1;
	IBridge_LCD5110_MO(1);//SPI_MO = 1;
	IBridge_LCD5110_SCK(1);//SPI_SCK = 1;
	IBridge_LCD5110_CS(1);//SPI_CS = 1;
	
	IBridge_LCD5110_RST(0);//LCD_RST = 0;
	delay(10);
	IBridge_LCD5110_RST(1);//LCD_RST = 1;

	IBridge_LCD5110_LCD_write_byte(0x21,0);
	IBridge_LCD5110_LCD_write_byte(0xc6,0);
	IBridge_LCD5110_LCD_write_byte(0x06,0);
	IBridge_LCD5110_LCD_write_byte(0x13,0);
	IBridge_LCD5110_LCD_write_byte(0x20,0);
	IBridge_LCD5110_clear();
	IBridge_LCD5110_LCD_write_byte(0x0c,0);
}

void IBridge_LCD5110_LCD_write_byte(unsigned char dat,unsigned char mode)
{
	unsigned char i;

	IBridge_LCD5110_CS(0);//SPI_CS = 0;

	if (0 == mode)
		IBridge_LCD5110_DC(0);//LCD_DC = 0;
	else
		IBridge_LCD5110_DC(1);//LCD_DC = 1;

	for(i=0;i<8;i++)
	{
		IBridge_LCD5110_MO(dat & 0x80);//SPI_MO = dat & 0x80;
		dat = dat<<1;
		IBridge_LCD5110_SCK(0);//SPI_SCK = 0;
		IBridge_LCD5110_SCK(1);//SPI_SCK = 1;
	}

	IBridge_LCD5110_CS(1);//SPI_CS = 1;

}

void IBridge_LCD5110_write_char(unsigned char c)
{
	unsigned char line;
	unsigned char ch = 0;

	c = c - 32;

	for(line=0;line<6;line++)
	{
		ch = pgm_read_byte(&(font6_8[c][line]));
		IBridge_LCD5110_LCD_write_byte(ch,1);
		
	}
}
void IBridge_LCD5110_write_char_reg(unsigned char c)
{
	unsigned char line;
	unsigned char ch = 0;

	c = c - 32;

	for(line=0;line<6;line++)
	{
		ch = ~(pgm_read_byte(&(font6_8[c][line])));
		IBridge_LCD5110_LCD_write_byte(ch,1);
		
	}
}

void IBridge_LCD5110_write_string(char *s)
{
	unsigned char ch;
  	while(*s!='\0')
	{
		ch = *s;
		IBridge_LCD5110_write_char(ch);
		s++;
	}
}


void IBridge_LCD5110_clear()
{
	unsigned char i,j;
	for(i=0;i<6;i++)
		for(j=0;j<84;j++)
			IBridge_LCD5110_LCD_write_byte(0,1);
}

void IBridge_LCD5110_set_XY(unsigned char X,unsigned char Y)
{
	unsigned char x;
	x = 6*X;
	//y = 8*Y;

	IBridge_LCD5110_LCD_write_byte(0x40|Y,0);
	IBridge_LCD5110_LCD_write_byte(0x80|x,0);
}

void IBridge_LCD5110_Write_Dec(unsigned int b)
{
	unsigned char datas[3];

	datas[0] = b/1000;
	b = b - datas[0]*1000;
	datas[1] = b/100;
	b = b - datas[1]*100;
	datas[2] = b/10;
	b = b - datas[2]*10;
	datas[3] = b;

	datas[0]+=48;
	datas[1]+=48;
	datas[2]+=48;
	datas[3]+=48;

	IBridge_LCD5110_write_char(datas[0]);
	IBridge_LCD5110_write_char(datas[1]);
	IBridge_LCD5110_write_char(datas[2]);
	IBridge_LCD5110_write_char(datas[3]);

	
}

void IBridge_GPIO_Config()
{
  pinMode(IBridge_LCD5110_SCK_PIN, OUTPUT);
  pinMode(IBridge_LCD5110_MO_PIN, OUTPUT);
  pinMode(IBridge_LCD5110_DC_PIN, OUTPUT);
  pinMode(IBridge_LCD5110_RST_PIN, OUTPUT);
  pinMode(IBridge_LCD5110_CS_PIN, OUTPUT);
  
  pinMode(IBridge_Column_Pin0, OUTPUT);
  pinMode(IBridge_Column_Pin1, OUTPUT);
  pinMode(IBridge_Column_Pin2, OUTPUT);
  pinMode(IBridge_Column_Pin3, OUTPUT);

  pinMode(IBridge_Row_Pin0, INPUT);
  pinMode(IBridge_Row_Pin1, INPUT);
  pinMode(IBridge_Row_Pin2, INPUT);
  pinMode(IBridge_Row_Pin3, INPUT);
}

void IBridge_LCD5110_CS(unsigned char temp)
{
	if (temp) digitalWrite(IBridge_LCD5110_CS_PIN, HIGH);
	else digitalWrite(IBridge_LCD5110_CS_PIN, LOW);
}

void IBridge_LCD5110_RST(unsigned char temp)
{
	if (temp) digitalWrite(IBridge_LCD5110_RST_PIN, HIGH);
	else digitalWrite(IBridge_LCD5110_RST_PIN, LOW);
}

void IBridge_LCD5110_DC(unsigned char temp)
{
	if (temp) digitalWrite(IBridge_LCD5110_DC_PIN, HIGH);
	else digitalWrite(IBridge_LCD5110_DC_PIN, LOW);
}

void IBridge_LCD5110_MO(unsigned char temp)
{
	if (temp) digitalWrite(IBridge_LCD5110_MO_PIN, HIGH);
	else digitalWrite(IBridge_LCD5110_MO_PIN, LOW);
}

void IBridge_LCD5110_SCK(unsigned char temp)
{
	if (temp) digitalWrite(IBridge_LCD5110_SCK_PIN, HIGH);
	else digitalWrite(IBridge_LCD5110_SCK_PIN, LOW);
}

unsigned char IBridge_Read_Key()
{
  //unsigned char i = 10;

  //Column 0 scan

  
  digitalWrite(IBridge_Column_Pin1, LOW);
  digitalWrite(IBridge_Column_Pin2, LOW);
  digitalWrite(IBridge_Column_Pin3, LOW);
  digitalWrite(IBridge_Column_Pin0, HIGH);
 
  //i=10;
  //while(i--);
  delay(1);

  if((digitalRead(IBridge_Row_Pin0)) &&
    !(digitalRead(IBridge_Row_Pin1))  &&
    !(digitalRead(IBridge_Row_Pin2))  &&
    !(digitalRead(IBridge_Row_Pin3)))
    return (1);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
      (digitalRead(IBridge_Row_Pin1))  &&
     !(digitalRead(IBridge_Row_Pin2))  &&
     !(digitalRead(IBridge_Row_Pin3)))
    return (2);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
     !(digitalRead(IBridge_Row_Pin1))  &&
      (digitalRead(IBridge_Row_Pin2))  &&
     !(digitalRead(IBridge_Row_Pin3)))
    return (3);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
     !(digitalRead(IBridge_Row_Pin1))  &&
     !(digitalRead(IBridge_Row_Pin2))  &&
      (digitalRead(IBridge_Row_Pin3)))
    return (4);

  //Column 2 Scan

  digitalWrite(IBridge_Column_Pin0, LOW);
  digitalWrite(IBridge_Column_Pin1, HIGH);
  digitalWrite(IBridge_Column_Pin2, LOW);
  digitalWrite(IBridge_Column_Pin3, LOW);

  //i=10;
  //while(i--);
  delay(1);

  if((digitalRead(IBridge_Row_Pin0)) &&
    !(digitalRead(IBridge_Row_Pin1))  &&
    !(digitalRead(IBridge_Row_Pin2))  &&
    !(digitalRead(IBridge_Row_Pin3)))
    return (5);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
      (digitalRead(IBridge_Row_Pin1))  &&
     !(digitalRead(IBridge_Row_Pin2))  &&
     !(digitalRead(IBridge_Row_Pin3)))
    return (6);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
     !(digitalRead(IBridge_Row_Pin1))  &&
      (digitalRead(IBridge_Row_Pin2))  &&
     !(digitalRead(IBridge_Row_Pin3)))
    return (7);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
     !(digitalRead(IBridge_Row_Pin1))  &&
     !(digitalRead(IBridge_Row_Pin2))  &&
      (digitalRead(IBridge_Row_Pin3)))
    return (8);

  //Column 3 Scan

  digitalWrite(IBridge_Column_Pin0, LOW);
  digitalWrite(IBridge_Column_Pin1, LOW);
  digitalWrite(IBridge_Column_Pin2, HIGH);
  digitalWrite(IBridge_Column_Pin3, LOW);

  //i=10;
  //while(i--);
  delay(1);

  if((digitalRead(IBridge_Row_Pin0)) &&
    !(digitalRead(IBridge_Row_Pin1))  &&
    !(digitalRead(IBridge_Row_Pin2))  &&
    !(digitalRead(IBridge_Row_Pin3)))
    return (9);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
      (digitalRead(IBridge_Row_Pin1))  &&
     !(digitalRead(IBridge_Row_Pin2))  &&
     !(digitalRead(IBridge_Row_Pin3)))
    return (10);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
     !(digitalRead(IBridge_Row_Pin1))  &&
      (digitalRead(IBridge_Row_Pin2))  &&
     !(digitalRead(IBridge_Row_Pin3)))
    return (11);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
     !(digitalRead(IBridge_Row_Pin1))  &&
     !(digitalRead(IBridge_Row_Pin2))  &&
      (digitalRead(IBridge_Row_Pin3)))
    return (12);

  //Column 4 Scan

  digitalWrite(IBridge_Column_Pin0, LOW);
  digitalWrite(IBridge_Column_Pin1, LOW);
  digitalWrite(IBridge_Column_Pin2, LOW);
  digitalWrite(IBridge_Column_Pin3, HIGH);


  delay(1);

  if((digitalRead(IBridge_Row_Pin0)) &&
    !(digitalRead(IBridge_Row_Pin1))  &&
    !(digitalRead(IBridge_Row_Pin2))  &&
    !(digitalRead(IBridge_Row_Pin3)))
    return (13);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
      (digitalRead(IBridge_Row_Pin1))  &&
     !(digitalRead(IBridge_Row_Pin2))  &&
     !(digitalRead(IBridge_Row_Pin3)))
    return (14);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
     !(digitalRead(IBridge_Row_Pin1))  &&
      (digitalRead(IBridge_Row_Pin2))  &&
     !(digitalRead(IBridge_Row_Pin3)))
    return (15);

  if(!(digitalRead(IBridge_Row_Pin0)) &&
     !(digitalRead(IBridge_Row_Pin1))  &&
     !(digitalRead(IBridge_Row_Pin2))  &&
      (digitalRead(IBridge_Row_Pin3)))
    return (16);

  return(0);

}
