/***********************************************************************
* lib_CAN.c
* Cette librairie g�re le bus CAN. Il permet la configuration initial du
* bus, l'envoi et la r�ception de messages. 
* 
* AUTEUR: 	Etienne Collard-Fr�chette (WalkingMachine)
* DATE:		1 mars 2011
* MODIFI�:	BAJA ETS (11 oct. 2011)
************************************************************************
****************************** INCLUDE *********************************/
#define DSPIC33F
#ifdef DSPIC33F
#include "can.h"
#include "can_chinook3.h"
#include "p33EP512MC806.h"
#include <stdlib.h>
#include "globaldef.h"
#include "dma.h"


/* Fonction priv�e. */
//static void init_DMA_channel(unsigned int DMA_channel, char direction, unsigned int IRQ, unsigned int PAD, unsigned int offset);

static unsigned int set_SID_reg(unsigned long ID, T_TYPE_ID type_ID, char mask_or_filter);

static unsigned int set_EID_reg(unsigned long ID, T_TYPE_ID type_ID);

/***************** D�CLARATION DES VARIABLES GLOBALES ******************/
/* Buffer utilis� dans la DMA. */

unsigned int CAN_msg_Buf[32][8] __attribute__((aligned(32 * 16)));

T_CAN_CONFIG config_CAN;

/* Les tableaux suivant contiennent les adresses des diff�rent registres. */
/* Utilis� pour les acc�der plus facilement. */
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
* 	temporelle du module (fr�quence, synchronisation) sont ajust� aux 
*	valeurs par d�faut de la librairie.
*
* 	Exemple d'utilisation standard:
*	init_CAN(CAN_NORMAL, 4, 0, 1, 5);  	
*
* Input :	
*	T_CAN_MODE	mode		Mode d'utilisation du module. Valeurs 
*							possible: CAN_NORMAL, CAN_LISTEN_ONLY, CAN_LISTEN_ALL, 
*							CAN_LISTEN_ALL, CAN_LOOPBACK, CAN_DISABLE.
*
*	unsigned int nbr_buf_Tx	Nombre de buffer devant �tre configur�	en Tx. 
*							Valeurs possibles: 0 � 8.
*
*	unsigned int DMA_Rx		Cannal DMA � utiliser pour la r�ception	de 
*							messages. Valeurs possibles: 0 � 7.
*
*	unsigned int DMA_Tx		Cannal DMA � utiliser pour la transmission de
*							messages. Valeurs possibles: 0 � 7.
*
*	unsigned int int_level	Niveau de priorit� d'interruption du CAN. 
*							Valeurs possibles: 0 � 7. 
*							Il est recommand� de mettre un niveau 
*							de priorit� �lev�.
*
* Output :	AUCUN
*
************************************************************************/
void init_CAN(T_CAN_MODE mode, unsigned int nbr_buf_Tx, unsigned int DMA_Rx, unsigned int DMA_Tx, unsigned int int_level)
{
	unsigned int ii;
	int old_ipl;

	/* Bloque les interrupt le temps de changer le d�placement. */
	SET_AND_SAVE_CPU_IPL(old_ipl, 7);
	
	/* S�lectionne la bonne fen�tre de registre. */
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
	
	/* Les buffers qui ne sont pas utilis� en Tx sont utilis� en FIFO. */
	C1FCTRLbits.FSA = nbr_buf_Tx & 0x001F;
	
	/* Efface les flag des buffers et des overflow */
	C1RXFUL1 = C1RXFUL2 = C1RXOVF1 = C1RXOVF2 = 0x0000;
	
	/* Par d�faut, met tout les buffers en Rx. */
	C1TR01CON = C1TR23CON = C1TR45CON = C1TR67CON = 0x0000;
	
	/* D�sactive tout les filtres. */
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
	
	/* Tout les buffer sont assum� comme libre. */
	for(ii = 0; ii <= 8; ii++)
	{
		config_CAN.etat_Buf[ii] = FALSE;
		config_CAN.Buf_Tx_assign[ii] = NULL;
	}
	
	/* Configuration des canaux DMA. */
	init_DMA_channel(DMA_Tx, 1, 0x0046, 0x0442, __builtin_dmaoffset(CAN_msg_Buf));
	init_DMA_channel(DMA_Rx, 0, 0x0022, 0x0440, __builtin_dmaoffset(CAN_msg_Buf));
	
	/* Configuration de l'interuption. */	
	/* Commence par faire un clear des interuption. */
	IFS2bits.C1IF = 0;
	C1INTFbits.TBIF = 0;
	C1INTFbits.RBIF = 0;
        C1INTFbits.ERRIF = 0;
        C1INTFbits.FIFOIF = 0;
        C1INTFbits.RBOVIF = 0;
        C1INTFbits.IVRIF = 0;
	
	/* Configure le niveau de priotit�. */
	IPC8bits.C1IP = int_level & 0x0007;
	
	/* Active les interruption en transmission et en interruption. */
	IEC2bits.C1IE=1;
	C1INTEbits.RBIE=1;
	C1INTEbits.TBIE=1;
        C1INTEbits.ERRIE=1;
        C1INTEbits.FIFOIE=1;
        C1INTEbits.IVRIE=1;
        C1INTEbits.RBOVIE=1;
	
	/* Met le module dans le mode demand�. */ 
	/* Attend que le changement soit effectifs. */
	C1CTRL1bits.REQOP = mode;
	while(C1CTRL1bits.OPMODE != mode);

	/* D�bloque les interupt. */
	RESTORE_CPU_IPL(old_ipl);
	
	return;
}

