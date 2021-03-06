/*
 * GccApplication1.c
 *
 * Created: 26.11.2019 16:29:04
 * Author : rolfl
 */ 

#include <avr/io.h>

#include "AppIncludes.h"


void TestFunction(uint8_t* data, uint8_t nrOfBytes);
//uint8_t FindBit(uint8_t ch);

int main(void)
{
    InitializePorts();
	Usart_Init();
	RegisterTestHandler( TestFunction );
	RegisterFsm(&_motorCtrl.fsm);
	RegisterFsm(&_mainCtrl.fsm);
	// letzte Funktion die aufgerufen wird!
	InitializeStart();
}

void TestFunction(uint8_t* data, uint8_t nrOfBytes)
{
	if( data[0] == 1 )
	{
		char *ch = "Hello-World\n";
		while( *ch != 0 )
		{
			Usart_PutChar(*ch++);
		}
	}
	else if( data[0] == 2 )
	{
		SendEvent(SignalSourceApp, Message_MoveTo, data[1], 0);
	}
}

//uint8_t FindBit(uint8_t ch){
	//
	//return (0x72f588 >> ((((ch * 29) >> 5) & 0x7) * 3)) & 0x7;
//}

