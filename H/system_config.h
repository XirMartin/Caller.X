/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

#ifndef SYSTEM_CONFIG_H
#define	SYSTEM_CONFIG_H

#include "miwi_config.h"          //MiWi Application layer configuration file
#include "miwi_config_p2p.h"      //MiWi Protocol layer configuration file
#include "config_24j40.h"         //Transceiver configuration file


//MIWI DEFINITIONS
#define RFIF                        INTCON3bits.INT1IF
#define RFIE                        INTCON3bits.INT1IE
#define PHY_CS                      LATDbits.LATD2
#define PHY_CS_TRIS                 TRISDbits.TRISD2
#define RF_INT_PIN                  PORTBbits.RB1
#define RF_INT_TRIS                 TRISBbits.TRISB1
#define PHY_WAKE                    LATDbits.LATD0
#define PHY_WAKE_TRIS               TRISDbits.TRISD0
#define PHY_RESETn                  LATDbits.LATD3
#define PHY_RESETn_TRIS             TRISDbits.TRISD3


//DISPLAY DEFINITIONS
#define DISP_CS                      LATCbits.LATC2      //Chip Select
#define DISP_CS_TRIS                 TRISCbits.TRISC2
#define DISP_CD                      LATEbits.LATE0      //Command/Data
#define DISP_CD_TRIS                 TRISEbits.TRISE0
#define DISP_WR                      LATEbits.LATE1      //Write
#define DISP_WR_TRIS                 TRISEbits.TRISE1
#define DISP_RD                      LATEbits.LATE2      //Read
#define DISP_RD_TRIS                 TRISEbits.TRISE2
#define DISP_RST                     LATCbits.LATC2         //LATDbits.LATD1      //Reset
#define DISP_RST_TRIS                TRISCbits.TRISC2   //TRISDbits.TRISD1

//TOUCH DEFINITIONS
#define TOUCH_CS                     LATDbits.LATD7     //Chip Select del Touch
#define TOUCH_CS_TRIS                TRISDbits.TRISD7


//PUSH BUTTON DEFINITIONS
#define BOTON                       PORTAbits.RA4
#define BOTON_TRIS                  TRISAbits.TRISA4


//LED DEFINITIONS
#define LED_1                       LATAbits.LATA0
#define LED_1_TRIS                  TRISAbits.TRISA0
#define LED_2                       LATAbits.LATA1
#define LED_2_TRIS                  TRISAbits.TRISA1
#define LED_3                       LATAbits.LATA2
#define LED_3_TRIS                  TRISAbits.TRISA2
#define LED_4                       LATAbits.LATA3
#define LED_4_TRIS                  TRISAbits.TRISA3
#define LED_R                       LATDbits.LATD6
#define LED_R_TRIS                  TRISDbits.TRISD6
#define LED_G                       LATDbits.LATD5
#define LED_G_TRIS                  TRISDbits.TRISD5
#define LED_B                       LATDbits.LATD4
#define LED_B_TRIS                  TRISDbits.TRISD4


//MAIN DEFINITIONS
#define SCA                         LATBbits.LATB2   //Salida Clock de Arma
#define EFA1                        PORTBbits.RB7
#define EFA2                        PORTBbits.RB6
#define EFA3                        PORTBbits.RB5
#define BEACON                      PORTAbits.RA7
#define PdE                         PORTEbits.RE3   // PdE - Boton - Parada de Emergencia


/*#define Vibrar  PORTBbits.RB6 // Vibrar - Motor
#define MCS     PORTBbits.RB3 // CS modulo MiWi
#define MCS     PORTBbits.RB3 // CS modulo MiWi
#define DCS     PORTBbits.RB2 // CS modulo Display
#define SCS     PORTBbits.RB0 // CS modulo Sonido*/

//SD DEFINITIONS
#define SD_CS                       LATBbits.LATB0
#define SD_CS_TRIS                  TRISBbits.TRISB0

//AUDIO DEFINITIONS
//#define AUDIO_CS                       LATBbits.LATB3
//#define AUDIO_CS_TRIS                  TRISBbits.TRISB3



//EEPROM DEFINITIONS
#define TMRL                        TMR0L

#endif
/* SYSTEM_CONFIG_H */
//EOF