/*************************** CONFIG_CAN_TX_MSG ***************************
* Description : Fonction qui configure structure pour l'envoit d'un message
*	CAN.
*
* Input :
*	T_CAN_Tx_MSG * CAN_msg	Pointeur vers la structure CAN � configurer.
*
*	unsigned long ID		ID du message.
*
*	T_TYPE_ID 	type_ID		Type d'ID utilis�e.
*							Valeur possible: STANDARD_ID ou	EXTENDED_ID.
*
*	char 		priority	Priorit� assign� au message. Cette priorit�e
*							d�termine quel message est envoy� en premier.
*							Valeurs possibles: 0 � 3. 3 �tant le plus 
*							prioritaire.
*
* Output :
* 	T_CAN_Tx_MSG *			Pointeur vers la structure cr�er.
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
*	buffer de Tx est disponible, le message y est transf�rer et la fonction 
*	retourne TRUE. Dans le cas contraire, la fonction retourne FALSE.
*
* Input :
*	T_CAN_Tx_MSG * CAN_tx_msg	Pointeur vers la structure du message.
*
*	const void * 	data		Pointeur vers les donn�es � envoyer.
*
*	char 			nbr_data	Nombre d'octets � envoyer. Maximum: 8.
*
* Output :
* 	int						Retourne TRUE si le message a pu �tre mis dans
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

	/* Bloque les interrupt le temps de changer le d�placement. */
	SET_AND_SAVE_CPU_IPL(old_ipl, 7);
	
	/* S�lectionne la bonne fen�tre de registre. */
	C1CTRL1bits.WIN = 0;
	
	/* D�termine si un buffer de Tx est disponible. */
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
		/* D�bloque les interupt. */
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
	
	/* Inscrit le nombre de donn�es contenues dans le message. */
	word2 |= (unsigned int) nbr_data & 0x000F;
	
	/* Transfers des donn�es vers le buffer s�lectionn�. */
	CAN_msg_Buf[Buf_sel][0] = word0;
	CAN_msg_Buf[Buf_sel][1] = word1;
	CAN_msg_Buf[Buf_sel][2] = word2;

	ptr_dest = (char *) &CAN_msg_Buf[Buf_sel][3];
	ptr_src = (char *) data_scr;
	
	for(jj = 0; jj < nbr_data; jj++)
	{
		ptr_dest[jj] = ptr_src[jj];
	}

	/* Assigne la priorit� et active le flag pour transmettre le message. */
	*(C1TRiCON[Buf_sel]) &= ~(0x0003 << TXiPRI[Buf_sel]);
	*(C1TRiCON[Buf_sel]) |= CAN_tx_msg->priority << TXiPRI[Buf_sel];
	*(C1TRiCON[Buf_sel]) |= TXREQi[Buf_sel];
	
	/* D�bloque les interupt. */
	RESTORE_CPU_IPL(old_ipl);

	return TRUE;		
}

