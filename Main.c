//  HEADERS DE ESTE PROYECTO (TODO DENTRO DE LA MISMA CARPETA) HEADERS
#include "H/system.h"
#include "H/system_config.h"
#include "H/miwi_api.h"
#include "H/pic18f46k20.h"
#include <plib/timers.h>
#include "H/Adafruit_ILI9341.h"
#include "H/registers.h"
#include <string.h>
#include <GenericTypeDefs.h>
#define _XTAL_FREQ 64000000

#if 1

#pragma config FOSC = INTIO67   //SELECCIONO OSCILADOR INTERNO
#pragma config BOREN = OFF      //DESHABILITO BROWN OUT DETECT
#pragma config WDTEN = OFF      //DESHABILITO WDT
#pragma config LVP = OFF        //DESHABILITO LOW VOLTAGE PROGRAMMING
#pragma config PBADEN = OFF     //DESHABILITO PORT B A/Ds
#pragma config HFOFST = OFF     //ESPERO A Q EL OSCILADOR SE ESTABILICE
#pragma config MCLRE = OFF      //DESHABILITO MCLR
#pragma config STVREN = OFF     //DESHABILITO MCLR

#endif // BITS DE CONFIGURACION

#if 1

// *************************** DEFINICIONES FISICAS DEL MICRO ******************

#define LED_R                       LATDbits.LATD6
#define LED_G                       LATDbits.LATD5
#define LED_B                       LATDbits.LATD4

#define LED_PAN                     LATAbits.LATA4
#define LED_MiWi                    LATAbits.LATA5
#define LED_U1                      LATEbits.LATE1
#define LED_U2                      LATEbits.LATE2



#define Uart1_R                     PORTAbits.RA6
#define Uart2_R                     PORTAbits.RA2
#define Uart1_CS                    LATAbits.LATA7
#define Uart2_CS                    LATAbits.LATA1

#define Mod1_CS                     LATAbits.LATA0
#define Mod1_R                      PORTAbits.RA3
#define Mod2_CS                     LATAbits.LATA1
#define Mod2_R                      PORTAbits.RA2
#define Mod3_CS                     LATAbits.LATA7
#define Mod3_R                      PORTAbits.RA6
#define Mod4_CS                     LATAbits.LATA3
#define Mod5_CS                     LATDbits.LATD1


#define Boton_1                     PORTBbits.RB2
#define Boton_2                     PORTBbits.RB3
#define Boton_3                     PORTBbits.RB4
#define Boton_4                     PORTBbits.RB5


// Constantes

#define TRx   0xFCDF  // 63BF(20ms) B1DF(10ms) FCDF (400 uS) FE6F(200 uS) FF37(100 us) para la espera entre byte y byte
#define TACK  0x159F  // 159F(30ms) para esperar el ACK , sino reenvio el Mjs

 uint8_t FN;
 uint8_t DATA;
 uint8_t DAUX;


// *************************** Configuraciones Generales (0 y FN´s de pruebas) *

 uint8_t myChannel = 11;


 BYTE  LNID; //Local Node ID


// *************************** Variables del Juego (21 - 40) *******************

 BYTE    EQUIPO; //hola


// *************************** Configuracion Balistica (151 - 169) *************

 BYTE VRx[25];
 BYTE VRx_Aux[25];

 BIT   F1msp;    // (1ms patron) FLAG en el que la Interrupcion avisa que paso 1 ms
 BYTE  R10msp;   // (10ms patron) Registro que la interupcion incrementa cuando paso 10 ms
 BYTE  R100msp;   // (100ms patron) Registro que la interupcion incrementa cuando paso 100 ms
 BYTE  R1sp;    // (1s patron) Registro que la interupcion incrementa cuando paso 1 s

 BIT   F1ms;   // FLAG QUE INDICA QUE PASARON 1 ms
 BIT   F10ms;  // FLAG QUE INDICA QUE PASARON 10 ms
 BIT   F100ms; // FLAG QUE INDICA QUE PASARON 100 ms
 BIT   F1s;    // FLAG QUE INDICA QUE PASARON 1 s

 BYTE  T100;  // REGISTRO AUXILIAR PARA CONTAR 100 ms
 BYTE  T10;   // REGISTRO AUXILIAR PARA CONTAR 10 ms
 BYTE  T1;    // REGISTRO AUXILIAR PARA CONTAR 1 s


 BIT FTPB1;      // Flag de Tiempo Pregunta del Boton 1
 BIT FTRB1;      // Flag de Tiempo Repuesta del Boton 1
 BIT FTPB2;
 BIT FTRB2;
 BIT FTPB3;
 BIT FTRB3;
 BIT FTPB4;
 BIT FTRB4;


 BIT FB1P;       // Flag Boton 1 Presionado
 BIT FB2P;
 BIT FB3P;
 BIT FB4P;


 BYTE TARB;      // Tiempo AntiRebote para Botones

 BYTE RTB1;      // Registro de Tiempo del Boton 1
 BYTE RTB2;
 BYTE RTB3;
 BYTE RTB4;

 BYTE T_LED;         // Tiempo que se mantiene prendido los leds de estado

 BYTE RT_LED_PAN;
 BIT FTR_LED_PAN;
 BYTE RT_LED_MiWi;
 BIT FTR_LED_MiWi;
 BYTE RT_LED_U1;
 BIT FTR_LED_U1;
 BYTE RT_LED_U2;
 BIT FTR_LED_U2;

 BYTE RMW;        // Tiempo de Retrasnsmisión por MiWi
 uint8_t RTRMW;   // Registro de Tiempo de Retrasnsmisión por MiWi
 BIT FTPRMW;      // Flag de Tiempo Pregunta de Retrasnsmisión por MiWi
 BIT FTRRMW;      // Flag de Tiempo Repuesta de Retrasnsmisión por MiWi



 uint16_t  CRC;
 uint16_t  CRC_Mk;
 uint16_t  CRC_Rx;


//**************** VARIABLES DEL DISPLAY *****************

   uint16_t
	WIDTH, HEIGHT;   // this is the 'raw' display w/h - never changes
   uint16_t
	//_width, _height, // dependent on rotation
	cursor_x, cursor_y;
   uint16_t
	textcolor, textbgcolor;
   uint8_t
	textsize,
	rotation;
   BIT	wrap; // If set, 'wrap' text at right edge of display


const char font[] = {
        0x00, 0x00, 0x00, 0x00, 0x00,
	0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
	0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
	0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
	0x18, 0x3C, 0x7E, 0x3C, 0x18,
	0x1C, 0x57, 0x7D, 0x57, 0x1C,
	0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
	0x00, 0x18, 0x3C, 0x18, 0x00,
	0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
	0x00, 0x18, 0x24, 0x18, 0x00,
	0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
	0x30, 0x48, 0x3A, 0x06, 0x0E,
	0x26, 0x29, 0x79, 0x29, 0x26,
	0x40, 0x7F, 0x05, 0x05, 0x07,
	0x40, 0x7F, 0x05, 0x25, 0x3F,
	0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
	0x7F, 0x3E, 0x1C, 0x1C, 0x08,
	0x08, 0x1C, 0x1C, 0x3E, 0x7F,
	0x14, 0x22, 0x7F, 0x22, 0x14,
	0x5F, 0x5F, 0x00, 0x5F, 0x5F,
	0x06, 0x09, 0x7F, 0x01, 0x7F,
	0x00, 0x66, 0x89, 0x95, 0x6A,
	0x60, 0x60, 0x60, 0x60, 0x60,
	0x94, 0xA2, 0xFF, 0xA2, 0x94,
	0x08, 0x04, 0x7E, 0x04, 0x08,
	0x10, 0x20, 0x7E, 0x20, 0x10,
	0x08, 0x08, 0x2A, 0x1C, 0x08,
	0x08, 0x1C, 0x2A, 0x08, 0x08,
	0x1E, 0x10, 0x10, 0x10, 0x10,
	0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
	0x30, 0x38, 0x3E, 0x38, 0x30,
	0x06, 0x0E, 0x3E, 0x0E, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x5F, 0x00, 0x00,
	0x00, 0x07, 0x00, 0x07, 0x00,
	0x14, 0x7F, 0x14, 0x7F, 0x14,
	0x24, 0x2A, 0x7F, 0x2A, 0x12,
	0x23, 0x13, 0x08, 0x64, 0x62,
	0x36, 0x49, 0x56, 0x20, 0x50,
	0x00, 0x08, 0x07, 0x03, 0x00,
	0x00, 0x1C, 0x22, 0x41, 0x00,
	0x00, 0x41, 0x22, 0x1C, 0x00,
	0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
	0x08, 0x08, 0x3E, 0x08, 0x08,
	0x00, 0x80, 0x70, 0x30, 0x00,
	0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x60, 0x60, 0x00,
	0x20, 0x10, 0x08, 0x04, 0x02,
	0x3E, 0x51, 0x49, 0x45, 0x3E,
	0x00, 0x42, 0x7F, 0x40, 0x00,
	0x72, 0x49, 0x49, 0x49, 0x46,
	0x21, 0x41, 0x49, 0x4D, 0x33,
	0x18, 0x14, 0x12, 0x7F, 0x10,
	0x27, 0x45, 0x45, 0x45, 0x39,
	0x3C, 0x4A, 0x49, 0x49, 0x31,
	0x41, 0x21, 0x11, 0x09, 0x07,
	0x36, 0x49, 0x49, 0x49, 0x36,
	0x46, 0x49, 0x49, 0x29, 0x1E,
	0x00, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x40, 0x34, 0x00, 0x00,
	0x00, 0x08, 0x14, 0x22, 0x41,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x00, 0x41, 0x22, 0x14, 0x08,
	0x02, 0x01, 0x59, 0x09, 0x06,
	0x3E, 0x41, 0x5D, 0x59, 0x4E,
	0x7C, 0x12, 0x11, 0x12, 0x7C,
	0x7F, 0x49, 0x49, 0x49, 0x36,
	0x3E, 0x41, 0x41, 0x41, 0x22,
	0x7F, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x49, 0x49, 0x49, 0x41,
	0x7F, 0x09, 0x09, 0x09, 0x01,
	0x3E, 0x41, 0x41, 0x51, 0x73,
	0x7F, 0x08, 0x08, 0x08, 0x7F,
	0x00, 0x41, 0x7F, 0x41, 0x00,
	0x20, 0x40, 0x41, 0x3F, 0x01,
	0x7F, 0x08, 0x14, 0x22, 0x41,
	0x7F, 0x40, 0x40, 0x40, 0x40,
	0x7F, 0x02, 0x1C, 0x02, 0x7F,
	0x7F, 0x04, 0x08, 0x10, 0x7F,
	0x3E, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x09, 0x09, 0x09, 0x06,
	0x3E, 0x41, 0x51, 0x21, 0x5E,
	0x7F, 0x09, 0x19, 0x29, 0x46,
	0x26, 0x49, 0x49, 0x49, 0x32,
	0x03, 0x01, 0x7F, 0x01, 0x03,
	0x3F, 0x40, 0x40, 0x40, 0x3F,
	0x1F, 0x20, 0x40, 0x20, 0x1F,
	0x3F, 0x40, 0x38, 0x40, 0x3F,
	0x63, 0x14, 0x08, 0x14, 0x63,
	0x03, 0x04, 0x78, 0x04, 0x03,
	0x61, 0x59, 0x49, 0x4D, 0x43,
	0x00, 0x7F, 0x41, 0x41, 0x41,
	0x02, 0x04, 0x08, 0x10, 0x20,
	0x00, 0x41, 0x41, 0x41, 0x7F,
	0x04, 0x02, 0x01, 0x02, 0x04,
	0x40, 0x40, 0x40, 0x40, 0x40,
	0x00, 0x03, 0x07, 0x08, 0x00,
	0x20, 0x54, 0x54, 0x78, 0x40,
	0x7F, 0x28, 0x44, 0x44, 0x38,
	0x38, 0x44, 0x44, 0x44, 0x28,
	0x38, 0x44, 0x44, 0x28, 0x7F,
	0x38, 0x54, 0x54, 0x54, 0x18,
	0x00, 0x08, 0x7E, 0x09, 0x02,
	0x18, 0xA4, 0xA4, 0x9C, 0x78,
	0x7F, 0x08, 0x04, 0x04, 0x78,
	0x00, 0x44, 0x7D, 0x40, 0x00,
	0x20, 0x40, 0x40, 0x3D, 0x00,
	0x7F, 0x10, 0x28, 0x44, 0x00,
	0x00, 0x41, 0x7F, 0x40, 0x00,
	0x7C, 0x04, 0x78, 0x04, 0x78,
	0x7C, 0x08, 0x04, 0x04, 0x78,
	0x38, 0x44, 0x44, 0x44, 0x38,
	0xFC, 0x18, 0x24, 0x24, 0x18,
	0x18, 0x24, 0x24, 0x18, 0xFC,
	0x7C, 0x08, 0x04, 0x04, 0x08,
	0x48, 0x54, 0x54, 0x54, 0x24,
	0x04, 0x04, 0x3F, 0x44, 0x24,
	0x3C, 0x40, 0x40, 0x20, 0x7C,
	0x1C, 0x20, 0x40, 0x20, 0x1C,
	0x3C, 0x40, 0x30, 0x40, 0x3C,
	0x44, 0x28, 0x10, 0x28, 0x44,
	0x4C, 0x90, 0x90, 0x90, 0x7C,
	0x44, 0x64, 0x54, 0x4C, 0x44,
	0x00, 0x08, 0x36, 0x41, 0x00,
	0x00, 0x00, 0x77, 0x00, 0x00,
	0x00, 0x41, 0x36, 0x08, 0x00,
	0x02, 0x01, 0x02, 0x04, 0x02,
	0x3C, 0x26, 0x23, 0x26, 0x3C,
	0x1E, 0xA1, 0xA1, 0x61, 0x12,
	0x3A, 0x40, 0x40, 0x20, 0x7A,
	0x38, 0x54, 0x54, 0x55, 0x59,
	0x21, 0x55, 0x55, 0x79, 0x41,
	0x21, 0x54, 0x54, 0x78, 0x41,
	0x21, 0x55, 0x54, 0x78, 0x40,
	0x20, 0x54, 0x55, 0x79, 0x40,
	0x0C, 0x1E, 0x52, 0x72, 0x12,
	0x39, 0x55, 0x55, 0x55, 0x59,
	0x39, 0x54, 0x54, 0x54, 0x59,
	0x39, 0x55, 0x54, 0x54, 0x58,
	0x00, 0x00, 0x45, 0x7C, 0x41,
	0x00, 0x02, 0x45, 0x7D, 0x42,
	0x00, 0x01, 0x45, 0x7C, 0x40,
	0xF0, 0x29, 0x24, 0x29, 0xF0,
	0xF0, 0x28, 0x25, 0x28, 0xF0,
	0x7C, 0x54, 0x55, 0x45, 0x00,
	0x20, 0x54, 0x54, 0x7C, 0x54,
	0x7C, 0x0A, 0x09, 0x7F, 0x49,
	0x32, 0x49, 0x49, 0x49, 0x32,
	0x32, 0x48, 0x48, 0x48, 0x32,
	0x32, 0x4A, 0x48, 0x48, 0x30,
	0x3A, 0x41, 0x41, 0x21, 0x7A,
	0x3A, 0x42, 0x40, 0x20, 0x78,
	0x00, 0x9D, 0xA0, 0xA0, 0x7D,
	0x39, 0x44, 0x44, 0x44, 0x39,
	0x3D, 0x40, 0x40, 0x40, 0x3D,
	0x3C, 0x24, 0xFF, 0x24, 0x24,
	0x48, 0x7E, 0x49, 0x43, 0x66,
	0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
	0xFF, 0x09, 0x29, 0xF6, 0x20,
	0xC0, 0x88, 0x7E, 0x09, 0x03,
	0x20, 0x54, 0x54, 0x79, 0x41,
	0x00, 0x00, 0x44, 0x7D, 0x41,
	0x30, 0x48, 0x48, 0x4A, 0x32,
	0x38, 0x40, 0x40, 0x22, 0x7A,
	0x00, 0x7A, 0x0A, 0x0A, 0x72,
	0x7D, 0x0D, 0x19, 0x31, 0x7D,
	0x26, 0x29, 0x29, 0x2F, 0x28,
	0x26, 0x29, 0x29, 0x29, 0x26,
	0x30, 0x48, 0x4D, 0x40, 0x20,
	0x38, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x38,
	0x2F, 0x10, 0xC8, 0xAC, 0xBA,
	0x2F, 0x10, 0x28, 0x34, 0xFA,
	0x00, 0x00, 0x7B, 0x00, 0x00,
	0x08, 0x14, 0x2A, 0x14, 0x22,
	0x22, 0x14, 0x2A, 0x14, 0x08,
	0xAA, 0x00, 0x55, 0x00, 0xAA,
	0xAA, 0x55, 0xAA, 0x55, 0xAA,
	0x00, 0x00, 0x00, 0xFF, 0x00,
	0x10, 0x10, 0x10, 0xFF, 0x00,
	0x14, 0x14, 0x14, 0xFF, 0x00,
	0x10, 0x10, 0xFF, 0x00, 0xFF,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x14, 0x14, 0x14, 0xFC, 0x00,
	0x14, 0x14, 0xF7, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x14, 0x14, 0xF4, 0x04, 0xFC,
	0x14, 0x14, 0x17, 0x10, 0x1F,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0x1F, 0x00,
	0x10, 0x10, 0x10, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0xF0, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0xFF, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x14,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x00, 0x1F, 0x10, 0x17,
	0x00, 0x00, 0xFC, 0x04, 0xF4,
	0x14, 0x14, 0x17, 0x10, 0x17,
	0x14, 0x14, 0xF4, 0x04, 0xF4,
	0x00, 0x00, 0xFF, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x14, 0x14, 0xF7, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x17, 0x14,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0xF4, 0x14,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x00, 0x00, 0x1F, 0x10, 0x1F,
	0x00, 0x00, 0x00, 0x1F, 0x14,
	0x00, 0x00, 0x00, 0xFC, 0x14,
	0x00, 0x00, 0xF0, 0x10, 0xF0,
	0x10, 0x10, 0xFF, 0x10, 0xFF,
	0x14, 0x14, 0x14, 0xFF, 0x14,
	0x10, 0x10, 0x10, 0x1F, 0x00,
	0x00, 0x00, 0x00, 0xF0, 0x10,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x38, 0x44, 0x44, 0x38, 0x44,
	0x7C, 0x2A, 0x2A, 0x3E, 0x14,
	0x7E, 0x02, 0x02, 0x06, 0x06,
	0x02, 0x7E, 0x02, 0x7E, 0x02,
	0x63, 0x55, 0x49, 0x41, 0x63,
	0x38, 0x44, 0x44, 0x3C, 0x04,
	0x40, 0x7E, 0x20, 0x1E, 0x20,
	0x06, 0x02, 0x7E, 0x02, 0x02,
	0x99, 0xA5, 0xE7, 0xA5, 0x99,
	0x1C, 0x2A, 0x49, 0x2A, 0x1C,
	0x4C, 0x72, 0x01, 0x72, 0x4C,
	0x30, 0x4A, 0x4D, 0x4D, 0x30,
	0x30, 0x48, 0x78, 0x48, 0x30,
	0xBC, 0x62, 0x5A, 0x46, 0x3D,
	0x3E, 0x49, 0x49, 0x49, 0x00,
	0x7E, 0x01, 0x01, 0x01, 0x7E,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x44, 0x44, 0x5F, 0x44, 0x44,
	0x40, 0x51, 0x4A, 0x44, 0x40,
	0x40, 0x44, 0x4A, 0x51, 0x40,
	0x00, 0x00, 0xFF, 0x01, 0x03,
	0xE0, 0x80, 0xFF, 0x00, 0x00,
	0x08, 0x08, 0x6B, 0x6B, 0x08,
	0x36, 0x12, 0x36, 0x24, 0x36,
	0x06, 0x0F, 0x09, 0x0F, 0x06,
	0x00, 0x00, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x00,
	0x30, 0x40, 0xFF, 0x01, 0x01,
	0x00, 0x1F, 0x01, 0x01, 0x1E,
	0x00, 0x19, 0x1D, 0x17, 0x12,
	0x00, 0x3C, 0x3C, 0x3C, 0x3C,
	0x00, 0x00, 0x00, 0x00, 0x00,
};

 BYTE Fn210;     // variable para saber de que son los datos brutos que llegan en Rx MiWi

 BYTE LEDc;       // color del Led del traje
 BYTE PWML;      // indica el duty del LED


