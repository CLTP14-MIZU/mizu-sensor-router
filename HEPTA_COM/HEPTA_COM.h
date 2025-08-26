#ifndef MBED_HEPTAXBEE_H
#define MBED_HEPTAXBEE_H
#include "mbed.h"

class HEPTA_COM
{
public:
    RawSerial xbee;
    HEPTA_COM(PinName tx, PinName rx, int rate);
    int rcmd;
    int cmdflag;
//    void baud(int rate);
    void xbee_receive(int *xrcmd, int *xcmdflag);
    void initialize();
    void commandget();
    // void xbee_transmit(char dataA[],char dataB[],char dataC[],char dataD[],char dataE[],
    //                    char dataF[],char dataG[],char dataH[],char dataI[],char dataJ[],
    //                    char dataK[],char dataL[],char dataM[],char dataN[]);
    void puts( char *s );
    void printf(const char *format, ... );
    void putc(char data);
    char getc();

private:
    // RawSerial xbee;
};

#endif