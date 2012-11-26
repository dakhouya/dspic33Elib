/************************************************************/
/* uart.h													*/
/* Library for the UART module of dspic33E			 		*/
/*															*/
/* MCU : dspic33E											*/
/*															*/
/* Author : David Khouya									*/
/* Date	  :	xx/11/2012										*/
/* Version: 0.1 											*/
/************************************************************/

/************************************************************/
#ifndef _UART_MODULE_
#define _UART_MODULE_
/************************************************************/

/************************************************************/
/*						   INCLUDES			 				*/
/************************************************************/
//#include "p33EP512MU810.h"
#include "globaldef.h"
#include "p33EP512MC806.h"

/************************************************************/


/************************************************************/
/*			           MACRO DEFINITIONS			 		*/
/************************************************************/
/*Should be in hardware profil definitions*/
#define ifndef CPU_CLOCK
#define CPU_CLOCK 				120000000
#endif

#define UART_BUFFER_SIZE 		256
#define NBUART 					4

/*For BRGG = 0*/
#define BAUDRATE(BAUD) 			(((CPU_CLOCK)/2)/(16*BAUD))-1

/*For BRGG = 1*/
//#define BAUDRATE(BAUD) 		(((CPU_CLOCK)/2)/(4*BAUD))-1

/*UartParam*/
#define	BRGH_LOW_SPEED			0
#define	BRGH_LOW_SPEED			1

#define 8BITS_NOPARITY			0
#define 8BITS_EVENPARITY		1
#define 8BITS_ODDPARITY			2
#define 9BITS_NOPARITY 			3

#define	2STOP_BITS				0
#define	1STOP_BIT				1

#define 2400BAUD				BAUDRATE(2400)
#define 4800BAUD				BAUDRATE(4800)
#define 9600BAUD				BAUDRATE(9600)
#define 14400BAUD				BAUDRATE(14400)
#define 19600BAUD				BAUDRATE(19600)
#define 38400BAUD				BAUDRATE(38400)	
#define 57600BAUD				BAUDRATE(57600)
#define 115200BAUD				BAUDRATE(115200)	
/************************************************************/


/************************************************************/
/*			        STRUCTURE DEFINITIONS			 		*/
/************************************************************/
typedef struct Param
{
	uint8 BRGH;
	uint8 RxPolarity;
	uint8 Parity;
	uint8 StopBit;
	uint8 BaudRate;
}sUartParam;


typedef struct UartPort
{
	uint8 	Uartno;
	uint8	TxMessage[UART_BUFFER_SIZE];
	uint8	TxMessageLength;
	uint8*	RxMessage;
	uint8	TxIsBusy;
	void	(*Txfct)(void);
	void	(*Rxfct)(void); 
}sUartPort_t;

typedef struct UartInit
{
	uint16	Uxmode;
	uint16	Uxsta;
	uint16  Uxtxreg;
	uint16	Uxrxreg;
	uint16	Uxbrg;
}sUartInit_t;
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
/*				     PUBLIC PROTOTYPES			 			*/
/************************************************************/
void UartInit(uint8 ubUartNo, sUartParam*)
void UartInitPort(sUartPort_t* sUartPort);
void UartEcho(&ubChar ubChar);
void UartTxFrame(sUartPort_t* sUartPort);

void UartRxEnable(uint8 ubUartNo, bool state);
void UartTxEnable(uint8 ubUartNo, bool state);
void UartInterruptRxEnable(uint8 ubUartNo, bool state);
void UartInterruptTxEnable(uint8 ubUartNo, bool state);
/************************************************************/


/************************************************************/
#endif
/************************************************************/
