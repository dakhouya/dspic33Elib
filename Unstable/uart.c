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
const uint16 UartBase[]
{
	REVERVED0 = 0;
	UARTBASE1 = (sUartInit_t*)0x0220,	/*Address 0x0220*/
	UARTBASE2 = (sUartInit_t*)0x0230,	/*Address 0x0230*/
	UARTBASE3 = (sUartInit_t*)0x0250,	/*Address 0x0250*/
	UARTBASE4 = (sUartInit_t*)0x02B0	/*Address 0x02B0*/
};
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
Init_UART
	Initialise the UART module with no flow control

	INPUT 		: 
				ubUartNo : Uart Interface that need to be initialise
				sUartParam : Uart Parameters to be configure
				
	OUTPUT 		:	
				-None				
*/
void UartInit(uint8 ubUartNo, sUartParam* sUartParam)
{
	uint8 ubValid = TRUE;
	sUartInit* sUartInit= NULL;
	
	ubValid = IrUartIterfaceValid(ubUartNo);
	if(ubValid)
	{
		sUartInit = UartBase[ubUartNo];
		/*Clear every UART registers*/
		sUartInit->Uxmode 	= 	0x0000;
		sUartInit->Uxsta 	= 	0x0000;
		sUartInit->Uxrxreg 	= 	0x0000;		
		
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
/*
Init_UART1
	Initialise the UART module with no flow control, Autobaud OFF

	INPUT 		: 
				sUartPort: UartPort structure to be initialize
				*Txfct 	 : Configure a callback function for Tx interrupt
				*Rxfct	 : Configure a callback function for Rx interrupt
	OUTPUT 		:	
				-None				
*/
void UartInitPortStruc(sUartPort_t* sUartPort, 
					   void (*pTxfct)(uint8 ubUartNo, uint8 ubChar), 
					   void (*pRxfct)(uint8 ubUartNo, uint8 ubChar))
{
	/*Initialise all parameters to 0*/
	sUartPort->Uartno = 0;
	for(counter = 0; counter<UART_BUFFER_SIZE;++counter)
	{
		sUartPort->TxBuffer[counter] = 0;
		sUartPort->RxBuffer[counter] = 0;
	}
	sUartPort->TxMessageLength = 0;	
	sUartPort->RxMessageLength = 0;
	sUartPort->TxIsBusy = 0;
	
	/*Configure callback function if necessary*/
	if(Rxfct != NULL)
	{
		sUartPort->Rxfct = pRxfct;
	}
	else
	{
		sUartPort->Rxfct = NULL;
	}
	
	if(Txfct != NULL)
	{
		sUartPort->Txfct = pTxfct;	
	}
	else
	{
		sUartPort->Txfct = NULL;	
	}
}

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

