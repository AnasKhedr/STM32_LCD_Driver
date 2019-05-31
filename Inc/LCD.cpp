/*
 * LCD.cpp
 *
 *  Created on: May 28, 2019
 *      Author: Anas Khedr
 */


#include "LCD.h"
#include <stm32f1xx.h>
#include <stm32f103xb.h>
#include "stm32f1xx_hal.h"
//#include <avr/io.h>
//#include <util/delay.h>			//for HAL_Delay()
#include <string.h>				//for strlen()
#include <stdarg.h>				//for va_list
#include <stdio.h>				//for vsprintf()

#ifndef BUFFER_SIZE
#warning The user didn't define BUFFER_SIZE, using default value: 40
#define BUFFER_SIZE 40			//each LCD display has 80 location. if it's a 2-Line LCD then 40 per-line (only 16 characters are visible per-line in 16x2 LCD)
#endif
char buffer[BUFFER_SIZE];

MyLCD::MyLCD(char Port_of_D,uint8_t D7,uint8_t D6,uint8_t D5,uint8_t D4,uint8_t RS,uint8_t E){
	GPIO_TypeDef *Port_of_D_address;
	HAL_Delay(11);
	switch (Port_of_D)
	{
		/*	if you have different ports then you need to find their address. they are usually in a header called iom*.h where * is the name of the MCU (for atmega328p the header is called iom328p.h)
		 * all the iom*.h are located here C:\Program Files (x86)\Atmel\Studio\7.0\packs\atmel\ATmega_DFP\1.1.130\include\avr
		 */
		case 'A':
			RCC->APB2ENR |= 1<<2;
			/*
 			if(D7<8 && D6<8 && D5<8 && D4<8 && RS<8 && E<8 )
				GPIOG->CRL = (2<<(D7*4) | 2<<(D6*4) | 2<<(D5*4) | 2<<(D4*4) | 2<<(RS*4) | 2<<(E*4));
			else
				GPIOG->CRH = (2<<((D7-8)*4) | 2<<((D6-8)*4) | 2<<((D5-8)*4) | 2<<((D4-8)*4) | 2<<((RS-8)*4) | 2<<((E-8)*4));
			 */
			Port_of_D_address = GPIOA;
			break;

		case 'B':
			RCC->APB2ENR |= 1<<3;
			Port_of_D_address = GPIOB;
			break;

		case 'C':
			RCC->APB2ENR |= 1<<4;
			Port_of_D_address = GPIOC;
			break;

		case 'D':
			RCC->APB2ENR |= 1<<5;
			Port_of_D_address = GPIOD;
			break;

		case 'E':
			RCC->APB2ENR |= 1<<6;
			Port_of_D_address = GPIOE;
			break;
		/*
		 * Port "F" and "G" do not exist in my STM32F103C8T6
		case 'F':
			RCC->APB2ENR |= 1<<7;
			Port_of_D_address = GPIOF;
			break;

		case 'G':
			RCC->APB2ENR |= 1<<8;
			Port_of_D_address = GPIOG;
			break;
		*/
	}

	GPIO_Pin_Output(Port_of_D_address,D7);
	GPIO_Pin_Output(Port_of_D_address,D6);
	GPIO_Pin_Output(Port_of_D_address,D5);
	GPIO_Pin_Output(Port_of_D_address,D4);
	GPIO_Pin_Output(Port_of_D_address,RS);
	GPIO_Pin_Output(Port_of_D_address,E);
	//-------------------------------

	pins.Port_of_D=Port_of_D_address;		//pins.Port_of_D is equivalent to GPIOx where x is the letter passed to the constructor
	pins.D7=D7;
	pins.D6=D6;
	pins.D5=D5;
	pins.D4=D4;
	pins.RS=RS;
	pins.E=E;

	pins.RW_Status=0;		//0 => NOT using(issuing) read/write(RW) command/pin (only write by default)
	pins.wire_num=0;		//0 => 4-wires & 1 => 8-wires

}

