/************************************************************/
/* spi.c                                                    */
/* Library for the SPI module of dspic33E                   */
/*                                                          */
/* MCU : dspic33E                                           */
/*                                                          */
/* Author : David Khouya                                    */
/* Date	  : 29/04/2013                                      */
/************************************************************/

/************************************************************/
/*			INCLUDES                            */
/************************************************************/
#include"spi.h"
/************************************************************/


/************************************************************/
/*                  PRIVATE VARIABLES                       */
/************************************************************/
static SPI1CON1BITS* Spixcon1bits[] =
{
	 (SPI1CON1BITS*)&SPI1CON1bits,
	 (SPI1CON1BITS*)&SPI2CON1bits,
	 (SPI1CON1BITS*)&SPI3CON1bits,
	 (SPI1CON1BITS*)&SPI4CON1bits
};

static SPI1CON2BITS* Spixcon2bits[] =
{
	 (SPI1CON2BITS*)&SPI1CON2bits,
	 (SPI1CON2BITS*)&SPI2CON2bits,
	 (SPI1CON2BITS*)&SPI3CON2bits,
	 (SPI1CON2BITS*)&SPI4CON2bits
};

static SPI1STATBITS* Spixstatbits[] =
{
	 (SPI1STATBITS*)&SPI1STATbits,
	 (SPI1STATBITS*)&SPI2STATbits,
	 (SPI1STATBITS*)&SPI3STATbits,
	 (SPI1STATBITS*)&SPI4STATbits
};

static uint16_t* Spixbuf[] =
{
	 (uint16_t*)&SPI1BUF,
	 (uint16_t*)&SPI2BUF,
	 (uint16_t*)&SPI3BUF,
	 (uint16_t*)&SPI4BUF
};

/************************************************************/


/************************************************************/
/*			PRIVATE PROTOTYPES                  */
/************************************************************/
bool IsSPIInterfaceValid(uint8_t ubSpiNo);
/************************************************************/

/************************************************************/
/*			PUBLIC FUNCTIONS                    */
/************************************************************/
/*
 SPIInit
	Initialise the SPI module

	INPUT 		:
                                ubSpiNo :   Used Interface
                                fSpeed  :   Operating Speed in Hz
	OUTPUT 		:
                                bool    :   False if it failed

*/
/************************************************************/
bool SPIInit(uint8_t ubSpiNo, sSPIParam_t* sParam)
{
    uint8_t ubValid = FALSE;


    /*Validity check*/
    ubValid = IsSPIInterfaceValid(ubSpiNo);
    if(ubValid)
    {
        Spixstatbits[ubSpiNo]->SPIEN = DISABLE;                         /*Disable SPI module*/
        Spixcon1bits[ubSpiNo]->CKE = sParam->ubClkEdge;                 /*Set Clk edge polarity*/
        Spixcon1bits[ubSpiNo]->CKP = sParam->ubClkIdle;                 /*Set Clk Idle State*/
        Spixcon1bits[ubSpiNo]->MODE16 = sParam->ubTransmissionMode;     /*SPI 8 or 16 bits mode*/
        Spixcon1bits[ubSpiNo]->PPRE = sParam->ubPrimaryPrescaler;       /*Primary Prescaler*/
        Spixcon1bits[ubSpiNo]->SPRE = sParam->ubSecondaryPrescaler;     /*Secondary Prescaler*/
        Spixcon1bits[ubSpiNo]->MSTEN = 1;                               /*Set to master mode*/
        Spixcon1bits[ubSpiNo]->DISSDO = 0;                              /*SDO pin mode*/
        Spixcon1bits[ubSpiNo]->DISSCK = 0;                              /*SCK pin mode*/
        Spixcon2bits[ubSpiNo]->SPIBEN = 1;                              /*Enable enhanced buffer*/
        Spixstatbits[ubSpiNo]->SPIEN = ENABLE;                          /*Enable SPI module*/
    }
    return ubValid;
}

