/************************************************************/
/* I2C.h													*/
/* Library for the I2C module of the MCU DSPIC30F5011 		*/
/* Monitoring circuit(V1.0)									*/
/*															*/
/* MCU : DSPIC30F5011										*/
/*															*/
/* Author : David Khouya, Chinook 2							*/
/* Date	  :	03/05/2012										*/
/* Version: 0.1												*/
/************************************************************/

/************************************************************/
#ifndef I2C_MODULE
#define I2C_MODULE
/************************************************************/

/************************************************************/
/*						   INCLUDES			 				*/
/************************************************************/
#include"p33EP512MC806.h"
#include "hardware_profile.h"
/************************************************************/

/************************************************************/
/*				       PUBLIC FUNCTIONS			 			*/
/************************************************************/
/*
Init_I2C
	Initialise the I2C module 

	INPUT 		: 
				-None
				
	OUTPUT 		:	
				-None
				
	EFFECT 		:
				-Initialise the I2C module

	SPEC		: 	
				-Interrupt are not enable
				-ERRATA : DSPIC30F5011 should work at 20MIPS or less
				
	CALL Example:

*/
void Init_I2C(void);
/************************************************************/
/*
I2C_Send
	Send data to a slave on I2C bus

	INPUT 		: 
				-(unsigned char) Slave address
				-(unsigned char) Slave Register
				-(unsigned char) Data that will be written in the register
				
	OUTPUT 		:	
				-(char) operation status : >=0 OK, -1 Fail
				
	EFFECT 		:
				-This routine can't be interrupt

	SPEC		: 	
				-This function is working with 8bits address,register and data only
				
				
	CALL Example:

*/
char I2C_Send(unsigned char address,unsigned char reg,unsigned char data);
/************************************************************/
/*
I2C_receive
	Read data from a slave on I2C bus

	INPUT 		: 
				-(unsigned char) Slave write address
				-(unsigned char) Slave read address
				-(unsigned char) Slave register 
				-(unsigned char*) variable for the receive byte
				
	OUTPUT 		:	
				-(char) operation status : >=0 OK, -1 Fail
				
	EFFECT 		:
				-This routine can't be interrupt

	SPEC		: 	
				-This function is working with 8bits addresses and data only
				
				
	CALL Example:

*/
char I2C_Receive(unsigned char address_w,unsigned char address_r,unsigned char reg,unsigned char * data);
/************************************************************/

/************************************************************/
#endif
/************************************************************/