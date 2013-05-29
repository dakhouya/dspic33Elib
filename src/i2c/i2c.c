/************************************************************/
/* i2c.c                                                    */
/* Library for the I2C module of dspic33E                   */
/*                                                          */
/* MCU : dspic33E                                           */
/*                                                          */
/* Author : David Khouya                                    */
/* Date	  :	26/05/2013                                  */
/************************************************************/

/************************************************************/
/*			INCLUDES                            */
/************************************************************/
#include"i2c.h"
/************************************************************/

/************************************************************/
/*                  PUBLIC VARIABLES                       */
/************************************************************/
uint8_t ubI2cTimeout = 0;
/************************************************************/

/************************************************************/
/*                  PRIVATE VARIABLES                       */
/************************************************************/
static I2C1CONBITS* I2cxconbits[] =
{
	 (I2C1CONBITS*)&I2C1CONbits,
	 (I2C1CONBITS*)&I2C2CONbits
};

static I2C1STATBITS* I2cxstatbits[] =
{
	 (I2C1STATBITS*)&I2C1STATbits,
	 (I2C1STATBITS*)&I2C2STATbits
};

static uint16_t* I2cxbrg[] =
{
	 (uint16_t*)&I2C1BRG,
	 (uint16_t*)&I2C2BRG
};

static uint16_t* I2cxtrn[] =
{
	 (uint16_t*)&I2C1TRN,
	 (uint16_t*)&I2C2TRN
};

static uint16_t* I2cxrcv[] =
{
	 (uint16_t*)&I2C1RCV,
	 (uint16_t*)&I2C2RCV
};

/************************************************************/


/************************************************************/
/*			PRIVATE PROTOTYPES                  */
/************************************************************/
bool I2CFlag(uint8_t ubI2cNo);
bool IsI2CInterfaceValid(uint8_t ubI2cNo);
/************************************************************/

/************************************************************/
/*			PUBLIC FUNCTIONS                    */
/************************************************************/
/*
Init_I2C
	Initialise the I2C module 

	INPUT 		: 
                                ubI2cNo :   Used Interface
                                fSpeed  :   Operating Speed in Hz
	OUTPUT 		:	
                                bool    :   False if it failed

*/
/************************************************************/
bool I2C_Init(uint8_t ubI2cNo,float fSpeed)
{
    uint8_t ubValid = FALSE;


    /*Validity check*/
    ubValid = IsI2CInterfaceValid(ubI2cNo);
    if(fSpeed==0)
    {
        ubValid = FALSE;
    }

    if(ubValid)
    {
        I2cxconbits[ubI2cNo]->I2CEN=0;          //Disable I2C
        I2cxconbits[ubI2cNo]->I2CSIDL=0;        //Discontinue module IDLE mode
        I2cxconbits[ubI2cNo]->SCLREL=1;         //Release clock
        I2cxconbits[ubI2cNo]->IPMIEN=0;         //Disable IPMI mode
        I2cxconbits[ubI2cNo]->A10M=0;           //Slave address 7bit
        I2cxconbits[ubI2cNo]->DISSLW=1;         //Disable Slew rate control
        I2cxconbits[ubI2cNo]->ACKDT=1;          //NACK mode
        *I2cxbrg[ubI2cNo] = I2CSPEED(fSpeed);   //Set I2C speed
        I2cxconbits[ubI2cNo]->I2CEN=1;          //Enable

        /*Restart slaves in a known mode*/
        I2cxconbits[ubI2cNo]->RSEN = 1;
        I2CFlag(ubI2cNo);
        I2cxconbits[ubI2cNo]->PEN = 1;//Stop
	I2CFlag(ubI2cNo);
    }
    return ubValid;
}

/************************************************************/
/*
I2C_InterruptEnable
        Set the interrupt configuration for I2C module

	INPUT 		: 
                                ubI2cNo     :   Used Interface
                                ubPriority  :   Interrupt priority
                                bState      :   Interrupt State(enable or disable)
	OUTPUT 		:	
                                bool    :   False if it failed

*/
/************************************************************/
bool I2C_InterruptEnable(uint8_t ubI2cNo, uint8_t ubPriority, bool bState)
{
    uint8_t ubValid = FALSE;


    /*Validity check*/
    ubValid = IsI2CInterfaceValid(ubI2cNo);
    if(ubPriority>0x07)
    {
        ubValid = FALSE;
    }

    if(ubValid)
    {
        switch(ubI2cNo)
        {
            case I2C_1:
                /*Set Priority*/
                IPC4bits.MI2C1IP = ubPriority;
                /*Enable or disable interrup*/
                IEC1bits.MI2C1IE = bState;
                /*Clear flag*/
                IFS1bits.MI2C1IF = 0;
                break;

            case I2C_2:
                /*Set Priority*/
                IPC12bits.MI2C2IP = ubPriority;
                /*Enable or disable interrup*/
                IEC3bits.MI2C2IE = bState;
                /*Clear flag*/
                IFS3bits.MI2C2IF = 0;
                break;
        }
    }
    return ubValid;
}

