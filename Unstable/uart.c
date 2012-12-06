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
const uint16 UartBase[] =
{
	0x0000,
	0x0220,	/*Address 0x0220*/
	0x0230,	/*Address 0x0230*/
	0x0250,	/*Address 0x0250*/
	0x02B0	/*Address 0x02B0*/
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
void UartInit(uint8 ubUartNo, sUartParam_t* sUartParam)
{
	uint8 ubValid = TRUE;
	sUartInit_t* sUartInit= NULL;
	
	ubValid = IsUartInterfaceValid(ubUartNo);
	if(ubValid)
	{
		sUartInit = UartBase[ubUartNo];
		/*Clear every UART registers*/
		sUartInit->Uxmode 	= 	0x0000;
		sUartInit->Uxsta 	= 	0x0000;
		sUartInit->Uxrxreg 	= 	0x0000;		
		
		/*Polarity*/
		sUartInit->Uxmode |= ((sUartParam->RxPolarity)<<4);

		/*BRGH*/
		sUartInit->Uxmode |= ((sUartParam->BRGH)<<3);
		
		/*Parity*/
		sUartInit->Uxmode |= ((sUartParam->Parity)<<2);
		
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
	uint8 counter;
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
	if(pRxfct != NULL)
	{
		sUartPort->Rxfct = pRxfct;
	}
	else
	{
		sUartPort->Rxfct = NULL;
	}
	
	if(pTxfct != NULL)
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

/************************************************************/
/*				  		  INTERRUPT	   			 			*/
/************************************************************/
void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void)
{

}

