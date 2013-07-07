/***********************************************************************
* lib_CAN.c
* Cette librairie gï¿½re le bus CAN. Il permet la configuration initial du
* bus, l'envoi et la rï¿½ception de messages. 
* 
* AUTEUR: 	Etienne Collard-Frï¿½chette (WalkingMachine)
* DATE:		1 mars 2011
* MODIFIï¿½:	BAJA ETS (11 oct. 2011)
************************************************************************
****************************** INCLUDE *********************************/
#define DSPIC33F
#ifdef DSPIC33F
#include "can.h"
#include "global.h"
#include "can_chinook3.h"
#include "p33EP512MC806.h"
#include <stdlib.h>
#include "dma.h"


/* Fonction privï¿½e. */
//static void init_DMA_channel(unsigned int DMA_channel, char direction, unsigned int IRQ, unsigned int PAD, unsigned int offset);

static unsigned int set_SID_reg(unsigned long ID, T_TYPE_ID type_ID, char mask_or_filter);

static unsigned int set_EID_reg(unsigned long ID, T_TYPE_ID type_ID);

/***************** DÉCLARATION DES VARIABLES GLOBALES ******************/
/* Buffer utilisé dans la DMA. */

__eds__ unsigned int CAN_msg_Buf[32][8] __attribute__((space(dma),eds,aligned(32*16)));
T_CAN_CONFIG config_CAN;

/* Les tableaux suivant contiennent les adresses des diffï¿½rent registres. */
/* Utilisï¿½ pour les accï¿½der plus facilement. */
/*
volatile unsigned int * DMAiCON[] = {&DMA0CON, &DMA1CON, &DMA2CON, &DMA3CON, &DMA4CON, &DMA5CON, &DMA6CON, &DMA7CON};
volatile unsigned int * DMAiPAD[] = {&DMA0PAD, &DMA1PAD, &DMA2PAD, &DMA3PAD, &DMA4PAD, &DMA5PAD, &DMA6PAD, &DMA7PAD};
volatile unsigned int * DMAiCNT[] = {&DMA0CNT, &DMA1CNT, &DMA2CNT, &DMA3CNT, &DMA4CNT, &DMA5CNT, &DMA6CNT, &DMA7CNT};
volatile unsigned int * DMAiREQ[] = {&DMA0REQ, &DMA1REQ, &DMA2REQ, &DMA3REQ, &DMA4REQ, &DMA5REQ, &DMA6REQ, &DMA7REQ};
volatile unsigned int * DMAiSTA[] = {&DMA0STAL, &DMA1STAL, &DMA2STAL, &DMA3STAL, &DMA4STAL, &DMA5STAL, &DMA6STAL, &DMA7STAL};*/

volatile unsigned int * C1RXFiSID[] = {&C1RXF0SID, &C1RXF1SID, &C1RXF2SID, &C1RXF3SID, &C1RXF4SID, &C1RXF5SID, &C1RXF6SID, &C1RXF7SID, &C1RXF8SID, &C1RXF9SID, &C1RXF10SID, &C1RXF11SID, &C1RXF12SID, &C1RXF13SID, &C1RXF14SID, &C1RXF15SID};
volatile unsigned int * C1RXFiEID[] = {&C1RXF0EID, &C1RXF1EID, &C1RXF2EID, &C1RXF3EID, &C1RXF4EID, &C1RXF5EID, &C1RXF6EID, &C1RXF7EID, &C1RXF8EID, &C1RXF9EID, &C1RXF10EID, &C1RXF11EID, &C1RXF12EID, &C1RXF13EID, &C1RXF14EID, &C1RXF15EID};

volatile unsigned int * C1RXMiSID[] = {&C1RXM0SID, &C1RXM1SID, &C1RXM2SID};
volatile unsigned int * C1RXMiEID[] = {&C1RXM0EID, &C1RXM1EID, &C1RXM2EID};

volatile unsigned int * C1TRiCON[] = {&C1TR01CON, &C1TR01CON, &C1TR23CON, &C1TR23CON, &C1TR45CON, &C1TR45CON, &C1TR67CON, &C1TR67CON};

