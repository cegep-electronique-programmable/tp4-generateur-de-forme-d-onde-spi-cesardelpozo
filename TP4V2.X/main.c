/**
 * Auteur 
 * Maxime Champagne
 * 23 mars 2022
 * 
 * Modifié par César Antonio Del Pozo Catalan
 *
 *
 * SPI/main.c
 * 
*/
#include <stdio.h>
#include "mcc_generated_files/mcc.h"

#define MAX 60
uint8_t const sin[MAX] ={
            254,254,252,249,244,238,231,222,213,202,
            191,179,167,154,141,127,114,101,88,76,
            64,53,42,33,24,17,11,6,3,1,
            0,1,3,6,11,17,24,33,42,53,
            64,76,88,101,114,128,141,154,167,179,
            191,202,213,222,231,238,244,249,252,254};

uint8_t const car[MAX] ={
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255};

uint8_t const tri[MAX] ={
            9,17,26,34,43,51,60,68,77,85,
            94,102,111,119,128,136,145,153,162,170,
            179,187,196,204,213,221,230,238,247,255,
            247,238,230,221,213,204,196,187,179,170,
            162,153,145,136,128,119,111,102,94,86,
            77,68,60,51,43,34,26,17,9,0};

void out_dig(uint8_t x);
void sinus_60(void);
void carre_60(void);
void triangle_60(void);
void myTimer1_ISR(void);
uint8_t i = 0;			//Variable globale d'incrémentation
bool h = 0;				//Variable globale pour s'assurer d'envoyer un point une fois par interruption
uint16_t freq = 0xEFB9;	//Fréquence d'onde de base (20Hz)

/*
                         Main application
 */
void main(void)
{
    //uint8_t valeur, lecture;		Pour code test
    //float tension;				Pour code test
    char onde;		//Variable d'état
    char oldonde;	//Variable qui prends la valeur de l'état si on veut changer la fréquence
    int j = 0;
    
    SYSTEM_Initialize();
    
    INTERRUPT_GlobalInterruptEnable();
    
    INTERRUPT_PeripheralInterruptEnable();
    
    TMR1_SetInterruptHandler(myTimer1_ISR);

    SSPCON1bits.SSPEN = 1;
    IO_RA5_SetHigh();
    
    while (1)
    {
//        //Code de test pour valider le fonctionnement du potentiomètre
//        {
//            printf("\n\rEntrer une valeur entre 0 et 255, suivie de [Enter]");
//            valeur = 0;
//            do
//            {
//                do
//                {
//                    lecture = EUSART1_Read();               
//                }
//                while (((lecture < '0') || (lecture > '9')) && (lecture != 0x0d));
//                if ((lecture >= '0') && (lecture <= '9')) 
//                {
//                    valeur = 10 * valeur + lecture - '0';
//                    putchar(lecture);
//                }
//            }
//        
//            while ((lecture != 0x0d) && (valeur < 26)); 
//            tension = (float)5* valeur /256;
//            printf("\n\rValeur = %u tension = %3.2f ", valeur, tension);
//            out_dig(valeur);    // envoi sur potentiometre 
//        } 
        
        if(EUSART1_is_rx_ready()){	//Si une touche est touchée
            onde = EUSART1_Read();	//Mettre la valeur de la touche dans la variable d'état
        }
        
        else{
            switch(onde){
                case 's'://Si on choisi une onde SINUS
                    if(h==1)//S'il y a eu une interruption
                    {
                        sinus_60();
                        h=0;
                    }
                    oldonde = 's';
                    break;
                case 'c'://Si on choisi une onde CARRE
                    if(h==1)//S'il y a eu une interruption
                    {
                        carre_60();
                        h=0;
                    }
                    oldonde = 'c';
                    break;
                case 't'://Si on choisi une onde TRIANGLE
                    if(h==1)//S'il y a eu une interruption
                    {
                        triangle_60();
                        h=0;
                    }
                    oldonde = 't';
                    break;
                case 'p'://Si on veut augementer la fréquence
                    j++;
                    if(j == 1){
                        freq = 0xF7DD;
                    }
                    else if (j == 2){
                        freq = 0xFA94;
                    }
                    else if (j == 3){
                        freq = 0xFBEE;
                    }
                    else if (j == 4){
                        freq = 0xFCBF;
                    }
                    else{
                        j = 4;
                    }
                    onde = oldonde;
                    break;
                case 'm'://Si on veut diminuer la fréquence
                    j--;
                    if(j == 3){
                        freq = 0xFBEE;
                    }
                    else if (j == 2){
                        freq = 0xFA94;
                    }
                    else if (j == 1){
                        freq = 0xF7DD;
                    }
                    else if (j == 0){
                        freq = 0xEFB9;
                    }
                    else{
                        j = 0;
                    }
                    onde = oldonde;
                    break;
                default://Si une autre touche est touchée
                    printf("Choisir une forme d'onde valide : DEFAULT ENTRY = SINUS\n\r");
                    onde = 's';
                    break;
            }
        }
    }
}

//---------------------------------------------------------------
// Routine d'interruption du Timer1
//---------------------------------------------------------------
void myTimer1_ISR(void){
    //static uint8_t i; 
    
    TMR1_WriteTimer(freq);
//    0xEFB9 - 20Hz
//    0xF7DD - 40Hz
//    0xFA94 - 60Hz
//    0xFBEE - 80Hz
//    0xFCBF - 100Hz
    i++;
    h=1;
    if (i==MAX){
        i=0;
    }
}
    
//----------------------------------------------------------------
// Transmission au pot. d'une onde comprenant 60 points par cycle.
//----------------------------------------------------------------
void sinus_60(void) {
    //uint8_t i;
    out_dig(sin[i]);
    __delay_ms(1);
}
void carre_60(void) {
    //uint8_t i;
    out_dig(car[i]);
    __delay_ms(1);
}
void triangle_60(void) {
    //uint8_t i;
    out_dig(tri[i]);
    __delay_ms(1);
}

//----------------------------------------------------------------
//  Transmission d'une donnee a la sortie du pot. numerique
//----------------------------------------------------------------
void out_dig(uint8_t x)
{
	IO_RA5_SetLow();   // selection du potentiometre
	SPI_ExchangeByte(0x11);  // ecriture, pot. 0
	SPI_ExchangeByte(x);
	IO_RA5_SetHigh();
	//__delay_ms(1);
}
