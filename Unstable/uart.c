/************************************************************/
/* uart.c													*/
/* Library for the UART module of dspic33E			 		*/
/*															*/
/* MCU : dspic33E											*/
/*															*/
/* Author : David Khouya									*/
/* Date	  :	xx/11/2012										*/
/* Version: 0.1 											*/
/************************************************************/

/************************************************************/
/*						   INCLUDES			 				*/
/************************************************************/
#include "uart.h"
/************************************************************/

/************************************************************/
/*			           MACRO DEFINITIONS			 		*/
/************************************************************/

/************************************************************/


/************************************************************/
/*			        STRUCTURE DEFINITIONS			 		*/
/************************************************************/
typedef enum UartBase
{
	PADDING = 0;
	UARTBASE1 = (sUartInit_t*)0x0220,	/*Address 0x0220*/
	UARTBASE2 = (sUartInit_t*)0x0230,	/*Address 0x0230*/
	UARTBASE3 = (sUartInit_t*)0x0250,	/*Address 0x0250*/
	UARTBASE4 = (sUartInit_t*)0x02B0	/*Address 0x02B0*/
}UartBase;
/************************************************************/


/************************************************************/
/*			          PUBLIC VARIABLES			 			*/
/************************************************************/

/************************************************************/


/************************************************************/
/*			         PRIVATE VARIABLES			 			*/
/************************************************************/

/************************************************************/


/************************************************************/
/*			         PRIVATE PROTOTYPES			 			*/
/************************************************************/
bool IsUartInterfaceValid(uint8 ubUartNo);
/************************************************************/


/************************************************************/
/*				     PUBLIC FUNCTIONS			 			*/
/************************************************************/
/*
Init_UART1
	Initialise the UART module with no flow control, Autobaud OFF

	INPUT 		: 
				ubUartNo : Uart Interface that need to be initialise
				sUartParam : Uart Parameters to be configure
				
	OUTPUT 		:	
				-None				
				
	uint8 BRGH
	uint8 Parity;
	uint8 StopBit;
	uint8 BaudRate;
*/
void UartInit(uint8 ubUartNo, sUartParam* sUartParam)
{
	uint8 ubValid = TRUE;
	sUartInit* sUartInit= NULL;
	
	ubValid = IrUartIterfaceValid;
	if(ubValid)
	{
		sUartInit = UartBase[ubUartNo];
		sUartInit->Uxmode = 0x00;	
		
		/*Polarity*/
		sUartInit->Uxmode |= (4<<(sUartParam->RxPolarity));

		/*BRGH*/
		sUartInit->Uxmode |= (3<<(sUartParam->BRGH));
		
		/*Parity*/
		sUartInit->Uxmode |= (2<<(sUartParam->Parity));
		
		/*Stop bits*/
		sUartInit->Uxmode |= (sUartParam->StopBit);
		
		/*Baudrate*/
		sUartInit->Uxbrg =  sUartParam->BaudRate;
		
	}
}
/************************************************************/


/************************************************************/
/*				    PRIVATES FUNCTIONS			 			*/
/************************************************************/
bool IsUartInterfaceValid(uint8 ubUartNo)
{
	uint8 ubValid = FALSE;

	if(ubUartNo <= NBUART)
	{
		ubValid = TRUE;
	}
	
	return ubValid;
}
/************************************************************/

