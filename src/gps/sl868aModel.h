/*
 * sl868aModel.h
 *
 * Created: 05/05/2015 22:26:00
 * Author : Seve (seve@axelelettronica.it)
 */


#ifndef SL868AMODEL_H_
#define SL868AMODEL_H_

/*
* I.E. gps <c> <Standard SMEA Sentence>
*
*
* c = raw command to the sl868v2 in Standard SMEA Format excluding Start '$' and msg delimiter '*'
*     SMEA CMD is up to 76 characters long.
*/

#define NMEA_TALKER_LEN       2
#define NMEA_SENTENCE_ID_LEN  3


#define STD_NMEA_MAX_DATA_LEN 76
#define MTK_NMEA_MAX_DATA_LEN 242

typedef struct {

    uint8_t talker[2];
    uint8_t sentenceId[3];
    uint8_t data[STD_NMEA_MAX_DATA_LEN];
    uint8_t dataLenght;

} sl868aStdMsgT;

typedef struct {

    uint8_t talker[4];
    uint8_t msgId[3];
    uint8_t data[MTK_NMEA_MAX_DATA_LEN];
    uint8_t dataLenght;

} sl868aMTKMsgT;

typedef union {
    sl868aStdMsgT std;
    sl868aMTKMsgT mtk;
} sl868aMsgT;

typedef enum {
    STD_NMEA,
    MTK_NMEA
} sl868aMsgE;

typedef struct {
    sl868aMsgE     messageType;
    sl868aMsgT     nmea_msg;
} sl868aT;


void releaseSl868aModel(void);
sl868aT* getSl868aModel(void);
void initSl868aModel(void);

/* TX Messagese */
#define  SL868A_SET_STDBY_CMD    "$PMTK161,0*28\r\n"    // Set standby
#define  SL868A_HOT_RST_CMD      "$PMTK101*32\r\n"      // HOT RESTART
#define  SL868A_WARM_RST_CMD     "$PMTK102*31\r\n"      // WARM Restart
#define  SL868A_COLD_RST_CMD     "$PMTK103*30\r\n"      // Cold restart
#define  SL868A_FCOLD_RST_CMD    "$PMTK104*37\r\n"      // Full cold restart
#define  SL868A_CLR_FLASH_CMD    "$PMTK120*31\r\n"      // clear flash
#define  SL868A_CLEAR_ORBIT_CMD  "$PMTK127*36\r\n"      // clear predicted orbit data
#define  SL868A_SET_FIXINT_CMD   "$PMTK220,1000*1F\r\n" // set position fix interval to 1000

#define SL868A_CONF_CMD "PMTK"

typedef enum {
    SME_LAT,
    SME_LONG,
    SME_ALT,
    SME_COORD_UNDEF
} sme_coord_t;





typedef enum  {
    NMEA_UNMANAGED,
    NMEA_RMC,
    NMEA_GGA
}sl868a_nmea_out_msg_id;



int  sme_sl868a_get_latlong (char *msg, uint8_t *len, uint8_t msg_len);
bool sme_gps_position_fixed(void);



#endif /* SL868AMODEL_H_ */