unsigned int TXENi[] = {0x0080, 0x8000, 0x0080, 0x8000, 0x0080, 0x8000, 0x0080, 0x8000};
unsigned int TXREQi[] = {0x0008, 0x0800, 0x0008, 0x0800, 0x0008, 0x0800, 0x0008, 0x0800};
unsigned int TXiPRI[] = {0, 8, 0, 8, 0, 8, 0, 8};

unsigned int FiMSK[] = {0, 2, 4,6, 8, 10, 12, 14};

unsigned char buttons[2];

volatile int counter_total;
volatile int counter_rx;
volatile int counter_tx;

/******************************* INIT_CAN *******************************
* Description : Fonction qui initialise le module CAN. Tous les aspect
* 	temporelle du module (frï¿½quence, synchronisation) sont ajustï¿½ aux 
*	valeurs par dï¿½faut de la librairie.
*
* 	Exemple d'utilisation standard:
*	init_CAN(CAN_NORMAL, 4, 0, 1, 5);  	
*
* Input :	
*	T_CAN_MODE	mode		Mode d'utilisation du module. Valeurs 
*							possible: CAN_NORMAL, CAN_LISTEN_ONLY, CAN_LISTEN_ALL, 
*							CAN_LISTEN_ALL, CAN_LOOPBACK, CAN_DISABLE.
*
*	unsigned int nbr_buf_Tx	Nombre de buffer devant ï¿½tre configurï¿½	en Tx. 
*							Valeurs possibles: 0 ï¿½ 8.
*
*	unsigned int DMA_Rx		Cannal DMA ï¿½ utiliser pour la rï¿½ception	de 
*							messages. Valeurs possibles: 0 ï¿½ 7.
*
*	unsigned int DMA_Tx		Cannal DMA ï¿½ utiliser pour la transmission de
*							messages. Valeurs possibles: 0 ï¿½ 7.
*
*	unsigned int int_level	Niveau de prioritï¿½ d'interruption du CAN. 
*							Valeurs possibles: 0 ï¿½ 7. 
*							Il est recommandï¿½ de mettre un niveau 
*							de prioritï¿½ ï¿½levï¿½.
*
* Output :	AUCUN
*
************************************************************************/
void init_CAN(T_CAN_MODE mode, unsigned int nbr_buf_Tx, unsigned int DMA_Rx, unsigned int DMA_Tx, unsigned int int_level)
{
	unsigned int ii;
	int old_ipl;

	/* Bloque les interrupt le temps de changer le dï¿½placement. */
	SET_AND_SAVE_CPU_IPL(old_ipl, 7);
	
	/* Sï¿½lectionne la bonne fenï¿½tre de registre. */
	C1CTRL1bits.WIN = 0;
	
	/* Annule tout les messages Tx en attente. */
	C1CTRL1bits.ABAT = 1;

	/* Met le module en mode configuration. */ 
	/* Attend que le changement soit effectifs. */
	C1CTRL1bits.REQOP = CAN_CONFIGURATION;
	while(C1CTRL1bits.OPMODE != CAN_CONFIGURATION);
	
	/* Configuration des aspect temporels. */
	C1CTRL1bits.CANCKS = 0;
	C1CFG1 = CAN_PRESCALER | (CAN_SJW << 6);
	C1CFG2 = CAN_PROPAGATION | (CAN_PHASE_1 << 3) | (CAN_SAMPLE << 6) | (CAN_PHASE_2 << 8) | 0x0080;
	
	/* Configure l'utilisation de 32 buffers dans la DMA. */
	C1FCTRLbits.DMABS = 0b110;
	
	/* Les buffers qui ne sont pas utilisï¿½ en Tx sont utilisï¿½ en FIFO. */
	C1FCTRLbits.FSA = nbr_buf_Tx & 0x001F;
	
	/* Efface les flag des buffers et des overflow */
	C1RXFUL1 = C1RXFUL2 = C1RXOVF1 = C1RXOVF2 = 0x0000;
	
	/* Par dï¿½faut, met tout les buffers en Rx. */
	C1TR01CON = C1TR23CON = C1TR45CON = C1TR67CON = 0x0000;
	
	/* Dï¿½sactive tout les filtres. */
	C1FEN1 = 0x0000;
	
	/* Tout les filtres pointent vers le FIFO. */
	C1CTRL1bits.WIN = 1;
	C1BUFPNT1 = C1BUFPNT2 = C1BUFPNT3 = C1BUFPNT4 = 0xFFFF;
	C1CTRL1bits.WIN = 0;
	
	/* Ensuite, configure le nombre de buffer voulus en Tx. */
	for(ii = 0; ii < nbr_buf_Tx; ii++)
	{
		*C1TRiCON[ii] |= TXENi[ii];	
	}

	config_CAN.nbr_buf_Tx = nbr_buf_Tx;
	
	/* Tout les buffer sont assumï¿½ comme libre. */
	for(ii = 0; ii <= 8; ii++)
	{
		config_CAN.etat_Buf[ii] = FALSE;
		config_CAN.Buf_Tx_assign[ii] = NULL;
	}
	
	/* Configuration des canaux DMA. */
	init_DMA_channel(DMA_Tx, 1, 0x0046, 0x0442,__builtin_dmaoffset(CAN_msg_Buf));
	init_DMA_channel(DMA_Rx, 0, 0x0022, 0x0440,__builtin_dmaoffset(CAN_msg_Buf));
	
	/* Configuration de l'interuption. */	
	/* Commence par faire un clear des interuption. */
	IFS2bits.C1IF = 0;
	C1INTFbits.TBIF = 0;
	C1INTFbits.RBIF = 0;
        C1INTFbits.ERRIF = 0;
        C1INTFbits.FIFOIF = 0;
        C1INTFbits.RBOVIF = 0;
        C1INTFbits.IVRIF = 0;
	
	/* Configure le niveau de priotitï¿½. */
	IPC8bits.C1IP = int_level & 0x0007;
	
	/* Active les interruption en transmission et en interruption. */
	IEC2bits.C1IE=1;
	C1INTEbits.RBIE=1;
	C1INTEbits.TBIE=1;
        C1INTEbits.ERRIE=1;
        C1INTEbits.FIFOIE=1;
        C1INTEbits.IVRIE=1;
        C1INTEbits.RBOVIE=1;
	
	/* Met le module dans le mode demandï¿½. */ 
	/* Attend que le changement soit effectifs. */
	C1CTRL1bits.REQOP = mode;
	while(C1CTRL1bits.OPMODE != mode);

	/* Dï¿½bloque les interupt. */
	RESTORE_CPU_IPL(old_ipl);
	
	return;
}

