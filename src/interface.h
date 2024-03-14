#include "main.h"
#include "radio.h"
#include "mString.h"

extern radio_data1 rd;
extern ds1307_map_t time_tmp;
extern radio_cmd_s rcmd;
extern radio_cmd_resp rcmd_r;
mString<17> buf4;
//String Hostname="Clock30AEA49694B8"; //имя железки - выглядит как ESP7CDFA1C157BC т.е. ESP+mac адрес. //MAC адресс часов/ 30:ae:a4:96:94:b8
String Hostname="Emul30AEA49694B8"; //имя железки - выглядит как ESP7CDFA1C157BC т.е. ESP+mac адрес. //MAC адресс часов/ 30:ae:a4:96:94:b8

String lng() {
  // Вариант реализации многоязычности
  // Получить индекс языка
  // crm.getLang();
  String L = crm.getLang();
  uint8_t l = 0;
  if (L == "de") l = 1;
  else if (L == "ru") l = 2;
  switch (l) {
    case 0: return "English";
    case 1: return "Deutsch";
    case 2: return "Russian";
    default: return "n/a";
  }
}

String StrToDec(u_char x)
{
  String str;

if (x>9)
{
  str=String(x,16);
}
else
{
  str="0" + String(x,16);
}


  return str;
}

void myLoopRun() {
  //static int a[3] = {};
  //static int i = 0;
  //if (i > 2) i = 0;
  //a[i] = WiFi.RSSI();



  // Обновление значений элементов веб интерфейса
  // crm.webUpdate("[ID элемента]", "[Значение]");
  // Интервал отправки 1 раз в сек.
  //crm.webUpdate("rssi", String((a[0] + a[1] + a[2]) / 3));
  //crm.webUpdate("rssiraw", String(a[i]));
  //i++;
  if (rd.ds_error==0)
  {
   crm.webUpdate("G_0", String(rd.ext_temp,0)); 
  }
  if (rd.bm_error==0)
  {
    crm.webUpdate("G_1", String(rd.int_temp,0));
    crm.webUpdate("G_2", String(rd.press,0));
  }
  
  
 

  if(rd.ds_error==0)
  {
    crm.webUpdate("ext_temp", String(rd.ext_temp, 2));
  }
  else
   {
    String temp;
switch (rd.ds_error)
{
case 1: temp =". Шина занята"; break;
case 2: temp =". На шине никого нет"; break;
case 3: temp =". Чтение ROM"; break;
case 4: temp =". Режим датчика не 12бит"; break;
case 5: temp =". Датчик не запрограммирован"; break;

default: temp =". Другая ошибка";
  break;
}

    //crm.webUpdate("ext_temp", String("Ошибка связи с датчиком Код: ")+ String(rd.ds_error));
    crm.webUpdate("ext_temp", String("Ошибка связи с датчиком Код: "+ String(rd.ds_error,HEX) + temp));
    }

if (rd.bm_error==0)
{
  crm.webUpdate("press",    String(rd.press));
  crm.webUpdate("int_temp", String(rd.int_temp, 2));
}
else
{
  crm.webUpdate("press",    String("Ошибка связи с датчиком Код: ")+ String(rd.bm_error,HEX));
  crm.webUpdate("int_temp", String("Ошибка связи с датчиком Код: ")+ String(rd.bm_error,HEX));
}

  
  crm.webUpdate("seconds",String(rd.dt.seconds,HEX));
  crm.webUpdate("minutes",String(rd.dt.minutes,HEX));
  crm.webUpdate("hours",String(rd.dt.hours,HEX));
  crm.webUpdate("day",String(rd.dt.day,HEX));
  crm.webUpdate("date",String(rd.dt.date,HEX));
  crm.webUpdate("month",String(rd.dt.month,HEX));
  crm.webUpdate("year",String(rd.dt.year,HEX));
  crm.webUpdate("date1307",String(StrToDec(rd.dt.date) + ":" + StrToDec(rd.dt.month) + ":" + StrToDec(rd.dt.year)));
 // crm.webUpdate("time1307",String(String(rd.dt.hours,HEX) + ":" + String(rd.dt.minutes,HEX) + ":" + String(rd.dt.seconds,HEX)));

crm.webUpdate("time1307",String(StrToDec(rd.dt.hours) + ":" + StrToDec(rd.dt.minutes) + ":" + StrToDec(rd.dt.seconds)));
  
  crm.webUpdate("time1307_sec",String(rd.dt.seconds,HEX));
  crm.webUpdate("timeformat",String(rd.dt_format));
  crm.webUpdate("dt_error",String(rd.dt_error,HEX));
  crm.webUpdate("bm_error",String(rd.bm_error,HEX));
  crm.webUpdate("ds_error",String(rd.ds_error,HEX));


}



