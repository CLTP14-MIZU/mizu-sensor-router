#ifndef MBED_HEPTASENSOR_H
#define MBED_HEPTASENSOR_H
#include "HeptaCamera_GPS.h"
#include "Hepta9axis.h"
#include "HeptaTemp.h"

class HEPTA_SENSOR : public HeptaCamera_GPS ,public Hepta9axis, public HeptaTemp
{
public:
    HEPTA_SENSOR(
        PinName adcon,
        PinName sda,PinName scl,int aaddr,int agaddr, int amaddr,
        PinName tx,PinName rx,PinName sw1,PinName sw2
    );

private:
};

#endif