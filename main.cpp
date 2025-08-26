/**
* Mizu Sensor Hub - Continuous Telemetry Logger
* ------------------------------------------------
* This program reads telemetry from multiple sensors on the HEPTA platform and
* transmits a single, concise key=value line each second over the COM link.
*
* Output format (single line per sample, ends with CRLF):
*   device_id=MIZU_0001,ambient_temp=25.5,humidity=60.2,soil_moisture=45.8,soil_temp=22.1,wind_speed=5.2,longitude=-122.4194,latitude=37.7749\r\n
*
* Notes:
* - Values not yet available from hardware (e.g., soil moisture, soil temp, wind speed)
*   are filled with dummy placeholders; replace with real sensor reads when available.
* - All legacy com.printf statements are preserved but commented out for reference.
* - Variable names are intentionally descriptive for maintainability.
*/
 
#include "mbed.h"
#include "Dht11.h"
#include "HEPTA_EPS.h"
#include "HEPTA_SENSOR.h"
#include "HEPTA_COM.h"
 
// Serial debug port (left defined but unused here; keep if you want local USB debug)
// RawSerial pc(USBTX,USBRX,9600);
Serial pc(USBTX, USBRX);
 
// HEPTA drivers
HEPTA_COM     comms_link(p9, p10, 9600);                 // TX, RX, baud
HEPTA_EPS     power_system(p16, p26);                    // EPS control/voltage sense
HEPTA_SENSOR  hepta_sensors(p17,                         // MPU6050 INT / general INT
                            p28, p27, 0x19, 0x69, 0x13,  // I2C pins & addresses
                            p13, p14, p25, p24);         // GPS UART & other pins
Dht11         dht11_env_sensor(p15);                     // DHT11 data pin
AnalogIn      soil_moisture_sensor(p18);                 // Soil moisture analog input
 
// Wall-clock since boot (for engineering logs/timestamps if needed)
Timer system_uptime;
 
/*--------------------------- Configuration ----------------------------------*/
static const char* kDeviceId = "MIZU_0001";
 
// Dummy placeholders for sensors not yet integrated (replace when real sensors are added)
static const float kDummyWindSpeedMs         = 5.2f;   // [m/s]
 
/*--------------------------- Main Program -----------------------------------*/
int main() {
    // Start uptime timer
    system_uptime.start();
 
    // Initialize/prepare GPS
    hepta_sensors.gps_setting();
 
    // ----------------------- Telemetry working vars -------------------------
    // Electrical
    float battery_voltage_v = 0.0f;          // [V]
 
    // Board internal temperature (from HEPTA sensor block)
    float board_temp_c = 0.0f;               // [°C]
 
    // Environment from DHT11
    float ambient_temp_c = 0.0f;             // [°C]
    float relative_humidity_percent = 0.0f;  // [%]
 
    // GPS fields (as required by HEPTA gga_sensing)
    int   gps_quality_flag = 0;
    int   gps_sat_count    = 0;
    int   gps_fix_check    = 0;              // 0/1 means data available in this API
    char  gps_ns_indicator = 'N';            // 'N' or 'S'
    char  gps_ew_indicator = 'E';            // 'E' or 'W'
    char  gps_alt_unit     = 'm';
    float gps_time_utc     = 0.0f;           // HHMMSS.sss (float from library)
    float gps_latitude_deg = 0.0f;           // decimal degrees
    float gps_longitude_deg= 0.0f;           // decimal degrees
    float gps_horz_acc_m   = 0.0f;
    float gps_altitude_m   = 0.0f;
 
    // One-time startup message (commented as requested)
    // comms_link.printf("Mizu Sensor Hub Running\r\n");
 
    // Continuous logging loop
    while (true) {
        /*------------------ Read EPS (battery voltage) ----------------------*/
        power_system.vol(&battery_voltage_v);
        // comms_link.printf("MizuSensorHub::Battery Vol = %.2f [V]\r\n", battery_voltage_v);
 
        /*------------------ Read on-board temperature -----------------------*/
        hepta_sensors.temp_sense(&board_temp_c);
        // comms_link.printf("Time = %.2f [s] temp = %f [deg]\r\n", system_uptime.read(), board_temp_c);
         
        /*------------------ Read DHT11 env sensor ---------------------------*/
        // DHT11 library reports integer Fahrenheit & humidity; convert to float °C
        dht11_env_sensor.read();
        int ambient_temp_f_int = dht11_env_sensor.getFahrenheit();
        int humidity_int       = dht11_env_sensor.getHumidity();
        ambient_temp_c = (static_cast<float>(ambient_temp_f_int) - 32.0f) / 1.8f;
        relative_humidity_percent = static_cast<float>(humidity_int);
        // comms_link.printf("Ambient Temperature: %d, Humidity: %d\r\n", ambient_temp_f_int, humidity_int);


        /*------------------ Read Soil Moisture sensor ---------------------------*/
        float soil_moisture_percent = soil_moisture_sensor.read() * 100.0f;
 
        /*------------------ Read GPS (GPGGA fields) -------------------------*/
        hepta_sensors.gga_sensing(&gps_time_utc,
        &gps_latitude_deg,  &gps_ns_indicator,
        &gps_longitude_deg, &gps_ew_indicator,
        &gps_quality_flag,  &gps_sat_count,
        &gps_horz_acc_m,    &gps_altitude_m,
        &gps_alt_unit,      &gps_fix_check);
        // if ((gps_fix_check == 0) || (gps_fix_check == 1)) {
        //     comms_link.printf("GPGGA,%f,%f,%c,%f,%c,%d,%d,%f,%f,%c\r\n",
        //                       gps_time_utc, gps_latitude_deg, gps_ns_indicator,
        //                       gps_longitude_deg, gps_ew_indicator,
        //                       gps_quality_flag, gps_sat_count,
        //                       gps_horz_acc_m, gps_altitude_m, gps_alt_unit);
        // }
 
        /*------------------ Single consolidated output ----------------------*/
        // IMPORTANT: Keep as a single printf. Always end with CRLF (\r\n).
        // Uses live values for ambient_temp, humidity, latitude, longitude.
        // Soil moisture/temp and wind speed are dummy placeholders for now.
        comms_link.printf(
            "device_id=%s,ambient_temp=%.2f,humidity=%.2f,soil_moisture=%.1f,soil_temp=%.1f,wind_speed=%.1f,longitude=%.6f,latitude=%.6f\r\n",
            kDeviceId,
            ambient_temp_c,
            relative_humidity_percent,
            soil_moisture_percent,
            board_temp_c,
            kDummyWindSpeedMs,
            gps_longitude_deg,
            gps_latitude_deg
        );
 
        /*------------------ Loop cadence ------------------------------------*/
        wait(1.0f); // 1 Hz logging
    }
}