/*
 * sl868a.cpp
 *
 * Created: 05/05/2015 22:15:38
 *  Author: speirano
 */
 #include <Arduino.h>
 #include "sl868a.h"
 #include <Uart.h>
 #include "sl868aModel.h"
 #include "sl868aParser.h"
 #include "smeErrorCode.h"


static bool cold_boot = true;

void
Sl868a::setStandby(void)
{
     // set GPS in standby
     print(SL868A_SET_STDBY_CMD);
}

 void
 Sl868a::setWarmRestart(void)
 {
      // Power On the GPS module
     print(SL868A_WARM_RST_CMD);
 }

 void
 Sl868a::processGpsRxMsg(void)
 {
     sl868a_nmea_out_msg_id nmea_msg_id;

     if (cold_boot) {
         cold_boot = false;
     }

     if (msgPtrT.messageType == STD_NMEA) {
         nmea_msg_id = sl868aIdentifyNmeaRxMsg(msgPtrT.nmea_p.std_p.talker_p,
                                               msgPtrT.nmea_p.std_p.sentenceId_p);
         switch(nmea_msg_id) {
             case NMEA_RMC:
             sme_parse_coord(msgPtrT.nmea_p.std_p.data_p, msgPtrT.nmea_p.std_p.dataLenght, SME_LAT);
             sme_parse_coord(msgPtrT.nmea_p.std_p.data_p, msgPtrT.nmea_p.std_p.dataLenght, SME_LONG);
             break;

             case NMEA_GGA:
             sl868a_parse_gga(msgPtrT.nmea_p.std_p.data_p, msgPtrT.nmea_p.std_p.dataLenght);
             break;

             case NMEA_UNMANAGED:
             default:
             break;
         }
     }
 }



 uint8_t
 Sl868a::handleGpsRxData(uint8_t inChar)
 {
     if (rxMsg.idx < (SL868A_MAX_MSG_LEN-2)) {

         if ((inChar < 21) && !((inChar == '\n') || (inChar == '\r')))  {
             return SME_OK;
             }  else if (inChar == '$') {
             memset(&rxMsg, 0, sizeof(rxMsg));
         }

         rxMsg.data[rxMsg.idx++] = inChar;

         if (inChar == '\n')  {
             if (rxMsg.idx > 3) {
                 rxMsg.data[rxMsg.idx] = '\0';
                 if (crcCheck(rxMsg.data, rxMsg.idx)) {
                    parseGpsRxMsg();
                    processGpsRxMsg();
                 }
             }
         }
         } else {
         memset(&rxMsg, 0, sizeof(rxMsg));
     }

     return SME_OK;
 }


 int
 Sl868a::print(const char *msg)
 {
    GPS.print((const char*)msg);
    return SME_OK;
 }


 void
 Sl868a::readData(void)
 {
     while (GPS.available()) {
         // get the new byte:
         char inChar = (char)GPS.read();
         handleGpsRxData(inChar);
     }
 }

 /****************************************************************************/
 /*                               Public APIs                                */
 /****************************************************************************/

  const bool
  Sl868a::ready(void)
  {
     readData();
     return _ready;
  }

 Sl868a::Sl868a(void){
     _ready = false;
 }

 void Sl868a::begin (void) {
     GPS.begin(9600);
 }


int
Sl868a::getLatitudeDegrees()
{
    readData();
    return (isNorthLatitude() ? _data.lat_deg : -_data.lat_deg);
}

unsigned long
Sl868a::getLatitudeDecimals()
{
    readData();
    return (unsigned long) _data.lat_decimals;
}

double
Sl868a::getLatitude()
{
    readData();

    double lat = _data.lat_decimals;

    lat = (lat/LAT_LONG_DEC_UNIT);
    lat += _data.lat_deg;
    if (!isNorthLatitude()) {
       lat = -lat;
    }
    return lat;
}

bool
Sl868a::isNorthLatitude()
{
    readData();
    return (bool) _data.lat_direction;
}

int
Sl868a::getLongitudeDegrees()
{
    readData();
    return (isEastLongitude() ? _data.longit_deg : -_data.longit_deg);
}

unsigned long
Sl868a::getLongitudeDecimals()
{
    readData();
    return (unsigned long) _data.longit_decimals;
}

bool
Sl868a::isEastLongitude()
{
    readData();
    return (bool) _data.longit_direction;
}

double
Sl868a::getLongitude()
{
    readData();

    double longit = _data.longit_decimals;

    longit = (longit/LAT_LONG_DEC_UNIT);
    longit += _data.longit_deg;

    if (!isEastLongitude()) {
        longit = -longit;
    }
    return longit;
}

unsigned int
Sl868a::getAltitude()
{
    readData();
    return _data.altitude;
}

unsigned char
Sl868a::getLockedSatellites()
{
    readData();
    return _data.n_satellites;
}


Sl868a  smeGps;
