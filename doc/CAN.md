CAN Lib :
---------

dependencies:

- DMA lib


Configuration des pins
----------------------

**Attention, les pins ne sont pas les même d'un PCB à un autre, vérifier sur les schémas électriques** 

    // Configuration des I/O
    TRISFbits.TRISF2 = 1;                    //Input for CAN
    TRISFbits.TRISF3 = 0;                    //Output for CAN

    // Mappage du périphérique CAN (pour un DSPIC33EP512MC806
    _C1RXR = 98;       //RP98 Pin 34
    RP99R = 0b001110;  //RP99 Pin 33

Configuration du débit
----------------------

    // #define FCAN_800KBITS
    // #define FCAN_250KBITS
    #define FCAN_125KBITS

    // Pour une horloge de 100Mhz
    #define FCAN 100000000    
    // Ou
    #define FCAN FOSC


Utilisation de la librairie
---------------------------

- Ajouter les entêtes dans votre programme
  - `can_chinook3.h` 
  - `can.h`

    #include "can.h"
    #include "can_chinook3.h"

    
    int main(){
      ...
      // Configuration des pins
      TRISFbits.TRISF2 = 1;                    //Input for CAN
      TRISFbits.TRISF3 = 0;                    //Output for CAN

      // Mappage du périphérique CAN (pour un DSPIC33EP512MC806)
      _C1RXR = 98;       //RP98 Pin 34
      RP99R = 0b001110;  //RP99 Pin 33
      
      ...

      // Initialisation de la librarie
      init_CAN(CAN_NORMAL, 8, 2, 3, 7);

      ...
    }

Envoie d'un message
-------------------
    int main(){
      ... 
      // Après l'initialisation de la librarie CAN

      // Déclaration de la mémoire des messages d'envoie
      T_CAN_Tx_MSG can_msg_manual_pitch;

      // Configuration des messages
      config_CAN_Tx_msg(&can_msg_manual_pitch, CAN_MSG_MANUAL_PITCH_SID , STANDARD_ID, 3);
      
      ...


      // Envoie d'un message, 
      // un temps d'attente doit être mis entre chaque envoie

      // Buffer d'envoie
      unsigned char CanBuf[8] = {0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57};
      // Envoie
      send_CAN_msg(&can_msg_manual_pitch, CanBuf, 8);
      // Attente de l'envoie du message
      while(is_CAN_msg_send(&can_msg_manual_pitch) != TRUE);
        
    }
   
Réception des messages
----------------------

###Configuration de Rx

- Le filtre

Ne pas mettre dans la boucle du while principale

    config_CAN_filter(0, CAN_MSG_MANUAL_PITCH_SID, STANDARD_ID);



###Création de la fonction de réception

Lorsqu’un message can arrive dans le pic, un flag de déclenche automatiquement quand celui-ci est décodé. Les données sont envoyées dans la fonction : fct_can_manual_pitch.

    void fct_can_manual_pitch(unsigned long ID,
                              T_TYPE_ID type_ID, 
                              const void * data_rx,
                              char nbr_data){
      char* datReceive ;
      int old_ipl;
      // Block interruptions
      SET_AND_SAVE_CPU_IPL(old_ipl, 7);
    
      datReceive = (char *)data_rx;

      RESTORE_CPU_IPL(old_ipl);
    }


Toutes les données sont misent dans datReceive.

### Réception du message
    
    receive_CAN_msg(0, 3, fct_can_manual_pitch);
    // On doit ensuite récupérer les données de datReceive (dans la fonction fct_can_manual_pitch)
