/************************************************************/
/* i2c.h                                                    */
/* Library for the I2C module of dspic33E                   */
/*                                                          */
/* MCU : dspic33E                                           */
/*                                                          */
/* Author : David Khouya                                    */
/* Date	  : 26/05/2013                                 	    */
/************************************************************/

/************************************************************/
#ifndef I2C_MODULE
#define I2C_MODULE
/************************************************************/

/************************************************************/
/*			INCLUDES                            */
/************************************************************/
#include"../p33EP512MC806.h"
#include "../globaldef.h"
#include "../hardware_profile.h"
/************************************************************/

/************************************************************/
/*                  MACRO DEFINITIONS                       */
/************************************************************/
/*Macro for I2C bus speed calculation*/
#define I2CSPEED(DESIREDSPEED)  ((1.0f/((1.0f/(FOSC/2.0f))*(DESIREDSPEED)))-1.0f)
	
/*Number of I2C interfaces*/
#define NB_I2C					2

/*Interface for user*/
#define I2C_1					0
#define I2C_2					1

/************************************************************/


/************************************************************/
/*                      PUBLIC PROTOTYPES                   */
/************************************************************/
bool I2C_Init(uint8_t ubI2cNo, float fSpeed);
bool I2C_InterruptEnable(uint8_t ubI2cNo, uint8_t ubPriority, bool bState);
bool I2C_Send(uint8_t ubAddress,uint8_t ubRegister,uint8_t ubData, uint8_t ubI2cNo);
bool I2C_Receive(uint8_t ubAddressWrite,uint8_t ubAddressRead,uint8_t ubRegister,uint8_t* ubData, uint8_t ubI2cNo);
/************************************************************/

/************************************************************/
#endif
/************************************************************/


