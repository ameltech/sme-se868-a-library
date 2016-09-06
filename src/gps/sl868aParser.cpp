/*
 * sl868aParser.cpp
 *
 * Created: 06/05/2015 22:48:50
 * Author : Seve (seve@axelelettronica.it)
  * Modified 06/09/2016
 * Author Gabriel (gabriel.de.fombelle@gmail.com)
 * provide UTC time and date, speed and course
 */
#include <Arduino.h>
#include "sl868a.h"
#include "sl868aModel.h"
#include "sl868aParser.h"
#include "smeErrorCode.h"

sl868aPtrT  msgPtrT;
sl868aRxMsgT rxMsg;

/*****************************************************************************/
/*                       Parsing commands from sl868a                        */
/*****************************************************************************/

sl868a_nmea_out_msg_id
Sl868a::sl868aIdentifyNmeaRxMsg(uint8_t talker_p[], uint8_t sentenceId_p[])
{
     if ((talker_p[0] == 'G') && ((talker_p[1] == 'P')||
     (talker_p[1] == 'L') || (talker_p[1] == 'N'))) {
         if ((sentenceId_p[0] == 'R') &&
         (sentenceId_p[1] == 'M') &&
         (sentenceId_p[2] == 'C')) {
             return NMEA_RMC;
         }
         if ((sentenceId_p[0] == 'G') &&
         (sentenceId_p[1] == 'G') &&
         (sentenceId_p[2] == 'A')) {
             return NMEA_GGA;
         }
     }

     return NMEA_UNMANAGED;
 }


int
Sl868a::getTalkerType(uint8_t *in, sl868aMsgE *nmeaType)
{
     if (!in || !nmeaType) {
         return SME_EINVAL;
     }
     if ((((in[0]=='G') && ((in[1]=='P') || (in[1]=='L') || (in[1]=='N'))) ||
     ((in[0]=='B') && (in[1]=='D'))))
     {
         *nmeaType = STD_NMEA;
         return SME_OK;
         } else if ((in[0]=='P') && (in[1]=='M') && (in[2]=='T') && (in[3]=='K')) {
         *nmeaType = MTK_NMEA;
         return SME_OK;
     }

     return SME_EINVAL;
 }

 int
 Sl868a::getSentenceId(uint8_t *in, uint8_t *out)
 {
     bool found = false;

     if (!in || !out) {
         return SME_EINVAL;
     }
     //print_dbg("sentence %c %c %c  \n", in[0], in[1], in[2]);

     if ((in[0]=='G') && (in[1]=='G') && (in[2]=='A')) {
         found = true;
         goto end;
     }
     if ((in[0]=='G') && (in[1]=='L') && (in[2]=='L')) {
         found = true;
         goto end;
     }
     if ((in[0]=='G') && (in[1]=='S') && (in[2]=='A')) {
         found = true;
         goto end;
     }
     if ((in[0]=='G') && (in[1]=='S') && (in[2]=='V')) {
         found = true;
         goto end;
     }
     if ((in[0]=='R') && (in[1]=='M') && (in[2]=='C')) {
         found = true;
         goto end;
     }
     if ((in[0]=='V') && (in[1]=='T') && (in[2]=='G')) {
         found = true;
         goto end;
     }
     if ((in[0]=='Z') && (in[1]=='D') && (in[2]=='A')) {
         found = true;
         goto end;
     }
end:
     if (found) {
         out[0] = in[0];
         out[1] = in[1];
         out[2] = in[2];
         return SME_OK;
     }
     return SME_EINVAL;
 }



uint8_t *
Sl868a::sl868a_parse_param_offset(uint8_t in[], uint8_t in_len, uint8_t comma_num)
{
    bool comma_found = false;
    uint8_t i = 0, j = 0;

    do {
        if (i >= in_len) {
            return NULL;
        }
        if (in[i] == ',') {
            j++;
        }
        if (j == comma_num) {
            comma_found = true;
        }
        i++;
    } while (!comma_found);

    if (comma_found) {
        return &in[i];
    }
    return NULL;
}



