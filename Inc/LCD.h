/*
 * LCD.h
 *
 *  Created on: May 28, 2019
 *      Author: Anas Khedr
 */

#ifndef LCD_H_
#define LCD_H_

#include <stm32f1xx.h>
#include "stm32f1xx_hal.h"
#include <stm32f103xb.h>
//#include <avr/io.h>
//#include <util/delay.h>			//for _delay_ms()
#include <string.h>				//for strlen()
#include <stdarg.h>				//for va_list
#include <stdio.h>				//for vsprintf()

#define WRITEONLY 1
#define ENDADDRESS 0x27

#define RIGHT 1			//direction of shift to use with MyLCD::shift()
#define LEFT 0

#define LtoR 1
#define RtoL 0
#define LtoR 1

#define CURSOR_UNDERLINE_ON  1
#define CURSOR_UNDERLINE_OFF 0
#define BLINK_ON 1
#define BLINK_OFF 0
#define RESOLUTION_5x10 1
#define RESOLUTION_5x7  0

typedef unsigned char uint8_t;


class MyLCD
{
public:
	MyLCD(char Port_of_D,uint8_t D7,uint8_t D6,uint8_t D5,uint8_t D4,uint8_t D3,uint8_t D2,uint8_t D1,uint8_t D0,uint8_t RS,uint8_t RW,uint8_t E);
	MyLCD(char Port_of_D,uint8_t D7,uint8_t D6,uint8_t D5,uint8_t D4,uint8_t D3,uint8_t D2,uint8_t D1,uint8_t D0,uint8_t RS,uint8_t E);
	MyLCD(char Port_of_D,uint8_t D7,uint8_t D6,uint8_t D5,uint8_t D4,uint8_t RS,uint8_t RW,uint8_t E);
	MyLCD(char Port_of_D,uint8_t D7,uint8_t D6,uint8_t D5,uint8_t D4,uint8_t RS,uint8_t E);
	//overloaded constructor when creating an object, Port_of_D is the port that D0-D7 will be connected to e.g PORTA = 'A'. D7-D0 are the MCU pins (of Port_of_D) that will be connected to LCD D7-D0 respectively
	void GPIO_Pin_Output(GPIO_TypeDef* GPIOx, uint8_t Dx);	//set GPIOx Pin as output push pull

	void command();							//call this function before using writeChar() to send a command
	void data();							//call this function before using writeChar() to send a data(a character)

	void clear();							//clear the LCD (basically reset it)
	void home();							//return the cursor and the display to initial position without reseting everything (command: display and cursor home)

	bool read();	//I kinda abandoned this function
	void writeChar(char buffer);			//Helper Function, you need to specify what you are sending it (Data or Command)

	void upperWrite();						//Helper to Helper fullWrite()
	void lowerWrite();						//Helper to Helper fullWrite()
	void fullWrite();						//Helper to Helper writeChar()

	void printString(char*__restrict str, uint8_t n);			//Main(accepts string array only) and Helper
	void print(const char* format, ...);	//Main(works like printf() for the display)
	void print(char*__restrict str);					//Main(the same as printString)
	void print(char ch);					//Main(the same as writeChar for data)

	void send();							//Helper sending your operation(data or command) to the MCU peripherals connected to the LCD(this is called when data or command is constructed and ready to be sent to the LCD)

	void init(uint8_t Lines=2,bool Cursor=CURSOR_UNDERLINE_ON,bool Blink=BLINK_OFF,bool Resolution=RESOLUTION_5x7);
	//initialize the LCD by setting number of lines cursor settings and display resolution

	void setCursor(bool Line,uint8_t address);						//Main-> sets the postion of the cursor on the display(0,0 is start for line 0 and 1,0 is the start for line 1)
	void shiftR(uint8_t num, short delay=100);						//Main-> shift the visible part of the display(not the cursor) to the right
	void shiftL(uint8_t num, short delay=100);						//Main-> shift the visible part of the display(not the cursor) to the left
	void shift(uint8_t direction, uint8_t num, short delay=100);	//Helper and Main-> shift the visible part of the display(not the cursor) to the direction chosen a number of times "num" with a delay between each time = delay in ms

	void displayShift(char fixedStartAddress=0x10, bool line=0, bool direction=LtoR);	//Main-> a writing mode that allows you to write characters similer by inserting the firist on in the address passed to this API then shifts the character(to right or left) then insert the new character in the same address. best way to understand it is to call it then print() something with delay between each print
	void writeDirection(char startAddress=0x00, bool Line=0, bool direction=LtoR);		//Main-> chose write direction either left to right like English or right to left like Arabic

	uint8_t defineGraph(char * __restrict graph, int8_t address=-1);								//MAIN-> you pass graph which is an array of the character pattern (could be array of 7 or 8 accourding to your LCD), if you passed an address (should be 0-7) then it will use it and return it, if not then it will automatically assign the character to the next available address and return it. the return value is passed to printf() to show the character on the LCD
	//use this site to generate your custom character :https://www.quinapalus.com/hd44780udg.html
private:
	struct storage{
		uint8_t RS:3,RW:3,E:3,D0:3,D1:3,D2:3,D3:3,D4:3,D5:3,D6:3,D7:3,wire_num:1,RW_Status:1;
		GPIO_TypeDef *Port_of_D;	//Volatile is redundant
		uint8_t buffer;
	}pins;
};

//---------------------------------------------------------------------------------------------------------
//											inline functions
//---------------------------------------------------------------------------------------------------------

inline void MyLCD::print(char*__restrict str){
	printString(str, strlen(str));
}

inline void MyLCD::print(char ch){
	data();
	writeChar(ch);
}

inline void MyLCD::fullWrite(){
	upperWrite();
	lowerWrite();
}


inline void MyLCD::clear(){
	command();
	writeChar(0x1);
	HAL_Delay(2);
}

inline void MyLCD::home(){
	command();
	writeChar(0x2);
	HAL_Delay(2);
}

inline void MyLCD::shiftL(uint8_t num, short delay){
	shift(LEFT,num,delay);
}

inline void MyLCD::shiftR(uint8_t num, short delay){
	shift(RIGHT,num,delay);
}



#endif /* LCD_H_ */