MyLCD::MyLCD(char Port_of_D,uint8_t D7,uint8_t D6,uint8_t D5,uint8_t D4,uint8_t RS,uint8_t RW,uint8_t E) : MyLCD(Port_of_D,D7,D6,D5,D4,RS,E){
	/*
	if(D7<8 && D6<8 && D5<8 && D4<8 && RS<8 && E<8 )
		pins.Port_of_D->CRL |= 2<<RW;
	else
		pins.Port_of_D->CRH |= 2<<(RW-8)*4;
	*/
	GPIO_Pin_Output(pins.Port_of_D, RW);

	pins.RW=RW;
	pins.RW_Status=1;		//1 => using(issuing) read/write(RW) command/pin
	pins.wire_num=0;		//0 => 4-wires & 1 => 8-wires
}

MyLCD::MyLCD(char Port_of_D,uint8_t D7,uint8_t D6,uint8_t D5,uint8_t D4,uint8_t D3,uint8_t D2,uint8_t D1,uint8_t D0,uint8_t RS,uint8_t E){
//:MyLCD( Port_of_D, D7, D6, D5, D4, RS, E){
	GPIO_TypeDef *Port_of_D_address;
	HAL_Delay(11);
	//if(Port_of_D == Port_of_D) return -1; //error in Atmel328p but may work in other MCU
	switch (Port_of_D)
	{
		/*	if you have different ports then you need to find their address. they are usually in a header called iom*.h where * is the name of the MCU (for atmega328p the header is called iom328p.h)
		 * all the iom*.h are located here C:\Program Files (x86)\Atmel\Studio\7.0\packs\atmel\ATmega_DFP\1.1.130\include\avr
		 */
		case 'A':
			RCC->APB2ENR |= 1<<2;
			/*
			if(D7<8 && D6<8 && D5<8 && D4<8 && RS<8 && E<8 )
				GPIOG->CRL = (2<<(D7*4) | 2<<(D6*4) | 2<<(D5*4) | 2<<(D4*4) | 2<<(RS*4) | 2<<(E*4));
			else
				GPIOG->CRH = (2<<((D7-8)*4) | 2<<((D6-8)*4) | 2<<((D5-8)*4) | 2<<((D4-8)*4) | 2<<((RS-8)*4) | 2<<((E-8)*4));
			 */
			Port_of_D_address = GPIOA;
			break;

		case 'B':
			RCC->APB2ENR |= 1<<3;
			Port_of_D_address = GPIOB;
			break;

		case 'C':
			RCC->APB2ENR |= 1<<4;
			Port_of_D_address = GPIOC;
			break;

		case 'D':
			RCC->APB2ENR |= 1<<5;
			Port_of_D_address = GPIOD;
			break;

		case 'E':
			RCC->APB2ENR |= 1<<6;
			Port_of_D_address = GPIOE;
			break;
		/*
		 * Port "F" and "G" do not exist in my STM32F103C8T6
		case 'F':
			RCC->APB2ENR |= 1<<7;
			Port_of_D_address = GPIOF;
			break;

		case 'G':
			RCC->APB2ENR |= 1<<8;
			Port_of_D_address = GPIOG;
			break;
		*/
	}
	GPIO_Pin_Output(Port_of_D_address,D7);
	GPIO_Pin_Output(Port_of_D_address,D6);
	GPIO_Pin_Output(Port_of_D_address,D5);
	GPIO_Pin_Output(Port_of_D_address,D4);
	GPIO_Pin_Output(Port_of_D_address,D3);
	GPIO_Pin_Output(Port_of_D_address,D2);
	GPIO_Pin_Output(Port_of_D_address,D1);
	GPIO_Pin_Output(Port_of_D_address,D0);
	GPIO_Pin_Output(Port_of_D_address,RS);
	GPIO_Pin_Output(Port_of_D_address,E);
	//-------------------------------

	pins.Port_of_D=Port_of_D_address;
	pins.D7=D7;
	pins.D6=D6;
	pins.D5=D5;
	pins.D4=D4;
	pins.D3=D3;
	pins.D2=D2;
	pins.D1=D1;
	pins.D0=D0;
	pins.RS=RS;
	pins.E=E;

	pins.RW_Status=0;			//0 => NOT using(issuing) read/write(RW) command/pin (only write by default)
	pins.wire_num=1;			//1 => 8-wires & 0 => 4-wires

}