void
Sl868a::sme_parse_coord(uint8_t in[], uint8_t in_len, sme_coord_t type)
{
    uint8_t comma = 0, i = 0;
    uint16_t *deg_p = NULL;
    uint32_t *decimals_p = NULL;
    uint8_t *direction_p = NULL;
    uint8_t *ptr = NULL;
    uint8_t first_minute_idx = 0;
    uint64_t minutes_tmp = 0;
    uint64_t decimals_tmp = 0;

    sl868aCachedDataT &gpsRxData = smeGps.getChachedDataPtr();

    switch (type) {
        case SME_LAT:
            deg_p = &gpsRxData.lat_deg;
            decimals_p = &gpsRxData.lat_decimals;
            direction_p = &gpsRxData.lat_direction;
            comma = 3;
            // lat has deg in 2 chars format: ddmm.mmmmm
            first_minute_idx = SME_CTRL_LAT_MINUTES_START;

        break;
        case SME_LONG:
            deg_p = &gpsRxData.longit_deg;
            decimals_p = &gpsRxData.longit_decimals;
            direction_p = &gpsRxData.longit_direction;
            // lat has deg in 3 chars format: dddmm.mmmmm
            first_minute_idx = SME_CTRL_LONG_MINUTES_START;
            comma = 5;
        break;
        default:
        break;
            //print_err("No Lat/Long provided\n");
    }

    ptr = sl868a_parse_param_offset(in, in_len, comma);

    if (!ptr || (*ptr == ',')) {
        return; // data not valid
    }

    // degrees
    *deg_p = 0;
    for (i = 0; i < first_minute_idx; ++i) {
        *deg_p = ((*deg_p)*10) + (*ptr -'0'); // uint16_t
        ptr++;
    }

    minutes_tmp = 0;
    // minutes
    while (ptr && (*ptr != '.')) {
        minutes_tmp = ((minutes_tmp)*10) + (*ptr -'0'); // uint16_t
        ptr++;
    }
    // convert minutes in decimals *10000
    decimals_tmp = ((minutes_tmp * 10000)*10) /6;

    ptr++;

    // minutes decimals
    *decimals_p = 0;
    while (ptr && (*ptr != ',')) {
        *decimals_p = ((*decimals_p)*10) + (*ptr -'0'); // uint16_t
        ptr++;
    }
    // convert minute decimals from minutes in decimals
    *decimals_p = (*decimals_p)*10/6;

    // get the rest of the

    /* complete conversion
     * from:  DDmm.dddd (D = degree, m = minutes, d = decimals of minutes, all are in 1/60)
     * to  :  DDdddddd     [D are in 1/60, d are in decimals of Degree]
     */
     // calculate the whole 'dddddd'
     *decimals_p = *decimals_p + decimals_tmp;

     ptr++;
     // To be added and UT
     *direction_p = ((*ptr == 'N') || (*ptr == 'E')) ? 1 : 0;
}

void
Sl868a::sl868a_parse_gga(uint8_t in[], uint8_t in_len)
{
    uint8_t comma_alt = 9; // n. ',' before alt data
    uint8_t comma_nsat = 7;
    uint8_t comma_quality = 6;
	uint8_t comma_utc = 1;
    uint8_t *ptr;
    sl868aCachedDataT &gpsRxData = smeGps.getChachedDataPtr();
    ptr = sl868a_parse_param_offset(in, in_len, comma_quality);

    if (ptr && (*ptr != ',')) {
        gpsRxData.quality = *ptr-'0';  // ascii
    }

    ptr = sl868a_parse_param_offset(in, in_len, comma_nsat);
    gpsRxData.n_satellites = 0;
    while (ptr && (*ptr != ',')) {
        gpsRxData.n_satellites = (gpsRxData.n_satellites *10) + (*ptr -'0'); // uint16_t
        ptr++;
    }

    ptr = sl868a_parse_param_offset(in, in_len, comma_alt);
    gpsRxData.altitude = 0;
    while (ptr && (*ptr != '.') && (*ptr != ',')) {
        gpsRxData.altitude = (gpsRxData.altitude *10) + (*ptr -'0'); // uint16_t
        ptr++;
    }
	// utc
    
    ptr = sl868a_parse_param_offset(in, in_len, comma_utc);
    gpsRxData.utc_hour = 0;
    gpsRxData.utc_min = 0;
    gpsRxData.utc_sec = 0;
    gpsRxData.utc_sec_decimals = 0;
    unsigned char _cursor = 0;
    while (ptr &&  (*ptr != ',')) {
        if(_cursor<=1){
          gpsRxData.utc_hour = (gpsRxData.utc_hour *10) + (*ptr -'0');
        }
        if(_cursor>=2 &&_cursor <=3){
           gpsRxData.utc_min = (gpsRxData.utc_min *10) + (*ptr -'0');
        }
        if(_cursor >=4 && _cursor<=5){
            gpsRxData.utc_sec = (gpsRxData.utc_sec *10) + (*ptr -'0');         
        }
        if(_cursor>=7 && _cursor<=8){
            gpsRxData.utc_sec_decimals = (gpsRxData.utc_sec_decimals *10) + (*ptr -'0');         
 
        }
        _cursor++;
        ptr++;
    } 

    if (!_ready && gpsRxData.quality) {
        _ready = true;
    }
}