void Send_HC12Run() {
// Подпрограмма для передачи данных в радиоканал на HC12.
// Вызывается раз в 2сек
// Пока что сделано только передача информации от часов

radio_frame *rf_HC = (radio_frame *)radio_buf;
radio_data1 *rd_HC = (radio_data1 *)rf_HC->data;

rf_HC->msgid=1;
rd_HC->dt_format=rd.dt_format;
rd_HC->dt_error=rd.dt_error;
rd_HC->dt=rd.dt;
rd_HC->ds_error=rd.ds_error;
rd_HC->ext_temp=rd.ext_temp;
rd_HC->bm_error=rd.bm_error;
rd_HC->int_temp=rd.int_temp;
rd_HC->press=rd.press;

send_msgHC(rf_HC, sizeof(radio_data1));
 
}

void SendToNarodmon() { // Собственно формирование пакета и отправка.
  WiFiClient client;
  String buf;
    buf = "#" + Hostname + "\n"; //mac адрес для авторизации датчика
    if (rd.bm_error==0)
    {
      buf = buf + "#T1#" + String(rd.int_temp) + "\n"; //показания температуры в гараже
    } else
    {      buf = buf + "#T1#" + String(0.0) + "\n"; //показания температуры в гараже
    }
    if (rd.ds_error==0)
    {
      buf = buf + "#T2#" + String(rd.ext_temp) + "\n"; //показания температуры на улице
    }else
    {
      buf = buf + "#T2#" + String(0.0) + "\n"; //показания температуры на улице
    }
    
    if (rd.bm_error==0)
    {
      buf = buf + "#P1#" + String(rd.press) + "\n"; //показания давления
    } else
    {      buf = buf + "#P1#" + String(0.0) + "\n"; //показания давления
    }
    
    client.connect("narodmon.ru", 8283);   // подключение

 


  //Данные от ESP ( Напряжение питания,уровень wifi
  //buf = buf + "#VCC#" + String(ESP.getVcc() + 350) + "#Напряжение батареи\n"; //показания температуры
  buf = buf + "#WIFI#"  + String(WiFi.RSSI()) + "#Уровень WI-FI " + String(WiFi.SSID()) + "\n"; // уровень WIFI сигнала
 




  String worcktime = String(millis());
  float WTime = worcktime.toInt(); WTime /= 1000;
  buf = buf + "#WORKTIME#"  + String(WTime) + "#Время передачи данных" + "\n"; // уровень WIFI сигнала
  buf = buf + "##\n"; //окончание передачи
  client.print(buf); // и отправляем данные
  status_send_NarodMon=pdTRUE;//ушло
  
}


void update() {
  // Метод вызывается при каждом изменении значения элементов через веб интерфейса
  //Serial.println("Method update() run");
  //Serial.println("Language: " + lng());

  // Получить(Записать) значение переменной из(в) конфига
  // crm.var("ID")
  // crm.var("ID", "Значение")
  //bool a = crm.var("card1") == "true" ? true : false;

  //static bool b = false;
  //if (b != a) {
  //  digitalWrite(2, a ? HIGH : LOW);

    // Отправить уведомление на страницу веб интерфейса
    // crm.webNotif("[Цвет]", "[Сообщение]", [время показа, с], [крестик закрыть (1, true)]);
    // Цвет: green, red, orange, blue, "" - без цвета
 //   crm.webNotif(a ? "Red" : "Green", a ? "Motor start" : "Motor stop", 5);

 //   b = a;
 // }

setTime = crm.var("SetTime") == "true" ? true:false;
crm.webNotif(setTime ? "Red" : "Green", setTime ? "Время Установить" : "Время Установлено", 5);

 crm.var("datatime1").toCharArray(buf4.buf,sizeof(buf4));

time_tmp.year    = buf4.toInt(2);
time_tmp.month   = buf4.toInt(5);
time_tmp.day     = buf4.toInt(8);
time_tmp.hours   = buf4.toInt(11);
time_tmp.minutes = buf4.toInt(14);


}


