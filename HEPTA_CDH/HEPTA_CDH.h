#ifndef MBED_HEPTACDH_H
#define MBED_HEPTACDH_H
#include "SDFileSystem.h"
#include "mbed.h"

class HEPTA_CDH : public SDFileSystem{
public:
    
    HEPTA_CDH(PinName mosi, PinName miso, PinName sclk, PinName cs, const char* name);
  
private:  
};

#endif