void
Sl868a::sl868a_parse_rmc(uint8_t in[], uint8_t in_len)
{
	uint8_t comma_speed = 7; // n. ',' before alt data
	uint8_t comma_course = 8;
	uint8_t comma_date = 9;
	uint8_t *ptr;
	sl868aCachedDataT &gpsRxData = smeGps.getChachedDataPtr();

	// utc date
	gpsRxData.utc_year = 0;
	gpsRxData.utc_month = 0;
	gpsRxData.utc_dayOfMonth = 0;
	gpsRxData.speed_knots = 0;
	gpsRxData.course = 0;
	
	ptr = sl868a_parse_param_offset(in, in_len, comma_date);
	unsigned char _cursor = 0;
	while (ptr &&  (*ptr != ',')) {
		if(_cursor<=1){
			gpsRxData.utc_dayOfMonth = (gpsRxData.utc_dayOfMonth *10) + (*ptr -'0');
		}
		if(_cursor>=2 &&_cursor <=3){
			gpsRxData.utc_month = (gpsRxData.utc_month *10) + (*ptr -'0');
		}
		if(_cursor >=4 && _cursor<=5){
			gpsRxData.utc_year = (gpsRxData.utc_year *10) + (*ptr -'0');
		}
		
		_cursor++;
		ptr++;
	}
	gpsRxData.utc_year+=2000;
	
	ptr = sl868a_parse_param_offset(in, in_len, comma_speed);
	_cursor = 0;
	bool decimal = false;
	int decimalPart=0;
	int precision = 0;
	while (ptr &&  (*ptr != ',')) {
		if(!decimal && *ptr=='.') decimal = true;
		if (!decimal)
		gpsRxData.speed_knots = (gpsRxData.speed_knots *10) + (*ptr -'0');
		else{
			decimalPart = (decimalPart * 10) + (*ptr -'0');
			precision++;
		}
		ptr++;
	}
	gpsRxData.speed_knots+= decimalPart/ pow(10,precision);

	ptr = sl868a_parse_param_offset(in, in_len, comma_course);
	decimal = false;
	decimalPart=0;
	precision = 0;
	while (ptr &&  (*ptr != ',')) {
		if(!decimal && *ptr=='.') decimal = true;
		if (!decimal)
		gpsRxData.course = (gpsRxData.course *10) + (*ptr -'0');
		else{
			decimalPart = (decimalPart * 10) + (*ptr -'0');
			precision++;
		}
		ptr++;
	}
	gpsRxData.course+= decimalPart/ pow(10,precision);

}


 bool
 Sl868a::crcCheck(uint8_t data[], uint8_t len)
 {

     char checksum = 0;
     char checksum_str[3]={};
     uint8_t i = 1;

     while (data[i] != '*') {
         if (i >= len) {
             return false;
         }
         checksum ^=data[i++];
     }
     i++;
     sprintf(checksum_str, "%0X", checksum);
     if (data[i++] != ((checksum > 15) ? checksum_str[0]:'0')) {
         return false;
         } else if (data[i++] != ((checksum > 15) ? checksum_str[1] : checksum_str[0])) {
         return false;
     };
     return true;
 }

  void
  Sl868a::parseGpsRxMsg (void)
  {
      uint8_t offset= 0;
      uint8_t i = 0;

      if (rxMsg.idx <= 1) {
          return;
      }
      offset++;   // skip $

      if (SME_OK != getTalkerType(&rxMsg.data[offset],  &msgPtrT.messageType)) {
          return;
      }

      if (msgPtrT.messageType == STD_NMEA) {

          msgPtrT.nmea_p.std_p.talker_p =  &rxMsg.data[offset];
          offset+=2;

          // Filling Sentence Id
          msgPtrT.nmea_p.std_p.sentenceId_p = &rxMsg.data[offset];
          offset += NMEA_SENTENCE_ID_LEN;

          // Filling data
          msgPtrT.nmea_p.std_p.data_p = &rxMsg.data[offset];

          while( rxMsg.data[offset++] != '*') {
              if( rxMsg.data[offset] == 0) {
                  return;
              }
              i++;
          }
          msgPtrT.nmea_p.std_p.dataLenght = i;

          } else {
          msgPtrT.nmea_p.mtk_p.talker_p = &rxMsg.data[offset];
          offset+=4;
          msgPtrT.nmea_p.mtk_p.msgId_p = &rxMsg.data[offset];
          offset+=3;
          msgPtrT.nmea_p.mtk_p.data_p  = &rxMsg.data[offset];
          // Filling data
          while( rxMsg.data[offset++] != '*') {
              i++;
          }
          msgPtrT.nmea_p.mtk_p.dataLength = i;

      }
}