/* void api(String p) {
  // Метод вызывается при API запросах
  // Обязательно с параметром, тип String
  //
  // Ответ на запрос, тип JSON
  // crm.apiResponse("[ID]", "[Значение]");

  //Serial.print("API: ");
  //Serial.println(p);

  DynamicJsonDocument doc(200);
  deserializeJson(doc, p);

  //Запрос http://IP/api?hum
  const char* hum = doc["hum"];
  if (hum != NULL) {
    crm.apiResponse("Humidity", "51%");
  }

  //Использование параметров
  //Запрос http://IP/api?random=500
  const char* rnd = doc["random"];
  if (rnd != NULL) {
    crm.apiResponse("Random", String(random(String(rnd).toInt())));
  }

  //Запрос http://IP/api?print=[any_text]
  const char* prt = doc["print"];
  if (prt != NULL) {
    //Serial.println(prt);
  }
} */


//функции кнопок
void hw_butt() {
  //Serial.println("HW BUTTON PRESS!");
}

//void tablt2() {
  //Serial.println("Button STOP press.");
//  crm.webUpdate("t2", String(millis()));
//}

void Set_Time() {

  Serial.println("SetTime Button press.");
  setTime = true;
  crm.webUpdate("SetTime", setTime ? "Установлено" : "Установить");

//Сборка пакета на передачу для установки часов 

       radio_frame * rf = (radio_frame *)radio_buf;
       radio_cmd * rcmd = (radio_cmd *)rf->data;

rcmd->target_id=ID_SYS_Clock;
rcmd->cmd=2;                    // Команда установить часы
rcmd->dat[0]=10;                // Десятичный формат чисел в часах
rcmd->dat[1]=time_tmp.seconds;
rcmd->dat[2]=time_tmp.minutes;
rcmd->dat[3]=time_tmp.hours;
rcmd->dat[4]=time_tmp.date;
rcmd->dat[5]=time_tmp.day;
rcmd->dat[6]=time_tmp.month;
rcmd->dat[7]=time_tmp.year;
rcmd->len=sizeof(time_tmp)+1;
rf->msgid = 0x05;               // Тип сообщения выполнить команду

send_msg(rf, sizeof(radio_cmd));
setTime=false;


} 


void reboot() {
  crm.webNotif("info", "Reboot ESP", 5, 1);

  // Отправляет модуль на перезагрузку, с сохранением настроек, если требуется.
  crm.espReboot();
}