//**************** VARIABLES DE Leer_SD *****************
 

 BIT     Fsync;      // Flag de Sincronismo de Tiempo Externo
 BIT     FSP;        // Flag de Segundo Patron
 BYTE    TPs;        // Tiempo Patron de Segundo
 BYTE    TP_S;       // Tiempo de Partida Segundos
 BYTE    TP_M;       // Minutos
 BYTE    TP_H;       // Hora

 BYTE    ModTP;



 uint16_t    DDx;       // registro de prueba del so
 uint8_t     DDy;       // registro de prueba del so


// ********* Blue Screen *********

 BYTE BSO ;
 BYTE BST ;


// ********* Comms *****************


 uint8_t VDi[25];//[12];
 uint8_t VDo[25];//[12];
 uint8_t VDoMW[20];//[12];
 uint8_t VDa[20];//[10];

 uint8_t    Address_MiWi;

 BIT FPMw;
 BIT FPU1;
 BIT FPU2;
 
 BIT FMDL1;  // Modulo de Luces 1 presente
 BIT FMDL2;  // Modulo de Luces 2 presente
 BIT FMDL3;  // Modulo de Luces 3 presente
 
 
 uint8_t LRx;
 uint8_t LRx_Aux;
 uint8_t NSECTx;
 uint8_t NSECRx;

 BIT FACK_W;     //Flag de ACK en Espera
 BIT FACK_TO;    //Flag de espera de ACK _ Time Out (Reenvia lo que tenga VTx)
 BIT FACK_P;     //Flag de envio de ACK _ Pendiente
 BIT FACK_T;     //Flag de ACK _ Transmitiendo
 BIT FACK_R;     //Flag de ACK _ Recibiendo

 BIT FRxU0;
 BIT FTxU0;


 uint8_t NRAC;
 uint8_t OTx;
 uint8_t VTMID[30];     //vector de conteo de timpos para limpiar los MIDs
 uint8_t VCR[30][10];   // Vector de chekeo de Redundancia [WNID][Hist_MID]
 uint8_t VGMID[30]; // Vector de generacion de MID 
 uint8_t LTx;
 uint8_t ORx;



#define LVCola   100
uint8_t VCola[LVCola+1];

uint8_t NRAC75 = 75;   // 75% de la cola
uint8_t NRAC50 = 50;   // 50% de la cola
uint8_t NRAC25 = 25;   // 25% de la cola
uint8_t NRACP = 0;      // Indicador del Porcentaje 0, 25, 50, 75





 uint8_t VTx[25];

 BYTE    TargetLongAddress[8];   // Direccionamiento NID


 uint8_t CBNSec;

// *******  FN autonoma

 BIT     FTMR3ACK;  //
 BIT     FmCRC;

 bit    FDU = 0;    // Flag de Debug de la Usart0

uint16_t    DINERO_Aux;


uint8_t     MDebug_Bu;

BIT     FBC_MiWi;

uint16_t   Debug_Time_ms;
uint8_t    Debug_Time_s;
uint8_t    Debug_Time_m;



#define NTA_MAX 800
char Debug_Text[NTA_MAX];

uint16_t NTA;    // Next Text Address
uint8_t CMP;    // Cantidad de Mensajes Perdidos

uint8_t NTS;    // Next Text to Show

uint8_t DTS_x;     // Posición X
uint8_t DTS_y;      // Posición Y
uint8_t DTS_x_m;     // Posición de X Maxima
uint8_t DTS_y_t;    // Valor Top de Y
uint8_t DTS_y_b;    // Valor Bottom de Y
uint8_t PCM;        // Próximo caracter a mostrar
BIT FBPA =0;

BIT         FHFA;             //Flag de habilitacion de Funcion Autonoma

// *************** Sistema de Menú Secreto *************
uint8_t Clave_Menu;     // para seguir la secuencia para entrar al menu secreto
uint8_t Menu_Index;



  //**************** VARIABLES DEL TOUCH *****************
  uint16_t  Cal_X;          //Vble de calibracion del touch Eje X
  uint16_t  Cal_Y;          //Vble de calibracion del touch Eje Y
  uint16_t  X_T;            //Vble global de coordena X del pto tocado
  uint16_t  Y_T;            //Vble global de coordena Y del pto tocado
  uint16_t Off_X;
  uint16_t Off_Y;
  BIT     FHT;        // Flag de habilitacion del Touch
  uint8_t  OTouch;      // Orden de Chekeo del Touch (sirve para estabilizar la medicion del presionado y como antirebote



// ********* DEBUG *********
uint8_t Bat;
BIT     FHMR;   //Flag de Habliitación de Monitor de Recursos
//Aca iria la RAM usada pero utilizamos el NRA directamente
uint16_t BAT_16;        //Vble de 16 mits para guardar el valor del CAD de la bateria
uint8_t RxMid[10];      // cuenta para chequear los MID de los mjs de la red Cehn, para evitar mjs duplicados.
uint8_t TxMid;          // MID al PAN;
BIT     FTPMID;
BIT     FTRMID;
uint8_t RTMID;
uint8_t TargetLongAddress[8];
uint16_t P_PPCDBx;
uint16_t P_PPx;
uint16_t P_PPc;


uint16_t   Debug_Time_ms;   //Milisegundos
uint8_t    Debug_Time_s;    //Segundos
uint8_t    Debug_Time_m;    //Minutos
uint8_t    Debug_Time_s_BU;     //Back ups de los tiempos para debug
uint8_t    Debug_Time_m_BU;


uint16_t NTA;    // Next Text Address
uint8_t CMP;    // Cantidad de Mensajes Perdidos

uint8_t NTS;    // Next Text to Show

uint8_t DTS_x;     // Posición X
uint8_t DTS_y;      // Posición Y
uint8_t DTS_x_m;     // Posición de X Maxima
uint8_t DTS_y_t;    // Valor Top de Y
uint8_t DTS_y_b;    // Valor Bottom de Y
uint8_t PCM;        // Próximo caracter a mostrar
BIT FBorradoNextRenglon; // Sirve para borrar el proximo renglon al terminar de escribir el actual

uint16_t Mains_Max;     //Limite maximo de Mains por segundo (GUARDADO EN EEPROM)
uint16_t Mains_Min;     //Limite minimo de Mains por segundo (GUARDADO EN EEPROM)
uint16_t Mains_Last;    //Mains recorridos en el último segundo
uint16_t Mains_Last_Aux;//Auxiliar para ir contando los Mains recorridos
uint8_t  Mains_Porc;    //Porcentaje del máximo de mains por segundo
uint8_t  RAM_Porc;      //Porcentaje libre de memoria RAM
uint16_t Bat_V;         //Tesión de la bateria
uint16_t Bat_P;         //Porcentaje de la batería
uint16_t Vble_Aux1;     //Vbles auxiliares para monitorizar en el monitor de recursos
uint16_t Vble_Aux2;     //se deben tocar en el F1s en la rutina tiempos

uint16_t Mains_Max_BU;     //Limite maximo de Mains por segundo (GUARDADO EN EEPROM)
uint16_t Mains_Min_BU;     //Limite minimo de Mains por segundo (GUARDADO EN EEPROM)
uint16_t Mains_Last_BU;    //Mains recorridos en el último segundo
uint8_t  Mains_Porc_BU;    //Porcentaje del máximo de mains por segundo
uint8_t  RAM_Porc_BU;      //Porcentaje libre de memoria RAM
uint16_t Bat_V_BU;         //Tesión de la bateria
uint8_t  Bat_P_BU;         //Porcentaje de la batería
uint16_t NRA_BU;           //Next Ram Address Back Up
uint16_t Vble_Aux1_BU;
uint16_t Vble_Aux2_BU;

BIT      FPVMR;            //Flag de Primera vez de Monitor de Recursos
BIT      FDCPUP;           //Flag de Dibujar CPU Porcentual
BIT      FDRAMP;           //Flag de Dibujar RAM Porcentual

uint8_t  DMRH_s;            // Debug Monitor de Recursos _ Segundos (0 a 59)
uint8_t  DMRH_CPU;          // Debug Monitor de Recursos _ CPU
uint8_t  DMRH_Tx;           // Debug Monitor de Recursos _ Tx/s
uint8_t  DMRH_Rx;           // Debug Monitor de Recursos _ Rx/s
#define  DMRH_x_i    108       // Debug Monitor de Recursos _ Posicion X inicial del Histograma
uint8_t     MR_Bottom;         //Base del monitor de recursos (coordenadas del eje y) "FNAF" Hacer que sea modificable via MIWI
BIT     FPing_Cehn;
uint8_t ID_Ping_Cehn;
uint8_t NID_Cehn;
BIT     FPing_Spam;
uint8_t ID_Ping_Spam;
uint8_t NID_Spam;

uint8_t NRA;
uint8_t LRAM_D;

BIT     FEMOSI1;        //Errores MOSI no puede haber pues yo trasmito sin feedback
BIT     FEMOSI2;
BIT     FEMISO1;        //Los errores MISO serán cuando el SPI/USART no me destrabe ni con su timer
BIT     FEMISO2;
BIT     FENACK;
BIT     FECRC;

BIT     FOVFT2;
BIT     FRxAct;
#endif  // DECLARACION DE VARIABLES

#if 1

//Chat Window Application Variables
#define MAX_MESSAGE_LEN 35
uint8_t TxMessage[MAX_MESSAGE_LEN];
uint8_t TxMessageSize = 0;


//Uses ReadMacAddress routine to give unique ID to each node
extern uint8_t myLongAddress[];
extern CONNECTION_ENTRY ConnectionTable[CONNECTION_SIZE];

//Chat Application status variables
bool MessagePending = FALSE; //Flag de enviar msj
bool TransmitPending = FALSE; //Flag de recibir msj


//Timers used in the application to determine 'no activity'
MIWI_TICK tickCurrent;
MIWI_TICK tickPrevious;


#if ADDITIONAL_NODE_ID_SIZE > 0
uint8_t AdditionalNodeID[ADDITIONAL_NODE_ID_SIZE] = {0x00};
#endif

#endif  // CONFIGURACION INICIAL DE MiWi

#if 1
//Direcciones de EEPROM
#define MAINS_MAX_ADDRESS   0
#define MAINS_MIN_ADDRESS   2
#define TOUCH_X_ADDRESS     4
#define TOUCH_Y_ADDRESS     6
#define OFF_X_ADDRESS       8
#define OFF_Y_ADDRESS      10

//Valores de Plantilla()
#define MENU_HOME       10
#define MENU_MONITOR    11
#define MENU_RASTREO  12
#define MENU_EXIT       13

#endif  // DEFINICION DE ETIQUETAS


#if 1

void Inicializar_Micro(void);
//void Inicializar_Audio(void);
void Inicializar_Valores(void);
void Inicializar_MiWi(void);
void Bootear(void);
void Spam(void);
void ChkFnMIWI(void);
void ChkRxIR(void);
void ChkArma(void);

void Tiempos(void);
void Beacon(void);

void Audio(uint8_t cmd);
void Disparar(void);

void RxAF(INPUT BYTE A);


void Admin_MANA(void);
void ChkTrigger(void);
bool DispA1T1(void);
bool DispA1T2(void);
bool DispA2T1(void);
bool DispA2T2(void);
bool DispA3T1(void);
bool DispA3T2(void);


//Rutinas de Display

