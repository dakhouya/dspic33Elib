/************************************************************/
/* uart.c                                                   */
/* Library for the UART module of dspic33E                  */
/*                                                          */
/* MCU : dspic33E                                           */
/*                                                          */
/* Author : David Khouya                                    */
/* Date	  : 19/04/2013                                      */
/************************************************************/

/************************************************************/
/*			INCLUDES                            */
/************************************************************/
#include "uart.h"
/************************************************************/


/************************************************************/
/*                  PRIVATE VARIABLES                       */
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
static uint16_t* Iecx[] =
{
	(uint16_t*)0x0820,
	(uint16_t*)0x0822,
	(uint16_t*)0x0824,
	(uint16_t*)0x0826,
	(uint16_t*)0x0828,
	(uint16_t*)0x082A,
	(uint16_t*)0x082C,
	(uint16_t*)0x082E,
	(uint16_t*)0x0830
};

/*Flags Adresses*/
static uint16_t* Ifsx[] =
{
	(uint16_t*)0x0800,
	(uint16_t*)0x0802,
	(uint16_t*)0x0804,
	(uint16_t*)0x0806,
	(uint16_t*)0x0808,
	(uint16_t*)0x080A,
	(uint16_t*)0x080C,
	(uint16_t*)0x080E,
	(uint16_t*)0x0810
};

/*Interrupt Priority Adresses*/
static uint16_t* Ipcx[] =
{
	(uint16_t*)0x0840,
	(uint16_t*)0x0842,
	(uint16_t*)0x0844,
	(uint16_t*)0x0846,
	(uint16_t*)0x0848,
	(uint16_t*)0x084A,
	(uint16_t*)0x084C,
	(uint16_t*)0x084E,
	(uint16_t*)0x0850,
	(uint16_t*)0x0852,
	(uint16_t*)0x0854,
	(uint16_t*)0x0856,
	(uint16_t*)0x0858,
	(uint16_t*)0x085A,
	(uint16_t*)0x085C,
	(uint16_t*)0x085E,
	(uint16_t*)0x0860,
	(uint16_t*)0x0862,
	(uint16_t*)0x0864,
	(uint16_t*)0x0866,
	(uint16_t*)0x0868,
	(uint16_t*)0x086A,
	(uint16_t*)0x086C,
	(uint16_t*)0x086E,
	(uint16_t*)0x0870,
	(uint16_t*)0x0872,
	(uint16_t*)0x0874,
	(uint16_t*)0x0876,
	(uint16_t*)0x0878,
	(uint16_t*)0x087A,
	(uint16_t*)0x087C,
	(uint16_t*)0x087E,
	(uint16_t*)0x0880,
	(uint16_t*)0x0882,
	(uint16_t*)0x0884,
	(uint16_t*)0x0886
};

volatile sUartPort_t sUartPorts[NBUART] = {0};
/************************************************************/


/************************************************************/
/*                  PRIVATE PROTOTYPES                      */
/************************************************************/
bool IsUartInterfaceValid(uint8_t ubUartNo);
/************************************************************/