/**************************** IS_CAN_MSG_SEND ***************************
* Description : Fonction qui permet de savoir si un message � �t� envoy�.
*
* Input :
*	T_CAN_Tx_MSG * CAN_tx_msg	Pointeur vers la structure du message.		
*
* Output :
* 	int						Retourne TRUE si le message a �t� transmis.
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
*	char 	filter_number	Num�ro du filtre que l'on veut configurer.
*							Valeurs possible: 0 � 15.
*
*	unsigned long ID		ID devant �tre configur� dans le filtre.
*
*	T_TYPE_ID type_ID		Type d'ID utilis�e.
*							Valeur possible: STANDARD_ID ou	EXTENDED_ID.
*
* Output :	AUCUN
*
************************************************************************/
void config_CAN_filter(unsigned int filter_number, unsigned long ID, T_TYPE_ID type_ID)
{
	int old_ipl;

	/* Bloque les interrupt le temps de changer le d�placement. */
	SET_AND_SAVE_CPU_IPL(old_ipl, 7);
	
	/* Active la fen�tre pour acc�der aux bon registres. */
	C1CTRL1bits.WIN = 1;
	
	*(C1RXFiSID[filter_number]) = set_SID_reg(ID, type_ID, 0);	
	*(C1RXFiEID[filter_number]) = set_EID_reg(ID, type_ID);

	/* D�sactive la fen�tre pour acc�der aux bon registres. */
	C1CTRL1bits.WIN = 0;		

	/* D�bloque les interupt. */
	RESTORE_CPU_IPL(old_ipl);
	
	return;	
}	

/************************** CONFIG_CAN_MASK ***************************
* Description : Fonction qui permet de configurer un des masques d'un 
*	module CAN.
*
* Input :
*	char 	mask_number		Num�ro du filtre que l'on veut configurer.
*							Valeurs possible: 0 � 2.
*
*	unsigned long mask		Valeur devant �tre configur� dans le masque.
*
*	T_TYPE_ID type_ID		Type d'ID utilis�e.
*							Valeur possible: STANDARD_ID ou	EXTENDED_ID.
*
* Output :	AUCUN
*
************************************************************************/
void config_CAN_mask(unsigned int mask_number, unsigned long mask, T_TYPE_ID type_ID)
{
	int old_ipl;

	/* Bloque les interrupt le temps de changer le d�placement. */
	SET_AND_SAVE_CPU_IPL(old_ipl, 7);
	
	/* Active la fen�tre pour acc�der aux bon registres. */
	C1CTRL1bits.WIN = 1;
	
	*(C1RXMiSID[mask_number]) = set_SID_reg(mask, type_ID, 1);	
	*(C1RXMiEID[mask_number]) = set_EID_reg(mask, type_ID);
	
	/* D�sactive la fen�tre pour acc�der aux bon registres. */
	C1CTRL1bits.WIN = 0;
	
	/* D�bloque les interupt. */
	RESTORE_CPU_IPL(old_ipl);
	
	return;	
}

/************************** RECEIVE_CAN_MSG ***************************
* Description : Fonction qui configure la r�ception d'un message.
*
* Input :
*	char 	filter_number	Num�ro du filtre utilis�.
*							Valeurs possible: 0 � 2.
*
*	char 	filter_number	Num�ro du masque utilis�.
*							Valeurs possible: 0 � 3. (3 = aucun masque)
*
*	T_TYPE_BUF type_buf		Type de buffer dans lequel le message doit
*							�tre plac�.
*							Valeur possible: BUF_IND ou	BUF_FIFO.
*
*	void (* ptr_fct_receive) (unsigned long ID, T_TYPE_ID type_ID, const void * data_rx, char nbr_data)
*							Pointeur vers la fonction � utiliser lors de
*							la r�ception d'un message.
*
* Output :
*	int				TRUE si la r�ception � pu �tre configur� sinon, FALSE.
*
************************************************************************/
void receive_CAN_msg(unsigned int filter_number, unsigned int mask_number, void (* ptr_fct_receive) (unsigned long ID, T_TYPE_ID type_ID, const void * data_rx, char nbr_data))
{
	int old_ipl;

	/* Bloque les interrupt le temps de changer le d�placement. */
	SET_AND_SAVE_CPU_IPL(old_ipl, 7);
	
	/* Active la fen�tre pour acc�der aux bon registres. */
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
	
	/* D�sactive la fen�tre pour acc�der aux bon registres. */
	C1CTRL1bits.WIN = 0;		
	
	/* D�bloque les interupt. */
	RESTORE_CPU_IPL(old_ipl);
	
	return;
}	