void SetPointer(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void writeRegister16(uint16_t a, uint16_t d);
void writeRegister8(uint8_t a, uint8_t d);
void writeRegister32(uint8_t a, uint32_t d);
void write8(uint8_t a);
void WR_STROBE(void);
void setLR(void);
void WriteRegisterPair(uint8_t aH, uint8_t aL, uint16_t d);


void DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void Reset_Pantalla(void);
void Inicializar_Pantalla(void);
void SetAddrWindow(uint16_t x, uint16_t y, uint16_t A, uint16_t B);
void Flood(uint16_t color, uint32_t len);
void Llenar_Pantalla(uint16_t color);
void Linea_Horizontal(int16_t x, int16_t y, int16_t length, uint16_t color);
void Linea_Vertical(int16_t x, int16_t y, int16_t length, uint16_t color);
void Write(uint8_t c);
void Pong(void);
void DrawChar(uint16_t x, uint16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void DrawDigit(uint16_t x_d, uint16_t y_d, uint8_t c_d, uint16_t color_d, uint16_t bg_D, uint8_t size_d);
void DrawVble(uint16_t x_v, uint16_t y_v, uint16_t c_v, uint16_t c_v_BU, uint16_t color_v, uint16_t bg_v, uint8_t size_v, uint8_t bits);   //Fcion para dibujar Vbles.
void Plantilla(uint8_t Tipo);
void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void FloodDB(uint16_t color [36]);
void MOSTRAR_SD(INPUT BYTE Pos);
void CMD_SD(INPUT BYTE CMD,INPUT uint32_t ARG,INPUT BYTE ACRC,INPUT BYTE COND);
void ClrRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void DrawFastPixel(uint16_t color);

void DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);

void Write_Command(uint8_t c);
void Write_Data(uint8_t c);

void WT3(uint16_t Time);

//Rutinas de uSD
/*
uint8_t init_mmc();
uint8_t mmc_respuesta(unsigned char respuesta);
uint8_t SDCard_send_command(uint8_t cmd, uint32_t arg, uint8_t res);
*/
void Inicializar_SD();
void write_SD(uint32_t address);
void read_init_SD();
void Leer_SD();
void Read_RAW(INPUT uint32_t ADD_RD2);

void Reacomodar_Fn_Truncada();
void Fcion_SD();
void Fn_Autonoma();
uint8_t     SPIGetF(void);
uint24_t    Vble_D(INPUT BYTE ID);
uint24_t    Lim_Vble_D(INPUT BYTE ID);
void VD();
void D1(INPUT BYTE Num, uint16_t color);
void D1f(INPUT BYTE Num, uint16_t color);
void D2(INPUT BYTE Num, uint16_t color);
void D3(INPUT BYTE Num, uint16_t color);

void MiWiToSD(INPUT BIT FPVez);


void Chk_Touch(void);
void Calibrar_Touch(void);
void Chk_Botones(void);

void Baja_Fn_Autonoma(INPUT BYTE SlotB);
void Fn_Touch();

void Delay_Gif();

void Blue_Screen();
void SBS(INPUT BYTE Origen, INPUT BYTE Tipo);
/*
void REC_Audio(uint8_t Dir, uint8_t Len);
void ERASE_Audio(uint8_t Dir, uint8_t Len);
void PLAY_Audio(uint8_t Dir, uint8_t Len);
  */
uint8_t Invertir_Byte(uint8_t inv);

void Medir_Signal(void);
void Debug(void);

void MD(uint8_t NdD, uint16_t Num, uint16_t color, uint16_t X, uint16_t Y);

void Cola(uint8_t Puerto, INPUT BIT IO);

void Comms_Admin(void);
bool ChkRxU0(void);
void ChkMjs(void);
void DoTxMw(INPUT BYTE addr,INPUT BIT FBc);
void DoTxU0(void);
void DoTxU1(void);
void DoTxU2(void);
void DoTxMDL(uint8_t NMod);
bool ChkRxU1(void);
bool ChkRxU2(void);
bool ChkRxMw(void);
bool ChkNoRed(void);
void Encolar(void);
bool Dencolar(void);
void Protocolizador(void);


void Plantilla_Debug(void);
void Progreso_P(uint8_t Modo);


void PoolRxU0(void);
void PoolTxU0(void);
void Chk_BDebug(void);

void DTxt(uint16_t Color, uint8_t ToShow[45]);
void DTs(void);
void Monitor_Recursos(void);

void Chk_Touch(void);
void Calibrar_Touch(void);
void Menu_Interactivo(void);


#endif// DECLARACION DE SUBRUTINAS

void main(void) // PROGRAMA PRINCIPAL
{
    uint8_t i;
    Inicializar_Micro(); // CONFIGURA EL MICRO
    Inicializar_Valores(); // Valores por defecto necesarios
    DTxt(RED, "v0.4 Protocolo Estable al 17/9/17");
    Inicializar_Pantalla();
   // DTxt("Ejecutando Secuencia de Booteo...");
    //Bootear();
    DTxt(GREEN, "Inicio Completo, ejecutando el While(1) {...}");


    while (1) // Programa Principal  * * * NO OLVIDAR EL WDT * * *
    {
        Comms_Admin();
        Tiempos();
        Chk_Botones();
        DTs();
       // Menu_Interactivo();
    }
}


#if 1

void Inicializar_Micro()
{
// primary internal oscillator
    OSCCON = 0xF4;      //Habilito el oscilador interno en 16 MHz
    OSCTUNE = 0xC0;     //Habilito el PLL para tener 64 MHz => 16 MIPS

// WDT
    WDTCONbits.SWDTEN = 0;

// ** CONFIGURACION DE PUERTOS **
   TRISA = 0B01000100;         //Configuro como entrada el Bat_Input
   TRISB = 0B10111110;
   TRISC = 0B11010001;
   TRISD = 0B00000000;
   TRISE = 0B00001000;          //Nibble Mas significativo ya analizado
   LATA = 0;
   LATB = 0;
   LATC = 0;
   LATD = 0;
   LATE = 0;

//MiWi  -  RF CS, RESET, INT pin definitions
    PHY_CS_TRIS = 0;
    PHY_CS = 1;
    PHY_RESETn_TRIS = 0;        //Reset MIWI es salida
    PHY_RESETn = 1;             //Reset MIWI a uno
    RF_INT_TRIS = 1;
    PHY_WAKE_TRIS = 0;
    PHY_WAKE = 1;
    INTCON2bits.INTEDG1 = 0;    // enable INT effective edge
    RFIF = 0;
    RFIE = 1;

//Pantalla   -   Configuro los PINES de CTRL del DISPLAY
    DISP_CS_TRIS=0;                      //Salida
    DISP_CS=1;                           //Chip Select
    DISP_WR_TRIS=0;                      //Salida
    DISP_WR=1;                           //Write
    DISP_RD_TRIS=0;                      //Salida
    DISP_RD=1;                           //Read
    DISP_CD_TRIS=0;                      //Salida
    DISP_CD=1;                           //CD a 1 (data)
    DISP_RST_TRIS=0;                     //Salida
    DISP_RST=1;                          //Reset display a uno

// Touch
    TOUCH_CS_TRIS=0;                     //Defino como salida
    TOUCH_CS=1;

// SD
    SD_CS_TRIS = 0;
    SD_CS = 1;

//Con esta configuracion SPI funciona MIWI
    SSPSTAT = 0xC0;               //Configuracion SPI CKP=0; CKE=0
    SSPCON1 = 0x21;                 //Maxima velocidad para la pantalla (estaba a 21 )

// CONFIGURACION DE LA USART
    TXSTA = 0X64;
    RCSTA = 0XD0;
    BAUDCON = 0X08;     //
    SPBRGH = 0X00;
    SPBRG = 0X8A;   // 115200

    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;

// CONFIGURACION DEL TIMERS
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_8);
    WriteTimer0(0xB1DF); // USAR SOLO VALORES HEXAs
    OpenTimer3(TIMER_INT_ON & T3_16BIT_RW & T3_SOURCE_INT & T3_PS_1_8);
    CloseTimer3();
    PIE2bits.TMR3IE = 1; // interrupcion del Timer 1 habilitada
    OpenTimer2(TIMER_INT_ON & T2_PS_1_16 & T2_POST_1_2);        //Auxiliar del SPI (Escribir 0X05 para contar 500uSeg)
    PIE1bits.TMR2IE = 0;                                        //Interrupcion del Timer 2 deshabilitada

// Configuracion de las interrupciones y sus prioridades
    INTCONbits.GIEH = 1;    //Deshabilito interrupciones
    INTCONbits.GIEL = 1;    //Deshabilito interrupciones
    RCON = 0x80;
    IPR1 = 0B00110010;
    IPR2 = 0B00000010;
    INTCON2bits.TMR0IP = 1;
    INTCON3bits.INT1IP = 0;
}

void Inicializar_Valores() //inicializacion de las vbles que necesita cada una de las subrutinas
{
uint8_t i;


// Configuración básica del Repetidor

    LNID = 0;     // Local Node ID *** IDENTIFICADOR DE ESTE NODO ***
    FPU1 = 0;
    FPU2 = 0;
    FPMw = 1;
    FDU = 0;

    FMDL1 = 0;      // Modulo de Luces 1 (en Puerto 5)
    FMDL2 = 0;      // Modulo de Luces 2 (en Puerto 4)
    FMDL3 = 0;      // Modulo de Luces 3 (en Puerto 3)  //  * * * * No habilitar junto con Uart1 * * * *

    FEMOSI1 = 0;        //Errores MOSI no puede haber pues yo trasmito sin feedback
    FEMOSI2 = 0;
    FEMISO1 = 0;        //Los errores MISO serán cuando el SPI/USART no me destrabe ni con su timer
    FEMISO2 = 0;
    FENACK = 0;
    FECRC = 0;
    FOVFT2 = 0;
    NSECRx = 0;
    ORx =1;
    NRAC = 0;

#if 1

// Tiempos    
    T_LED = 5;          // 200ms - Tiempo que permanecen prendidos los LEDs de estado
    RMW = LNID - 130;   //Registro que definirá el delay en mS para transmitir por MiWi
    RMW = RMW * 4;    
    
//Chip Selects
    
    if(FPU1 == 1)
    {
        Uart1_CS=1;
    }

    if(FPU2 == 1)
    {
        Uart2_CS=1;
    }

    if(FMDL1 == 1) Mod5_CS = 1;     //Desactivo el Chip Select Modulo 1 de luces
    if(FMDL2 == 1) Mod4_CS = 1;     //Desactivo el Chip Select Modulo 2 de luces
    if(FMDL3 == 1) Mod3_CS = 1;     //Desactivo el Chip Select Modulo 3 de luces
    
    if(LNID == 0)
    {
        TXSTA = 0X24;
        RCSTA = 0X90;
    }

// Usart (U0 - Aguas Arribas)
    
    T1 = 0;
    T10 = 0;
    T100 = 0;

// Equipo

    EQUIPO = 1;

//  Tiempos Visualizables

    Fsync = 0;
    FSP = 0;
    TPs = 0;
    TP_S = TP_M = TP_H = 0;
    ModTP = 0;


    /* Definicion de la direccion de los Nodos*/
    TargetLongAddress[1] = EUI_1;
    TargetLongAddress[2] = EUI_2;
    TargetLongAddress[3] = EUI_3;
    TargetLongAddress[4] = EUI_4;
    TargetLongAddress[5] = EUI_5;
    TargetLongAddress[6] = EUI_6;
    TargetLongAddress[7] = EUI_7;
    

// Inicializacion de prueba para Test de Display

                DDx = 0;
                DDy = 120;

                LEDc = 62;
                LED_R  = 0;
                LED_G  = 0;
                LED_B  = 0;


// Botones
                TARB = 5;  // Tiempo AntiRebote para Botones



// Sistema de Debug
                DTS_y_t = 240;
                DTS_y_b = 0;
                NTA = 0;
                DTS_y = DTS_y_t - 10;
                CMP = 0;
                DTS_x_m = 0;

#endif // Definiciones fijas

}

void Inicializar_MiWi() //inicializacion del Modulo MRF, y coneccion a la RED del juego
{
uint8_t i;

    if (!MiApp_ProtocolInit(false))
    {
       // Display(1, 1); // ERROR MiWi - NO SE PUDO INICIALIZAR EL PROTOCOLO
    }

    if (MiApp_SetChannel(myChannel) == false)
    {
       // Display(1, 2); // ERROR MiWi - NO SE PUDO SETEAR EL CANAL RF
    }

    MiApp_ConnectionMode(ENABLE_ALL_CONN); // HABILITANDO CONECCION...

    if(!MiApp_StartConnection(START_CONN_DIRECT, 0, 0))
    {
        //Error
    }
    /*i = 0xFF;
    while (i == 0xFF)
    {
        i = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);
        if (i == 0xFF)
        {
           // Display(1, 3); // ERROR MiWi - NO CONECTADO - BUSCANDO...
        } // (ES "BUSCANDO..." PORQUE NO SALE DEL WHILE HASTA QUE NO SE CONECTA)
    }*/
}

void Bootear() // Procedimiento de Booteo
{
uint8_t i;
    for (i = 0; i < 16; i++)
    {
        LED_PAN = i;
        LED_MiWi = (i>>1);
        LED_U1 = (i>>2);
        LED_U2 = (i>>3);
        delay_ms (50);
    }
}

#endif  // Inicializacion



#if 1

void Comms_Admin()
{
    BIT     FBC;
    uint8_t i, Aux;

    if(ChkRxU0())   // <-- Usart 0 / Llegaron Datos de Aguas arriba ?
    {
#if 1



#if 0
            if(FBC == 1)             //Si es Broadcast debo utilizar otros valores de MID 201 - 255
            {
                VGMID[0]++;
                if(VGMID[0] < 200)  //Utilizare los valores 201-255
                {
                    VGMID[0] = 201;
                }
                for(i = 11; i > 2; i--) // Insersion del MID en el VDi
                {
                    VDi[i] = VDi[i-1]; // Se mueven los datos (+1) para poder insertar MID.
                }
                VDi[2] = VGMID[0];
                VDi[0] ++;              // Debo incrementar el largo del Mensaje
            }
            else
            {
                if((VDi[1] < 127)&&(VDi[1] != 0))  // Verifico si el mesaje es para un nodo inalambrico.
                {
                    VGMID[VDi[1]]++;    // incremento el MID para ese NID.
                    if(VGMID[VDi[1]] == 200)
                    {
                        VGMID[VDi[1]] = 1;    // incremento nuevamente el MID para ese NID. (no es valido un MID = 0.
                    }
                    for(i = 11; i > 2; i--) // Insersion del MID en el VDi
                    {
                        VDi[i] = VDi[i-1]; // Se mueven los datos (+1) para poder insertar MID.
                    }
                    VDi[2] = VGMID[VDi[1]];
                    VDi[0] ++;              // Debo incrementar el largo del Mensaje
                }
            }

#endif  // Generacion de MID

#if 0
        if(FPMw == 1) // Verifico si esta presente el Puerto MiWi
        {
            for(i = 0; i < 10; i++)
            {
                VDoMW[i] = VDi[i+2]; // Se cargan los Datos para Enviarlos por MiWi
            }
            if(LNID == 0)
            {
                FTRRMW = 1;         //El PAN no tiene delay para enviar por MiWi
            }
            else
            {
                FTPRMW = 1;         //Flag de tiempo pregunta retardo de MIWI
            }
            Address_MiWi = VDi[1];
            FBC_MiWi = FBC;
        }

#endif  // MiWi Tx

#endif         // Analisis del Mensaje
    }

#if 0
    if(ChkRxMw())   // <-- MiWi / Consulta si llegaron Datos de algun Nodo por el MRF
    {
        if(ChkNoRed())  // Chekea la No Redundancia del mjs, para ver si no se habia enviado ya ese mjs
        {
//             DTxt("Recibi algo Por MiWi");
             Encolar();  // Debo retransmitir al PAN
        }
    }
#endif// MiWi Rx

    DoTxU0();
}

bool ChkRxU0()
{
    BYTE i;
    BYTE Aux;

       if(FRxU0 == 0)     //Se recibió un dato nuevo?
       {
           return false;      // no hay que generar ningun msj y no llego nada nuevo
       }
       FRxU0 = 0;
       DTxt(CYAN, "Analizando Dato...");
#if 1

       for(i=0; i<23; i++)
       {
         //  D1(VRx[i], BLUE);
       }
       if(VRx[0] == 0) // Verifico si es un  ACK
       {
#if 1
          DTxt(CYAN, "ACK Detectado, verificando CRC...");
           if(VRx[2] == VRx[1])         // Verifica CRC
           {
               DTxt(GREEN, "CRC OK!, verificando Nro de Sec...");
               if(NSECTx == 0)
               {
                   Aux = 255;
               }
               else
               {
                   Aux = NSECTx -1;
               }
                if(VRx[1] == Aux)  // Nro de secuencia del ACK está bien? (con respecto al msj enviado)
                {
                   DTxt(GREEN, "Nro de Sec OK!!! - ACK OK!");
                    FACK_W = 0;      // Dejo de esperar el ACK
                    CloseTimer3();     // apaga el timer, ya que llego todo bien
                }
                else
                {
                    DTxt(RED, "Ups !  Nro de Sec MAL!!! Enviando NACK");
                    FACK_TO = 1;     // N° de SEc mal, pido reenviar el ultimo msj
                }
           }
           else
           {
               DTxt(RED, "Ups! CRC MAL!!! Enviando NACK");
               FACK_TO = 1;     // N° de SEc mal, pido reenviar el ultimo msj
           }
#endif  //  **** Verificacion del ACK
           return false;      // no hay que generar ningun msj y no llego nada nuevo
       }
       else
       {
           DTxt(CYAN, "Dato Detectado, verificando CRC...");
#if 1
           CRC_Rx = 0;
           CRC_Rx = VRx[20];
           CRC_Rx = CRC_Rx << 8;
           CRC_Rx = CRC_Rx + VRx[19];
          
           CRC_Mk = 0;

           for( i = 0 ; i < 19 ; i++)
           {
               CRC_Mk = CRC_Mk + VRx[i]; // Sumo la cantidad de Bytes variables
           }
           if(CRC_Mk != CRC_Rx)     //Si no esta bn el control de CRC
           {
               DTxt(RED, "Ups! CRC MAL!!!");
               FACK_P = 1;             //Activo el envío de  NACK al otro extremo
               return false;
           }
           DTxt(GREEN, "CRC OK!!!, verificando Nro de Sec...");
#endif  //  **** Verificacion del CRC Data
       }

#endif  //  **** Verificacion del CRC


#if 1
       Aux = NSECRx;
      // D1(NSECRx, GREEN);
        if(VRx[1] != (Aux ))    // El numero de secuencia esta bien?
        {
            DTxt(RED, "Ups! Nro de Sec MAL!!!");
            // El numero de secuencia es erroneo, dato no confiable. Envio un ACK con el ultimo numero de secuencia recibido correctamente
            if(CBNSec == 3)
            {
                // Por cansacio, vamos a tomar como valido este N° de Secuencia
                NSECRx = VRx[1];
                goto NSecOK;
            }
            else
            {
                FACK_P = 1;             //Activo el envío de  NACK al otro extremo
                CBNSec ++;
                return false;
            }
        }
        else
        {
            DTxt(GREEN, "Nro de Sec OK!!!");
            // El numero de secuencia esta bien, dato confiable.
NSecOK:     NSECRx ++;    //Actualizo el  Nro de Secuencia
            FACK_P = 1;         //Activo el envío de  ACK al otro extremo
            CBNSec = 0;
#endif  //  **** Verificacion del Numero de Secuencia


            VDi[0] = VRx[0];
            for(i = 1; i < 19; i++)
            {
                VDi[i] = VRx[i+1]; // Se cargan los Datos Recibidos
            }
            return true;
        }
}

