#include "HEPTA_SENSOR.h"
HEPTA_SENSOR::HEPTA_SENSOR(PinName adcon,
        PinName sda,PinName scl,int aaddr,int agaddr, int amaddr,
        PinName tx,PinName rx,PinName sw1,PinName sw2): HeptaTemp(adcon),Hepta9axis(sda,scl,aaddr,agaddr,amaddr),HeptaCamera_GPS(tx,rx,sw1,sw2){
}