/*************************** CONFIG_CAN_TX_MSG ***************************
* Description : Fonction qui configure structure pour l'envoit d'un message
*	CAN.
*
* Input :
*	T_CAN_Tx_MSG * CAN_msg	Pointeur vers la structure CAN ï¿½ configurer.
*
*	unsigned long ID		ID du message.
*
*	T_TYPE_ID 	type_ID		Type d'ID utilisï¿½e.
*							Valeur possible: STANDARD_ID ou	EXTENDED_ID.
*
*	char 		priority	Prioritï¿½ assignï¿½ au message. Cette prioritï¿½e
*							dï¿½termine quel message est envoyï¿½ en premier.
*							Valeurs possibles: 0 ï¿½ 3. 3 ï¿½tant le plus 
*							prioritaire.
*
* Output :
* 	T_CAN_Tx_MSG *			Pointeur vers la structure crï¿½er.
*
************************************************************************/
void config_CAN_Tx_msg(T_CAN_Tx_MSG * CAN_msg, unsigned long ID, T_TYPE_ID type_ID, unsigned int priority)
{
	CAN_msg->ID = ID;
	CAN_msg->type_ID = type_ID;
	CAN_msg->priority = priority & 0x0003;
	CAN_msg->Tx_Buf = -1;
	
	return;
}