void DoTxU0()
{
   if(FTxU0 == 1)  // Verifico si se esta transmitiendo algo
   {
       return;     //se esta transmitiendo, no puedo hacer nada
   }
   FTxU0 = 1;  // indico que voy a realizar una transmision
   if(FACK_P == 1)      // HAy una ACK pendiente?
   {
       FACK_P = 0;
       TXREG = 0;   // Envio el 1° byte, el resto sera por Interrupcion
       //DTxt("Byte 1 de un ACK enviado");
       OTx = 1;
       LTx = 0;
       FACK_T = 1;  // indico que voy a estar enviando un ACK
       PIE1bits.TXIE = 1; // Habilito interrupciones de TX Usart
       return;
   }
   FTxU0 = 0;  // indico que voy a realizar una transmision
   FTxU0 = 1;
   if(FACK_TO == 1)    // Verifico si hubo un TimeOut por un ACK que nunca llego, se debe retransmitir
   {
       FACK_TO = 0;
       TXREG = VTx[0];             // Se retransmite el ultimo Mjs
       OTx = 1;
       LTx = VTx[0] & 0x0F;  // Genero el Largo del Msj
       PIE1bits.TXIE = 1;          // Habilito interrupciones de TX Usart
       return;
   }

   FTxU0 = 0;  // indico que voy a realizar una transmision
   FTxU0 = 1;

   if(FACK_W == 1)
   {
       //FACK_W = 0; // Borrar !!!! FNAF
       FTxU0 = 0;  // Borro, porque no se esta por hacer una transmision
       return; // Si estoy esperando un ACK, no puedo enviar un msj nuevo
   }

   FTxU0 = 0;  // indico que voy a realizar una transmision
   FTxU0 = 1;

   if(Dencolar())
   {
       Protocolizador();   // Esta rutina arma el mjs para ser enviado por RS232 y inicia el proceso de enviado.
       //VTx[1] = 1;
//       for(uint8_t i=0; i<23; i++)
//       {
//            D1(VTx[i], RED);
//       }
//       while(Boton_4 == 0)
//       {
//
//       }
       TXREG = VTx[0]; // Envio el 1° byte, el resto sera por Interrupcion
       //DTxt("Byte 1 de un Dato enviado");

       OTx = 1;
       FACK_W = 1;  // Indico que voy a estar esperando un ACK

       PIE1bits.TXIE = 1; // Habilito interrupciones de TX Usart
       return;
   }
    FTxU0 = 0;  // Borro, porque no se esta por hacer una transmision

}

void Encolar()
{                       // Hay que Guardar un dato en la Cola
    uint8_t i;
    
    if((LVCola - NRAC) >= 18)     // Verifico si entra en Memoria 18 (bytes son necesarios)
    {
        for(i = 0; i <= 17; i++)
        {
            VCola[NRAC+i] = VDi[i]; // Se cargan los Datos para Encolarlos
        }
        NRAC = NRAC + 18;  // Dejamos el NRAC apuntando al proximo lugar libre en el VCola
    }
    else
    {
        // Logica en caso de que no haya mas lugar para encolar
    }
}

bool Dencolar()
{
    uint8_t i, Aux;

    if(NRAC == 0) // Verifico si hay datos encolados
    {
        return false;   // No hay datos en cola
    }
    else
    {
        for(i = 0; i <= 17; i++)
        {
            VDo[i] = VCola[i]; // Se cargan los Datos para Dencolarlos
        }

        // Los datos solicitados estan disponible ya en VDo, ahora hay que desfragmentar Vcola, asi se libera espacio
        NRAC = NRAC - 18;  // Genero el nuevo NRAC que quedara despues de desocpar la memoria con este mjs

        if(NRAC != 0)   // Verifico si hay otros Mjs encolados
        {
            // Si hay mas datos, debo desfragmentar
            for(i = 0; i < NRAC; i++)    // Se desfragmenta solo lo ocupado
            {
                VCola[i] = VCola[18+i];
            }
        }
        return true; // Termina el proceso con los datos en VDo
    }
}

void Protocolizador()       //Esta rutina toca SI O SI VTx. Se debe tener extremo cuidado a la hora de llamarla
{                           //Debo dejar el msj en VTx, listo para salir por UART
   uint8_t i;
   uint16_t CRC_16;

   VTx[0] = VDo[0];            //Dejo NID donde estaba

   for(i = 1; i < 20; i++)     //Dejo el espacio para NSeq
   {
       VTx[i+1] = VDo[i];
   }

   VTx[1] = NSECTx;
   NSECTx++;
   CRC_16 = 0;
   for(i = 0; i <19; i++)
   {
       CRC_16 = CRC_16 + VTx[i];  //  Armo el CRC
   }
   VTx[19] = CRC_16 & 0X00FF;      //Almaceno la parte baja del CRC,
   CRC_16 = CRC_16 >> 8;           //lo hago rotar
   VTx[20] = CRC_16 & 0X00FF;      //y almaceno la parte alta
   VTx[21] = 0xFF;
   VTx[22] = 0xFE;
   return;
}

void WT3(uint16_t Time)
{
    WriteTimer3(Time);
    OpenTimer3(TIMER_INT_ON & T3_16BIT_RW & T3_SOURCE_INT & T3_PS_1_8);
    if (Time == TACK)       //Si estoy escribiendo TACK en el TMR lo indicare con el flag
    {
        FTMR3ACK = 1;
    }
    else
    {
        FTMR3ACK = 0;
    }
}

#endif // Comms Caller


#if 1

void ChkFnMIWI()
{
    uint8_t  i;
    switch (FN)
    {

#if 1
        case(0): //CONFIGURACIONES GENERALES
            switch (DATA)
            {
                
                case(0): // PING (se devuelven los mismos valores recibidos)
                    D1(4, RED);
                    VDi[0] = 132;   // TM:2 , L:4
                    VDi[1] = LNID;   
                    VDi[2] = FN;
                    VDi[3] = DATA;
                    VDi[4] = DAUX;
                    Encolar(); // Se deja aqui para enviar al PAN
                    D1(5, RED);
                break;

                case(1): // RESTET (se reinicia el traje a los valores predeterminados)
                    RESET();
                break;

            }
            break;

#endif// configuraciones Generales (0 y FN´s de pruebas)

#if 1
        case(80): // Todos los Modulos
            
            DoTxMDL(1);      // SubPrograma para enviar los datos a los modulos de luces
            DoTxMDL(2);      // SubPrograma para enviar los datos a los modulos de luces
            DoTxMDL(3);      // SubPrograma para enviar los datos a los modulos de luces

          break;

        case(81): // Enviar al modulo 1 de Luces
            for(i=0; i<9; i++)
            {
                VDa[i] = VDi[i+1];
            }
            DoTxMDL(1);      // SubPrograma para enviar los datos a los modulos de luces

          break;

        case(82): // Enviar al modulo 2 de Luces
            for(i=0; i<9; i++)
            {
                VDa[i] = VDi[i+1];
            }
            DoTxMDL(2);      // SubPrograma para enviar los datos a los modulos de luces
          break;

        case(83): // Enviar al modulo 3 de Luces
            for(i=0; i<9; i++)
            {
                VDa[i] = VDi[i+1];
            }
            DoTxMDL(3);      // SubPrograma para enviar los datos a los modulos de luces
          break;

        case(84): 
                VDa[0] = 1;   // TM:2 , L:4
                VDa[1] = 0;
                VDa[2] = 0;
                VDa[3] = 0;
                DoTxMDL(1);      // SubPrograma para enviar los datos a los modulos de luces
          break;



#endif// Modulo de Luces ( 80- 100 )

    }

}


#endif // MiWi


#if 1

void Chk_Botones()
{
    uint8_t i;

#if 1
#if 1
    if(Boton_1 == 1)
    {
        FBPA = 1;       //Debug
        if(FB1P == 0)
        {
            if(FTPB1 == 1)
            {
                if(FTRB1 == 1)
                {
                    FB1P = 1;       // indico que se tomo el presionado del boton
                    FTPB1 = 0;
                    FTRB1 = 0;      // Reseteo el Timer.
                }
            }
            else
            {
                FTPB1 = 1;
#endif
#if 1
                if(FRxAct == 0)
                {
                    FRxAct = 1;
                    DTxt(WHITE, "Int Activa");
                    PIE1bits.RCIE = 1;
                }
                else
                {
                    FRxAct = 0;
                    DTxt(WHITE, "Int Desactivada");
                    PIE1bits.RCIE = 0;
                }
                

#endif                          //      < -------- Accion del boton 1
#if 1
            }
        }
    }
    else
    {
        if(FB1P == 1)
        {
            FB1P = 0;       // indico que ya se soltó
        }
        else
        {
            if(FTRB1 == 1)  // si el tiempo termino y no estaba presionado el boton.
            {
                FTPB1 = 0;  // reseteo la pregunta
                FTRB1 = 0;
            }
        }
    }
#endif
#endif  // Boton 1

#if 1
#if 1
    if(Boton_2 == 1)
    {
        if(FB2P == 0)
        {
            if(FTPB2 == 1)
            {
                if(FTRB2 == 1)
                {
                    FB2P = 1;       // indico que se tomo el presionado del boton
                    FTPB2 = 0;
                    FTRB2 = 0;      // Reseteo el Timer.
                }
            }
            else
            {
                FTPB2 = 1;
#endif
#if 1
//                VDi[0] = 2; // NID
//                VDi[1] = 1; // FN
//                VDi[2] = 2; // Data
//                for(i=3; i<19; i++)
//                {
//                    VDi[i]=0;
//                }
//                Encolar();
//                DTxt(BLUE, "Boton 2 Presionado");
                if(FRxAct == 0)
                {
                    FRxAct = 1;
                    DTxt(WHITE, "Int Activa");
                    PIE1bits.RCIE = 1;
                }
                else
                {
                    FRxAct = 0;
                    DTxt(WHITE, "Int Desactivada");
                    PIE1bits.RCIE = 0;
                }

#endif                          //      < -------- Accion del boton 2
#if 1
            }
        }
    }
    else
    {
        if(FB2P == 1)
        {
            FB2P = 0;       // indico que ya se soltó
        }
        else
        {
            if(FTRB2 == 1)  // si el tiempo termino y no estaba presionado el boton.
            {
                FTPB2 = 0;  // reseteo la pregunta
                FTRB2 = 0;
            }
        }
    }
#endif
#endif  // Boton 2



//    if(Boton_1 == 1)
//    {
//                VDi[0] = 3; // NID
//                VDi[1] = 1; // FN
//                VDi[2] = 3; // Data
//                for(i=3; i<19; i++)
//                {
//                    VDi[i]=0;
//                }
//                Encolar();
//    }
    
#if 1
#if 1
    if(Boton_3 == 1)
    {
        if(FB3P == 0)
        {
            if(FTPB3 == 1)
            {
                if(FTRB3 == 1)
                {
                    FB3P = 1;       // indico que se tomo el presionado del boton
                    FTPB3 = 0;
                    FTRB3 = 0;      // Reseteo el Timer.
                }
            }
            else
            {
                FTPB3 = 1;
#endif
#if 1
                VDi[0] = 3; // NID
                VDi[1] = 1; // FN
                VDi[2] = 3; // Data
                for(i=3; i<19; i++)
                {
                    VDi[i]=0;
                }
                Encolar();
                DTxt(BLUE, "Boton 3 Presionado");
#endif                          //      < -------- Accion del boton 3
#if 1
            }
        }
    }
    else
    {
        if(FB3P == 1)
        {
            FB3P = 0;       // indico que ya se soltó
        }
        else
        {
            if(FTRB3 == 1)  // si el tiempo termino y no estaba presionado el boton.
            {
                FTPB3 = 0;  // reseteo la pregunta
                FTRB3 = 0;
            }
        }
    }
#endif
#endif  // Boton 3

#if 1
#if 1
    if(Boton_4 == 1)
    {
        if(FB4P == 0)
        {
            if(FTPB4 == 1)
            {
                if(FTRB4 == 1)
                {
                    FB4P = 1;       // indico que se tomo el presionado del boton
                    FTPB4 = 0;
                    FTRB4 = 0;      // Reseteo el Timer.
                }
            }
            else
            {
                FTPB4 = 1;
#endif
#if 1
                VDi[0] = 4; // NID
                VDi[1] = 1; // FN
                VDi[2] = 1; // Data
                for(i=3; i<19; i++)
                {
                    VDi[i]=0;
                }
                Encolar();
                DTxt(BLUE, "Boton 4 Presionado");
                Reset();
#endif                          //      < -------- Accion del boton 4
#if 1
            }
        }
    }
    else
    {
        if(FB4P == 1)
        {
            FB4P = 0;       // indico que ya se soltó
        }
        else
        {
            if(FTRB4 == 1)  // si el tiempo termino y no estaba presionado el boton.
            {
                FTPB4 = 0;  // reseteo la pregunta
                FTRB4 = 0;
            }
        }
    }
#endif
#endif  // Boton 4

}

void Chk_Touch()
{
    uint8_t i, z1, z2;
    X_T = 0;
    Y_T = 0;
    if(F100ms == 1)
    {
        switch(OTouch)
        {
            case(0):
            {
                RFIE = 0;
                TOUCH_CS=0;
                SPIPut(0xBB);               //z1
                z1 = SPIGet();
                i = SPIGet();

                SPIPut(0xCB);               //z2
                z2 = SPIGet();
                i = SPIGet();
                TOUCH_CS=1;
                RFIE = 1;

                if ((z1 < 5) | (z2 > 110))      //Verifico si se presionó algo
                {
                    X_T = 0;
                    Y_T = 0;
                    return;
                }
                OTouch = 1;     // Se presionó algo, vamos a esperar que se estabilice
                break;
            }

            case(1):
            {
                RFIE = 0;
                TOUCH_CS=0;

                SPIPut(0x9B);                   //X
                X_T = SPIGet();
                i = SPIGet();

                SPIPut(0xDB);                   //Y
                Y_T = SPIGet();
                i = SPIGet();

                TOUCH_CS=1;
                RFIE = 1;
                X_T = X_T * Cal_X;
                X_T = X_T / 100;
                Y_T = Y_T * Cal_Y;
                Y_T = Y_T / 100;
                Y_T = Y_T - Off_Y;
                Y_T = 240 - Y_T;
                X_T = X_T - Off_X;
                OTouch = 2;     // Ya obtuve las coordenadas, vamos a esperar 100 ms mas para el antirebote
                break;
            }

            case(2):
            {
                OTouch = 0;     // Ya ta, volvemos a empezar
                break;
            }
        }


    }
}

void Calibrar_Touch()
{
    uint8_t i, z1, z2, Aux_8;
    uint16_t Xii, Xsi, Xid, Xsd, Yii, Ysi, Yid, Ysd;
    FHFA = 0;
    RFIE = 0;                       //Deshabilito la interrupción de MIWI
    Llenar_Pantalla(BLACK);         //Esquina Inferior Izquierda
    Linea_Vertical(5, 15, 20, WHITE);
    Linea_Horizontal(15, 5, 20, WHITE);
    TOUCH_CS=0;             //Habilito el Touch
    z1 = 0;
    z2 = 127;
    while ((z1 < 5) | (z2 > 110))
    {
        SPIPut(0xBB);               //z1
        z1 = SPIGet();
        i = SPIGet();
        SPIPut(0xCB);               //z2
        z2 = SPIGet();
        i = SPIGet();
    }
    delay_ms(50);
    SPIPut(0x9B);                   //Xii
    Xii = SPIGet();
    i = SPIGet();
    SPIPut(0xDB);                   //Yii
    Yii = SPIGet();
    i = SPIGet();
    TOUCH_CS=1;             //Deshabilito el Touch
    delay_ms(200);

    Llenar_Pantalla(BLACK);         //Esquina Superior Izquierda
    Linea_Horizontal(225, 5, 20, WHITE);
    Linea_Vertical(215, 15, 20, WHITE);
    TOUCH_CS=0;             //Habilito el Touch
    z1 = 0;
    z2 = 127;
    while ((z1 < 5) | (z2 > 110))
    {
        SPIPut(0xBB);               //z1
        z1 = SPIGet();
        i = SPIGet();
        SPIPut(0xCB);               //z2
        z2 = SPIGet();
        i = SPIGet();
    }
    delay_ms(50);
    SPIPut(0x9B);                   //Xii
    Xsi = SPIGet();
    i = SPIGet();
    SPIPut(0xDB);                   //Yii
    Ysi = SPIGet();
    i = SPIGet();
    TOUCH_CS=1;             //Deshabilito el Touch
    delay_ms(200);

    Llenar_Pantalla(BLACK);         //Esquina Inferior Derecha
    Linea_Horizontal(15, 295, 20, WHITE);
    Linea_Vertical(5, 305, 20, WHITE);
    TOUCH_CS=0;             //Habilito el Touch
    z1 = 0;
    z2 = 127;
    while ((z1 < 5) | (z2 > 110))
    {
        SPIPut(0xBB);               //z1
        z1 = SPIGet();
        i = SPIGet();
        SPIPut(0xCB);               //z2
        z2 = SPIGet();
        i = SPIGet();
    }
    delay_ms(50);
    SPIPut(0x9B);                   //Xii
    Xid = SPIGet();
    i = SPIGet();
    SPIPut(0xDB);                   //Yii
    Yid = SPIGet();
    i = SPIGet();
    TOUCH_CS=1;             //Deshabilito el Touch
    delay_ms(200);

    Llenar_Pantalla(BLACK);         //Esquina Superior Derecha
    Linea_Horizontal(225, 295, 20, WHITE);
    Linea_Vertical(215, 305, 20, WHITE);
    TOUCH_CS=0;             //Habilito el Touch
    z1 = 0;
    z2 = 127;
    while ((z1 < 5) | (z2 > 110))
    {
        SPIPut(0xBB);               //z1
        z1 = SPIGet();
        i = SPIGet();
        SPIPut(0xCB);               //z2
        z2 = SPIGet();
        i = SPIGet();
    }
    delay_ms(50);
    SPIPut(0x9B);                   //Xii
    Xsd = SPIGet();
    i = SPIGet();
    SPIPut(0xDB);                   //Yii
    Ysd = SPIGet();
    i = SPIGet();
    TOUCH_CS=1;             //Deshabilito el Touch
    delay_ms(200);

    Xii=Xii+Xsi;            //Calculo el factor de correccion en X
    Xii=Xii/2;
    Off_X = 2 * Xii;
    Xid=Xid+Xsd;
    Xid=Xid/2;
    Xii=Xid-Xii;
    Cal_X=29000/Xii;

    Yii=Yii+Yid;            //Calculo el factor de correccion en X
    Yii=Yii/2;
    Ysi=Ysi+Ysd;
    Ysi=Ysi/2;
    Off_Y = 2 * Ysi;
    Yii=Yii-Ysi;
    Cal_Y=22000/Yii;

        Aux_8 = Cal_X >> 8;
        eeprom_write(TOUCH_X_ADDRESS, Aux_8);    //Almaceno el MSB
        Aux_8 = Cal_X;
        eeprom_write(TOUCH_X_ADDRESS + 1, Aux_8);    //Almaceno el LSB
        Aux_8 = Cal_Y >> 8;
        eeprom_write(TOUCH_Y_ADDRESS, Aux_8);    //Almaceno el MSB
        Aux_8 = Cal_Y;
        eeprom_write(TOUCH_Y_ADDRESS + 1, Aux_8);    //Almaceno el LSB
        Aux_8 = Off_X >> 8;
        eeprom_write(OFF_X_ADDRESS, Aux_8);    //Almaceno el MSB
        Aux_8 = Off_X;
        eeprom_write(OFF_X_ADDRESS + 1, Aux_8);    //Almaceno el LSB
        Aux_8 = Off_Y >> 8;
        eeprom_write(OFF_Y_ADDRESS, Aux_8);    //Almaceno el MSB
        Aux_8 = Off_Y;
        eeprom_write(OFF_Y_ADDRESS + 1, Aux_8);    //Almaceno el LSB


    Plantilla(3);
    FHFA = 1;
    while(0)
    {
        Tiempos();
        DTs();
        //F100ms=1;
        //Fn_Autonoma();
        Chk_Touch();
        FillCircle(Y_T, X_T, 5, GREEN);
        Monitor_Recursos();
    }
    Llenar_Pantalla(BLACK);
    RFIE = 1;
}


