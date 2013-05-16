/************************************************************/
/* spi.h                                                    */
/* Library for the SPI module of dspic33E                   */
/*                                                          */
/* MCU : dspic33E                                           */
/*                                                          */
/* Author : David Khouya                                    */
/* Date	  :	02/04/2013                                  */
/* Version: 1.0                                             */
/************************************************************/

/************************************************************/
#ifndef _SPI_MODULE_
#define _SPI_MODULE_
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
/*Number of SPI interfaces*/
#define NB_SPI					4

/*Interface for user*/
#define SPI_1					0
#define SPI_2					1
#define SPI_3					2
#define SPI_4					3

/*SPI parameters*/
/*Transmission Mode*/
#define MODE_8_BITS                             0
#define MODE_16_BITS                            1

/*Sample mode*/
#define SAMPLE_MIDDLE_DATA_OUTPUT_TIME          0
#define SAMPLE_END_DATA_OUTPUT_TIME             1

/*Clock edge*/
#define OUTPUT_IDLE_TO_ACTIVE_CLK               0
#define OUTPUT_ACTIVE_TO_IDLE_CLK               1

/*Clock Polarity*/
#define IDLE_STATE_LOW                          0
#define IDLE_STATE_HIGH                         1

/*Primary Prescaler*/
#define PRIMARY_64_TO_1                         0
#define PRIMARY_16_TO_1                         1
#define PRIMARY_4_TO_1                          2

/*Secondary Prescaler*/
#define SECONDARY_8_TO_1                        0
#define SECONDARY_7_TO_1                        1
#define SECONDARY_6_TO_1                        2
#define SECONDARY_5_TO_1                        3
#define SECONDARY_4_TO_1                        4
#define SECONDARY_3_TO_1                        5
#define SECONDARY_2_TO_1                        6
/************************************************************/


/************************************************************/
/*                  STRUCTURE DEFINITIONS                   */
/************************************************************/
typedef struct sSPIParam
{
	uint8_t ubTransmissionMode;
	uint8_t ubSampleMode;
	uint8_t ubClkEdge;
	uint8_t ubClkIdle;
	uint8_t ubPrimaryPrescaler;
        uint8_t ubSecondaryPrescaler;
}sSPIParam_t;

/************************************************************/


/************************************************************/
/*                      PUBLIC VARIABLES                    */
/************************************************************/

/************************************************************/


/************************************************************/
/*                      PRIVATE VARIABLES                   */
/************************************************************/

/************************************************************/


/************************************************************/
/*                      PUBLIC PROTOTYPES                   */
/************************************************************/
bool SPIInit(uint8_t ubSpiNo, sSPIParam_t* sParam);
bool SPIInterruptEnable(uint8_t ubSpiNo, uint8_t ubPriority, bool bState);
bool SPISend8Bits(uint8_t ubData, uint8_t ubSpiNo);
bool SPISend16Bits(uint16_t usData, uint8_t ubSpiNo);
//bool SPIReceive(uint8_t ubRegister,uint8_t* ubData, uint8_t ubSpiNo);
/************************************************************/

/************************************************************/
#endif
/************************************************************/