/************************************************************/
/*                  PUBLIC FUNCTIONS                        */
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
void UartInit(uint8_t ubUartNo, sUartParam* sUartParam)
{
	uint16_t usBaudrate;
	uint8_t ubValid = TRUE;
	sUartInit_t* sUartInit = NULL;
	
	ubValid = IsUartInterfaceValid(ubUartNo);
	if(ubValid)
	{
		sUartInit = (sUartInit_t*)UartBase[ubUartNo];
		/*Clear every UART registers*/
		sUartInit->Uxmode 	= 	0x0000;
		sUartInit->Uxsta 	= 	0x0000;
		sUartInit->Uxrxreg 	= 	0x0000;
                sUartInit->Uxtxreg 	=       0x0000;
		
		/*Polarity*/
		sUartInit->Uxmode |= ((sUartParam->RxPolarity)<<4);

		/*BRGH*/
		sUartInit->Uxmode |= ((sUartParam->BRGH)<<3);
		
		if(sUartParam->BRGH == BRGH_LOW_SPEED)
			{
				usBaudrate = (uint16_t)((((FOSC/2.0f)/sUartParam->BaudRate)/16.0f)-1.0f);
			}
		else
			{
				usBaudrate = (uint16_t)((((FOSC/2.0f)/sUartParam->BaudRate)/4.0f)-1.0f);
			}
			
		/*Parity*/
		sUartInit->Uxmode |= ((sUartParam->Parity)<<2);
		
		/*Stop bits*/
		sUartInit->Uxmode |= (sUartParam->StopBit);
		
		/*Baudrate*/
		sUartInit->Uxbrg = usBaudrate;

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
void UartInitPortStruc(uint8_t ubUartNo,
					   void (*pTxfct)(uint8_t ubUartNo, uint8_t ubChar),
					   void (*pRxfct)(uint8_t ubUartNo, uint8_t ubChar))
{
	uint16_t usCounter;
	uint8_t ubValid = FALSE;
	uint8_t ubValidUartNo = 0xFF;
	
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
void UartEcho(uint8_t ubUartNo)
{
	uint8_t ubCounter;
	uint8_t ubValid = FALSE;
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
void UartTxFrame(uint8_t ubUartNo, uint8_t* ubString, uint8_t ubLength)
{

	uint8_t ubCounter;
	uint8_t ubValid = FALSE;
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
void UartTxEnable(uint8_t ubUartNo, bool bState)
{
	uint8_t ubValid = TRUE;
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
void UartInterruptRxEnable(uint8_t ubUartNo, uint16_t usMode, uint8_t ubPriority, bool bState)
{
	uint8_t ubValid = TRUE;
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
				*Ipcx[2] |= (uint16_t)ubPriority<<12;
				*Iecx[0] |= U1RXIE;
				break;
	
				case UART_2:
				*Ipcx[7] |= (uint16_t)ubPriority<<8;
				*Iecx[1] |= U2RXIE;
				break;
	
				case UART_3:
				*Ipcx[20] |= (uint16_t)ubPriority<<8;
				*Iecx[5] |= U3RXIE;
				break;
	
				case UART_4:
				*Ipcx[22] |= (uint16_t)ubPriority;
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
				*Ipcx[2] |= (uint16_t)ubPriority<<12;
				*Iecx[0] &= ~U1RXIE;
				break;
	
				case UART_2:
				*Ipcx[7] |= (uint16_t)ubPriority<<8;
				*Iecx[1] &= ~U2RXIE;
				break;
	
				case UART_3:
				*Ipcx[20] |= (uint16_t)ubPriority<<8;
				*Iecx[5] &= ~U3RXIE;
				break;
	
				case UART_4:
				*Ipcx[22] |= (uint16_t)ubPriority;
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
void UartInterruptTxEnable(uint8_t ubUartNo, uint16_t usMode, uint8_t ubPriority, bool bState)
{
	uint8_t ubValid = TRUE;
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
				*Ipcx[3] |= (uint16_t)ubPriority;
				*Iecx[0] |= U1TXIE;
				break;
	
				case UART_2:
				*Ipcx[7] |= (uint16_t)ubPriority<<12;
				*Iecx[1] |= U2TXIE;
				break;
	
				case UART_3:
				*Ipcx[20] |= (uint16_t)ubPriority<<12;
				*Iecx[5] |= U3TXIE;
				break;
	
				case UART_4:
				*Ipcx[22] |= (uint16_t)ubPriority<<4;
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
				*Ipcx[3] |= (uint16_t)ubPriority;
				*Iecx[0] &= ~U1TXIE;
				break;
	
				case UART_2:
				*Ipcx[7] |= (uint16_t)ubPriority<<12;
				*Iecx[1] &= ~U2TXIE;
				break;
	
				case UART_3:
				*Ipcx[20] |= (uint16_t)ubPriority<<12;
				*Iecx[5] &= ~U3TXIE;
				break;
	
				case UART_4:
				*Ipcx[22] |= (uint16_t)ubPriority<<4;
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
void UartInterruptTx(uint8_t ubUartNo)
{
	uint8_t ubCounter;

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
void UartInterruptRx(uint8_t ubUartNo)
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
bool IsUartInterfaceValid(uint8_t ubUartNo)
{
	uint8_t ubValid = FALSE;

	if(ubUartNo < NBUART)
	{
		ubValid = TRUE;
	}

	return ubValid;
}
/************************************************************/

/************************************************************/
/*                  INTERRUPT FUNCTIONS                     */
/************************************************************/
void __attribute__((interrupt, auto_psv)) _U1TXInterrupt(void)
{
	UartInterruptTx(UART_1);
	*Ifsx[0] &= ~(U1TXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void)
{
	UartInterruptRx(UART_1);
	*Ifsx[0] &= ~(U1RXIF); // clear RX interrupt flag
}
/************************************************************/
void __attribute__((interrupt, auto_psv)) _U2TXInterrupt(void)
{
	*Ifsx[1] &= ~(U2TXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void)
{
	*Ifsx[1] &= ~(U2RXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((interrupt, auto_psv)) _U3TXInterrupt(void)
{
	*Ifsx[5] &= ~(U3TXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((interrupt, auto_psv)) _U3RXInterrupt(void)
{
	*Ifsx[5] &= ~(U3RXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((interrupt, auto_psv)) _U4TXInterrupt(void)
{
	*Ifsx[5] &= ~(U4TXIF); // clear TX interrupt flag
}
/************************************************************/
void __attribute__((interrupt, auto_psv)) _U4RXInterrupt(void)
{
	*Ifsx[5] &= ~(U4RXIF); // clear TX interrupt flag
}
