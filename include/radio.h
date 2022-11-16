#ifndef __RADIO_H__
#define __RADIO_H__

#include <Arduino.h>


//---------------------------------

#define _ID_SYS_  0x02  /*идентификатор системы*/
/**
 * Идентификаторы системы
 * 0х01 - Дом
 * 0х02 - Шлюз ESP32 в гараже
 * 0x03 - Резерв
 * 0x04 - Часы в гараже
*/
//-----------------------------структура передаваемых данных
#pragma pack (push,1)
typedef struct
{
	unsigned char seconds;
	unsigned char minutes;
	unsigned char hours;
	unsigned char day;
	unsigned char date;
	unsigned char month;
	unsigned char year;
	//unsigned char control;
	//  unsigned char data[];
}ds1307_map_t;
#pragma pack(pop)

#pragma pack (push,1)
typedef struct radio_frame_s
{
	
 uint16_t stx;//стартовое слово 0xa544
 uint8_t crc; //контрольна¤ сумма всего сообщения с солью в зависимости от msgid
 uint8_t len;//длина поля данных
 uint8_t seq;//счетчик пакетов
 uint8_t sysid;// ид отправителя
 uint8_t msgid;//тип сообщения 
 uint8_t data[];//данные максимум 50 байт если hc12
}radio_frame;
#pragma pack(pop)
//------------------------------структура передаваемого сообщения (msgid 1)
#pragma pack (push,1)
typedef struct radio_data1_s
{
 uint8_t dt_format; // формат календаря 10 или 16 
 uint8_t dt_error;  // код ошибки часов
 ds1307_map_t dt; //часы, календарь (7 байт)
 uint8_t bm_error; //код ошибки bmp280
 float int_temp;//внутренняя температура град (bmp280)
 float press; //атмосферное давление мм рт ст (bmp280)
 uint8_t ds_error; //код ошибки ds18b20
 float ext_temp;//внешняя температура град (das18b20) 
}radio_data1;
#pragma pack(pop)
//--------------------------структура команды (msgid 5)
#pragma pack (push,1)
typedef struct radio_cmd_s
{
  uint8_t target_id; // идентификатор получателя команды
  uint8_t cmd;       // команда
  uint8_t len;       // число доп байт в команде
  uint8_t dat[17];   // данные
}radio_cmd;
#pragma pack(pop)
//-----------------------ответ на команду (msgid 6)
#pragma pack (push,1)
typedef struct radio_cmd_resp_s
{
  uint8_t cmd;       // команда
  uint8_t res;       // результат операции
  uint8_t dat[17];   // дополнительные данные
}radio_cmd_resp;
#pragma pack(pop)
//-----------------прототипы функций
uint8_t finalize_message_chan(radio_frame* msg, uint8_t length);//готовит пакет к передаче (заполняет заголовок, считает crc)
uint8_t send_msg(radio_frame* msg, uint8_t length);//готовит и передает пакет в uart
uint8_t send_msgHC(radio_frame* msg, uint8_t length);//готовит и передает пакет в uart HC12
void radio_pool(void);//прием данных
void radio_poolHC(void);//прием данных от HC12
extern void rx_radio_filter(radio_frame * msg);
static uint8_t crc8(uint8_t *pcBlock, uint8_t len);
//-----------------
#endif