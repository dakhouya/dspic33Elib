/****************************************************************************************************
                               ``                   
                             +:''+;'                
                          ;''''   ';;'`             
                      .+'''' .+';;;;;;;'            
                   .+'.;'; ';;;;;;;;''''            
                  ;'''''' ';;;;;'''''++'            
                  ''',:`'+''''''''+@@@@#            
                  ;  ;'+,+'';'''#@@@@@@#            
                  @       @@++#@@@@@@@@#            
                  ,       @@@@++''@@@@@#.           BAJA �TS
                 +       ,@@@@'+++'@@@@#:           �cole de technologie sup�rieure
                 '       @@'@+++++'@@@@#'           baja.etsmtl.ca
                 @       @@@@+++++'@@@@#'           baja@ens.etsmtl.ca
                 ,       @@@@++++''@@@@#'           
                +        @+@++++++'@@@@#+           
                '       `@#;+'++++++@@@#'           
                @       ':''#@'''#++@@@#;           
                :       '@;;'''++'+#@@@#;           
               +    :.@@@';;'+''''+@@@@#+           
               +   .';'.;@''''''''+@@@@#+@          
               @  :;'',;#'++'''''+#@@@@#@@          
          @;   : `;;' ;'+;@+''''''#@@@@#@@          
          @#  '+'''' ';'#@'+'+++''#@@@@#@'          
           #'+''';' ';;'+''+'++++'@#'':;+`.   `'.   
            ''';;' ';;'@'''@##++''';'+:#@#`  @';+#  
           @##+'; ;;;;;'+#@#@@'';'++;'' # '`:+;:++' 
           `'''+'';;'''#'''''''++++;;'+@' : #','+#+ 
       `  #@##++#'''''++@::;'+'++'''+##@#@`.+;:+:;+ 
    #+'++'@@@@@@@'';+.:+'++;;;''''++##;  @'#+:;#;'+ 
  ,'';'++ @#@@@@#+++`#++.#++;;''#+#+##@'` ;#+;;'+,+ 
  +;,'+#+:+@@@@@@@###+'+'.+++###+#+       :@+''#+.' 
 #':;+'+'#@#@@@@##'+'#+;# ++####           @+++;,.+ 
 '','+':`+`@@@@@@@''+'#;+`;##++'+.         :#++#.;@ 
 +';+##;@+,@#@@@@@++''#;@.;`++''++          #'++++. 
`#''#@,@@@@#,    ;#####@@ #+':;+++#         ###++@  
 +++#+#.;@           @   @#+;.'@;#+          @@#@   
 +#++...',             @  #+;;+:::+                 
 '##++:'#                @#+;;#@;'+                 
 :###+++                  ++;';;;:'                 
  #####                   #+'':+,,'                 
    ,.                    ++++',;:+                 
                          @++++:.;@                 
                          @+++++'+                  
                           +##+++@                  
                            @#@+@                   
                             `,                     
                                                                                                                                       
********************************************* BAJA �TS **********************************************
* 
* FILE NAME :			dma.c
* 
* Description:			DMA functions
* 
* Authors : 			Jean-S�bastien Breault,
* 
* Modifications :
* 
*	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 	| Person         	| Date      			| Comments
*	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 	| JSB		 		| 5 november 2011		| first integration
*	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*  Source inspired by the WalkingMachine CAN Library (Etienne Collard-Fr�chette, �TS)
****************************************************************************************************/


#include "dma.h"
#include <p33EP512MC806.h>

/*Registers*/

volatile unsigned int * DMAiCON[] = {&DMA0CON, &DMA1CON, &DMA2CON, &DMA3CON, &DMA4CON, &DMA5CON, &DMA6CON, &DMA7CON};
volatile unsigned int * DMAiPAD[] = {&DMA0PAD, &DMA1PAD, &DMA2PAD, &DMA3PAD, &DMA4PAD, &DMA5PAD, &DMA6PAD, &DMA7PAD};
volatile unsigned int * DMAiCNT[] = {&DMA0CNT, &DMA1CNT, &DMA2CNT, &DMA3CNT, &DMA4CNT, &DMA5CNT, &DMA6CNT, &DMA7CNT};
volatile unsigned int * DMAiREQ[] = {&DMA0REQ, &DMA1REQ, &DMA2REQ, &DMA3REQ, &DMA4REQ, &DMA5REQ, &DMA6REQ, &DMA7REQ};
volatile unsigned int * DMAiSTA[] = {&DMA0STAL, &DMA1STAL, &DMA2STAL, &DMA3STAL, &DMA4STAL, &DMA5STAL, &DMA6STAL, &DMA7STAL};
volatile unsigned int * DMAiSTB[] = {&DMA0STBL, &DMA1STBL, &DMA2STBL, &DMA3STBL, &DMA4STBL, &DMA5STBL, &DMA6STBL, &DMA7STBL};

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
void init_DMA_channel(unsigned int DMA_channel, char DMA_dir, unsigned int IRQ, unsigned int address, unsigned int offset)
{	
	*(DMAiCON[DMA_channel]) = 0x0020 | ((unsigned int)DMA_dir << 13);
	*(DMAiPAD[DMA_channel]) = address;
	*(DMAiCNT[DMA_channel]) = 7;
 	*(DMAiREQ[DMA_channel]) = IRQ;	
	*(DMAiSTA[DMA_channel]) = offset;
	*(DMAiCON[DMA_channel]) |= 0x8000;
	
	return;
}
