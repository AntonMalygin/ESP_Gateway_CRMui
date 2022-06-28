#include "Arduino.h"
#include "radio.h"


#define _MSG_STX_ 0xa544 /*слово начала пакета*/
const uint8_t extra_tab[] = {10,12,15,18,33,134,65,234,98,68,45,234,54,57,21,61,201,69,5,241,168,23,79,62,77};
static uint8_t sh_seq;
static uint8_t rx_val[64];

/*
  Name  : CRC-8
  Poly  : 0x31    x^8 + x^5 + x^4 + 1
  Init  : 0xFF
  Revert: false
  XorOut: 0x00
  Check : 0xF7 ("123456789")
  MaxLen: 15 байт(127 бит) - обнаружение
    одинарных, двойных, тройных и всех нечетных ошибок
*/
//---------------------------------
static uint8_t crc8(uint8_t *pcBlock, uint8_t len)
{
    unsigned char crc = 0xFF;
    uint8_t i;

    while (len--)
    {
        crc ^= *pcBlock++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }

    return crc;
}
//---------------------------------
static uint8_t finalize_message_chan(radio_frame* msg, uint8_t length)
{
  msg->stx = _MSG_STX_;//вставляем заголовок 
  msg->crc = extra_tab[msg->msgid]; //вставляем соль для расчета crc
  msg->len = length; //вставляем длину данных
  msg->seq = sh_seq++;//вставляем счетчик пакетов
  msg->sysid = _ID_SYS_; //вставляем идентификатор отправителя
  msg->crc = crc8((uint8_t *)msg, sizeof(radio_frame) + length); //вставляем контрольную сумму
  
  return sizeof(radio_frame) + length;
}
//------------------------------------
uint8_t send_msg(radio_frame* msg, uint8_t length)
{
  uint8_t len = finalize_message_chan(msg, length);
  return Serial.write((uint8_t*)msg,len);
}
//------------------------------------
void radio_pool(void)
{
  static uint8_t ukz;
  static uint32_t tst;
  if(ukz > 0)
	{
	if((millis() - tst) > 20)ukz = 0;
	}
 
  while(Serial.available()>0)
  {
    rx_val[ukz] = Serial.read();
    tst = millis();

        if((ukz == 0)&&(rx_val[0]== 0xA5)) //первый байт заголовка
         {
              ukz++;
              continue;
         }
        //------------
         if(ukz == 1)
         {
            if(rx_val[1]== 0x44)       //второй байт заголовка
            {
                ukz++;
             }  else
                  {
                     ukz = 0;
                  }
            continue;
          }
        //--------------    
        if (ukz<2){continue;}
		
		ukz++;
        if(ukz >= 7)
        {
           if(rx_val[3] + 7 == ukz)
           {
            radio_frame *mt = (radio_frame *)rx_val;
            uint8_t in_crc = mt->crc;
            mt->crc = extra_tab[mt->msgid];
            uint8_t crc = crc8(rx_val, ukz);              
            if(crc == in_crc) 
            {
rx_radio_filter(mt);
              
            }
               ukz = 0;
               return;
           }

        }    
    
    
  }
  
}
//---------------------------------