#endif // Touch


#if 1

void interrupt high_priority nuestra()
{
    BYTE i, Aux, mCRC;
    
#if 1
    if (INTCONbits.TMR0IF == 1) // Timer0 - Conteo de Tiempos internos / LED PWM
    {
        WriteTimer0(0xF82F); // 1 ms para 64 Mhz
        INTCONbits.TMR0IF = 0;

#if 0
        PWML++;
        if(PWML == 4)
        {
            PWML = 1;
        }
        i = LEDc & 0x03; // tomo los 2 bytes del Azul
        if (PWML <= i)
        {
            LED_B = 0;
        }
        else
        {
            LED_B = 1;
        }
        i = (LEDc>>2) & 0x03; // tomo los 2 bytes del Verde
        if (PWML <= i)
        {
            LED_G = 0;
        }
        else
        {
            LED_G = 1;
        }
        i = (LEDc>>4) & 0x03; // tomo los 2 bytes del Rojo
        if (PWML <= i)
        {
            LED_R = 0;
        }
        else
        {
            LED_R = 1;
        }
#endif // PWM para el LED RGB

#if 1
        F1msp = 1;
        T10 ++;
        if (T10 == 10)
        {
            
            T10 = 0;
            R10msp ++;
            T100 ++;
            if (T100 == 10)
            {
              T100 = 0;
              R100msp ++;
              T1 ++;
              if (T1 == 10)
              {
                  T1 = 0;
                  R1sp ++;
              }
            }
        }
#endif // Tiempo Patron

#if 1
      Debug_Time_ms ++;
      if(Debug_Time_ms == 1000)
      {
          Debug_Time_ms = 0;
          Debug_Time_s ++;
          if(Debug_Time_s == 60)
          {
              Debug_Time_s = 0;
              Debug_Time_m ++;
              if(Debug_Time_m == 100)
              {
                  Debug_Time_s = 0;
              }
          }
      }
#endif  // Tiempo para debugueo seg y mseg

    }
#endif // Timer 0 - Conteo de Tiempos internos / LED PWM

#if 1
    if (TMR2IF == 1 && TMR2IE == 1) // Timer0 - Conteo de Tiempos internos / LED PWM
    {
        TMR2IF = 0;
        FOVFT2 = 1; //Aca deberé ver que hacer si me interrumpe el TMR2
    }
#endif  // Timer 2 - Auxiliar de SPI

#if 1
    if (PIR1bits.TXIF == 1 && PIE1bits.TXIE == 1) // USART/TX - Envios de Mensajes al PAN
    {
        if(FACK_T == 1)         // Pregunto si se debe transmitir un ACK
        {                       //Hay que transmitir un ACK
            switch (OTx)        // Registro de Orden de Transmision , indica el proximo byte a transmitir
            {
                case(1):        // 2° Byte      El Nro de Seq del ACK
                {
                    TXREG = NSECRx-1;
                    break;
                }
                case(2):        // 3° Byte      El CRC que es igual al Nro de Seq
                {
                    TXREG = NSECRx-1;
                    break;
                }
                case(3):        // 4° Byte      Los dos bytes de fin de msj
                {
                    TXREG = 255;
                    break;
                }
                case(4):        // 5° Byte
                {
                    TXREG = 254;
                    OTx = 1;            //Pongo el Orden a "0" para que al salir termine en "1"
                    FTxU0 = 0;          // Indico que termina la trasnmision
                    PIE1bits.TXIE = 0;  // Apago interrupciones de USART Tx
                    FACK_T = 0;
                    DTxt(PINK, "ACK Enviado");
                    break;
                }
            }
        }
        else
        {
            TXREG = VTx[OTx];     // no es un ACK, es un dato
        }
        if(OTx == 22)       //Si es 24 acabo de terminar de mandar un msj, si es 1 acabo de terminar de mandar un ACK
        {
            DTxt(PINK, "Dato Enviado");
            OTx = 0;            //Pongo el Orden a "0" para que al salir termine en "1"
            FTxU0 = 0;          // Indico que termina la trasnmision
            if(FACK_W == 1)     // Pregunto si estaba esperando un ACK
            {
                WT3(TACK);      // Iniciamos el Timer esperando el ACK   FNAF
            }
            PIE1bits.TXIE = 0;  // Apago interrupciones de USART Tx
        }
        OTx ++;
    }

#endif// USART/TX - Envios de Mensajes al PAN

#if 1
    if (PIR1bits.RCIF == 1) // USART/RX - Recepcion de Mensajes del PAN
    {
        VRx_Aux[ORx] = RCREG;
        WT3(TRx); // Iniciamos el Timer para resetear la recepcion si no llega el byte que sigue
        if(ORx == 1)    // verifico si es el primer byte 
        {
            if(VRx_Aux[ORx] == 0)   // verifico si es un ACK
            {
                //DTxt("Byte 1 de un ACK Recibido");
                FACK_R = 1;
            }
            else
            {
               // DTxt("Byte 1 de un Mensaje Recibido");
                FACK_R = 0;
            }
        }
        if(((ORx == 5) && (FACK_R == 1)) || (ORx == 23) )    // verifico si llegue al fin de un ACK O un dato comun
        {
            
            FRxU0 = 1;         // indica que ya se recibió el Mjs de la PC
            if(ORx == 23)
            {
                DTxt(PINK, "Dato Recibido");
            }
            if(ORx == 5)
            {
                DTxt(PINK, "ACK Recibido");
            }
            ORx = 0;         // reinicia el contador de byte recibido, para la proxima recepcion
            if(FACK_W == 1) // Pregunto si estaba esperando un ACK
            {
                WT3(TACK); // Iniciamos el Timer esperando el ACK
            }
            else
            {
                CloseTimer3();     // apaga el timer, ya que llego todo bien
            }
            i=0;
            while(i<=23)
            {
                VRx[i]=VRx_Aux[i+1];
                i++;
            }
        }
        ORx ++;
    
    }

#endif // USART/RX - Recepcion de Mensajes del PAN

#if 1
    if (PIR2bits.TMR3IF == 1)   // Timer 3 Auxiliar de Rx Usart
    {
        PIR2bits.TMR3IF = 0; // Apagamos el motivo de la interrupcion
        CloseTimer3();

        if(FTMR3ACK == 1) // Pregunto si estaba esperando un ACK
        { // Nunca llego un ACK, reenvio el ultimo mjs
            FACK_TO = 1;
            FENACK = 1;
        }
        else
        { // En plena recepcion nunca llego el proximo byte
            ORx = 1;         // reinicia el contador de byte recibido, para la proxima recepcion
            if(FACK_W == 1)
            {
                WT3(TACK);   // Si estaba esperando un ACK, reinicio la cuenta
            }
        }
    }
#endif // Timer 3 Auxiliar de Rx Usart

    return;
}

void Tiempos()
{
    uint8_t i, j;

F1ms = 0;
F10ms = 0;
F100ms = 0;
F1s = 0;

#if 1
  if(F1msp == 1)
  {
    F1msp = 0;
    F1ms = 1;
  }

  if(R10msp != 0)
  {
    R10msp --;
    F10ms = 1;
  }

  if(R100msp != 0)
  {
    R100msp --;
    F100ms = 1;
  }

  if(R1sp != 0)
  {
    R1sp --;
    F1s = 1;
  }

#endif // Calculo de tiempos Internos

#if 1
  if (F1ms == 1)
  {

     if(FTPRMW == 1)   // PREGUNTA ACTIVA Retransmisión por MiWi
     {
       if (FTRRMW == 0)  // SI YA HAY UNA RESPUESTA NO HACE NADA
       {
         RTRMW ++;
         if(RTRMW == RMW)
         {
           RTRMW = 0;
           FTRRMW = 1;
         }
       }
     }
  }

#endif  // 1 ms

#if 1
    if (F10ms == 1) // PREGUNTA SI PASARON 10 ms
    {

      if(LED_PAN == 1)   // PREGUNTA ACTIVA Ping de Ruido
      {
        if (FTR_LED_PAN == 0)  // SI YA HAY UNA RESPUESTA NO HACE NADA
        {
          RT_LED_PAN ++;
          if(RT_LED_PAN == T_LED )  // 10 = 100 ms
          {
            RT_LED_PAN = 0;
            FTR_LED_PAN = 1;
          }
        }
      }

      if(LED_MiWi == 1)   // PREGUNTA ACTIVA Ping de Ruido
      {
        if (FTR_LED_MiWi == 0)  // SI YA HAY UNA RESPUESTA NO HACE NADA
        {
          RT_LED_MiWi ++;
          if(RT_LED_MiWi == T_LED )  // 10 = 100 ms
          {
            RT_LED_MiWi = 0;
            FTR_LED_MiWi = 1;
          }
        }
      }

      if(LED_U1 == 1)   // PREGUNTA ACTIVA Ping de Ruido
      {
        if (FTR_LED_U1 == 0)  // SI YA HAY UNA RESPUESTA NO HACE NADA
        {
          RT_LED_U1 ++;
          if(RT_LED_U1 == T_LED )  // 10 = 100 ms
          {
            RT_LED_U1 = 0;
            FTR_LED_U1 = 1;
          }
        }
      }

      if(LED_U2 == 1)   // PREGUNTA ACTIVA Ping de Ruido
      {
        if (FTR_LED_U2 == 0)  // SI YA HAY UNA RESPUESTA NO HACE NADA
        {
          RT_LED_U2 ++;
          if(RT_LED_U2 == T_LED )  // 10 = 100 ms
          {
            RT_LED_U2 = 0;
            FTR_LED_U2 = 1;
          }
        }
      }



      if(FTPB1 == 1)   // PREGUNTA ACTIVA (Boton)
      {
        if (FTRB1 == 0)  // SI YA HAY UNA RESPUESTA NO HACE NADA
        {
          RTB1 ++;
          if(RTB1 == TARB )  //
          {
            RTB1 = 0;
            FTRB1 = 1;
          }
        }
      }

      if(FTPB2 == 1)   // PREGUNTA ACTIVA (Boton)
      {
        if (FTRB2 == 0)  // SI YA HAY UNA RESPUESTA NO HACE NADA
        {
          RTB2 ++;
          if(RTB2 == TARB )  // 200 ms
          {
            RTB2 = 0;
            FTRB2 = 1;
          }
        }
      }

      if(FTPB3 == 1)   // PREGUNTA ACTIVA (Boton)
      {
        if (FTRB3 == 0)  // SI YA HAY UNA RESPUESTA NO HACE NADA
        {
          RTB3 ++;
          if(RTB3 == TARB )  // 200 ms
          {
            RTB3 = 0;
            FTRB3 = 1;
          }
        }
      }

      if(FTPB4 == 1)   // PREGUNTA ACTIVA (Boton)
      {
        if (FTRB4 == 0)  // SI YA HAY UNA RESPUESTA NO HACE NADA
        {
          RTB4 ++;
          if(RTB4 == TARB )  // 200 ms
          {
            RTB4 = 0;
            FTRB4 = 1;
          }
        }
      }
    }
#endif // 10 ms

#if 1

    if (F100ms == 1) // PREGUNTA SI PASARON 100 ms
    {
        if(FBPA == 1)
        {
            FBPA = 0;
//            DTxt("hola");
        }

    }
#endif // 100 ms

#if 1

if (F1s == 1)
{

}

#endif  // 1 s

#if 1

if(F100ms == 1)
{
    if(Fsync == 1)  // verifico si llega un pulso de sincronismo externo
    {
        Fsync = 0;
        if(TPs >= 5)
        {
            FSP = 1;
        }
        TPs = 0;
    }
    else
    {
        TPs ++;
        if(TPs == 10)
        {
            FSP = 1;
            TPs = 0;
        }
    }
    if(FSP == 1)    // Verifico si paso un segundo patron
    {
        if(PORTBbits.RB4 == 1)
        {
                PORTBbits.RB4 = 0;

        }
        else
        {
                PORTBbits.RB4 = 1;
        }
        FSP = 0;
        if(ModTP == 1)  // verifico si debo incrementar el Tiempo de Partida
        {
            TP_S ++;
            if(TP_S == 60)
            {
                TP_S = 0;
                TP_M ++;
                if(TP_M == 60)
                {
                    TP_M = 0;
                    TP_H ++;
                }
            }
        }
        if(ModTP == 2)  // verifico si debo decrementar el Tiempo de Partida
        {
            TP_S --;
            if(TP_S == 255)
            {
                TP_S = 59;
                TP_M --;
                if(TP_M == 255)
                {
                    TP_M = 59;
                    TP_H --;
                }
            }
        }
      
    }
}



#endif // Calculo de tiempos Visualizables (con sincronizacion externa)

#if 1

if (FTR_LED_PAN == 1)
{
    LED_PAN = 0;
    FTR_LED_PAN = 0;
}

if (FTR_LED_MiWi == 1)
{
    LED_MiWi = 0;
    FTR_LED_MiWi = 0;
}

if (FTR_LED_U1 == 1)
{
    LED_U1 = 0;
    FTR_LED_U1 = 0;
}

if (FTR_LED_U2 == 1)
{
    LED_U2 = 0;
    FTR_LED_U2 = 0;
}

//i=0;
//while(i<30)
//{
//    if(VTMID[i] != 0)   //Si tengo un conteo de tiempos en dicho NID
//    {
//        VTMID[i] --;
//        if(VTMID[i] == 0)
//        {
//            for(j=0; j<10; j++)
//            {
//                VCR[i][j] = 0;
//            }
//        }
//    }
//    i++;
//}

#endif// Implementaciones automaticas de Respuestas
}

uint8_t Invertir_Byte(uint8_t inv)      //Rutina creada para invertir los datos a enviar por SPI al ISD
{
    uint8_t ret;
    ret=0;
    if((inv & 0B00000001) == 0B00000001)
    {
        ret = ret | 0B10000000;
    }
    if((inv & 0B00000010) == 0B00000010)
    {
        ret = ret | 0B01000000;
    }
    if((inv & 0B00000100) == 0B00000100)
    {
        ret = ret | 0B00100000;
    }
    if((inv & 0B00001000) == 0B00001000)
    {
        ret = ret | 0B00010000;
    }
    if((inv & 0B00010000) == 0B00010000)
    {
        ret = ret | 0B00001000;
    }
    if((inv & 0B00100000) == 0B00100000)
    {
        ret = ret | 0B00000100;
    }
    if((inv & 0B01000000) == 0B01000000)
    {
        ret = ret | 0B00000010;
    }
    if((inv & 0B10000000) == 0B10000000)
    {
        ret = ret | 0B00000001;
    }
    return ret;
}

#endif // Auxiliares / Pruebas


#if 1

