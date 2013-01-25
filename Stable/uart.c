/************************************************************/
/* uart.c													*/
/* Library for the UART module of dspic33E			 		*/
/*															*/
/* MCU : dspic33E											*/
/*															*/
/* Author : David Khouya									*/
/* Date	  :	25/01/2012										*/
/* Version: 1.0												*/
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

/************************************************************/


/************************************************************/
/*			          PUBLIC VARIABLES			 			*/
/************************************************************/

/************************************************************/


/************************************************************/
/*			         PRIVATE VARIABLES			 			*/
/************************************************************/
/*Base address*/
const static sUartInit_t* UartBase[] =
{
	 (sUartInit_t*)0x0220,	/*Address 0x0220*/
	 (sUartInit_t*)0x0230,	/*Address 0x0230*/
	 (sUartInit_t*)0x0250,	/*Address 0x0250*/
	 (sUartInit_t*)0x02B0	/*Address 0x02B0*/
};

/*Interrupt Addresses*/
volatile static uint16* Iecx[] =
{
	(uint16*)0x0820,
	(uint16*)0x0822,
	(uint16*)0x0824,
	(uint16*)0x0826,
	(uint16*)0x0828,
	(uint16*)0x082A,
	(uint16*)0x082C,
	(uint16*)0x082E,
	(uint16*)0x0830
};

/*Flags Adresses*/
volatile static uint16* Ifsx[] =
{
	(uint16*)0x0800,
	(uint16*)0x0802,
	(uint16*)0x0804,
	(uint16*)0x0806,
	(uint16*)0x0808,
	(uint16*)0x080A,
	(uint16*)0x080C,
	(uint16*)0x080E,
	(uint16*)0x0810
};

/*Interrupt Priority Adresses*/
volatile static uint16* Ipcx[] =
{
	(uint16*)0x0840,
	(uint16*)0x0842,
	(uint16*)0x0844,
	(uint16*)0x0846,
	(uint16*)0x0848,
	(uint16*)0x084A,
	(uint16*)0x084C,
	(uint16*)0x084E,
	(uint16*)0x0850,
	(uint16*)0x0852,
	(uint16*)0x0854,
	(uint16*)0x0856,
	(uint16*)0x0858,
	(uint16*)0x085A,
	(uint16*)0x085C,
	(uint16*)0x085E,
	(uint16*)0x0860,
	(uint16*)0x0862,
	(uint16*)0x0864,
	(uint16*)0x0866,
	(uint16*)0x0868,
	(uint16*)0x086A,
	(uint16*)0x086C,
	(uint16*)0x086E,
	(uint16*)0x0870,
	(uint16*)0x0872,
	(uint16*)0x0874,
	(uint16*)0x0876,
	(uint16*)0x0878,
	(uint16*)0x087A,
	(uint16*)0x087C,
	(uint16*)0x087E,
	(uint16*)0x0880,
	(uint16*)0x0882,
	(uint16*)0x0884,
	(uint16*)0x0886
};

static volatile sUartPort_t sUartPorts[NBUART] = {0};
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
/************************************************************/
void UartInit(uint8 ubUartNo, sUartParam* sUartParam)
{
	uint8 ubValid = TRUE;
	sUartInit_t* sUartInit= NULL;
	
	ubValid = IsUartInterfaceValid(ubUartNo);
	if(ubValid)
	{
		sUartInit = (sUartInit_t*)UartBase[ubUartNo];
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

		/*Start Uart module*/
		sUartInit->Uxmode |= UARTEN;
		
	}
}