/************************************************************/
/*
I2C_Send
	Send data to a slave on I2C bus

	INPUT 		: 
				ubAddress   : Slave address
				ubRegister  : Slave Register
				ubData      : Data that will be written in the register
				
	OUTPUT 		:	
				bool        :   False if it failed
				

*/
/************************************************************/
bool I2C_Send(uint8_t ubAddress,uint8_t ubRegister,uint8_t ubData, uint8_t ubI2cNo)
{
	uint8_t ubValid = FALSE;

	/*Validity check*/
	ubValid = IsI2CInterfaceValid(ubI2cNo);

	if(ubValid)
	{
		I2cxconbits[ubI2cNo]->SEN=1;//Start
		I2CFlag(ubI2cNo);//Wait for the end of the transmission

		*I2cxtrn[ubI2cNo]=ubAddress;//Slave Address(Write)
		I2CFlag(ubI2cNo);

		*I2cxtrn[ubI2cNo]=ubRegister;//Slave Register Address
		I2CFlag(ubI2cNo);

		*I2cxtrn[ubI2cNo]=ubData;//write data to selected register
		I2CFlag(ubI2cNo);

		I2cxconbits[ubI2cNo]->PEN=1;//Stop
		I2CFlag(ubI2cNo);
	}
	return ubValid;
}

/************************************************************/
/*
I2C_receive
	Read data from a slave on I2C bus

	INPUT 		: 
				address_w 	: Slave write address
				address_r 	: Slave read address
				reg 		: Slave register 
				data		: Receive buffer
				
	OUTPUT 		:	
				bool            :   False if it failed
*/
/************************************************************/
bool I2C_Receive(uint8_t ubAddressWrite,uint8_t ubAddressRead,uint8_t ubRegister,uint8_t* ubData, uint8_t ubI2cNo)
{
	uint8_t ubValid = FALSE;

	/*Validity check*/
	ubValid = IsI2CInterfaceValid(ubI2cNo);

	if(ubValid)
	{
	
		I2cxconbits[ubI2cNo]->SEN=1;//Start
		I2CFlag(ubI2cNo);
	
		*I2cxtrn[ubI2cNo]=ubAddressWrite;//Slave Address(Write)
		I2CFlag(ubI2cNo);
	
		*I2cxtrn[ubI2cNo]= ubRegister;//Slave Register Address
		I2CFlag(ubI2cNo);
	
		I2C1CONbits.RSEN = 1;//Restart
		I2CFlag(ubI2cNo);
	
		*I2cxtrn[ubI2cNo]= ubAddressRead;//Slave Address(Read)
		I2CFlag(ubI2cNo);
	
		I2cxconbits[ubI2cNo]->RCEN = 1;//Receive mode
		I2CFlag(ubI2cNo);
	
		*ubData = *I2cxrcv[ubI2cNo];//I2C Receive buffer to unsigned char
		I2cxconbits[ubI2cNo]->ACKEN = 1;//Enable Acknowledge
		I2CFlag(ubI2cNo);
	
		I2cxconbits[ubI2cNo]->PEN = 1;//Stop
		I2CFlag(ubI2cNo);
	}

	return ubValid;
}


/************************************************************/
/*			PRIVATES FUNCTIONS                  */
/************************************************************/
/*
I2C_receive
	Read data from a slave on I2C bus 

	INPUT 		: 
				-None
				
	OUTPUT 		:	
				bool            :   False if there's a timeout

*/
/************************************************************/
bool I2CFlag(uint8_t ubI2cNo)
{
    bool bValid = TRUE;
    bValid = IsI2CInterfaceValid(ubI2cNo);
    
    if(bValid)
    {
        switch(ubI2cNo)
        {
            case I2C_1:
                /*Clear flag*/
                while(IFS1bits.MI2C1IF == 0 || !ubI2cTimeout);
                break;

            case I2C_2:
                /*Clear flag*/
                while(IFS3bits.MI2C2IF == 0 || !ubI2cTimeout);
                break;
        }
    }

    if(ubI2cTimeout)
    {
        bValid = FALSE;
    }


    IFS1bits.MI2C1IF=0;//I2C interrupt flag

    return bValid;
}
/************************************************************/
/*
IsI2CInterfaceValid
	Read data from a slave on I2C bus 

	INPUT 		: 
				-None
				
	OUTPUT 		:	
				-TRUE if the I2C address is good, FALSE if it's not

*/
/************************************************************/
bool IsI2CInterfaceValid(uint8_t ubI2cNo)
{
	uint8_t ubValid = FALSE;
	if(ubI2cNo < NB_I2C)
	{
		ubValid = TRUE;
	}
	return ubValid;
}
