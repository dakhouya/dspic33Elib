#include"p33EP512MC806.h"
#include"globaldef.h"
#include"hardware_profile.h"

void I2CInit(void)
{
IEC1bits.MI2C1IE=0;//I2C interrupt
I2C1CONbits.I2CEN=0;//Disable I2C
I2C1CONbits.I2CSIDL=0;//Discontinue module IDLE mode
I2C1CONbits.SCLREL=1;//Release clock
I2C1CONbits.IPMIEN=0;//Disable IPMI mode
I2C1CONbits.A10M=0;//Slave address 7bit
I2C1CONbits.DISSLW=1;//Disable Slew rate control
I2C1CONbits.ACKDT=1;//NACK mode
I2C1BRG =68; //Set BAUD rate 
I2C1CONbits.I2CEN=1;//Enable
IFS1bits.MI2C1IF=0;//Reset MasterI2C Flag
}

void I2CFlag(void)
{
while(IFS1bits.MI2C1IF==0);
IFS1bits.MI2C1IF=0;//I2C interrupt flag
}

I2CWrite(unsigned char Address,unsigned char Register,unsigned char data)
{
I2C1CONbits.SEN=1;//Start
I2CFlag();//Wait for the end of the transmission
I2C1TRN=Address;//Slave Address(Write)
I2CFlag();
I2C1TRN=Register;//Slave Register Address
I2CFlag();
I2C1TRN=data;//write data to selected register
I2CFlag();
I2C1CONbits.PEN=1;//Stop
I2CFlag();
}

char I2C_Receive(unsigned char address_w,unsigned char address_r,unsigned char reg,unsigned char * data)
{
	char error = 0;
	
	I2C1CONbits.SEN=1;			//Start
	I2CFlag();

	I2C1TRN = address_w;			//Slave Address(Write)
	I2CFlag();

	I2C1TRN = reg;				//Slave Register Address
	I2CFlag();

	I2C1CONbits.RSEN = 1;		//Restart
	I2CFlag();

	I2C1TRN = address_r;			//Slave Address(Read)
	I2CFlag();

	I2C1CONbits.RCEN = 1;		//Receive mode
	I2CFlag();

	*data = I2C1RCV;				//I2C Receive buffer to unsigned char
	I2C1CONbits.ACKEN = 1;		//Enable Acknowledge
	I2CFlag();

	I2C1CONbits.PEN = 1;			//Stop
	I2CFlag();

	return error;
}