/************************************************************/
/*
UartInitPortStruc
	Initialise the UART structure

	INPUT 		: 
				sUartPort: UartPort structure to be initialize
	OUTPUT 		:	
				-None				
*/
/************************************************************/
void UartInitPortStruc(uint8 ubUartNo, 
					   void (*pTxfct)(uint8 ubUartNo, uint8 ubChar), 
					   void (*pRxfct)(uint8 ubUartNo, uint8 ubChar))
{
	uint16 usCounter;
	uint8 ubValid = FALSE;
	uint8 ubValidUartNo = 0xFF;
	
	/*Validity Check*/
	ubValid = IsUartInterfaceValid(ubUartNo);
	if(ubValid)
	{
		ubValidUartNo = ubUartNo;
	}
	
	/*Initialise all parameters to 0*/
	sUartPorts[ubValidUartNo].TxMessageLength = 0x00;	
	sUartPorts[ubValidUartNo].RxMessageLength = 0x00;
	sUartPorts[ubValidUartNo].TxIsBusy = 0x00;

	for(usCounter = 0; usCounter<UART_BUFFER_SIZE;usCounter++)
	{
		sUartPorts[ubValidUartNo].TxBuffer[usCounter] = 0x00;
		sUartPorts[ubValidUartNo].RxBuffer[usCounter] = 0x00;
	}
	/*Configure callback function if necessary*/
	if(pRxfct != NULL)
	{
		sUartPorts[ubValidUartNo].Rxfct = pRxfct;
	}
	else
	{
		sUartPorts[ubValidUartNo].Rxfct = NULL;
	}
	
	if(pTxfct != NULL)
	{
		sUartPorts[ubValidUartNo].Txfct = pTxfct;	
	}
	else
	{
		sUartPorts[ubValidUartNo].Txfct = NULL;	
	}
}
/************************************************************/
/*
UartEcho
	Send the content of the receive buffer on UART tx

	INPUT 		: 
				ubUartNo	: Uart number
	OUTPUT 		:	
				-None				
*/
/************************************************************/
void UartEcho(uint8 ubUartNo)
{
	uint8 ubCounter;
	uint8 ubValid = FALSE;
	sUartInit_t* sUartReg = NULL;

	/*Validity Check*/
	ubValid = IsUartInterfaceValid(ubUartNo);
	if(ubValid && sUartPorts[ubUartNo].RxMessageLength)
	{
		sUartReg = (sUartInit_t*)UartBase[ubUartNo];
		/*Send Rx data to Tx*/
		UartTxFrame(ubUartNo, sUartPorts[ubUartNo].RxBuffer, sUartPorts[ubUartNo].RxMessageLength);

		/*Clear structure informations*/
		for(ubCounter=0;ubCounter<sUartPorts[ubUartNo].RxMessageLength;ubCounter++)
		{
			sUartPorts[ubUartNo].RxBuffer[ubCounter] = 0;
		}
		sUartPorts[ubUartNo].RxMessageLength = 0;
	}
}
/************************************************************/
/*
UartTxFrame
	Prepare a frame to be send

	INPUT 		: 
				ubUartNo	: Uart number
				ubString	: Char array
				ubLength	: Number of bit in the char array(ubString)
	OUTPUT 		:	
				-None				
*/
/************************************************************/
void UartTxFrame(uint8 ubUartNo, uint8* ubString, uint8 ubLength)
{

	uint8 ubCounter;
	uint8 ubValid = FALSE;
	sUartInit_t* sUartReg = NULL;

	/*Validity Check*/
	ubValid = IsUartInterfaceValid(ubUartNo);
	if(ubValid && (!sUartPorts[ubValid].TxIsBusy))
	{
		sUartReg = (sUartInit_t*)UartBase[ubUartNo];

		/*Copy the chars to the buffer*/
		for(ubCounter = 0; ubCounter<ubLength;ubCounter++)
		{
			sUartPorts[ubUartNo].TxBuffer[ubCounter] = ubString[ubCounter];
		}
		sUartPorts[ubUartNo].TxMessageLength = ubLength;
		sUartPorts[ubUartNo].TxLocation = 0;

		/*Port is use*/	
		sUartPorts[ubUartNo].TxIsBusy = 1;

		/*Send first Char*/
		sUartReg->Uxtxreg = sUartPorts[ubUartNo].TxBuffer[sUartPorts[ubUartNo].TxLocation];
	}
}
/************************************************************/
/*
UartTxEnable
	Enable or disable uart TX module

	INPUT 		: 
				ubUartNo	: Uart number
				bState	 	: State (disable of enable)
	OUTPUT 		:	
				-None				
*/
/************************************************************/
void UartTxEnable(uint8 ubUartNo, bool bState)
{
	uint8 ubValid = TRUE;
	sUartInit_t* sUartReg= NULL;
	
	ubValid = IsUartInterfaceValid(ubUartNo);
	if(ubValid)
	{
		sUartReg = (sUartInit_t*)UartBase[ubUartNo];
		if(bState==TRUE)
		{
			sUartReg->Uxsta |= UTXEN;
		}
		else
		{
			sUartReg->Uxsta &= ~(UTXEN);
		}
	}
	
}
/************************************************************/
/*
UartInterruptRxEnable
	Enable or disable uart module

	INPUT 		: 
				ubUartNo	: Uart number
				usMode		: Interrupt Mode
				ubPriority	: Interrupt Priority
				bState	 	: State (disable of enable)
	OUTPUT 		:	
				-None				
*/
/************************************************************/
void UartInterruptRxEnable(uint8 ubUartNo, uint16 usMode, uint8 ubPriority, bool bState)
{
	uint8 ubValid = TRUE;
	sUartInit_t* sUartReg= NULL;
	
	/*Validity check*/
	ubValid = IsUartInterfaceValid(ubUartNo);

	if(ubValid)
	{
		/*Allocate the right base address*/
		sUartReg = (sUartInit_t*)UartBase[ubUartNo];

		/*Set the interrupt mode*/
		sUartReg->Uxmode |= usMode;
		
		if(bState == ENABLE)
		{
			/*Set the interrupt priority and interrupt enable*/
			switch(ubUartNo)
			{
				case UART_1:
				*Ipcx[2] |= (uint16)ubPriority<<12;
				*Iecx[0] |= U1RXIE;
				break;
	
				case UART_2:
				*Ipcx[7] |= (uint16)ubPriority<<8;
				*Iecx[1] |= U2RXIE;
				break;
	
				case UART_3:
				*Ipcx[20] |= (uint16)ubPriority<<8;
				*Iecx[5] |= U3RXIE;
				break;
	
				case UART_4:
				*Ipcx[22] |= (uint16)ubPriority;
				*Iecx[5] |= U4RXIE;
				break;

				default:
				break;
			}
		}
		else
		{
			switch(ubUartNo)
			{
				case UART_1:
				*Ipcx[2] |= (uint16)ubPriority<<12;
				*Iecx[0] &= ~U1RXIE;
				break;
	
				case UART_2:
				*Ipcx[7] |= (uint16)ubPriority<<8;
				*Iecx[1] &= ~U2RXIE;
				break;
	
				case UART_3:
				*Ipcx[20] |= (uint16)ubPriority<<8;
				*Iecx[5] &= ~U3RXIE;
				break;
	
				case UART_4:
				*Ipcx[22] |= (uint16)ubPriority;
				*Iecx[5] &= ~U4RXIE;
				break;

				default:
				break;
			}
		}
	}
}
/************************************************************/
/*
UartInterruptTxEnable
	Tx interrupt routine

	INPUT 		: 
				ubUartNo	: Uart number
				usMode		: Interrupt Mode
				ubPriority	: Interrupt Priority
				bState	 	: State (disable of enable)
	OUTPUT 		:	
				-None				
*/
/************************************************************/
void UartInterruptTxEnable(uint8 ubUartNo, uint16 usMode, uint8 ubPriority, bool bState)
{
	uint8 ubValid = TRUE;
	sUartInit_t* sUartReg= NULL;
	
	/*Validity check*/
	ubValid = IsUartInterfaceValid(ubUartNo);

	if(ubValid)
	{
		/*Allocate the right base address*/
		sUartReg = (sUartInit_t*)UartBase[ubUartNo];

		/*Set the interrupt mode*/
		sUartReg->Uxmode |= usMode;
		
		if(bState == ENABLE)
		{
			/*Set the interrupt priority and interrupt enable*/
			switch(ubUartNo)
			{
				case UART_1:
				*Ipcx[3] |= (uint16)ubPriority;
				*Iecx[0] |= U1TXIE;
				break;
	
				case UART_2:
				*Ipcx[7] |= (uint16)ubPriority<<12;
				*Iecx[1] |= U2TXIE;
				break;
	
				case UART_3:
				*Ipcx[20] |= (uint16)ubPriority<<12;
				*Iecx[5] |= U3TXIE;
				break;
	
				case UART_4:
				*Ipcx[22] |= (uint16)ubPriority<<4;
				*Iecx[5] |= U4TXIE;
				break;

				default:
				break;
			}
		}
		else
		{
			switch(ubUartNo)
			{
				case UART_1:
				*Ipcx[3] |= (uint16)ubPriority;
				*Iecx[0] &= ~U1TXIE;
				break;
	
				case UART_2:
				*Ipcx[7] |= (uint16)ubPriority<<12;
				*Iecx[1] &= ~U2TXIE;
				break;
	
				case UART_3:
				*Ipcx[20] |= (uint16)ubPriority<<12;
				*Iecx[5] &= ~U3TXIE;
				break;
	
				case UART_4:
				*Ipcx[22] |= (uint16)ubPriority<<4;
				*Iecx[5] &= ~U4TXIE;
				break;

				default:
				break;
			}
		}
	}
}
/************************************************************/
/*
UartInterruptTx
	Enable or disable uart module

	INPUT 		: 
				ubUartNo	: Uart number
	OUTPUT 		:	
				-None				
*/
/************************************************************/
void UartInterruptTx(uint8 ubUartNo)
{
	uint8 ubCounter;

	sUartInit_t* sUartReg = NULL;
	sUartReg = (sUartInit_t*)UartBase[ubUartNo];

	/*Increment Loction in the buffer*/
	sUartPorts[ubUartNo].TxLocation++;

	/*Check if the transmit buffer is empty*/
	while(!(sUartReg->Uxsta & TRMT));

	while(sUartPorts[ubUartNo].TxLocation != sUartPorts[ubUartNo].TxMessageLength)
	{
		/*Send Char to transmit buffer*/
		sUartReg->Uxtxreg = sUartPorts[ubUartNo].TxBuffer[sUartPorts[ubUartNo].TxLocation++];

		/*Check if the transmit buffer is empty*/
		while(!(sUartReg->Uxsta & TRMT));
	}

	/*Clear structure informations*/
	for(ubCounter=0;ubCounter<sUartPorts[ubUartNo].TxMessageLength;ubCounter++)
	{
		sUartPorts[ubUartNo].TxBuffer[ubCounter] = 0;
	}

	sUartPorts[ubUartNo].TxLocation = 0;
	sUartPorts[ubUartNo].TxIsBusy = 0;
	sUartPorts[ubUartNo].TxMessageLength = 0;
	
}
/************************************************************/
/*
UartInterruptRx
	Enable or disable uart module

	INPUT 		: 
				ubUartNo	: Uart number
	OUTPUT 		:	
				-None				
*/
/************************************************************/
void UartInterruptRx(uint8 ubUartNo)
{
	sUartInit_t* sUartReg = NULL;
	sUartReg = (sUartInit_t*)UartBase[ubUartNo];

	/*Check if there's something in the input buffer*/
	if(sUartReg->Uxsta & URXDA)
	{
		/*Insert Char to receive buffer*/
		sUartPorts[ubUartNo].RxBuffer[sUartPorts[ubUartNo].RxMessageLength++] = sUartReg->Uxrxreg;
	}	
}
/************************************************************/
/*				    PRIVATES FUNCTIONS			 			*/
/************************************************************/
bool IsUartInterfaceValid(uint8 ubUartNo)
{
	uint8 ubValid = FALSE;

	if(ubUartNo < NBUART)
	{
		ubValid = TRUE;
	}

	return ubValid;
}
/************************************************************/

/************************************************************/
/*				    INTERRUPT FUNCTIONS			 			*/
/************************************************************/
void __attribute__((__interrupt__)) _U1TXInterrupt(void)
{
	UartInterruptTx(UART_1);
	*Ifsx[0] &= ~(U1TXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((__interrupt__)) _U1RXInterrupt(void)
{
	UartInterruptRx(UART_1);
	*Ifsx[0] &= ~(U1RXIF); // clear RX interrupt flag
}
/************************************************************/
void __attribute__((__interrupt__)) _U2TXInterrupt(void)
{
	*Ifsx[1] &= ~(U2TXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((__interrupt__)) _U2RXInterrupt(void)
{
	*Ifsx[1] &= ~(U2RXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((__interrupt__)) _U3TXInterrupt(void)
{
	*Ifsx[5] &= ~(U3TXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((__interrupt__)) _U3RXInterrupt(void)
{
	*Ifsx[5] &= ~(U3RXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((__interrupt__)) _U4TXInterrupt(void)
{
	*Ifsx[5] &= ~(U4TXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((__interrupt__)) _U4RXInterrupt(void)
{
	*Ifsx[5] &= ~(U4RXIF); // clear TX interrupt flag
}
