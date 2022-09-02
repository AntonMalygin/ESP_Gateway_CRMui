#include <Arduino.h>
//Example use CRMui3  /  Пример использования CRMui3
#include "CRMui3.h"
#include <Ticker.h> // Входит в состав ядра
//#include "BluetoothSerial.h"

// Объявление объектов
CRMui3 crm;     // CRMui
Ticker myLoop;  // Ticker
Ticker Send_HC12;  // Задача отправки данных в HC12
//BluetoothSerial SerialBT;

// Переменные в примере
bool st3, st4, st5, setTime;
uint8_t radio_buf[64];//буфер передаваемых данных

#define TXD_PIN (GPIO_NUM_26)
#define RXD_PIN (GPIO_NUM_27)

#include "interface.h"
#include "main.h"
#include "radio.h"
      

radio_data1 rd;          // Структура данных для MSG ID = 1 . Простые данные от часов
radio_cmd rcmd;          // Структура данных для MSG ID = 5 . Cтруктура команды для часов
radio_cmd_resp rcmd_r;   // Структура данных для MSG ID = 6 . Cтруктура ответа на команду от часов


void rx_radio_filter(radio_frame * msg);






void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(4, INPUT_PULLUP);            // Button pin

  // Инициализация библиотеки, памяти и прочих функций
  // Параметры со * обязательны.
  // crm.begin("[*Название проекта]", [*Ф-я интерфейса], [Ф-я обновления переменных], [Ф-я API], [Скорость серийного порта, доп. отладка]);
  //crm.begin("Project-28", interface, update);
  //crm.begin("Project-28", interface, NULL, NULL, 115200);
  crm.begin("ESP-Gateway", interface, update,NULL);
  Serial2.begin(BAUD_RATE2); //Выставляем скорость для общения с HC12 
  Serial2.flush();
  Serial1.begin(BAUD_RATE); //Выставляем скорость для общения с часами 
  Serial1.setPins(RXD_PIN, TXD_PIN);
  Serial1.flush();
 // SerialBT.begin("ESP_Gateway",false);
  // Авторизация в веб интерфейсе
  // Параметры со * обязательны.
  // crm.setWebAuth("[*Имя пользователя]", "[Пароль]");
  //crm.setWebAuth("admin", "admin");

  // Авторизация на устройстве для API запросов. Без setWebAuth не работает.
  // Пример запроса: http://IP/api?k=d1h6d5&p1=2&...,
  //  где первый параметр должен быть ключ, имя параметра любое
  // crm.setApiKey("[API ключ]");
  //crm.setApiKey("d1h6d5");

  // Задать лицензионный ключ
  // Позволяет отключить уведомление о бесплатной версии, а так же задать свои контакты.
  // Переменные со * обязательны.
  // crm.license([*Лицензионный ключ], [Электронная почта], [Телеграм], [Домашняя страница]);
  //crm.license("5s72to1", "crm.dev@bk.ru", "user624", "https://github.com/WonderCRM/CRMui3");

  // Аналог FreeRTOS
  // NAME.once_ms(ms, Fn); - Выполнить единожды через указанный интервал
  // NAME.attach_ms(ms, Fn); - Цикличное выполнение через указанный интервал
  // NAME.detach(); - Деактивировать
  myLoop.attach_ms(2000, myLoopRun);
  Send_HC12.attach_ms(2000,Send_HC12Run);
}


void loop() {
  // Обслуживание системных функций библиотеки
  crm.run();

radio_pool(); // Получение данных от часов
radio_poolHC(); // Получение данных от HC12 

  // Проверка состояния нажатия совтовых кнопок. Проверка не обязательна.
  if (crm.btnSwStatus()) {
    // Проверка конкретных кнопок на нажатие
    // crm.btnCallback("[ID кнопки]", [Функция для выполнения]);
    crm.btnCallback("reboot", reboot);    // Check "reboot" SW button
    crm.btnCallback("card3", card_sw3);   // Check "card3" SW button
    crm.btnCallback("card4", card_sw4);   // Check "card4" SW button
    crm.btnCallback("card5", card_sw5);   // Check "card5" SW button
    crm.btnCallback("b3", tablt2);        // Check "b3" SW button
    crm.btnCallback("SetTime", Set_Time); // Проверка кнопки установки времени
  }
  // Проверка аппаратных кнопок на нажатие
  // crm.btnCallback("[пин подключения кнопки]", [Функция для выполнения], [уровень при нажатии]);
  crm.btnCallback(4, hw_butt, LOW);      // Check pin33 HW button

 

}

void rx_radio_filter(radio_frame * msg)
{


/* if (msg->msgid == 1)
{

} */

switch (msg->msgid)
{
case 1:{
  radio_data1 *rd1 =( radio_data1 *)msg->data;
  memcpy(&rd,rd1,sizeof(radio_data1));
}break;

case 5:{
  radio_cmd *rcmd1 =( radio_cmd *)msg->data;
  memcpy(&rcmd,rcmd1,sizeof(radio_cmd));
}break;

case 6:{
  radio_cmd_resp *rcmd_r1 =( radio_cmd_resp *)msg->data;
  memcpy(&rcmd_r,rcmd_r1,sizeof(radio_cmd_resp));
}break;

default:{
ets_delay_us(1);
  }break;
}


}

