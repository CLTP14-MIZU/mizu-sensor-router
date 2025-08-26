#include "HEPTA_CDH.h"
HEPTA_CDH::HEPTA_CDH(PinName mosi, PinName miso, PinName sclk, PinName cs, const char* name) : SDFileSystem( mosi, miso, sclk, cs, name){
}