/***************************** SEND_CAN_MSG ****************************
* Description : Fonction permettant l'envoie d'un message CAN. Si un
*	buffer de Tx est disponible, le message y est transfï¿½rer et la fonction 
*	retourne TRUE. Dans le cas contraire, la fonction retourne FALSE.
*
* Input :
*	T_CAN_Tx_MSG * CAN_tx_msg	Pointeur vers la structure du message.
*
*	const void * 	data		Pointeur vers les donnï¿½es ï¿½ envoyer.
*
*	char 			nbr_data	Nombre d'octets ï¿½ envoyer. Maximum: 8.
*
* Output :
* 	int						Retourne TRUE si le message a pu ï¿½tre mis dans
*							dans un buffer de Tx. Sinon, retourne FALSE.
*
************************************************************************/
int send_CAN_msg(T_CAN_Tx_MSG * CAN_tx_msg, const void * data_scr, char nbr_data)
{
	int  Buf_sel=-1;
	unsigned int word0, word1, word2;
	char * ptr_dest, * ptr_src;
	unsigned int jj;
	int old_ipl;

	/* Bloque les interrupt le temps de changer le dï¿½placement. */
	SET_AND_SAVE_CPU_IPL(old_ipl, 7);
	
	/* Sï¿½lectionne la bonne fenï¿½tre de registre. */
	C1CTRL1bits.WIN = 0;
	
	/* Dï¿½termine si un buffer de Tx est disponible. */
	for(Buf_sel = config_CAN.nbr_buf_Tx - 1; Buf_sel >= 0; Buf_sel--)
	{
		if(config_CAN.etat_Buf[Buf_sel] == FALSE)
		{
			config_CAN.etat_Buf[Buf_sel] = TRUE;
			break;	
		}		
	}	

	/* Si aucun buffer de Tx est disponible, impossible d'envoyer le message. */
	if(Buf_sel == -1)
	{	
		/* Dï¿½bloque les interupt. */
		RESTORE_CPU_IPL(old_ipl);	
		return FALSE;
	}
	
	/* Enregistre le buffer choisi. */
	CAN_tx_msg->Tx_Buf = Buf_sel;
	config_CAN.Buf_Tx_assign[Buf_sel] = CAN_tx_msg;
		
	/* Configure l'ID selon le type*/
	if(CAN_tx_msg->type_ID == STANDARD_ID)
	{
		/* Configure pour un ID standard. */
		word0 = (unsigned int) ((CAN_tx_msg->ID & 0x000007FF) << 2);
		word1 = 0x0000;
		word2 = 0x0000;	
	}	
	else
	{
		/* Configure pour un extended ID. */		
		word0 = (unsigned int) (((CAN_tx_msg->ID & 0x000007FF) << 2) | 0x0001);
		word1 = (unsigned int) ((CAN_tx_msg->ID & 0x1FFE0000) >> 17);
		word2 = (unsigned int) ((CAN_tx_msg->ID & 0x0001F800) >> 1);	
	}
	
	/* Inscrit le nombre de donnï¿½es contenues dans le message. */
	word2 |= (unsigned int) nbr_data & 0x000F;
	
	/* Transfers des donnï¿½es vers le buffer sï¿½lectionnï¿½. */
	CAN_msg_Buf[Buf_sel][0] = word0;
	CAN_msg_Buf[Buf_sel][1] = word1;
	CAN_msg_Buf[Buf_sel][2] = word2;

	ptr_dest = (char *) &CAN_msg_Buf[Buf_sel][3];
	ptr_src = (char *) data_scr;
	
	for(jj = 0; jj < nbr_data; jj++)
	{
		ptr_dest[jj] = ptr_src[jj];
	}

	/* Assigne la prioritï¿½ et active le flag pour transmettre le message. */
	*(C1TRiCON[Buf_sel]) &= ~(0x0003 << TXiPRI[Buf_sel]);
	*(C1TRiCON[Buf_sel]) |= CAN_tx_msg->priority << TXiPRI[Buf_sel];
	*(C1TRiCON[Buf_sel]) |= TXREQi[Buf_sel];
	
	/* Dï¿½bloque les interupt. */
	RESTORE_CPU_IPL(old_ipl);
	return TRUE;		
}

