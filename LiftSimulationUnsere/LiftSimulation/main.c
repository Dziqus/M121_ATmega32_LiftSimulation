/**
 * @file LiftSimulation.c
 * @brief Hauptprogramm und entry point der Liftsteuerung
 *
 * @date 20.11.2019 Erstellen des Templates
 * @author Epci Dev
 */ 

#include "LiftSimulationCommon.h"
#include "LiftLibrary.h"

void TestFunction(uint8_t* data, uint8_t nrOfBytes);

int main(void)
{
    /* Replace with your application code */
   InitializePorts();
   Usart_Init();
   RegisterTestHandler( TestFunction );
   InitializeStart();
}

void TestFunction(uint8_t* data, uint8_t nrOfBytes)
{
	if ( data[0] == 1 )
	{
		char *ch = "Hello World\n";
		while ( *ch != 0 )
		{
			Usart_PutChar(*ch++);
		} 
	}
}