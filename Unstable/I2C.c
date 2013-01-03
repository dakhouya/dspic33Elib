/************************************************************/
/* i2c.c													*/
/* Library for the I2C module of dspic33E			 		*/
/*															*/
/* MCU : dspic33E											*/
/*															*/
/* Author : David Khouya									*/
/* Date	  :	xx/1/2012										*/
/* Version: 1.0												*/
/************************************************************/

/************************************************************/
/*						   INCLUDES			 				*/
/************************************************************/
#include"i2c.h"
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

static uint16* I2cxbrg[] =
{
	 &I2C1BRG,
	 &I2C2BRG
};

static uint16* I2cxtrn[] =
{
	 &I2C1TRN,
	 &I2C2TRN
};

static uint16* I2cxrcv[] =
{
	 &I2C1RCV,
	 &I2C2RCV
};

/************************************************************/


/************************************************************/
/*			         PRIVATE PROTOTYPES			 			*/
/************************************************************/
void I2CFlag(void);
bool IsI2CInterfaceValid(uint8 ubI2cNo);
/************************************************************/

/************************************************************/
/*				     PUBLIC FUNCTIONS			 			*/
/************************************************************/
/*
Init_I2C
	Initialise the I2C module 

	INPUT 		: 
				-None
				
	OUTPUT 		:	
				-None

*/
/************************************************************/
bool I2C_Init(uint8 ubI2cNo,float fSpeed)
{
	uint8 ubValid = FALSE;


	/*Validity check*/
	ubValid = IsI2CInterfaceValid(ubI2cNo);
	if(fSpeed==0)
	{
		ubValid = FALSE;
	}

	if(ubValid)
	{
		I2cxconbits[ubI2cNo]->I2CEN=0;//Disable I2C
		I2cxconbits[ubI2cNo]->I2CSIDL=0;//Discontinue module IDLE mode
		I2cxconbits[ubI2cNo]->SCLREL=1;//Release clock
		I2cxconbits[ubI2cNo]->IPMIEN=0;//Disable IPMI mode
		I2cxconbits[ubI2cNo]->A10M=0;//Slave address 7bit
		I2cxconbits[ubI2cNo]->DISSLW=1;//Disable Slew rate control
		I2cxconbits[ubI2cNo]->ACKDT=1;//NACK mode
		*I2cxbrg[ubI2cNo] = I2CSPEED(fSpeed);//Set I2C speed
		I2cxconbits[ubI2cNo]->I2CEN=1;//Enable

		/*Restart slaves in a known mode*/
		I2cxconbits[ubI2cNo]->RSEN = 1;

		/*Reset flags and interrupts*/
		IFS1bits.MI2C1IF=0;//Reset MasterI2C Flag
	}
	return ubValid;
}

/************************************************************/
/*
I2C_Send
	Send data to a slave on I2C bus

	INPUT 		: 
				ubAddress		: Slave address
				ubRegister		: Slave Register
				ubData			: Data that will be written in the register
				
	OUTPUT 		:	
				operation status : >=0 OK, -1 Fail
				

*/
/************************************************************/
bool I2C_Send(uint8 ubAddress,uint8 ubRegister,uint8 ubData, uint8 ubI2cNo)
{
	uint8 ubValid = FALSE;

	/*Validity check*/
	ubValid = IsI2CInterfaceValid(ubI2cNo);

	if(ubValid)
	{
		I2cxconbits[ubI2cNo]->SEN=1;//Start
		I2CFlag();//Wait for the end of the transmission

		*I2cxtrn[ubI2cNo]=ubAddress;//Slave Address(Write)
		I2CFlag();

		*I2cxtrn[ubI2cNo]=ubRegister;//Slave Register Address
		I2CFlag();

		*I2cxtrn[ubI2cNo]=ubData;//write data to selected register
		I2CFlag();

		I2cxconbits[ubI2cNo]->PEN=1;//Stop
		I2CFlag();
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
				operation status : >=0 OK, -1 Fail
*/
/************************************************************/
bool I2C_Receive(uint8 ubAddressWrite,uint8 ubAddressRead,uint8 ubRegister,uint8* ubData, uint8 ubI2cNo)
{
	uint8 ubValid = FALSE;

	/*Validity check*/
	ubValid = IsI2CInterfaceValid(ubI2cNo);

	if(ubValid)
	{
	
		I2cxconbits[ubI2cNo]->SEN=1;//Start
		I2CFlag();
	
		*I2cxtrn[ubI2cNo]=ubAddressWrite;//Slave Address(Write)
		I2CFlag();
	
		*I2cxtrn[ubI2cNo]= ubRegister;//Slave Register Address
		I2CFlag();
	
		I2C1CONbits.RSEN = 1;//Restart
		I2CFlag();
	
		*I2cxtrn[ubI2cNo]= ubAddressRead;//Slave Address(Read)
		I2CFlag();
	
		I2cxconbits[ubI2cNo]->RCEN = 1;//Receive mode
		I2CFlag();
	
		*ubData = *I2cxrcv[ubI2cNo];//I2C Receive buffer to unsigned char
		I2cxconbits[ubI2cNo]->ACKEN = 1;//Enable Acknowledge
		I2CFlag();
	
		I2cxconbits[ubI2cNo]->PEN = 1;//Stop
		I2CFlag();
	}

	return ubValid;
}


/************************************************************/
/*				    PRIVATES FUNCTIONS			 			*/
/************************************************************/
/*
I2C_receive
	Read data from a slave on I2C bus 

	INPUT 		: 
				-None
				
	OUTPUT 		:	
				-None

*/
void I2CFlag(void)
{
while(IFS1bits.MI2C1IF==0);
IFS1bits.MI2C1IF=0;//I2C interrupt flag
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
bool IsI2CInterfaceValid(uint8 ubI2cNo)
{
	uint8 ubValid = FALSE;
	if(ubI2cNo < NB_I2C)
	{
		ubValid = TRUE;
	}
	return ubValid;
}