MyLCD::MyLCD(char Port_of_D,uint8_t D7,uint8_t D6,uint8_t D5,uint8_t D4,uint8_t D3,uint8_t D2,uint8_t D1,uint8_t D0,uint8_t RS,uint8_t RW,uint8_t E) \
:MyLCD(Port_of_D, D7, D6, D5, D4, D3, D2, D1, D0, RS, E){

	GPIO_Pin_Output(pins.Port_of_D,RW);
	pins.RW=RW;
	pins.RW_Status=1;			//1 => using(issuing) read/write(RW) command/pin
	pins.wire_num=1;			//1 => 8-wires & 0 => 4-wires
}


void MyLCD::GPIO_Pin_Output(GPIO_TypeDef* GPIOx, uint8_t Dx){
	
	if(Dx<8){
		Dx *=4;								//shift the pin to the corresponding start bit in CRL
		GPIOx->CRL &= ~(1<<(Dx+0));
		GPIOx->CRL |=  (1<<(Dx+1));
		GPIOx->CRL &= ~(1<<(Dx+2));
		GPIOx->CRL &= ~(1<<(Dx+3));
	}
	else{
		Dx %=8; Dx *=4;				//shift the pin to the corresponding start bit in CRH
		GPIOx->CRH &= ~(1<<(Dx+0));
		GPIOx->CRH |=  (1<<(Dx+1));
		GPIOx->CRH &= ~(1<<(Dx+2));
		GPIOx->CRH &= ~(1<<(Dx+3));
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------

void MyLCD::send(){											//send command/data to LCD
	pins.Port_of_D->ODR |= 1<<pins.E;				//E High
	//_delay_us(1000);
	pins.Port_of_D->ODR &=~ (1<<pins.E);			//then E low
	//_delay_us(1000);

	if(pins.wire_num){								//1 => 8-wires & 0 => 4-wires
		pins.Port_of_D->ODR &= ~((1<<pins.D0) | (1<<pins.D1) | (1<<pins.D2) | (1<<pins.D3) | (1<<pins.D4) | (1<<pins.D1) | (1<<pins.D6) | (1<<pins.D7));//reset all pins D0-D7 for next use of |=
	}else{											//0 => 4-wires & 1 => 4-wires
		pins.Port_of_D->ODR &=~ ((1<<pins.D7) | (1<<pins.D6) | (1<<pins.D5) | (1<<pins.D4)); //reset used pins D4-D7 for next use of |=
	}

}

void MyLCD::command(){		//could be inlined
	pins.Port_of_D->ODR &=~ (1<<pins.RS);

}

void MyLCD::data(){
	pins.Port_of_D->ODR |= (1<<pins.RS);

}
/*		inlined, definition is in header file
void MyLCD::clear(){
	command();
	writeChar(0x1);
	HAL_Delay(2);
}

void MyLCD::home(){
	command();
	writeChar(0x2);
	HAL_Delay(2);
}
*/
bool MyLCD::read(){
	if (pins.wire_num){		//pins.wire_num == 1 => 8-wires
		if (pins.RW_Status){	//1 => using(issuing) read/write(RW) command/pin
			pins.Port_of_D->ODR |= (1<<pins.RW);	//set RW pin to 1 to read
			//the user should select what is he\she is reading data\command and its address

			send();												//send command to LCD

			pins.buffer = pins.Port_of_D->IDR;					//read from GPIOx where x is the port you selected and store value in buffer(this line needs alignment in case the Dx bits are not the first 8 bits in GPIOx)
			return 0;											//please note that development of this API wan't  completed due to lack of usefulness
		}
		else{					//0 => NOT using(issuing) read/write(RW) command/pin
			//error pins.RW_Status cant be 0, that means you only write
			return 1;
		}
	}
	return 0;
}

void MyLCD::writeChar(char buffer){	//write data on D7-D4 or D7-D0 (according to 4\8-wire mode) without using E(enable pin) "without invoking send();"
	pins.buffer=buffer;
	if (pins.wire_num){			//pins.wire_num == 1 => 8-wires
		if (pins.RW_Status){	//1 => using(issuing) read/write(RW) command/pin

			pins.Port_of_D->ODR &=~ (1<<pins.RW);	//reset RW pin to 0 to write
			//the user should select what is he\she is reading data\command and its address
			//fullWrite();
			//send();
		}
		else{					//0 => NOT using(issuing) read/write(RW) command/pin
			//the user should select what is he\she is reading data\command and its address
		}
		fullWrite();
		send();
		/*
		pins.Port_of_D->ODR = pins.buffer;		//output the command\data on the pins of D7-D0
		pins.Port_of_D->ODR |= (1<<pins.E);		//E high
		HAL_Delay(1000);
		pins.Port_of_D->ODR &=~ (1<<pins.E);	//the E low
		*/
	}
	else{														//pins.wire_num == 0 => 4-wires
		if(pins.RW_Status){										//1 => using(issuing) read/write(RW) command/pin
			pins.Port_of_D->ODR &=~ (1<<pins.RW);				//reset RW pin to 0 to write
		}
		else{													//0 => NOT using(issuing) read/write(RW) command/pin
				//no need to reset RW since you're not using RW
		}
		upperWrite();
		send();
		lowerWrite();
		send();
	}

	//pins.Port_of_D->ODR = pins.Port_of_D->ODR &= ~((1<<pins.D0) | (1<<pins.D1) | (1<<pins.D2) | (1<<pins.D3) | (1<<pins.D4) | (1<<pins.D1=5) | (1<<pins.D6) | (1<<pins.D7));		//reset the PORT cuz |= will keep the pin 1		"send() clears it instead"
}

void MyLCD::upperWrite(){
	if(pins.buffer & (1<<7))
		pins.Port_of_D->ODR |= (1<<pins.D7);
	if(pins.buffer & (1<<6))
		pins.Port_of_D->ODR |= (1<<pins.D6);
	if(pins.buffer & (1<<5))
		pins.Port_of_D->ODR |= (1<<pins.D5);
	if(pins.buffer & (1<<4))
		pins.Port_of_D->ODR |= (1<<pins.D4);

}

void MyLCD::lowerWrite(){
	if(pins.wire_num){								//pins.wire_num == 1 => 8-wires
		if(pins.buffer & (1<<3))
			pins.Port_of_D->ODR |= (1<<pins.D3);
		if(pins.buffer & (1<<2))
			pins.Port_of_D->ODR |= (1<<pins.D2);
		if(pins.buffer & (1<<1))
			pins.Port_of_D->ODR |= (1<<pins.D1);
		if(pins.buffer & (1<<0))
			pins.Port_of_D->ODR |= (1<<pins.D0);
	}
	else{											//pins.wire_num == 0 => 4-wires
		pins.buffer = pins.buffer <<4;
		upperWrite();
	}

}
/*		inlined, definition is in header file
void MyLCD::fullWrite(){
	upperWrite();
	lowerWrite();
}
*/
void MyLCD::printString(char*__restrict str, uint8_t n){

	data();
	for(size_t i=0;i<n;i++){
		writeChar(str[i]);
	}
}

void MyLCD::print(const char* format, ...){
	va_list arguments;
	va_start(arguments,format);

	uint8_t n = vsprintf(buffer,format,arguments);
	va_end(arguments);

	printString(buffer,n);
}
/*		inlined, definition is in header file
void MyLCD::print(char *__restrict str){
	printString(str);
}

void MyLCD::print(char ch){
	data();
	writeChar(ch);
}
*/

/*------xxxx-------	i decided that overloading init() is not necessary and i't better if i used default parameters instead (i forgot i was using C++ when i created this)
void MyLCD::init(uint8_t Lines){
	Lines--;
	char fun_set = 0b00110000;  //this is the default command "function set"
	fun_set |= Lines<<3 | pins.wire_num<<4;
	pins.Port_of_D->ODR = fun_set;
	send();
	clear();
}
*/
void MyLCD::init(uint8_t Lines,bool Cursor,bool Blink,bool Resolution){	//C is Cursor underline on\off and B is Blink on\off
	Lines--;
	char fun_set = 0b00100000;  //this is the default command "function set"
	char display = 0b00001100;  //this is the default command "Display on\off & cursor" with display set on
	fun_set |= Lines<<3 | pins.wire_num<<4 | Resolution<<2;
	display |= Cursor<<1 | Blink<<0;
	command();
	if(!pins.wire_num){			//pins.wire_num == 0 => 4-wires
		pins.Port_of_D->ODR |= (1<<pins.D5);	//this is the function set command for choosing 4-bit mode
		send();
	}
	writeChar(fun_set);
	HAL_Delay(5);
	writeChar(display);
	HAL_Delay(5);
	clear();
	HAL_Delay(5);
	//writeDirecion(0,0,0);		//this is set by default
	//send();
}


void MyLCD::setCursor(bool Line,uint8_t address){
	address += Line*(0x40);			//0x40 is the start address of Line 2
	command();
	writeChar(address|0x80);		//0x80 which is 0b10000000 to tell the LCD that this is display address command

}

void MyLCD::shift(uint8_t direction, uint8_t num, short delay){
	if(direction){
		direction = 0x18;						//0x18 which is 0b00011000 to tell the LCD to shift the addresses left one time
	}
	else if(direction==0){
		direction = 0x1C;						//0x1C which is 0b00011100 to tell the LCD to shift the addresses right one time
	}

	uint8_t i;
	command();

	for(i=0;i<num;i++){
		writeChar(direction);					//write the binary for Right or Left (whatever user chose) to D7-D4,0
		for(short i=1;i<delay;i+=10){
			HAL_Delay(10);
		}
	}
}
/*		inlined, definition is in header file
void MyLCD::shiftL(uint8_t num, short delay){
	shift(LEFT,num,delay);
}

void MyLCD::shiftR(uint8_t num, short delay){
	shift(RIGHT,num,delay);
}
*/
void MyLCD::displayShift(char fixedStartAddress, bool line, bool direction){
	setCursor(line,fixedStartAddress);
	command();
	if(direction){
		writeChar(0b00000111);
	}
	else{
		writeChar(0b00000110);
	}
}

void MyLCD::writeDirection(char startAddress/* =0x00 */, bool Line/* =0 */, bool direction/* =LtoR */){
	setCursor(Line,startAddress);
	command();
	if(direction){
		writeChar(0b0110);
	}
	else{
		writeChar(0b0100);
		/* This sites the visible part of the display to be the left part(from 0x18 to 0x27)
		if(startAddress == ENDADDRESS)
			shiftR(0x18,0);
		*/
	}
}


uint8_t MyLCD::defineGraph(char * graph, int8_t address/* =-1 */){	//again this is C++ not C so plain old  restrict doesn't work, i need to use  __restrict__ . See: http://gcc.gnu.org/onlinedocs/gcc/Restricted-Pointers.html
	static uint8_t idx=0x40;		//this is the first address we can save a new char in
	if(address != -1){
		idx = 0x40 | address*8;
	}

	if(idx >= 0x80){				//you have defined 8 new characters which is the max number on new characters that can be defined
		//return -1;				//return error (0xff)
	}

	command();
	writeChar(idx);

	data();
	for(uint8_t i=0; i<8;i++){
		writeChar(graph[i]);
		//HAL_Delay(100);
	}

	idx+=8;
	return (idx-0x40)/8 -1;				//return the handler which you can use to print the char you defined
}