/**************************** IS_CAN_MSG_SEND ***************************
* Description : Fonction qui permet de savoir si un message ï¿½ ï¿½tï¿½ envoyï¿½.
*
* Input :
*	T_CAN_Tx_MSG * CAN_tx_msg	Pointeur vers la structure du message.		
*
* Output :
* 	int						Retourne TRUE si le message a ï¿½tï¿½ transmis.
*							Sinon, retourne FALSE.
*
************************************************************************/
int is_CAN_msg_send(T_CAN_Tx_MSG * CAN_tx_msg)
{
	if(CAN_tx_msg->Tx_Buf == -1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	
}

/************************** CONFIG_CAN_FILTER ***************************
* Description : Fonction qui permet de configurer un des filtres d'un 
*	module CAN.
*
* Input :
*	T_CAN_MODULE CAN_module	Module CAN que l'on veut configurer.
*							Valeurs possible: CAN_1 ou CAN_2.
*
*	char 	filter_number	Numï¿½ro du filtre que l'on veut configurer.
*							Valeurs possible: 0 ï¿½ 15.
*
*	unsigned long ID		ID devant ï¿½tre configurï¿½ dans le filtre.
*
*	T_TYPE_ID type_ID		Type d'ID utilisï¿½e.
*							Valeur possible: STANDARD_ID ou	EXTENDED_ID.
*
* Output :	AUCUN
*
************************************************************************/
void config_CAN_filter(unsigned int filter_number, unsigned long ID, T_TYPE_ID type_ID)
{
	int old_ipl;

	/* Bloque les interrupt le temps de changer le dï¿½placement. */
	SET_AND_SAVE_CPU_IPL(old_ipl, 7);
	
	/* Active la fenï¿½tre pour accï¿½der aux bon registres. */
	C1CTRL1bits.WIN = 1;
	
	*(C1RXFiSID[filter_number]) = set_SID_reg(ID, type_ID, 0);	
	*(C1RXFiEID[filter_number]) = set_EID_reg(ID, type_ID);

	/* Dï¿½sactive la fenï¿½tre pour accï¿½der aux bon registres. */
	C1CTRL1bits.WIN = 0;		

	/* Dï¿½bloque les interupt. */
	RESTORE_CPU_IPL(old_ipl);
	
	return;	
}	

/************************** CONFIG_CAN_MASK ***************************
* Description : Fonction qui permet de configurer un des masques d'un 
*	module CAN.
*
* Input :
*	char 	mask_number		Numï¿½ro du filtre que l'on veut configurer.
*							Valeurs possible: 0 ï¿½ 2.
*
*	unsigned long mask		Valeur devant ï¿½tre configurï¿½ dans le masque.
*
*	T_TYPE_ID type_ID		Type d'ID utilisï¿½e.
*							Valeur possible: STANDARD_ID ou	EXTENDED_ID.
*
* Output :	AUCUN
*
************************************************************************/
void config_CAN_mask(unsigned int mask_number, unsigned long mask, T_TYPE_ID type_ID)
{
	int old_ipl;

	/* Bloque les interrupt le temps de changer le dï¿½placement. */
	SET_AND_SAVE_CPU_IPL(old_ipl, 7);
	
	/* Active la fenï¿½tre pour accï¿½der aux bon registres. */
	C1CTRL1bits.WIN = 1;
	
	*(C1RXMiSID[mask_number]) = set_SID_reg(mask, type_ID, 1);	
	*(C1RXMiEID[mask_number]) = set_EID_reg(mask, type_ID);
	
	/* Dï¿½sactive la fenï¿½tre pour accï¿½der aux bon registres. */
	C1CTRL1bits.WIN = 0;
	
	/* Dï¿½bloque les interupt. */
	RESTORE_CPU_IPL(old_ipl);
	
	return;	
}

/************************** RECEIVE_CAN_MSG ***************************
* Description : Fonction qui configure la rï¿½ception d'un message.
*
* Input :
*	char 	filter_number	Numï¿½ro du filtre utilisï¿½.
*							Valeurs possible: 0 ï¿½ 2.
*
*	char 	filter_number	Numï¿½ro du masque utilisï¿½.
*							Valeurs possible: 0 ï¿½ 3. (3 = aucun masque)
*
*	T_TYPE_BUF type_buf		Type de buffer dans lequel le message doit
*							ï¿½tre placï¿½.
*							Valeur possible: BUF_IND ou	BUF_FIFO.
*
*	void (* ptr_fct_receive) (unsigned long ID, T_TYPE_ID type_ID, const void * data_rx, char nbr_data)
*							Pointeur vers la fonction ï¿½ utiliser lors de
*							la rï¿½ception d'un message.
*
* Output :
*	int				TRUE si la rï¿½ception ï¿½ pu ï¿½tre configurï¿½ sinon, FALSE.
*
************************************************************************/
void receive_CAN_msg(unsigned int filter_number, unsigned int mask_number, void (* ptr_fct_receive) (unsigned long ID, T_TYPE_ID type_ID, T_CAN_DATA* data_rx, char nbr_data))
{
	int old_ipl;

	/* Bloque les interrupt le temps de changer le dï¿½placement. */
	SET_AND_SAVE_CPU_IPL(old_ipl, 7);
	
	/* Active la fenï¿½tre pour accï¿½der aux bon registres. */
	C1CTRL1bits.WIN = 1;
	
	/* Assigne le masque au filtre. */
	if(filter_number <= 7)
	{
		C1FMSKSEL1 &= ~(0x0003 << FiMSK[filter_number]);
		C1FMSKSEL1 |= (mask_number << FiMSK[filter_number]);	
	}
	else
	{
		C1FMSKSEL2 &= ~(0x0003 << FiMSK[filter_number - 8]);
		C1FMSKSEL2 |= (mask_number << FiMSK[filter_number - 8]);
	}
				
	/* Active le filtre. */
	C1FEN1 |= 0x0001 << filter_number;
	
	/* Enregistre le pointeur de fonction. */
	config_CAN.ptr_fct_receive[filter_number] = ptr_fct_receive;
	
	/* Dï¿½sactive la fenï¿½tre pour accï¿½der aux bon registres. */
	C1CTRL1bits.WIN = 0;		
	
	/* Dï¿½bloque les interupt. */
	RESTORE_CPU_IPL(old_ipl);
	
	return;
}	

/************************ STOP_RECEIVE_CAN_MSG *************************
* Description : Fonction qui arrï¿½te la rï¿½ception d'un message. 
*
* Input :
*	char 	filter_number	Numï¿½ro du filtre dont on veut arrï¿½ter la 
*							rï¿½ception. Valeurs possible: 0 ï¿½ 15.
*
*	Output :	AUCUN
*
************************************************************************/
void stop_receive_CAN_msg(unsigned int filter_number)
{
	/* Dï¿½sactive le filtre. */
	C1FEN1 &= ~(0x0001 << filter_number);
	
	return;
} 

/*************************** INIT_DMA_CHANNEL **************************
* Description : Fonction qui initialise un cannal DMA.
*
* Input :
*	char 	DMA_channel		Cannal DMA ï¿½ initialiser.
*							Valeurs possibles: 0 ï¿½ 7. 
*
*	char 	DMA_dir			Direction du canal. 1 = Read from SRAM
*							0 = Write to SRAM
*
*	unsigned int IRQ		IRQ du pï¿½riphï¿½rique
*
*	unsigned int address	Adresse du pï¿½riphï¿½rique
*
*	void * 	Buf_ptr			Adresse du buffer dans la RAM.
*
* Output :	AUCUN
*
************************************************************************/
/*static void init_DMA_channel(unsigned int DMA_channel, char DMA_dir, unsigned int IRQ, unsigned int address, unsigned int offset)
{	
	*(DMAiCON[DMA_channel]) = 0x0020 | ((unsigned int)DMA_dir << 13);
	*(DMAiPAD[DMA_channel]) = address;
	*(DMAiCNT[DMA_channel]) = 7;
 	*(DMAiREQ[DMA_channel]) = IRQ;	
	*(DMAiSTA[DMA_channel]) = offset;
	*(DMAiCON[DMA_channel]) |= 0x8000;
	
	return;
}*/

/**************************** SET_SID_REG ******************************
* Description : Fonction qui dï¿½termine la valeur ï¿½ mettre dans un 
* registre SID pour configurer un filtre ou un mask.
*
* Input :
*	unsigned long ID		ID devant ï¿½tre configurï¿½ dans le filtre.
*
*	T_TYPE_ID type_ID		Type d'ID utilisï¿½e.
*							Valeur possible: STANDARD_ID ou	EXTENDED_ID.
*
*	char mask_or_filter		Flag qui indique si il s'agit d'un filtre ou 
*							d'un mask. 1 = mask, 0 = filter.
*
* Output :	
*	unsigned int			Valeur ï¿½ mettre dans le registre.
*
************************************************************************/
static unsigned int set_SID_reg(unsigned long ID, T_TYPE_ID type_ID, char mask_or_filter)
{
	if(type_ID == STANDARD_ID)
	{
		return (unsigned int) ((ID & 0x000007FF) << 5) | (((unsigned int) mask_or_filter) << 3); 		
	}
	else if(type_ID == EXTENDED_ID)
	{
		return (unsigned int) (((ID & 0x000007FF) << 5) | 0x0008 | ((ID & 0x18000000) >> 27));
	}		

	return 0;
}

/**************************** SET_EID_REG ******************************
* Description : Fonction qui dï¿½termine la valeur ï¿½ mettre dans un 
* registre EID pour configurer un filtre ou un mask.
*
* Input :
*	unsigned long ID		ID devant ï¿½tre configurï¿½ dans le filtre.
*
*	T_TYPE_ID type_ID		Type d'ID utilisï¿½e.
*							Valeur possible: STANDARD_ID ou	EXTENDED_ID.
*
* Output :	
*	unsigned int			Valeur ï¿½ mettre dans le registre.
*
************************************************************************/
static unsigned int set_EID_reg(unsigned long ID, T_TYPE_ID type_ID)
{
	if(type_ID == STANDARD_ID)
	{
		return 0; 		
	}
	else if(type_ID == EXTENDED_ID)
	{
		return (unsigned int) ((ID & 0x07FFF800) >> 11);
	}		
	
	return 0;	
}	

/***************************** INTERRUPT CAN1 **************************
* Description : Routine d'interruption du module CAN1.
*
* Input :	AUCUN
*
* Output :	AUCUN
*
************************************************************************/
void __attribute__((interrupt,no_auto_psv))_C1Interrupt(void)
{
	char temp_win;
	unsigned int ii=0, Buf_read_ptr=0, filter_hit=0;
	unsigned long ID_rx=0;
	T_TYPE_ID type_ID_rx;
        unsigned int test[8]={0};
        T_CAN_DATA recopie;
	
	temp_win = C1CTRL1bits.WIN;
	C1CTRL1bits.WIN = 0;

	//counter_total++;
        /*Rx Buffer interrupt flag*/
	if(C1INTFbits.RBIF == 1)
	{
		/* C'est un buffer configurï¿½ en Rx qui a gï¿½nï¿½rï¿½ l'interrupt. */
		while( (C1RXFUL1 != 0) || (C1RXFUL2 != 0) )
		{
			Buf_read_ptr = C1FIFO & 0x003F;
                        test[0] = *(CAN_msg_Buf[Buf_read_ptr]);// & 0x0001;
                        recopie.data0=*(CAN_msg_Buf[Buf_read_ptr]);
                        recopie.data1=*(CAN_msg_Buf[Buf_read_ptr]+1*sizeof(char));
                        recopie.data2=*(CAN_msg_Buf[Buf_read_ptr]+2*sizeof(char));
                        recopie.data3=*(CAN_msg_Buf[Buf_read_ptr]+3*sizeof(char));
                        recopie.data4=*(CAN_msg_Buf[Buf_read_ptr]+4*sizeof(char));
                        recopie.data5=*(CAN_msg_Buf[Buf_read_ptr]+5*sizeof(char));
                        recopie.data6=*(CAN_msg_Buf[Buf_read_ptr]+6*sizeof(char));
                        recopie.data7=*(CAN_msg_Buf[Buf_read_ptr]+7*sizeof(char));

			/* Dï¿½termine l'ID du message reï¿½u selon le type. */
			if((recopie.data0 & 0x0001) == 1)
			{
				/* Extended ID */
				ID_rx = ((unsigned long)(*(CAN_msg_Buf[Buf_read_ptr]+1) & 0x0FFF) << 17) | ((unsigned long)(CAN_msg_Buf[Buf_read_ptr][2] & 0xFC00) << 1) | ((unsigned long)(CAN_msg_Buf[Buf_read_ptr][0] & 0x1FFC) >> 2);
				type_ID_rx = EXTENDED_ID;
			}
			else
			{
				/* Standard ID */
				ID_rx = (unsigned long)((recopie.data0 & 0x1FFC) >> 2);
				type_ID_rx = STANDARD_ID;						
			}		 
			
			/* Clear le flag de ce buffer. */
			if(Buf_read_ptr > 0x000F)
			{
				C1RXFUL2 &= ~(0x0001 << (Buf_read_ptr - 0x0010));
			}
			else	
			{
				C1RXFUL1 &= ~(0x0001 << Buf_read_ptr);
			}

			/* Exï¿½cute la fonction associï¿½ ï¿½ ce message. */
			filter_hit = (recopie.data7 >> 8) & 0x001F;
			if(config_CAN.ptr_fct_receive[filter_hit] != NULL)
			{
				config_CAN.ptr_fct_receive[filter_hit](ID_rx, type_ID_rx,&recopie, (int)(recopie.data2 & 0x000F));//(CAN_msg_Buf[Buf_read_ptr]+3), (int)(recopie.data2 & 0x000F));
				
			}

		}		
		
		C1INTFbits.RBIF = 0;
	}	
	/*Tx Buffer interrupt flag*/
	if(C1INTFbits.TBIF == 1)
	{
		counter_tx++;

		C1INTFbits.TBIF = 0;
		/* C'est un buffer configurï¿½ en Tx qui a gï¿½nï¿½rï¿½ l'interrupt. */
		/* Vï¿½rifie quel buffer en Tx s'est libï¿½rï¿½. */
		for(ii = 0; ii < config_CAN.nbr_buf_Tx; ii++)
		{
			if( (config_CAN.etat_Buf[ii] == TRUE) && ((*C1TRiCON[ii] & TXREQi[ii]) == 0) )
			{ 
				config_CAN.etat_Buf[ii] = FALSE;
				config_CAN.Buf_Tx_assign[ii]->Tx_Buf = -1;
				config_CAN.Buf_Tx_assign[ii] = NULL;
			}
		}	
	}
	
	C1CTRL1bits.WIN = temp_win;

        /*Error interrupt flag*/
        if(C1INTFbits.ERRIF == 1)
        {
            C1INTFbits.ERRIF = 0;
        }

	/*Invalid message interrupt flag*/
        if(C1INTFbits.IVRIF == 1)
        {
            C1INTFbits.IVRIF = 0;
        }

  	/*Rx buffer overflow interrupt flag*/
        if(C1INTFbits.RBOVIF == 1)
        {
            C1INTFbits.RBOVIF = 0;
        }

        /*Fifo almost full interrupt flag*/
        if(C1INTFbits.FIFOIF == 1)
        {
            C1INTFbits.FIFOIF = 0;
        }

        /*Transmit in bus passive flag*/
        if(C1INTFbits.TXBP == 1)
        {
            C1INTFbits.TXBP = 0;
        }

	C1CTRL1bits.WIN = temp_win;	

	IFS2bits.C1IF = 0;

	return;	
}	
#endif // DSPIC33F
