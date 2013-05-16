/***********************************************************************
* lib_CAN.h
* Cette librairie gère le bus CAN. Il permet la configuration 
* initial du  bus, l'envoi et la réception de messages. 
* 
* AUTEUR: 	Etienne Collard-Fréchette
* DATE:		1 mars 2011
* MODIFIÉ:	
************************************************************************/

#ifndef __lib_CAN_H
#define __lib_CAN_H


//#define FCAN_800KBITS
//#define FCAN_250KBITS
#define FCAN_125KBITS

//#define DEBUG_CAN
/***************************** GENERAL ********************************/
#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif

/***************************** TEMPORELLE ******************************/
/*CAN Baud Rate Configuration*/
#ifdef FCAN_125KBITS
	#define FCAN  	      	100000000
	#define CAN_BITRATE 	125000
	#define CAN_NTQ 		14			// 20 Time Quanta in a Bit Time
	#define CAN_PRESCALER 	((FCAN/(2*CAN_NTQ*CAN_BITRATE))-1)
	
	#define CAN_PHASE_1		10
	#define CAN_PHASE_2		4
	#define CAN_PROPAGATION	0x0005
	#define CAN_SJW			4
	#define CAN_SAMPLE		0x0001
#endif
#ifdef FCAN_250KBITS
	#define FCAN  	      	100000000
	#define CAN_BITRATE 	250000  
	#define CAN_NTQ 		14			// 20 Time Quanta in a Bit Time
	#define CAN_PRESCALER 	((FCAN/(2*CAN_NTQ*CAN_BITRATE))-1)
	
	#define CAN_PHASE_1		10
	#define CAN_PHASE_2		4
	#define CAN_PROPAGATION	0x0005
	#define CAN_SJW			4
	#define CAN_SAMPLE		0x0001
#endif
#ifdef FCAN_500KBITS
	#define FCAN  	      	35000000
	#define CAN_BITRATE 	500000  
	#define CAN_NTQ 		8			// 20 Time Quanta in a Bit Time
	#define CAN_PRESCALER 	((FCAN/(2*CAN_NTQ*CAN_BITRATE))-1)
	
	#define CAN_PHASE_1		6
	#define CAN_PHASE_2		2
	#define CAN_PROPAGATION	0x0005
	#define CAN_SJW			4
	#define CAN_SAMPLE		0x0001
#endif
#ifdef FCAN_800KBITS
	#define FCAN  	      	35000000
	#define CAN_BITRATE 	795440  
	#define CAN_NTQ 		22			// 20 Time Quanta in a Bit Time
	#define CAN_PRESCALER 	((FCAN/(2*CAN_NTQ*CAN_BITRATE))-1)
	
	#define CAN_PHASE_1		16
	#define CAN_PHASE_2		6
	#define CAN_PROPAGATION	0x0005
	#define CAN_SJW			2
	#define CAN_SAMPLE		0x0001
#endif

/****************************** STRUCTURE *****************************/

typedef enum
{
	CAN_NORMAL=0,
	CAN_DISABLE=1,
	CAN_LOOPBACK=2,
	CAN_LISTEN_ONLY=3,
	CAN_CONFIGURATION=4,
	CAN_LISTEN_ALL=7
}T_CAN_MODE;

typedef enum
{
	STANDARD_ID,
	EXTENDED_ID
}T_TYPE_ID;

typedef struct
{
	unsigned long	ID;
	T_TYPE_ID 		type_ID;			
	unsigned int	priority;			// Niveau de priorité.
	int				Tx_Buf;				// Numéro du buffer utilisé pour transmettre le message.
}T_CAN_Tx_MSG;

typedef struct
{
	char nbr_buf_Tx;					// Nombre de buffer configuré en Tx.
	char etat_Buf[8];					// Flag intiquant si un buffer est présentement utilisé.
	T_CAN_Tx_MSG * Buf_Tx_assign[8];	// Pointeur vers les messages contenu dans les buffer en Tx
	void (* ptr_fct_receive[16]) (unsigned long ID, T_TYPE_ID type_ID, const void * data_rx, char nbr_data);	// Liste des pointeur de fonction à executer lorsqu'un
																												// message est reçu dans un buffer.
}T_CAN_CONFIG;	

/***************************** PROTOTYPES *****************************/

/* Fonction d'initialisation. */
void init_CAN(T_CAN_MODE mode, unsigned int nbr_buf_Tx, unsigned int DMA_Rx, unsigned int DMA_Tx, unsigned int int_level);

/* Fonction pour l'envoi de messages. */
void config_CAN_Tx_msg(T_CAN_Tx_MSG * CAN_msg, unsigned long ID, T_TYPE_ID type_ID, unsigned int priority);

int send_CAN_msg(T_CAN_Tx_MSG * CAN_tx_msg, const void * data_scr, char nbr_data);

int is_CAN_msg_send(T_CAN_Tx_MSG * CAN_tx_msg);

/* Fonction pour la réception de messages. */
void config_CAN_filter(unsigned int filter_number, unsigned long ID, T_TYPE_ID type_ID);

void config_CAN_mask(unsigned int mask_number, unsigned long mask, T_TYPE_ID type_ID);

void receive_CAN_msg(unsigned int filter_number, unsigned int mask_number, void (* ptr_fct_receive) (unsigned long ID, T_TYPE_ID type_ID, const void * data_rx, char nbr_data));

void stop_receive_CAN_msg(unsigned int filter_number);

void can_get_buttons(unsigned long ID, T_TYPE_ID type_ID, const void * data_rx, char nbr_data);

#endif