/************************ STOP_RECEIVE_CAN_MSG *************************
* Description : Fonction qui arr�te la r�ception d'un message. 
*
* Input :
*	char 	filter_number	Num�ro du filtre dont on veut arr�ter la 
*							r�ception. Valeurs possible: 0 � 15.
*
*	Output :	AUCUN
*
************************************************************************/
void stop_receive_CAN_msg(unsigned int filter_number)
{
	/* D�sactive le filtre. */
	C1FEN1 &= ~(0x0001 << filter_number);
	
	return;
} 

/*************************** INIT_DMA_CHANNEL **************************
* Description : Fonction qui initialise un cannal DMA.
*
* Input :
*	char 	DMA_channel		Cannal DMA � initialiser.
*							Valeurs possibles: 0 � 7. 
*
*	char 	DMA_dir			Direction du canal. 1 = Read from SRAM
*							0 = Write to SRAM
*
*	unsigned int IRQ		IRQ du p�riph�rique
*
*	unsigned int address	Adresse du p�riph�rique
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
* Description : Fonction qui d�termine la valeur � mettre dans un 
* registre SID pour configurer un filtre ou un mask.
*
* Input :
*	unsigned long ID		ID devant �tre configur� dans le filtre.
*
*	T_TYPE_ID type_ID		Type d'ID utilis�e.
*							Valeur possible: STANDARD_ID ou	EXTENDED_ID.
*
*	char mask_or_filter		Flag qui indique si il s'agit d'un filtre ou 
*							d'un mask. 1 = mask, 0 = filter.
*
* Output :	
*	unsigned int			Valeur � mettre dans le registre.
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
* Description : Fonction qui d�termine la valeur � mettre dans un 
* registre EID pour configurer un filtre ou un mask.
*
* Input :
*	unsigned long ID		ID devant �tre configur� dans le filtre.
*
*	T_TYPE_ID type_ID		Type d'ID utilis�e.
*							Valeur possible: STANDARD_ID ou	EXTENDED_ID.
*
* Output :	
*	unsigned int			Valeur � mettre dans le registre.
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
    //Led3^=1;
	char temp_win;
	unsigned int ii=0, Buf_read_ptr=0, filter_hit=0;
	unsigned long ID_rx=0;
	T_TYPE_ID type_ID_rx;
	
	temp_win = C1CTRL1bits.WIN;
	C1CTRL1bits.WIN = 0;

	//counter_total++;
        /*Rx Buffer interrupt flag*/
	if(C1INTFbits.RBIF == 1)
	{
		/* C'est un buffer configur� en Rx qui a g�n�r� l'interrupt. */
		while( (C1RXFUL1 != 0) || (C1RXFUL2 != 0) )
		{
			Buf_read_ptr = C1FIFO & 0x003F;
		
			/* D�termine l'ID du message re�u selon le type. */
			if( (CAN_msg_Buf[Buf_read_ptr][0] & 0x0001) == 1)
			{
				/* Extended ID */
				ID_rx = ((unsigned long)(CAN_msg_Buf[Buf_read_ptr][1] & 0x0FFF) << 17) | ((unsigned long)(CAN_msg_Buf[Buf_read_ptr][2] & 0xFC00) << 1) | ((unsigned long)(CAN_msg_Buf[Buf_read_ptr][0] & 0x1FFC) >> 2);
				type_ID_rx = EXTENDED_ID;
			}
			else
			{
				/* Standard ID */
				ID_rx = (unsigned long)((CAN_msg_Buf[Buf_read_ptr][0] & 0x1FFC) >> 2);
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

			/* Ex�cute la fonction associ� � ce message. */
			filter_hit = (CAN_msg_Buf[Buf_read_ptr][7] & 0x1F00) >> 8;
			if(config_CAN.ptr_fct_receive[filter_hit] != NULL)
			{
				config_CAN.ptr_fct_receive[filter_hit](ID_rx, type_ID_rx, &CAN_msg_Buf[Buf_read_ptr][3], (char)(CAN_msg_Buf[Buf_read_ptr][2] & 0x000F));
				
			}

		}		
		
		C1INTFbits.RBIF = 0;
	}	
	/*Tx Buffer interrupt flag*/
	if(C1INTFbits.TBIF == 1)
	{
		counter_tx++;

		C1INTFbits.TBIF = 0;
		/* C'est un buffer configur� en Tx qui a g�n�r� l'interrupt. */
		/* V�rifie quel buffer en Tx s'est lib�r�. */
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
