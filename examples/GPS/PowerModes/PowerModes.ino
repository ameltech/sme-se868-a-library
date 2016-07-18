/*
    SmeGPS Library - Power modes

    This example shows how to trigger the GPS module power status:
    Standby, Warm, Cold and Hot restarts.

    It shows also the Latitute / Longitude data once the position is locked.
    This information is useful to understand the power consumption and the 
    time to collect the localization information

    created 19 Dec 2015
    by Seve (seve@axelelettronica.it)

    This example is in the public domain
    https://github.com/ameltech

    SE868  more information available here:
    http://www.telit.com/products/product-service-selector/product-service-selector/show/product/jupiter-se868-as/
 */

#include <Wire.h>

#include <sl868a.h>
#include <Arduino.h>

int led_status = HIGH;

typedef enum   {
    GPS_STANDBY,
    GPS_WARM,
    GPS_HOT,
    GPS_COLD,

} gpsStatus_t;


const unsigned char LOOP_DELAY     = 5;   // 5 ms
const unsigned int  MS_IN_S        = 1000;

const unsigned int  PRINT_PERIOD   = 2*(MS_IN_S)/LOOP_DELAY; // 2 sec  

gpsStatus_t gpsStatus = GPS_HOT;

// Print everi PRIN_PERIOD Cycles 
#define checkToPrint(loop)  ((loop%PRINT_PERIOD) == 0)



// the setup function runs once when you press reset or power the board
void setup() {
    SerialUSB.begin(115200);
    smeGps.begin();

    while (!SerialUSB) {
        ;
    }

    SerialUSB.println("Press one of the following keys to change Power Status:");
    SerialUSB.println("- 's'  : GPS to STANDBY power mode");
    SerialUSB.println("- 'w'  : GPS WARM Restart");
    SerialUSB.println("- 'c'  : GPS COLD Restart");
    SerialUSB.println("- 'h'  : GPS HOT Restart");
}

void printGpsData(void) 
{
    unsigned int altitude          = smeGps.getAltitude();
    double latitude                = smeGps.getLatitude();
    double longitude               = smeGps.getLongitude();
    unsigned char lockedSatellites = smeGps.getLockedSatellites();

    SerialUSB.println(" ");
    SerialUSB.print("Latitude    =  ");
    SerialUSB.println(latitude, 6);
    SerialUSB.print("Longitude   =  ");
    SerialUSB.println(longitude, 6);
    SerialUSB.print("Altitude    =  ");
    SerialUSB.println(altitude, DEC);
    SerialUSB.print("Satellites  =  ");
    SerialUSB.println(lockedSatellites, DEC);
}



// the loop function runs over and over again forever
void loop() 
{
    static unsigned print_loop_cnt = 0;

    if(SerialUSB.available()) { 
        // get the new byte: 
        char inChar = (char)SerialUSB.read(); 

        switch (inChar) {
        case 's':  gpsStatus = GPS_STANDBY;
        SerialUSB.println("Set GPS to Standby...");
        smeGps.setStandby();
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
        ledBlueLight(HIGH);
        delay(500);
        ledBlueLight(LOW);
#endif
        break;
        case 'h':  gpsStatus = GPS_HOT;
        SerialUSB.println("GPS Hot Restart...");
        smeGps.setHotRestart();
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
        ledBlueLight(HIGH);
        ledRedLight(HIGH);
        delay(500);
        ledBlueLight(LOW);
        ledRedLight(LOW);
#endif
        break;
        case 'w':  gpsStatus = GPS_WARM;
        SerialUSB.println("GPS Warm Restart...");
        smeGps.setWarmRestart();
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
        ledGreenLight(HIGH);
        delay(500);
        ledGreenLight(LOW);
#endif
        break;
        case 'c':  gpsStatus = GPS_COLD;
        SerialUSB.println("GPS Cold Restart...");
        smeGps.setColdRestart();  
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
        ledRedLight(HIGH);
        delay(500);
        ledRedLight(LOW);
#endif
        break;
        default:
            break;
        }
    } 

    if (smeGps.ready()) {
        if (checkToPrint(print_loop_cnt)) {
            printGpsData();
        }
    }


    delay(LOOP_DELAY);
    print_loop_cnt++;
}