// Метод, вызывается при открытии веб интерфейса.
void interface() {
  // Заголовок новой страницы
  crm.page("&#xe802;Гараж");

  //Разделитель
  //crm.output({[Тип], ["Размер в px"], ["Отступы, смотри свойство: margin html"]});
  crm.output({OUTPUT_HR, "1px", "20px 10% -31px"});

  // Вывод значений в виде таблицы
  // crm.output({[Тип], ["ID"], ["Заголовок"], ["Значение при загрузке страницы"], ["цвет в HEX формате"]});
  crm.output({OUTPUT_TABL, "press", "Давление", "press", "0f0"});
  crm.output({OUTPUT_TABL, "int_temp", "Температура внутри", "int_temp"});
  crm.output({OUTPUT_TABL, "ext_temp", "Температура снаружи", "ext_temp" });
  crm.output({OUTPUT_TABL, "date1307", "День + Месяц + Год", "2020.07.04" });
  crm.output({OUTPUT_TABL, "time1307", "Час + минуты + секунды", "16.23.04"});
  crm.output({OUTPUT_HR, "1px", "-3px 10% 0"});

Serial.print("rd.ds_error=");
Serial.println(rd.ds_error);
// Дуговые индикаторы
  // Тип: GAUDE_1 - со стрелкой, GAUDE_2 - без стрелки
  // crm.gauge({[Тип], "[ID]", "[Заголовок]", [Min, шкала], [Max шкала], [Значение при загрузке], {[Цветовая палитра]}, ["Единицы измерения"], [Группировка]});
  if (rd.ds_error==0){
    
crm.gauge({GAUDE_1, "G_0", "Температура Снаружи", -40, 60, rd.ext_temp,
    {
      {"#1C14FF", "-40", "0"},   // Указываются конкретные значения
      {"#19F3FF", "1", "10"},    // Цвет, начало заны, конец зоны, в формате HEX
      {"#00FF00", "11", "25"},   // Количество не больше 6
      {"#FF0800", "26", "60"}    //
    }, "°C",                     // Единицы измерения
    true                         // Группировать с предыдущим, def = false
  });}
    else {

crm.gauge({GAUDE_1, "G_0", "&#xe80c; Температура Снаружи", -40, 60, 0,
    {
      {"#1C14FF", "-40", "0"},   // Указываются конкретные значения
      {"#19F3FF", "1", "10"},    // Цвет, начало заны, конец зоны, в формате HEX
      {"#00FF00", "11", "25"},   // Количество не больше 6
      {"#FF0800", "26", "60"}    //
    }, "°C",                     // Единицы измерения
    pdFALSE                         // Группировать с предыдущим, def = false
  });

    }
  crm.gauge({GAUDE_1, "G_1", "Температура Внутри", -40, 60, rd.int_temp,
    {
      {"#1C14FF", "-40", "0"},   // Указываются конкретные значения
      {"#19F3FF", "1", "10"},    // Цвет, начало заны, конец зоны, в формате HEX
      {"#00FF00", "11", "25"},   // Количество не больше 6
      {"#FF0800", "26", "60"}    //
    }, "°C",                     // Единицы измерения
    true                         // Группировать с предыдущим, def = false
  });
  crm.gauge({GAUDE_1, "G_2", "Давление мм. рт. ст.", 740, 766, rd.press,
    {
      {"#7FB2B2", "740", "744"},        // 0.0 = 0%, 1.0 = 100%
      {"#AACC98", "745", "749"},        // Цвет, расположение на шкале, в формате HEX
      {"#D5E57F", "750", "754"},        // Количество не больше 6
      {"#FFFF66", "755", "760"},         //
      {"#FFD166", "761", "766"}         //
      }, "мм.рт.ст.",                      // Единицы измерения
    true                         // Группировать с предыдущим, def = false
  });


  // График
  // Тип: CHART_L - линии, CHART_B - бары (столбики)
  // Данные: [] - сохранять значения при навигации по разделам; "" - не сохранять
  //crm.chart({ [Тип], ["ID"], ["Заголовок"], ["[Массив заголовков]"], ["[Данные]"], ["цвет в HEX формате"], ["высота графика"] });
 //crm.chart({CHART_L, "int_temp", "Температура внутри", "",  "", "#00dd00", "75"});
// crm.chart({CHART_L, "ext_temp", "Температура снаружи", "",  "", "#00dd00", "50"});
// crm.chart({CHART_L, "press", "Давление", "",  "", "#00dd00", "50"});

  // Плитки / Карточки
  // Переключатель
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"], ["Значок"], ["цвет в HEX формате"], [Новая группа]});
  // Значок указывается из списка icon.pdf, в формате &#[CODE];  без 0
  //crm.card({CARD_CHECKBOX, "card1", "Motor", "false", "&#xf2c5;", "aaa"});
  // График столбцы
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["[Массив заголовков]"], ["[Данные]"], ["цвет в HEX формате"], [Новая группа]]});
  //crm.card({CARD_CHART_L, "rssiraw", "WiFi RSSI RAW", "",  "", "#dddd00"});
  // Кнопка
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"], ["Значок"], ["Цвет"], [Новая группа]});
  //crm.card({CARD_BUTTON, "card3", "Door 3", (st3 ? "Open" : "Close"), "&#xe802;", "0ab", true});


  // Для отображения значков в текстовых полях заключаем их в <z></z>
  // Пример <z>&#xf1c9;</z>
 // String txt = F("<z>&#xf1c9;</z> In computer science, an array data structure, or simply an array, is a data structure consisting of a collection of elements (values or variables), each identified by at least one array index or key. An array is stored such that the position of each element can be computed from its index tuple by a mathematical formula.[1][2][3] The simplest type of data structure is a linear array, also called one-dimensional array.");

  // Текстовое поле справка
  //crm.output({[Тип], ["ID"], "", ["ТЕКСТ"], ["цвет в HEX формате"]});
  //crm.output({OUTPUT_TEXT, "t1", "", txt, "#ff5"});

  // Кнопки
  // crm.input({[Тип], ["ID"], ["Заголовок / значок"], ["Внутренние отступы, смотри: padding html"], ["r - вряд"], ["Размер"]});
  //crm.input({INPUT_BUTTON, "b1", "&#xe816;", "10px 11px 10px 15px", "r", "55"});



  crm.page("&#xe83a; Settings");
  // Поле выбора (селект)
  // crm.select({["ID"], ["Заголовок / значок"], ["Значение по умолчанию"], ["Значения {{A:1},{B:2},{N:n}}] });
 //  crm.select({"select1", "Elements", "0", "Значения {{Hide:0}, {Show:1}}});

  crm.selOpt({"Скрыть", "0"});
  crm.selOpt({"Показать", "1"});
  crm.select({"select1", "Доп. опции", "1", "1"});

  // Получить значение из конфига
  // crm.var(["ID переменной"])
  if (crm.var("select1").toInt() > 0) {
    // Поля ввода даты времени
    // crm.input({[Тип], ["ID"], ["Заголовок"]});
    //crm.input({INPUT_DATE, "date1", "Date"});
    //crm.input({INPUT_TIME, "time1", "Time"});
    crm.input({INPUT_DATETIME, "datatime1", "Установка времени"});

    // Поле ввода текста и цифр, поддерживает паттерн, смотри в интернете.
    // crm.input({[Тип], ["ID"], ["Заголовок / значок"], ["Значение по умолчанию"], ["паттерн, смотри в инете"]});
    //crm.input({INPUT_TEXT, "input1", "Text (pattern)", "145", "[0-9]{1,8}"});
    //crm.input({INPUT_TEXT, "input2", "Output template", "Температура %T1"});
    //crm.input({INPUT_NUMBER, "num1", "Only number", "123"});
    crm.input({INPUT_CHECKBOX, "chk_HC12", "Включить радио канал", "false"});
  }

 
