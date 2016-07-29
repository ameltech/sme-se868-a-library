/*
 * sl868a.h
 *
 * Created: 05/05/2015 22:14:39
 * Author : Seve (seve@axelelettronica.it)
 */


#ifndef SL868A_H_
#define SL868A_H_

#include "gps/sl868aModel.h"


#define LAT_LONG_DEC_UNIT 1000000



typedef struct {
    uint16_t  lat_deg;
    uint32_t lat_decimals;  // always 6 digit, measurement = 1/100000
    uint8_t  lat_direction; // 1= north/+,  0 = South/-
    uint16_t  longit_deg;
    uint32_t longit_decimals;  //always 6 digit, measurement = 1/100000
    uint8_t  longit_direction; // 1= East/+, 0 = West/-
    uint16_t altitude;
    uint8_t  quality;
    uint8_t  n_satellites;
}sl868aCachedDataT;


class Sl868a {
public:

    Sl868a(void);
    virtual ~Sl868a(){};

private:
    Uart *gpsComm;
    bool crcCheck(uint8_t data[], uint8_t len);
    uint8_t handleGpsRxData(uint8_t inChar);
    void parseGpsRxMsg (void);
    void processGpsRxMsg(void);

    bool sme_gps_position_fixed() {
        return (_data.quality && (_data.n_satellites > 4));
    }
    bool dataReady() {return _ready;}

    bool _ready;
    sl868aCachedDataT _data;


    int parseNmeaCommandToken(sl868aT *usartMsg);
    sl868a_nmea_out_msg_id sl868aIdentifyNmeaRxMsg(uint8_t talker_p[], uint8_t sentenceId_p[]);
    int getTalkerType(uint8_t *in, sl868aMsgE *nmeaType);
    int getSentenceId(uint8_t *in, uint8_t *out);
    uint8_t * sl868a_parse_param_offset(uint8_t in[], uint8_t in_len, uint8_t comma_num);
    void sme_parse_coord(uint8_t in[], uint8_t in_len, sme_coord_t type);
    void sl868a_parse_gga(uint8_t in[], uint8_t in_len);
    void readData(void);
    int print(const char *msg);
    sl868aCachedDataT & getChachedDataPtr() {return _data;}
    int getLatitudeDegrees();
    unsigned long getLatitudeDecimals();
    bool isNorthLatitude();
    int getLongitudeDegrees();
    unsigned long getLongitudeDecimals();
    bool isEastLongitude();

// library API
public:

    #ifdef ARDUINO_SAMD_SMARTEVERYTHING
    void begin(Uart *serial=&GPS);
    #elif defined (ASME3_REVISION)
    void begin(Uart *serial=&GPS);
    #else
    void begin(Uart *serial=&Serial1);
    #endif
    void setStandby();
    void setWarmRestart();
    void setHotRestart();
    void setColdRestart();
    const bool ready(void);
    unsigned int getAltitude();
    double getLatitude();
    double getLongitude();


    unsigned char getLockedSatellites();
};


extern Sl868a  smeGps;


#endif /* SL868A_H_ */
