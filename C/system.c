#include "../H/system.h"
#include "../H/system_config.h"
#include <plib/timers.h>

//Configuration parameters
//#pragma config OSC = HS
//#pragma config OSCS = OFF
//#pragma config WDT = OFF
#pragma config LVP = OFF

BYTE    LED;

void SYSTEM_Initialize(void)
{

    // primary internal oscillator
    OSCCON = 0x00;
    WDTCONbits.SWDTEN = 0;

    INTCON = 0;


    TRISA = 0xF0;//PORTA,(0-3) = LEDs de CHKPNT
    TRISB = 0xF7;//PORTB,3 = SC
    TRISC = 0x97;//PORTC,5 = SDO ; PORTC,3 = SCK ; PORTC,6 = TX
    TRISD = 0xFF;
    TRISE = 0xFF;

    TXSTA = 0X24;
    RCSTA = 0X90;
    SPBRG = 0X81;


    
    LATA = 0;
    LATB = 0;
    //LATC = 0;
    LATD = 0;
    LATE = 0;

    //Push button definitions
    LED_1_TRIS = 0;
    LED_2_TRIS = 0;

    //RF CS, RESET, INT pin definitions
    PHY_CS_TRIS = 0;
    PHY_CS = 1;
    PHY_RESETn_TRIS = 0;
    PHY_RESETn = 1;
    RF_INT_TRIS = 1;
    
    #if defined(HARDWARE_SPI)
        SSPSTAT = 0xC0;
        SSPCON1 = 0x20;
    #else
        SPI_SDO = 0;
        SPI_SCK = 0;
    #endif
   //Wake pin definitions
    PHY_WAKE_TRIS = 0;
    PHY_WAKE = 1;

    // enable INT effective edge
    INTCON2bits.INTEDG1 = 0;

    
    //Enable Interrupts
    INTCONbits.GIEH = 1;
    RFIF = 0;
    RFIE = 1;
    // CONFIGURACION DE LA USART
    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;


}

void UserInterruptHandler()
{
    if (PIR1bits.RCIF == 1)
    {
                FLAGS = 0;

            INTCONbits.GIEH = 0;
            NID = RCREG;

            while (!PIR1bits.RCIF)
            {
            }

         FN = RCREG;

            while (!PIR1bits.RCIF)
            {
            }

         DATA = RCREG;

             while (!PIR1bits.RCIF)
            {
            }

         DAUX = RCREG;

         FLAGS = 1;

            INTCONbits.GIEH = 1;
    }
    if(INTCONbits.TMR0IF == 1)
    {
        if (LED == 1)
        {
             LATAbits.LATA1 = 0;
            LED = 0;
        }
        else
        {
             LATAbits.LATA1 = 1;
            LED = 1;
        }
            INTCONbits.TMR0IF = 0;
        WriteTimer0(0xD9DA); //Please use HEX. Decimal don't work

    }

}



    

