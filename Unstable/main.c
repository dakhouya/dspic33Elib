#include "p33EP512MC806.h"
#include "globaldef.h"
#include "uart.h"
sUartParam_t sUartParam;

int main(void)
{
	sUartParam.BRGH=BRGH_LOW_SPEED;
	sUartParam.RxPolarity=1;
	sUartParam.Parity=UART_8BITS_NOPARITY;
	sUartParam.StopBit=UART_1STOP_BIT;
	sUartParam.BaudRate=(uint8)BAUDRATE(9600);
		UartInit(1, &sUartParam);
	while(1)
	{
	}
	return 0;
}