/************************************************************/
/*
SPIInterruptEnable
        Set the interrupt configuration for SPI module

	INPUT 		:
                                ubSpiNo     :   Used Interface
                                ubPriority  :   Interrupt priority
                                bState      :   Interrupt State(enable or disable)
	OUTPUT 		:
                                bool    :   False if it failed

*/
/************************************************************/
bool SPIInterruptEnable(uint8_t ubSpiNo, uint8_t ubPriority, bool bState)
{
    uint8_t ubValid = FALSE;


    /*Validity check*/
    ubValid = IsSPIInterfaceValid(ubSpiNo);
    if(ubPriority>7)
    {
        ubValid = FALSE;
    }

    if(ubValid)
    {
        switch(ubSpiNo)
        {
            case SPI_1:
                /*Set Priority*/
                IPC2bits.SPI1IP = ubPriority;
                /*Enable or disable interrup*/
                IEC0bits.SPI1IE = bState;
                /*Clear flag*/
                IFS0bits.SPI1IF = 0;
                break;

            case SPI_2:
                /*Set Priority*/
                IPC8bits.SPI2IP = ubPriority;
                /*Enable or disable interrup*/
                IEC2bits.SPI2IE= bState;
                /*Clear flag*/
                IFS2bits.SPI2IF = 0;
                break;

            case SPI_3:
                /*Set Priority*/
                IPC22bits.SPI3IP = ubPriority;
                /*Enable or disable interrup*/
                IEC5bits.SPI3IE = bState;
                /*Clear flag*/
                IFS5bits.SPI3IF = 0;
                break;

            case SPI_4:
                /*Set Priority*/
                IPC30bits.SPI4IP = ubPriority;
                /*Enable or disable interrup*/
                IEC7bits.SPI4IE = bState;
                /*Clear flag*/
                IFS7bits.SPI4IF = 0;
                break;
        }
    }
    return ubValid;
}

/************************************************************/
/*
SPISend8Bits
        Send 16 bits data lenght on SPI bus

	INPUT 		:
				usData 		: 8bits to send
				ubSpiNo		: Used interface

	OUTPUT 		:
				bool        :   False if it failed


*/
/************************************************************/
bool SPISend8Bits(uint8_t ubData, uint8_t ubSpiNo)
{
	uint8_t ubValid = FALSE;

	/*Validity check*/
	ubValid = IsSPIInterfaceValid(ubSpiNo);

        /*Check if the buffer is empty -> SRMPT == 0 when it's not empty*/
        if(!(Spixstatbits[ubSpiNo]->SRMPT))
        {
            ubValid = FALSE;
        }

	if(ubValid)
	{
            /*Send 8 bits on SPI bus*/
            *Spixbuf[ubSpiNo] = ubData;
	}
	return ubValid;
}

/************************************************************/
/*
SPISend16Bits
        Send 16 bits data lenght on SPI bus

	INPUT 		:
				usData 		: 16bits to send
				ubSpiNo		: Used interface

	OUTPUT 		:
				bool            :   False if it failed
*/
/************************************************************/
bool SPISend16Bits(uint16_t usData, uint8_t ubSpiNo)
{
	uint8_t ubValid = FALSE;

	/*Validity check*/
	ubValid = IsSPIInterfaceValid(ubSpiNo);

        /*Check if the buffer is empty -> SRMPT == 0 when it's not empty*/
        if(!(Spixstatbits[ubSpiNo]->SRMPT))
        {
            ubValid = FALSE;
        }

	if(ubValid)
	{
            /*Send 16 bits on SPI bus*/
            *Spixbuf[ubSpiNo] = usData;
	}
	return ubValid;
}

/************************************************************/
/*			PRIVATES FUNCTIONS                  */
/************************************************************/

/************************************************************/
/*
IsSPIInterfaceValid
	Read data from a slave on SPI bus

	INPUT 		:
				-None

	OUTPUT 		:
				-TRUE if the SPI address is good, FALSE if it's not

*/
/************************************************************/
bool IsSPIInterfaceValid(uint8_t ubSpiNo)
{
	uint8_t ubValid = FALSE;
	if(ubSpiNo < NB_SPI)
	{
		ubValid = TRUE;
	}
	return ubValid;
}
