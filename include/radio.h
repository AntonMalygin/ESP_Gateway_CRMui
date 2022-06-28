#ifndef __RADIO_H__
#define __RADIO_H__

#include <Arduino.h>

#define _ID_SYS_  0x04  /*идентификатор системы*/
//-----------------------------структура передаваемых данных

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


typedef struct radio_frame_s
{
	
 uint16_t stx;//стартовое слово 0xa544
 uint8_t crc; //контрольна¤ сумма всего сообщени¤ с солью в зависимости от msgid
 uint8_t len;//длина пол¤ данных
 uint8_t seq;//счетчик пакетов
 uint8_t sysid;// ид отправител¤
 uint8_t msgid;//тип сообщени¤ 
 uint8_t data[];//данные максимум 50 байт если hc12
}radio_frame;
//------------------------------структура передаваемого сообщени¤ (msgid 1)
typedef struct radio_data1_s
{
 uint8_t dt_format; // формат календар¤ 10 или 16 
 uint8_t dt_error;  // код ошибки часов
 ds1307_map_t dt; //часы, календарь (7 байт)
 uint8_t bm_error; //код ошибки bmp280
 float int_temp;//внутренн¤¤ температура град (bmp280)
 float press; //атмосферное давление мм рт ст (bmp280)
 uint8_t ds_error; //код ошибки ds18b20
 float ext_temp;//внешн¤¤ температура град (das18b20) 
}radio_data1;
//--------------------------структура команды (msgid 5)
typedef struct radio_cmd_s
{
  uint8_t target_id; // идентификатор получател¤ команды
  uint8_t cmd;       // команда
  uint8_t len;       // число доп байт в команде
  uint8_t dat[17];   // данные
}radio_cmd;
//-----------------------ответ на команду (msgid 6)
typedef struct radio_cmd_resp_s
{
  uint8_t cmd;       // команда
  uint8_t res;       // результат операции
  uint8_t dat[17];   // дополнительные данные
}radio_cmd_resp;
//-----------------прототипы функций
//uint8_t finalize_message_chan(radio_frame* msg, uint8_t length);//готовит пакет к передаче (заполн¤ет заголовок, считает crc)
uint8_t send_msg(radio_frame* msg, uint8_t length);//готовит и передает пакет в uart
void radio_pool(void);//прием данных
extern void rx_radio_filter(radio_frame * msg);
//-----------------
#endif