void SBS(INPUT BYTE Origen, INPUT BYTE Tipo)
{
    if(Origen != 0)
    {
        BSO = Origen;
    }
    if(Tipo != 0)
    {
        BST = Tipo;
    }
}

void Blue_Screen()
{
    Llenar_Pantalla(BLUE);


}

void D1(INPUT BYTE Num, uint16_t color)     //Muestra un digito
{
        DrawChar(DDy, DDx*6, 48+Num, color, BLACK, 1);
        DDx ++;
        DrawChar(DDy, DDx*6, 48+Num, BLACK, BLACK, 1);
        DDx ++;

        if (DDx >= 45)
        {
            DDx = 0;
            DDy = DDy - 10;
            if(DDy > 230)
            {
                DDy = 230;
            }
        }
}

void D1f(INPUT BYTE Num, uint16_t color)    //Muestra un digito
{
        DrawChar(DDy, DDx*6, 48+Num, color, BLACK, 1);
        DDx ++;
        DDx ++;
        if (DDx == 45)
        {
            DDx = 0;
            if(DDy > 230)
            {
                DDy = 230;
            }
        }
}

void D2(INPUT BYTE Num, uint16_t color)     //Muestra dos digitos
{
uint8_t AuxCD;
        AuxCD = Num /10;
        DrawChar(DDy, DDx*6, 48+AuxCD, color, BLACK, 1);
        DDx ++;
        AuxCD = Num;
        while(AuxCD > 9)
        {
            AuxCD = AuxCD - 10;
        }
        DrawChar(DDy, DDx*6, 48+AuxCD, color, BLACK, 1);
        DDx ++;
        DrawChar(DDy, DDx*6, 48+Num, BLACK, BLACK, 1);
        DDx ++;
        if (DDx >= 45)
        {
            DDx = 0;
            DDy = DDy - 10;
            if(DDy > 230)
            {
                DDy = 230;
            }
        }
}

void D3(INPUT BYTE Num, uint16_t color)     //Muestra dos digitos
{
uint8_t AuxCD;
        AuxCD = Num /100;
        DrawChar(DDy, DDx*6, 48+AuxCD, color, BLACK, 1);
        DDx ++;
        AuxCD = Num;
        while(Num > 99)
        {
            Num = Num - 100;
        }
        AuxCD = Num /10;
        DrawChar(DDy, DDx*6, 48+AuxCD, color, BLACK, 1);
        DDx ++;
        AuxCD = Num;
        while(AuxCD > 9)
        {
            AuxCD = AuxCD - 10;
        }
        DrawChar(DDy, DDx*6, 48+AuxCD, color, BLACK, 1);
        DDx ++;
        DrawChar(DDy, DDx*6, 48+Num, BLACK, BLACK, 1);
        DDx ++;
        if (DDx >= 45)
        {
            DDx = 0;
            DDy = DDy - 10;
            FillRect(DDy-10, 0, 10, 320, BLACK);     //para hacer mas legible la pantalla
            if(DDy > 230)
            {
                DDy = 230;
            }
        }
}

void MD(uint8_t NdD, uint16_t Num, uint16_t color, uint16_t X, uint16_t Y)      //Muestra multiples digitos (1-4) 5 NO!!!
{

uint8_t AuxCD;
   if(NdD == 4)
   {
       AuxCD = Num /1000;
       DrawChar(Y, X, 48+AuxCD, color, BLACK, 1);
       X = X + 6;

       while(Num > 999)
       {
           Num = Num - 1000;
       }
   }
   if(NdD >= 3)
   {
       AuxCD = Num /100;
       DrawChar(Y, X, 48+AuxCD, color, BLACK, 1);
       X = X + 6;

       while(Num > 99)
       {
           Num = Num - 100;
       }
   }
   if(NdD >= 2)
   {
       AuxCD = Num /10;
       DrawChar(Y, X, 48+AuxCD, color, BLACK, 1);
       X = X + 6;

       while(Num > 9)
       {
           Num = Num - 10;
       }
   }

       AuxCD = Num;
       DrawChar(Y, X, 48+AuxCD, color, BLACK, 1);

}

void DTxt(uint16_t Color, uint8_t ToShow[45])
{
    uint8_t Largo=0;
    uint16_t fin;

    while(ToShow[Largo] != '\0')
    {
        Largo++;                    // Determino el largo de la cadena ingresada
    }
    Largo++;

    fin = NTA+Largo+8;  // Posición inicial + Cadena de Texto + Min/Seg/mSeg/Espacio

    if(fin > NTA_MAX)
    {
        if(CMP <99)
        {
            CMP ++;     // El mensaje no entra en el buffer, indico que perdí uno.
        }
        return;
    }

    Debug_Text[NTA] = 48 + (Debug_Time_m / 10 );
    NTA++;
    Debug_Text[NTA] = 48 + (Debug_Time_m % 10);
    NTA++;
    Debug_Text[NTA] = 48 + (Debug_Time_s / 10 );
    NTA++;
    Debug_Text[NTA] = 48 + (Debug_Time_s % 10);
    NTA++;
    Debug_Text[NTA] = 48 + (Debug_Time_ms / 100);
    NTA++;
    Debug_Text[NTA] = 48 + (Debug_Time_ms / 10 % 10);
    NTA++;
    Debug_Text[NTA] = 48 +  (Debug_Time_ms % 10);
    NTA++;
    Debug_Text[NTA] = 32; // Espacio ¬ ¬
    NTA++;
    if(DTS_x_m == 0)
    {
       // DTS_x_m = 0; //Largo+8;  // Es el primer mensaje, debo indicar cual será el largo del mismo
    }
    Largo=0;
    while(NTA != fin)
    {
        Debug_Text[NTA] = ToShow[Largo];
        Largo++;
        NTA++;
    }

}

void DTs()
{
    if(NTA == 0)
    {
        return;
    }

    uint16_t color;
    uint8_t i;
    uint8_t Largo = 0;

    if(DTS_x_m == 0)        // Verifico si es el primer caracter que dibujo
    {
        while(Debug_Text[DTS_x_m] != '\0')
        {
            DTS_x_m++;
        }
        //DTS_x_m++;                // Obtengo el largo de la nueva cadena de texto
    }


    if(DTS_x < 2)
    {
        color = BLUE;  // Minutos (2 digitos)
    }
    else
    {
        if(DTS_x < 4)
        {
            color = GREEN;  // Segundos (2 digitos)
        }
        else
        {
            if(DTS_x < 7)
            {
                color = YELLOW; // Mili Segundos (3 digitos)
            }
            else
            {
                color = WHITE;  // Mensaje de Texo (47 caracteres)
            }
        }
    }


    DrawChar(DTS_y, (DTS_x*6)+1, Debug_Text[DTS_x], color, BLACK, 1);
    DTS_x ++;
    if(DTS_x == DTS_x_m)        // Verifico si ya dibujé el ultimo caracter del renglón
    {
            DTS_x_m = 0;

#if 1
        DTS_x = 0;              // Reinicio el contador en el renglón

        if(DTS_y <= DTS_y_b)
        {
            DTS_y = DTS_y_t - 10;
        }
        else
        {
            DTS_y = DTS_y - 10;
        }
#endif                      // Manejo de coordenadas en la pantalla

        Largo = 0;
        while(Debug_Text[Largo] != '\0')
        {
            Largo++;
        }
        Largo++;                // Obtengo el largo de la cadena de texto

        if(NTA == Largo)        // Verifico si quedan mensajes
        {
            NTA = 0;            // No hay mas por mostrar
            DTS_x_m = 0;
        }
        else
        {
            for(i = 0; i <= (NTA-Largo) ; i++)
            {
                Debug_Text[i] = Debug_Text[Largo+i];   // Hay todavia mensajes por mostrar, desfragmento
            }
            NTA = NTA - Largo; // Libero lugar en el buffer
        }

        if(CMP != 0)            // Verifico si perdi mensajes mientras dibujaba
        {
            if(CMP<10)      // Largo del mensaje de error
            {
                Largo = 10;
            }
            else
            {
                Largo = 11;
            }
            if((NTA_MAX-NTA)>= Largo)   //Verifico si entra el mensaje de error
            {
                Debug_Text[NTA] = 48 + (Debug_Time_m / 10 );
                NTA++;
                Debug_Text[NTA] = 48 + (Debug_Time_m % 10);
                NTA++;
                Debug_Text[NTA] = 48 + (Debug_Time_s / 10 );
                NTA++;
                Debug_Text[NTA] = 48 + (Debug_Time_s % 10);
                NTA++;
                Debug_Text[NTA] = 48 + (Debug_Time_ms / 100);
                NTA++;
                Debug_Text[NTA] = 48 + (Debug_Time_ms / 10 % 10);
                NTA++;
                Debug_Text[NTA] = 48 +  (Debug_Time_ms % 10);
                NTA++;
                Debug_Text[NTA] = 45; // "-" indicador de comando interno
                NTA++;
                if(CMP < 10)
                {
                    Debug_Text[NTA] = 48 +  CMP;        // Es de un solo dígito la cantidad de mensajes perdidos
                    NTA++;
                }
                else
                {
                    Debug_Text[NTA] = 48 + (CMP / 10 ); // Es de mas de un dígito la cantidad de mensajes perdidos
                    NTA++;
                    Debug_Text[NTA] = 48 + (CMP % 10);  // (voy a suponer que no habrá mas de 99 perdidos.. osea... no samo tan qliao!!!)
                    NTA++;
                }
                Debug_Text[NTA] = 0;
                NTA++;
                CMP = 0;    // Acabo de mandar a informar los mensajes que perdí, reinicio la cuenta
            }
        }
    }


}

void Monitor_Recursos(void)
{
    uint8_t    D_5, D_4, D_3, D_2, D_1;    //Declaro 5 vbles para representar dígitos de una Vble a dibujar
    uint16_t   Aux_16;
    uint32_t   Aux, Div1, Div2, Div3;    //Vble auxiliar para calcular varias cosas
    uint8_t    Aux_8;

    if(FHMR == 1)       //Cuento los Mains solamente si está habilitado el monitor de recursos
    {
        Mains_Last_Aux ++;
        if(F1s == 1)    //Aca mismo dibujaré el timempo pues acaba de pasar un segundo
        {
            Mains_Last = Mains_Last_Aux;    //Paso el valor contado a la Vble que se mostrará
            Mains_Last_Aux = 0;

            Linea_Vertical(MR_Bottom, DMRH_s + DMRH_x_i, 20, BLACK);    // Borro la columna que estoy por dibujar

            DrawPixel(MR_Bottom + DMRH_CPU, DMRH_s + DMRH_x_i, CYAN);  // Grafico el uso del CPU

            D_1 = (DMRH_Tx + 1)/2;      // Calculo el Largo de la línea a Dibujar de Tx
            DMRH_Tx = 0;                // Reinicio el Contador de Mensajes Tx
            Linea_Vertical(MR_Bottom, DMRH_s + DMRH_x_i, D_1, RED); // Tx

            D_1 = (DMRH_Rx + 1)/2;      // Calculo el Largo de la línea a Dibujar de Rx
            DMRH_Rx = 0;                // Reinicio el Contador de Mensajes Rx
            Aux_16 = MR_Bottom + 21 - D_1;// Calculo en donde debo empezar a dibujar la linea
            Linea_Vertical(Aux_16, DMRH_s + DMRH_x_i, D_1, GREEN);  // Rx

            DMRH_s ++;          // Dejo apuntado a la próxima posición
            if(DMRH_s == 60)
            {
                DMRH_s = 0;     // Si desbordé reinicio
            }

            Linea_Vertical(MR_Bottom, DMRH_s + DMRH_x_i, 20, WHITE);    // Borro la columna que estoy por dibujar
            return;
        }
        if(Debug_Time_s != Debug_Time_s_BU)
        {
            Debug_Time_s_BU = Debug_Time_s; //Tomo el valor nuevo
            D_2 = Debug_Time_s / 10;
            D_1 = Debug_Time_s % 10;
            DrawDigit(MR_Bottom + 10, 24, D_1, GREEN, DARKGREY, 1);
            if(D_1 == 0)    //Si el D1 es 0, entonces debo dibujar el D2
            {
                DrawDigit(230, 18, D_2, GREEN, DARKGREY, 1);
            }
            return;
        }
        if(Debug_Time_m != Debug_Time_m_BU)    //Si los segundos son 0, deberia dibujar minutos
        {
            Debug_Time_m_BU = Debug_Time_m; //Tomo el valor nuevo
            D_2 = Debug_Time_m / 10;
            D_1 = Debug_Time_m % 10;
            DrawDigit(MR_Bottom + 10, 6, D_1, NAVY, DARKGREY, 1);
            if(D_1 == 0)    //Si el D1 es 0, entonces debo dibujar el D2
            {
                DrawDigit(230, 0, D_2, NAVY, DARKGREY, 1);
            }
            return;
        }
        if(FPVMR == 1)   //Si es la primera vez que recorro el monitor de recursos
        {           //deberé dibujar la plantilla
           FPVMR = 0;    //Para no redibujar la plantilla
           Linea_Horizontal(MR_Bottom - 2,0,320,LIGHTGREY);
           FillRect(MR_Bottom - 1,0, 21,320, DARKGREY);
           DrawChar(MR_Bottom + 10, 12, ':', WHITE, DARKGREY, 1); //Los dos puntos de la hora
           DrawChar(MR_Bottom + 10, 222, 'R', NAVY, DARKGREY, 1);
           DrawChar(MR_Bottom + 10, 228, 'A', NAVY, DARKGREY, 1);
           DrawChar(MR_Bottom + 10, 234, 'M', NAVY, DARKGREY, 1);
           DrawChar(MR_Bottom, 222, 'N', NAVY, DARKGREY, 1);
           DrawChar(MR_Bottom, 228, 'R', NAVY, DARKGREY, 1);
           DrawChar(MR_Bottom, 234, 'A', NAVY, DARKGREY, 1);
           DrawChar(MR_Bottom + 10, 306, 'V', GREEN, DARKGREY, 1); //V de bateria
           DrawChar(MR_Bottom, 306, '%', GREEN, DARKGREY, 1); //% de bateria
           DrawChar(MR_Bottom + 10, 270, '%', YELLOW, DARKGREY, 1); //% de la RAM ocupada
           DrawChar(MR_Bottom, 96, '%', WHITE, DARKGREY, 1);  //% de la CPU
                DrawVble(MR_Bottom + 10, 60, Mains_Max, Mains_Max_BU, MAGENTA, DARKGREY, 1, 16);   //Mando a dibujar la Vble
           Mains_Max_BU = Mains_Max;
                DrawVble(MR_Bottom, 60, Mains_Min, Mains_Min_BU, CYAN, DARKGREY, 1, 16);
           Mains_Min_BU = Mains_Min;

           Linea_Vertical(MR_Bottom, DMRH_x_i - 1, 20, NAVY);        //Dibujo los límites del Histograma
           Linea_Vertical(MR_Bottom, DMRH_x_i + 60, 20, NAVY);
           NRA_BU = 9999;
           RAM_Porc_BU = 199;
           Mains_Porc_BU = 199;
           Bat_V_BU = 0XFFFF;
           Bat_P_BU = 200;
           Debug_Time_s_BU = 61;
           Debug_Time_m_BU = 61;
        }
//Actualizar hora
        if(Mains_Last != Mains_Last_BU)     //Si la carga del CPU se modificó respecto de la última visualización
        {
                DrawVble(MR_Bottom + 10, 96, Mains_Last, Mains_Last_BU, WHITE, DARKGREY, 1, 16);

                //Calculo del CPU porcentual
            Div1 = Mains_Last - Mains_Min;        //Tomo el mínimo como base
            Div2 = Div1 * 100;                 //Aumento la apreciación
            Div1 = Mains_Max - Mains_Min;     //Tomo el Span
            Div3 = Div2 / Div1;    //Calculo el % respecto de los Mains
            Div1 = 100 - Div3;                        //Xq el máximo de MAINS es el 0% de carga
            Mains_Porc = Div1;

            Mains_Last_BU = Mains_Last;   //Almaceno el Nuevo Back Up luego de haber calculado el %

            if(Mains_Last > Mains_Max)      //Si el valor leido supera el máximo
            {
                Aux_8 = Mains_Last >> 8;
                eeprom_write(MAINS_MAX_ADDRESS, Aux_8);    //Lo almaceno como el nuevo máximo
                Aux_8 = Mains_Last;
                eeprom_write(MAINS_MAX_ADDRESS + 1, Aux_8);    //Lo almaceno como el nuevo máximo
                Mains_Max = Mains_Last;
            }
            if(Mains_Last < Mains_Min)      //Si el valor leido es inferior al mínimo
            {
                Aux_8 = Mains_Last >> 8;
                eeprom_write(MAINS_MIN_ADDRESS, Aux_8);    //Lo almaceno como el nuevo mínimo
                Aux_8 = Mains_Last;
                eeprom_write(MAINS_MIN_ADDRESS + 1, Aux_8);    //Lo almaceno como el nuevo mínimo
                Mains_Min = Mains_Last;
            }
            return;     //Estos return serán para que se actualice como máx un valor por main
        }
        if(Mains_Porc != Mains_Porc_BU)     //CPU %
        {
                DrawVble(MR_Bottom, 90, Mains_Porc, Mains_Porc_BU, WHITE, DARKGREY, 1, 8);
            Mains_Porc_BU = Mains_Porc;
            DMRH_CPU = Mains_Porc/5;
            return;
        }
        if(Mains_Max != Mains_Max_BU)     //Si la carga del CPU se modificó respecto de la última visualización
        {
                DrawVble(MR_Bottom + 10, 60, Mains_Max, Mains_Max_BU, MAGENTA, DARKGREY, 1, 16);
            Mains_Max_BU = Mains_Max;
            return;
        }
        if(Mains_Min != Mains_Min_BU)     //Si la carga del CPU se modificó respecto de la última visualización
        {
                DrawVble(MR_Bottom, 60, Mains_Min, Mains_Min_BU, CYAN, DARKGREY, 1, 16);
            Mains_Min_BU = Mains_Min;
            return;
        }
        //DTxt("Llego a verificar NRA");
        if(NRA != NRA_BU)     //Si la RAM ocupada se modificó respecto de la última visualización
        {
                DrawVble(MR_Bottom, 260, NRA, NRA_BU, YELLOW, DARKGREY, 1, 16);
            Aux = NRA * 100;       //Aumento la apreciación
            RAM_Porc = Aux / LRAM_D;    //Calculo el % respecto del Max de RAM
            NRA_BU = NRA;
            FDRAMP = 1;     //Pongo a "1" el flag de dibujar RAM porcentual
            return;
        }
        if(RAM_Porc != RAM_Porc_BU)
        {
                DrawVble(MR_Bottom + 10, 258, RAM_Porc, RAM_Porc_BU , YELLOW, DARKGREY, 1, 8);
            RAM_Porc_BU = RAM_Porc;
            return;
        }
        if(Bat_V != Bat_V_BU)     //Si la Tensión de la batería se modificó respecto de la última visualización
        {
                DrawVble(MR_Bottom + 10, 300, Bat_V, Bat_V_BU, GREEN, DARKGREY, 1, 16);  //FNAF Ver como colocar el "." ahora que uso DraxVble()
            Bat_V_BU = Bat_V;
            return;
        }
        if(Bat_P != Bat_P_BU)     //Si el porcentaje de la batería se modificó respecto de la última visualización
        {
                DrawVble(MR_Bottom, 294, Bat_P, Bat_P_BU, GREEN, DARKGREY, 1, 8);
            Bat_P_BU = Bat_P;
            return;
        }
        if(Vble_Aux1 != Vble_Aux1_BU)
        {
                DrawVble(MR_Bottom + 10, 210, Vble_Aux1, Vble_Aux1_BU, MAROON, DARKGREY, 1, 16);
            Vble_Aux1_BU = Vble_Aux1;
            return;
        }
        if(Vble_Aux2 != Vble_Aux2_BU)
        {
                DrawVble(MR_Bottom, 210, Vble_Aux2, Vble_Aux2_BU, MAROON, DARKGREY, 1, 16);
            Vble_Aux2_BU = Vble_Aux2;
            return;
        }
    }
}

