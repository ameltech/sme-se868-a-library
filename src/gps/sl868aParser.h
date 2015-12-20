/*
 * sl868aParser.h
 *
 * Created: 06/05/2015 22:48:14
 * Author : Seve (seve@axelelettronica.it)
 */


#ifndef SL868APARSER_H_
#define SL868APARSER_H_

#define SME_CTRL_COORD_LEN             9
#define SME_CTRL_LAT_MINUTES_START     2
#define SME_CTRL_LONG_MINUTES_START    3


#define SL868A_MAX_MSG_LEN   100

typedef struct {
    uint8_t idx;
    uint8_t data[SL868A_MAX_MSG_LEN];
} sl868aRxMsgT;


typedef struct {
    uint8_t    *talker_p;
    uint8_t    *sentenceId_p;
    uint8_t    *data_p;
    uint8_t    dataLenght;
} sl868aStdMsgPtrT;


typedef struct {
    void    *talker_p;
    void    *msgId_p;
    void    *data_p;
    uint8_t dataLength;
} sl868aMTKMsgPtrT;


typedef union {
    sl868aStdMsgPtrT std_p;
    sl868aMTKMsgPtrT mtk_p;
} sl868aMsgPtrT;

typedef struct {
    sl868aMsgE    messageType;
    sl868aMsgPtrT nmea_p;
} sl868aPtrT;



extern sl868aPtrT  msgPtrT;
extern sl868aRxMsgT rxMsg;

#endif /* SL868APARSER_H_ */