//Serial.println(crm.var("datatime1").toInt());

  // Поле ввода адреса электронной почты, цвета, переключателя (чекбокс).
  // crm.input({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"] });
 // crm.input({INPUT_EMAIL, "email", "Your mail", "test@mail.ru"});
 // crm.input({INPUT_COLOR, "input3", "Color", "#FF22FF"});
  crm.input({INPUT_CHECKBOX, "chk1", "Button Reboot", "false"});
  
  crm.card({CARD_BUTTON, "SetTime", "Установить время", (setTime ? "Установлено" : "Установить"), "&#xe80b;", "#f2300a", true});
  
  // Смотри выше ^
  //crm.output({OUTPUT_TEXT, "t11", "", txt, "#5f5"});

  // Ползунок
  // crm.range({["ID"], ["Заголовок"], ["Значение по умолчанию"], ["MIN"], ["MAX"], [Шаг], ["Единицы измерения"]});
  //crm.range({"range1", "Volume", 12, 0, 84, 1});
  //crm.range({"range2", "Brightness", 52, 0, 84, 1, " lux"});
  if (crm.var("chk1") == "true") crm.input({INPUT_BUTTON, "reboot", "&#xe810;", "8px 9px 8px 14px", "row", "50"});

  crm.page("&#xf1eb; Wi-Fi");
  // форма с полями для WiFi
  crm.wifiForm(WIFI_AP, "ESP-Gateway");
  crm.input({INPUT_BUTTON, "reboot", "REBOOT"});
}