void Menu_Interactivo(void)
{
    uint8_t i,j, Menu_Boton = 0;
    uint16_t Color_PC = 0;
    uint16_t Vble = 0;
    BIT FNoDibujar = 0;

#if 1

    Menu_Boton = 0;     // Por si no se presiona nada

    if(Menu_Index != MENU_MONITOR)
    {
        if((X_T > 10) && (X_T < 80) && (Y_T > 155) && (Y_T < 225))
        {
            Menu_Boton = 1;
        }

        if((X_T > 90) && (X_T < 160) && (Y_T > 155) && (Y_T < 225))
        {
            Menu_Boton = 2;
        }

        if((X_T > 170) && (X_T < 240) && (Y_T > 155) && (Y_T < 225))
        {
            Menu_Boton = 3;
        }

        if((X_T > 250) && (X_T < 320) && (Y_T > 155) && (Y_T < 225))
        {
            Menu_Boton = 4;
        }

        if((X_T > 10) && (X_T < 80) && (Y_T > 80) && (Y_T < 150))
        {
            Menu_Boton = 5;
        }

        if((X_T > 90) && (X_T < 160) && (Y_T > 80) && (Y_T < 150))
        {
            Menu_Boton = 6;
        }

        if((X_T > 170) && (X_T < 240) && (Y_T > 80) && (Y_T < 150))
        {
            Menu_Boton = 7;
        }

        if((X_T > 250) && (X_T < 320) && (Y_T > 80) && (Y_T < 150))
        {
            Menu_Boton = 8;
        }

        if((X_T > 10) && (X_T < 80) && (Y_T > 5) && (Y_T < 75))
        {
            Menu_Boton = 9;
        }

        if((X_T > 90) && (X_T < 160) && (Y_T > 5) && (Y_T < 75))
        {
            Menu_Boton = 10;
        }

        if((X_T > 170) && (X_T < 240) && (Y_T > 5) && (Y_T < 75))
        {
            Menu_Boton = 11;
        }

        if((X_T > 250) && (X_T < 320) && (Y_T > 5) && (Y_T < 75))
        {
            Menu_Boton = 12;
        }
    }
    if(Menu_Index == MENU_MONITOR)
    {
        uint8_t AuxT, AuxB;

        if(MR_Bottom < 6)
        {
            AuxB = 1;   // No puede estar por debado de esto el boton tactil
        }
        else
        {
            AuxB = MR_Bottom - 5; // Hago un poco mas grande el area del tactil Que el MR)
        }

        if(MR_Bottom > 214)
        {
            AuxT = 239;   // No puede estar por debado de esto el boton tactil
        }
        else
        {
            AuxT = MR_Bottom + 20 + 5; // Hago un poco mas grande el area del tactil Que el MR)
        }

        if((Y_T > AuxB) && (Y_T < AuxT))
        {
            Menu_Boton = 13;        // Valor a Devolver para salir de MENU_MONITOR
        }
    }


#endif  // Conversión Coordenadas a Botones

switch(Menu_Index)
    {
        case(0):        //Esta en el case por defecto (sistema de menu apagado)
        {
#if 1
            if(X_T > 250 && Y_T < 80)   //Boton de abajo
            {
                DTxt(ORANGE, "Abajo");
                if(Clave_Menu == 1)
                {
                    DTxt(GREEN, "MUY Bien! 2/3");
                    Clave_Menu = 2;
                }
                else
                {
                    DTxt(RED, "UPS! La cagaste -.- 2");
                    Clave_Menu = 0;
                }

            }
            if(X_T > 250 && (Y_T < 160) && (Y_T > 80))   //Boton del medio
            {
                DTxt(ORANGE, "Medio");
                if(Clave_Menu == 2)
                {
                    Menu_Index = MENU_HOME;// Entraste al menu secreto
                    Clave_Menu = 0;
                    Plantilla(MENU_HOME);
                    FPVMR = 1;
                }
                else
                {
                    DTxt(RED, "UPS! La cagaste -.- 3");
                    Clave_Menu = 0;
                }
            }
            if(X_T > 250 && Y_T > 160)   //Boton de ashiba
            {
                DTxt(ORANGE, "Arriba");
                if(Clave_Menu == 0)
                {
                    Clave_Menu = 1;
                    DTxt(GREEN, "Bien! 1/3");
                }
                else
                {
                    DTxt(RED, "UPS! La cagaste -.- 1");
                    Clave_Menu = 0;
                }
            }
            break;
#endif      // Combinacion secreta para entrar al menu interactivo secreto  --- NO ENTRAR !!! ---
        }

        case(MENU_HOME):
        {
#if 1
            Monitor_Recursos();
            if(Menu_Boton == 1)
            {
                Plantilla(MENU_MONITOR);
                Menu_Index = MENU_MONITOR;     // Entramos en Ping de Cobertura
                FPVMR = 1;    //Para si redibujar la plantilla
            }
            if(Menu_Boton == 2)
            {
                Plantilla(MENU_RASTREO);
                Menu_Index = MENU_RASTREO;     // Entramos en Ping de Cobertura
                FPVMR = 1;    //Para si redibujar la plantilla
            }
            if(Menu_Boton == 4)
            {
                Plantilla(0);
                Menu_Index = 0;
            }
            break;
#endif
        }
        case(MENU_MONITOR):
        {
#if 1
            Monitor_Recursos();
            if(Menu_Boton == 13)
            {
                Plantilla(MENU_HOME);
                Menu_Index = MENU_HOME;     // Salimos del modo monitor y volvemos al menu principal
                FPVMR = 1;    //Para si redibujar la plantilla
            }
            break;
#endif
        }
        case(MENU_RASTREO):
        {
#if 1
            Monitor_Recursos();
            DTs();
            if(Menu_Boton == 12)        //HOME
            {
                Plantilla(MENU_HOME);
                Menu_Index = MENU_HOME;     // Salimos del modo monitor y volvemos al menu principal
                FPVMR = 1;    //Para si redibujar la plantilla
            }
            if(Menu_Boton == 4)    // Boton de Pausa/Reanudar
            {

            }
            if(Menu_Boton == 8)    //Reset
            {
                FillRect( 0, 0, 179, 239, BLACK);   //Limpio toda la zona de los datos
                Linea_Vertical(0, 80, 219, WHITE);
                Linea_Vertical(0, 160, 219, WHITE);
                Linea_Vertical(0, 240, 219, WHITE);
                Linea_Vertical(0, 40, 199, DARKGREY);
                Linea_Vertical(0, 120, 199, DARKGREY);
                Linea_Vertical(0, 200, 199, DARKGREY);
            }


            break;
#endif
        }
        case(MENU_EXIT):
        {
#if 1
            if(Menu_Boton == 4)
            {
                Menu_Index = 0;     // Salimos del modo monitor y volvemos al menu principal
            }
            break;
#endif
        }
    }
}

#endif  // Debug


#if 1
//* * * * ACA COMIENZA LA NUEVA LIBRERIA * * *

void Write_Command(uint8_t c)
{
    DISP_CS=0;
    DISP_CD=0;

    SPIPut(c);

    DISP_CS=1;
}

void Write_Data(uint8_t c)
{
    DISP_CS=0;
    DISP_CD=1;

    SPIPut(c);

    DISP_CS=1;
}

void Inicializar_Pantalla(void)
{
   // Reset_Pantalla();

    RFIE=0;
    Write_Command(0xEF);
    Write_Data(0x03);
    Write_Data(0x80);
    Write_Data(0x02);

    Write_Command(0xCF);
    Write_Data(0x00);
    Write_Data(0XC1);
    Write_Data(0X30);

    Write_Command(0xED);
    Write_Data(0x64);
    Write_Data(0x03);
    Write_Data(0X12);
    Write_Data(0X81);

    Write_Command(0xE8);
    Write_Data(0x85);
    Write_Data(0x00);
    Write_Data(0x78);

    Write_Command(0xCB);
    Write_Data(0x39);
    Write_Data(0x2C);
    Write_Data(0x00);
    Write_Data(0x34);
    Write_Data(0x02);

    Write_Command(0xF7);
    Write_Data(0x20);

    Write_Command(0xEA);
    Write_Data(0x00);
    Write_Data(0x00);

    Write_Command(ILI9341_PWCTR1);    //Power control
    Write_Data(0x23);   //VRH[5:0]

    Write_Command(ILI9341_PWCTR2);    //Power control
    Write_Data(0x10);   //SAP[2:0];BT[3:0]

    Write_Command(ILI9341_VMCTR1);    //VCM control
    Write_Data(0x3e); //¶Ô±È¶Èµ÷½Ú
    Write_Data(0x28);

    Write_Command(ILI9341_VMCTR2);    //VCM control2
    Write_Data(0x86);  //--

    Write_Command(ILI9341_MADCTL);    // Memory Access Control
    Write_Data(0x48);

    Write_Command(ILI9341_PIXFMT);
    Write_Data(0x55);

    Write_Command(ILI9341_FRMCTR1);
    Write_Data(0x00);
    Write_Data(0x18);

    Write_Command(ILI9341_DFUNCTR);    // Display Function Control
    Write_Data(0x08);
    Write_Data(0x82);
    Write_Data(0x27);

    Write_Command(0xF2);    // 3Gamma Function Disable
    Write_Data(0x00);

    Write_Command(ILI9341_GAMMASET);    //Gamma curve selected
    Write_Data(0x01);

    Write_Command(ILI9341_GMCTRP1);    //Set Gamma
    Write_Data(0x0F);
    Write_Data(0x31);
    Write_Data(0x2B);
    Write_Data(0x0C);
    Write_Data(0x0E);
    Write_Data(0x08);
    Write_Data(0x4E);
    Write_Data(0xF1);
    Write_Data(0x37);
    Write_Data(0x07);
    Write_Data(0x10);
    Write_Data(0x03);
    Write_Data(0x0E);
    Write_Data(0x09);
    Write_Data(0x00);

    Write_Command(ILI9341_GMCTRN1);    //Set Gamma
    Write_Data(0x00);
    Write_Data(0x0E);
    Write_Data(0x14);
    Write_Data(0x03);
    Write_Data(0x11);
    Write_Data(0x07);
    Write_Data(0x31);
    Write_Data(0xC1);
    Write_Data(0x48);
    Write_Data(0x08);
    Write_Data(0x0F);
    Write_Data(0x0C);
    Write_Data(0x31);
    Write_Data(0x36);
    Write_Data(0x0F);

    Write_Command(ILI9341_SLPOUT);    //Exit Sleep
    delay_ms(120);
    Write_Command(ILI9341_DISPON);    //Display on
    RFIE=1;
    Llenar_Pantalla(BLACK);
}

void Reset_Pantalla(void)
{
    RFIE=0;
        DISP_CS=1;
	DISP_CD=1;

        DISP_RST=1;
        delay_ms(5);
        DISP_RST=0;
        delay_ms(20);
	DISP_RST=1;
        delay_ms(150);

        DISP_CS=0;
	DISP_CD=1;
        DISP_CS=1;
    RFIE=1;
}

void DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    SetAddrWindow(x, y, 0XEF, 0X13F);
    RFIE=0;
    DISP_CD=1;
    DISP_CS=0;
    SPIPut(color >> 8);
    SPIPut(color);
    DISP_CS=1;
    RFIE=1;
}

void DrawFastPixel(uint16_t color)
{
    RFIE=0;
    DISP_CD=1;
    DISP_CS=0;
    SPIPut(color >> 8);
    SPIPut(color);
    DISP_CS=1;
    RFIE=1;
}

void SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    RFIE=0;
    Write_Command(ILI9341_CASET); // Column addr set
    Write_Data(x0 >> 8);
    Write_Data(x0 & 0xFF);     // XSTART
    Write_Data(x1 >> 8);
    Write_Data(x1 & 0xFF);     // XEND

    Write_Command(ILI9341_PASET); // Row addr set
    Write_Data(y0>>8);
    Write_Data(y0);     // YSTART
    Write_Data(y1>>8);
    Write_Data(y1);     // YEND

    Write_Command(ILI9341_RAMWR); // write to RAM
    RFIE=1;
}

void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{

    uint8_t hi, lo;
    hi = color >> 8;
    lo = color;
    SetAddrWindow(x, y, x+w-1, y+h-1);
    RFIE=0;
    DISP_CD = 1;
    DISP_CS = 0;

    for(y=h; y>0; y--)
    {
        for(x=w; x>0; x--)
        {
            SPIPut(hi);
            SPIPut(lo);
        }
    }
    DISP_CS = 1;
    RFIE=1;
}

void Llenar_Pantalla(uint16_t color)
{
    FillRect(0, 0, 240, 320, (color));
    if(color == BLACK)
    {
    }
}

void Linea_Horizontal(int16_t x, int16_t y, int16_t length, uint16_t color)
{
    uint8_t hi, lo;
    hi = color >> 8;
    lo = color;

    SetAddrWindow(x, y, x, (y+length-1));
    RFIE=0;
    DISP_CD = 1;
    DISP_CS = 0;

    while (length--)
    {
        SPIPut(hi);
        SPIPut(lo);
    }
    DISP_CS = 1;
    RFIE=1;
}

void Linea_Vertical(int16_t x, int16_t y, int16_t length, uint16_t color)
{
    uint8_t hi, lo;
    hi = color >> 8;
    lo = color;

    SetAddrWindow(x, y, (x+length-1), y);
    RFIE=0;
    DISP_CD = 1;
    DISP_CS = 0;

    while (length--)
    {
        SPIPut(hi);
        SPIPut(lo);
    }
    DISP_CS = 1;
    RFIE=1;
}

void Flood(uint16_t color, uint32_t len)
{
    uint8_t  hi, lo;
    hi = color >> 8;
    lo = color;
    RFIE=0;
    DISP_CS=0;
    DISP_CD=1;
    while (len--)
    {
        SPIPut(hi);
        SPIPut(lo);
    }
    DISP_CS=1;
    RFIE=1;
}

void DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
      int16_t f = 1 - r;
      int16_t ddF_x = 1;
      int16_t ddF_y = -2 * r;
      int16_t x = 0;
      int16_t y = r;

      DrawPixel(x0  , y0+r, color);
      DrawPixel(x0  , y0-r, color);
      DrawPixel(x0+r, y0  , color);
      DrawPixel(x0-r, y0  , color);

      while (x<y)
      {
          if (f >= 0)
          {
              y--;
              ddF_y += 2;
              f += ddF_y;
          }
          x++;
          ddF_x += 2;
          f += ddF_x;
          DrawPixel(x0 + x, y0 + y, color);
          DrawPixel(x0 - x, y0 + y, color);
          DrawPixel(x0 + x, y0 - y, color);
          DrawPixel(x0 - x, y0 - y, color);
          DrawPixel(x0 + y, y0 + x, color);
          DrawPixel(x0 - y, y0 + x, color);
          DrawPixel(x0 + y, y0 - x, color);
          DrawPixel(x0 - y, y0 - x, color);
      }
}

void FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  Linea_Horizontal(x0, y0-r, 2*r+1, color);
  FillCircleHelper(x0, y0, r, 3, 0, color);
}

void FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1)
    {
      Linea_Horizontal(x0+x, y0-y, 2*y+1+delta, color);
      Linea_Horizontal(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2)
    {
      Linea_Horizontal(x0-x, y0-y, 2*y+1+delta, color);
      Linea_Horizontal(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

void ClrRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
        Linea_Vertical(x, y, w, color);
        Linea_Horizontal(x, y, h, color);
        Linea_Vertical(x, y+h-1, w, color);
        Linea_Horizontal(x+w-1, y, h, color);
}

void Write(uint8_t c)
{

    DrawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize*6;
    if (wrap && (cursor_x > (WIDTH - textsize*6)))
    {
        cursor_y += textsize*8;
        cursor_x = 0;
    }

}

void DrawChar(uint16_t x, uint16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size)
{

    SetAddrWindow(x, y, x+(size*8)-1, y+(size*6)-1);         //No estoy seguro si esta bn la ventana... comprobar

    for (int8_t i=0; i<6; i++ )                             //Esta sería la version vieja
    {
        uint8_t line, line_aux;
        if (i == 5)
        {
            line = 0x0;         //Este sería el espaciado entre caracteres
        }
        else
        {
            line = font[(c*5)+i];
        }
        line = Invertir_Byte(line);
        for (int8_t k = 0; k<size; k++)
        {
            line_aux = line;                        //Tomo nuevamente la linea si size>1

            for (int8_t j = 0; j<8; j++)
            {
                if (line_aux & 0x1)
                {
                    for (int8_t l = 0; l<size; l++)
                    {
                        DrawFastPixel(color);
                    }
                }
                else
                {
                    for (int8_t l = 0; l<size; l++)
                    {
                        DrawFastPixel(bg);
                    }
                }
                line_aux >>= 1;
            }
        }
    }
}

void DrawDigit(uint16_t x_d, uint16_t y_d, uint8_t c_d, uint16_t color_d, uint16_t bg_d, uint8_t size_d)    //FNAF pasar esta rutina a martin
{
    if(c_d>9)     //Si el valor pasado no es un dígito
    {
        return; //retorno
    }
    DrawChar( x_d,  y_d,  48 + c_d,  color_d,  bg_d,  size_d);
}

void DrawVble(uint16_t x_v, uint16_t y_v, uint16_t c_v, uint16_t c_v_BU, uint16_t color_v, uint16_t bg_v, uint8_t size_v, uint8_t bits)    //FNAF pasar esta rutina a martin
{   //Esta función solo mostrará Vbles de hasta 16 bits, siempre sin ceros a la Izq. Las coordenadas deben ser las del Dígito Menos Significativo
    uint16_t D_5, D_4, D_3, D_2, D_1;
    uint16_t D_5_BU, D_4_BU, D_3_BU, D_2_BU, D_1_BU;    //Los digitos del Back Up

    BIT FDigito_Dibujado = 0;
    D_5 = c_v / 10000;       //Armo los 5 dígitos a dibujar
    D_4 = c_v % 10000;
    D_4 = D_4 / 1000;
    D_3 = c_v % 1000;
    D_3 = D_3 / 100;
    D_2 = c_v % 100;
    D_2 = D_2 / 10;
    D_1 = c_v % 10;
    D_5_BU = c_v_BU / 10000;       //Armo los 5 dígitos a dibujar
    D_4_BU = c_v_BU % 10000;
    D_4_BU = D_4_BU / 1000;
    D_3_BU = c_v_BU % 1000;
    D_3_BU = D_3_BU / 100;
    D_2_BU = c_v_BU % 100;
    D_2_BU = D_2_BU / 10;
    D_1_BU = c_v_BU % 10;
    if(bits == 16)
    {
        if(D_5 != D_5_BU)
        {
            if(D_5!=0)
            {
                DrawDigit(x_v, y_v-(24*size_v), D_5, color_v, bg_v, size_v);    //Los dibujo
                FDigito_Dibujado = 1;
            }
            else
            {
                DrawDigit(x_v, y_v-(24*size_v), 1, bg_v, bg_v, size_v);
            }
        }
        if(D_5 != 0)
        {
            FDigito_Dibujado = 1;
        }
        if(D_4 != D_4_BU)
        {
            if(D_4!=0 || FDigito_Dibujado ==1)
            {
                DrawDigit(x_v, y_v-(18*size_v), D_4, color_v, bg_v, size_v);
                FDigito_Dibujado = 1;
            }
            else
            {
                DrawDigit(x_v, y_v-(18*size_v), 1, bg_v, bg_v, size_v);
            }
        }
        if(D_4 != 0)
        {
            FDigito_Dibujado = 1;
        }
    }
    if(D_3 != D_3_BU)
    {
        if(D_3!=0 || FDigito_Dibujado ==1)
        {
            DrawDigit(x_v, y_v-(12*size_v), D_3, color_v, bg_v, size_v);
            FDigito_Dibujado = 1;
        }
        else
        {
            DrawDigit(x_v, y_v-(12*size_v), 1, bg_v, bg_v, size_v);
        }
    }
    if(D_3 != 0)
    {
        FDigito_Dibujado = 1;
    }
    if(D_2 != D_2_BU)
    {
        if(D_2!=0 || FDigito_Dibujado ==1)
        {
            DrawDigit(x_v, y_v-(6*size_v), D_2, color_v, bg_v, size_v);
            FDigito_Dibujado = 1;
        }
        else
        {
            DrawDigit(x_v, y_v-(6*size_v), 1, bg_v, bg_v, size_v);
        }
    }
    if(D_1 != D_1_BU)
    {
        DrawDigit(x_v, y_v, D_1, color_v, bg_v, size_v);
    }
}

void SetPointer(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{

}

void FloodDB(uint16_t color [36])
{
    uint8_t  i, hi, lo;
    uint8_t  len = 36;
    RFIE=0;
    DISP_CS=0;
    DISP_CD=1;
    for(i=0 ;i == len; i++)
    {
        hi = color[i] >> 8;
        lo = color[i];
        SPIPut(hi);
        SPIPut(lo);
    }
    DISP_CS=1;
    RFIE=1;
}

void Plantilla(uint8_t Tipo)
{
    uint8_t i;
    Llenar_Pantalla(BLACK);
    switch(Tipo)
    {
        case(1):  // *** Plantilla con Variables.
        {

            Linea_Horizontal(222, 0, 275, DARKGREY);
            Linea_Horizontal(205, 0, 275, DARKGREY);
            Linea_Horizontal(188, 0, 275, DARKGREY);
            Linea_Horizontal(171, 0, 275, DARKGREY);
            Linea_Horizontal(154, 0, 275, DARKGREY);
            Linea_Horizontal(137, 0, 319, DARKGREY);
            Linea_Horizontal(136, 0, 319, DARKGREY);
            Linea_Horizontal(135, 0, 319, DARKGREY);
            Linea_Horizontal(50, 0, 319, BLUE);
            Linea_Horizontal(49, 0, 319, BLUE);
            Linea_Horizontal(48, 0, 319, BLUE);
            Linea_Vertical(137, 161, 102, DARKGREY);
            Linea_Vertical(137, 275, 102, DARKGREY);
            Linea_Vertical(137, 276, 102, DARKGREY);
            Linea_Vertical(137, 277, 102, DARKGREY);
            Linea_Vertical(0, 64, 50, BLUE);
            Linea_Vertical(0, 128, 50, BLUE);
            Linea_Vertical(0, 192, 50, BLUE);
            Linea_Vertical(0, 256, 50, BLUE);
            break;
        }
        case(2):        //Cuadricula 10 x 10
        {
            i=0;
            while (i<24)
            {
                Linea_Horizontal((i*10), 0, 320, WHITE);
                i++;
            }
            i=0;
            while (i<32)
            {
                Linea_Vertical(0, (i*10), 240, WHITE);
                i++;
            }
            break;
        }
        case(3):        //Cuadricula 50 x 50
        {
            i=0;
            while (i<5)
            {
                Linea_Horizontal((i*50), 0, 320, WHITE);
                i++;
            }
            i=0;
            while (i<7)
            {
                Linea_Vertical(0, (i*50), 240, WHITE);
                i++;
            }
            break;
        }
        case(MENU_HOME):
        {
            Linea_Horizontal(75, 0, 319, WHITE);
            Linea_Horizontal(150, 0, 319, WHITE);
            Linea_Vertical(0, 80, 239, WHITE);
            Linea_Vertical(0, 160, 239, WHITE);
            Linea_Vertical(0, 240, 239, WHITE);
            DrawChar( 200, 10, 'M', GREEN, BLACK, 1);    //Botón de Monitor de Recursos
            DrawChar( 200, 16, 'o', GREEN, BLACK, 1);
            DrawChar( 200, 22, 'n', GREEN, BLACK, 1);
            DrawChar( 200, 28, 'i', GREEN, BLACK, 1);
            DrawChar( 200, 34, 't', GREEN, BLACK, 1);
            DrawChar( 200, 40, 'o', GREEN, BLACK, 1);
            DrawChar( 200, 46, 'r', GREEN, BLACK, 1);
            DrawChar( 200, 58, 'd', GREEN, BLACK, 1);
            DrawChar( 200, 64, 'e', GREEN, BLACK, 1);
            DrawChar( 180, 16, 'R', GREEN, BLACK, 1);
            DrawChar( 180, 22, 'e', GREEN, BLACK, 1);
            DrawChar( 180, 28, 'c', GREEN, BLACK, 1);
            DrawChar( 180, 34, 'u', GREEN, BLACK, 1);
            DrawChar( 180, 40, 'r', GREEN, BLACK, 1);
            DrawChar( 180, 46, 's', GREEN, BLACK, 1);
            DrawChar( 180, 52, 'o', GREEN, BLACK, 1);
            DrawChar( 180, 58, 's', GREEN, BLACK, 1);
            DrawChar( 200, 99, 'P', RED, BLACK, 1);    //Botón de Ping de Cobertura
            DrawChar( 200, 105, 'i', RED, BLACK, 1);
            DrawChar( 200, 111, 'n', RED, BLACK, 1);
            DrawChar( 200, 117, 'g', RED, BLACK, 1);
            DrawChar( 200, 129, 'd', RED, BLACK, 1);
            DrawChar( 200, 135, 'e', RED, BLACK, 1);
            DrawChar( 180, 93, 'C', RED, BLACK, 1);
            DrawChar( 180, 99, 'o', RED, BLACK, 1);
            DrawChar( 180, 105, 'b', RED, BLACK, 1);
            DrawChar( 180, 111, 'e', RED, BLACK, 1);
            DrawChar( 180, 117, 'r', RED, BLACK, 1);
            DrawChar( 180, 123, 't', RED, BLACK, 1);
            DrawChar( 180, 129, 'u', RED, BLACK, 1);
            DrawChar( 180, 135, 'r', RED, BLACK, 1);
            DrawChar( 180, 141, 'a', RED, BLACK, 1);
            DrawChar( 180, 256, 'E', BLUE, BLACK, 2);   //Botón de EXIT
            DrawChar( 180, 268, 'X', BLUE, BLACK, 2);
            DrawChar( 180, 280, 'I', BLUE, BLACK, 2);
            DrawChar( 180, 292, 'T', BLUE, BLACK, 2);
            break;
        }
        case(MENU_MONITOR):
        {
            break;
        }
        case(MENU_RASTREO):
        {
            FillRect( 180, 10, 30, 60, BLACK);          //Borro las letras del boton 1
            FillRect( 180, 90, 30, 60, BLACK);          //Borro las letras del boton 2
            FillRect( 180, 270, 30, 60, BLACK);         //Borro las letras del boton 4
            Linea_Horizontal(75, 240, 80, WHITE);
            Linea_Horizontal(150, 240, 80, WHITE);
            Linea_Vertical(0, 80, 219, WHITE);
            Linea_Vertical(0, 160, 219, WHITE);
            Linea_Vertical(0, 240, 219, WHITE);
            Linea_Vertical(0, 40, 199, DARKGREY);
            Linea_Vertical(0, 120, 199, DARKGREY);
            Linea_Vertical(0, 200, 199, DARKGREY);
            Linea_Horizontal(199, 0, 239, DARKGREY);
            Linea_Horizontal(179, 0, 239, DARKGREY);
            DrawChar( 180, 268, 'O', GREEN, BLACK, 2);   //Botón de ON/OFF
            DrawChar( 180, 280, 'N', GREEN, BLACK, 2);
            DrawChar( 115, 265, 'R', YELLOW, BLACK, 1);   //Botón de EXIT
            DrawChar( 115, 271, 'E', YELLOW, BLACK, 1);
            DrawChar( 115, 277, 'S', YELLOW, BLACK, 1);
            DrawChar( 115, 283, 'E', YELLOW, BLACK, 1);
            DrawChar( 115, 289, 'T', YELLOW, BLACK, 1);
            DrawChar( 45, 256, 'H', BLUE, BLACK, 2);
            DrawChar( 45, 268, 'O', BLUE, BLACK, 2);
            DrawChar( 45, 280, 'M', BLUE, BLACK, 2);
            DrawChar( 45, 292, 'E', BLUE, BLACK, 2);

            DrawChar( 201, 5, '1', WHITE, BLACK, 2);
            DrawChar( 201, 85, '2', WHITE, BLACK, 2);
            DrawChar( 201, 165, '3', WHITE, BLACK, 2);

            DrawChar( 190, 10, 'R', BLUE, BLACK, 1);
            DrawChar( 190, 16, 'S', BLUE, BLACK, 1);
            DrawChar( 190, 22, 'S', BLUE, BLACK, 1);
            DrawChar( 190, 28, 'I', BLUE, BLACK, 1);
            DrawChar( 190, 52, 'L', WHITE, BLACK, 1);
            DrawChar( 190, 58, 'Q', WHITE, BLACK, 1);
            DrawChar( 190, 64, 'I', WHITE, BLACK, 1);
            DrawChar( 190, 90, 'R', BLUE, BLACK, 1);
            DrawChar( 190, 96, 'S', BLUE, BLACK, 1);
            DrawChar( 190, 102, 'S', BLUE, BLACK, 1);
            DrawChar( 190, 108, 'I', BLUE, BLACK, 1);
            DrawChar( 190, 132, 'L', WHITE, BLACK, 1);
            DrawChar( 190, 138, 'Q', WHITE, BLACK, 1);
            DrawChar( 190, 144, 'I', WHITE, BLACK, 1);
            DrawChar( 190, 170, 'R', BLUE, BLACK, 1);
            DrawChar( 190, 176, 'S', BLUE, BLACK, 1);
            DrawChar( 190, 182, 'S', BLUE, BLACK, 1);
            DrawChar( 190, 188, 'I', BLUE, BLACK, 1);
            DrawChar( 190, 212, 'L', WHITE, BLACK, 1);
            DrawChar( 190, 218, 'Q', WHITE, BLACK, 1);
            DrawChar( 190, 224, 'I', WHITE, BLACK, 1);

            DrawChar( 180, 10, 'T', RED, BLACK, 1);
            DrawChar( 180, 16, 'x', RED, BLACK, 1);
            DrawChar( 180, 28, 'R', GREEN, BLACK, 1);
            DrawChar( 180, 34, 'x', GREEN, BLACK, 1);
            DrawChar( 180, 43, 'T', RED, BLACK, 1);
            DrawChar( 180, 49, 'x', RED, BLACK, 1);
            DrawChar( 180, 61, 'R', GREEN, BLACK, 1);
            DrawChar( 180, 67, 'x', GREEN, BLACK, 1);
            DrawChar( 180, 90, 'T', RED, BLACK, 1);
            DrawChar( 180, 96, 'x', RED, BLACK, 1);
            DrawChar( 180, 108, 'R', GREEN, BLACK, 1);
            DrawChar( 180, 114, 'x', GREEN, BLACK, 1);
            DrawChar( 180, 123, 'T', RED, BLACK, 1);
            DrawChar( 180, 129, 'x', RED, BLACK, 1);
            DrawChar( 180, 141, 'R', GREEN, BLACK, 1);
            DrawChar( 180, 147, 'x', GREEN, BLACK, 1);
            DrawChar( 180, 170, 'T', RED, BLACK, 1);
            DrawChar( 180, 176, 'x', RED, BLACK, 1);
            DrawChar( 180, 188, 'R', GREEN, BLACK, 1);
            DrawChar( 180, 194, 'x', GREEN, BLACK, 1);
            DrawChar( 180, 203, 'T', RED, BLACK, 1);
            DrawChar( 180, 209, 'x', RED, BLACK, 1);
            DrawChar( 180, 221, 'R', GREEN, BLACK, 1);
            DrawChar( 180, 227, 'x', GREEN, BLACK, 1);
            break;
        }
        case(MENU_EXIT):
        {

        }
    }
}


#endif// Libreria Display

