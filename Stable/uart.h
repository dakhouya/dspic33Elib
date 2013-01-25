/************************************************************/
/* uart.h													*/
/* Library for the UART module of dspic33E			 		*/
/*															*/
/* MCU : dspic33E											*/
/*															*/
/* Author : David Khouya									*/
/* Date	  :	25/01/2012										*/
/* Version: 1.0												*/
/************************************************************/

/************************************************************/
#ifndef _UART_MODULE_
#define _UART_MODULE_
/************************************************************/

/************************************************************/
/*						   INCLUDES			 				*/
/************************************************************/
#include "globaldef.h"
#include "hardware_profile.h"
/************************************************************/


/************************************************************/
/*			           MACRO DEFINITIONS			 		*/
/************************************************************/
/*Should be in hardware profil definitions*/
/*#ifndef CPU_CLOCK
#define CPU_CLOCK 				20000000.0f
#endif*/

#define UART_BUFFER_SIZE 		256
#define NBUART 					4

#define UART_1					0
#define UART_2					1
#define UART_3					2
#define UART_4					3

/*Formula doesn't work very well...*/
/*For BRGH = 0*/
#define BAUDRATE(BAUD) 			((CPU_CLOCK/BAUD)/90.0f)-1.0f
/*For BRGH = 1*/
//#define BAUDRATE(BAUD) 			((CPU_CLOCK/BAUD)/4.0f)-1.0f

/*UartParam*/
#define	BRGH_LOW_SPEED				0
#define	BRGH_HIGH_SPEED				1

#define UART_8BITS_NOPARITY			0
#define UART_8BITS_EVENPARITY		1
#define UART_8BITS_ODDPARITY		2
#define UART_9BITS_NOPARITY 		3

#define	UART_2STOP_BITS				0
#define	UART_1STOP_BIT				1

#define UART_2400BAUD				BAUDRATE(2400.0f)
#define UART_4800BAUD				BAUDRATE(4800.0f)
#define UART_9600BAUD				BAUDRATE(9600.0f)
#define UART_14400BAUD				BAUDRATE(14400.0f)
#define UART_19600BAUD				BAUDRATE(19600.0f)
#define UART_38400BAUD				BAUDRATE(38400.0f)	
#define UART_57600BAUD				BAUDRATE(57600.0f)
#define UART_115200BAUD				BAUDRATE(115200.0f)	

/*TX Interrupt Mode*/
#define	CHAR_N_BUFFER_EMPTY			  0x8000		
#define	TRANSMIT_OPERATION_COMPLETED  0x2000
#define ANY_CHAR_N_BUFFER_EMPTY		  0x0000

/*RX Interrupt Mode*/
#define	BUFFER_FULL					  0x0080	//4 char
#define	BUFFER_3_4_FULL				  0x0020	//3 char
#define CHAR_RECEIVE				  0x0000	//1 char

/*Registers*/
/*UXMODE*/
#define URXINV						0x0010
#define ABAUD						0x0020
#define	LPBACK						0x0040
#define	WAKE						0x0080
#define RTSMD						0x0800
#define IREN						0x1000
#define USIDL						0x2000
#define	UARTEN						0x8000

/*UXSTA*/
#define URXDA						0x0001
#define OERR						0x0002
#define FERR						0x0004
#define PERR						0x0008
#define RIDLE						0x0010
#define ADDEN						0x0020
#define TRMT						0x0100
#define UTXBF						0x0200
#define UTXEN						0x0400
#define UTXBRK						0x0800
#define UTXINV						0x4000

/*Interrupt and Flags*/
/*IF0 & IEC0*/
#define U1TXIE						0x1000
#define U1RXIE						0x0800
#define U1TXIF						0x1000
#define U1RXIF						0x0800

/*IFS1 & IEC1*/
#define U2TXIE						0x8000
#define U2RXIE						0x4000
#define U2TXIF						0x8000
#define U2RXIF						0x4000

/*IFS5 & IEC5*/
#define U3TXIE						0x0008
#define U3RXIE						0x0004
#define U3TXIF						0x0008
#define U3RXIF						0x0004

/*IFS5 & IEC5*/
#define U4TXIE						0x0020
#define U4RXIE						0x0010
#define U4TXIF						0x0020
#define U4RXIF						0x0010

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
	uint8	TxBuffer[UART_BUFFER_SIZE];
	uint8	RxBuffer[UART_BUFFER_SIZE];
	uint8	TxLocation;
	uint8	TxMessageLength;
	uint8	RxMessageLength;
	uint8	TxIsBusy;
	void	(*Txfct)(uint8 ubUartNo, uint8 ubChar);
	void	(*Rxfct)(uint8 ubUartNo, uint8 ubChar); 
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
void UartInit(uint8 ubUartNo, sUartParam*);
void UartInitPortStruc(uint8 ubUartNo, 
					   void (*Txfct)(uint8 ubUartNo, uint8 ubChar), 
					   void (*Rxfct)(uint8 ubUartNo, uint8 ubChar));
void UartEcho(uint8 ubUartNo);
void UartTxFrame(uint8 ubUartNo, uint8* ubString, uint8 ubLength);

void UartTxEnable(uint8 ubUartNo, bool state);
void UartInterruptRxEnable(uint8 ubUartNo, uint16 usMode, uint8 ubPriority, bool state);
void UartInterruptTxEnable(uint8 ubUartNo, uint16 usMode, uint8 ubPriority, bool state);

void UartInterruptTx(uint8 ubUartNo);
void UartInterruptRx(uint8 ubUartNo);
void IntToChar(char tab[], int tab_l, int * number);
/************************************************************/


/************************************************************/
#endif
/************************************************************/
