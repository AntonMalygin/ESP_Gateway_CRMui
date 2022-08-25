#include "main.h"
#include "radio.h"


extern radio_data1 rd;

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

void myLoopRun() {
  static int a[3] = {};
  static int i = 0;
  if (i > 2) i = 0;
  a[i] = WiFi.RSSI();



  // Обновление значений элементов веб интерфейса
  // crm.webUpdate("[ID элемента]", "[Значение]");
  // Интервал отправки 1 раз в сек.
  crm.webUpdate("rssi", String((a[0] + a[1] + a[2]) / 3));
  crm.webUpdate("rssiraw", String(a[i]));
  i++;
 
  crm.webUpdate("press",    String(rd.press, 4));
  crm.webUpdate("ext_temp", String(rd.ext_temp, 4));
  crm.webUpdate("int_temp", String(rd.int_temp, 4));

Serial.print("Press");
Serial.println(rd.press, 0000);
Serial.print("ext_temp");
Serial.println(rd.ext_temp, 0000);
Serial.print("int_temp");
Serial.println(rd.int_temp, 0000);

  crm.webUpdate("seconds",String(rd.dt.seconds,HEX));
  crm.webUpdate("minutes",String(rd.dt.minutes,HEX));
  crm.webUpdate("hours",String(rd.dt.hours,HEX));
  crm.webUpdate("day",String(rd.dt.day,HEX));
  crm.webUpdate("date",String(rd.dt.date,HEX));
  crm.webUpdate("month",String(rd.dt.month,HEX));
  crm.webUpdate("year",String(rd.dt.year,HEX));
  crm.webUpdate("date1307",String(String(rd.dt.day,HEX) + ":" + String(rd.dt.month,HEX) + ":" + String(rd.dt.year,HEX)));
  crm.webUpdate("time1307",String(String(rd.dt.hours,HEX) + ":" + String(rd.dt.minutes,HEX) + ":" + String(rd.dt.seconds,HEX)));
  crm.webUpdate("time1307_sec",String(rd.dt.seconds,HEX));
  crm.webUpdate("timeformat",String(rd.dt_format));
}


void update() {
  // Метод вызывается при каждом изменении значения элементов через веб интерфейса
  //Serial.println("Method update() run");
  //Serial.println("Language: " + lng());

  // Получить(Записать) значение переменной из(в) конфига
  // crm.var("ID")
  // crm.var("ID", "Значение")
  bool a = crm.var("card1") == "true" ? true : false;

  static bool b = false;
  if (b != a) {
    digitalWrite(2, a ? HIGH : LOW);

    // Отправить уведомление на страницу веб интерфейса
    // crm.webNotif("[Цвет]", "[Сообщение]", [время показа, с], [крестик закрыть (1, true)]);
    // Цвет: green, red, orange, blue, "" - без цвета
    crm.webNotif(a ? "Red" : "Green", a ? "Motor start" : "Motor stop", 5);

    b = a;
  }
}


void api(String p) {
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
}


//функции кнопок
void hw_butt() {
  //Serial.println("HW BUTTON PRESS!");
}

void tablt2() {
  //Serial.println("Button STOP press.");
  crm.webUpdate("t2", String(millis()));
}

void card_sw3() {
  //Serial.println("Card 3 Button press.");
  st3 = !st3;
  crm.webUpdate("card3", st3 ? "Open" : "Close");
}

void card_sw4() {
  //Serial.println("Card 4 Button press.");
  st4 = !st4;
  crm.webUpdate("card4", st4 ? "Open" : "Close");
}

void card_sw5() {
  //Serial.println("Card 5 Button press.");
  st5 = !st5;
  crm.webUpdate("card5", st5 ? "Open" : "Close");
}


void reboot() {
  crm.webNotif("info", "Reboot ESP", 5, 1);

  // Отправляет модуль на перезагрузку, с сохранением настроек, если требуется.
  crm.espReboot();
}





// Метод, вызывается при открытии веб интерфейса.
void interface() {
  // Заголовок новой страницы
  crm.page("Гараж");

  //Разделитель
  //crm.output({[Тип], ["Размер в px"], ["Отступы, смотри свойство: margin html"]});
  crm.output({OUTPUT_HR, "1px", "20px 10% -31px"});

  // Вывод значений в виде таблицы
  // crm.output({[Тип], ["ID"], ["Заголовок"], ["Значение при загрузке страницы"], ["цвет в HEX формате"]});
  crm.output({OUTPUT_TABL, "press", "Давление", "press", "0f0"});
  crm.output({OUTPUT_TABL, "int_temp", "Температура внутри", "int_temp"});
  crm.output({OUTPUT_TABL, "ext_temp", "Температура снаружи", "ext_temp", "f0f"});
  crm.output({OUTPUT_TABL, "year", "Год", "01", "f0f"});
  crm.output({OUTPUT_TABL, "month", "Месяц", "01", "f0f"});
  crm.output({OUTPUT_TABL, "day", "День", "01", "f0f"});
  crm.output({OUTPUT_TABL, "hours", "Часов", "01", "f0f"});
  crm.output({OUTPUT_TABL, "minutes", "Минут", "01", "f0f"});
  crm.output({OUTPUT_TABL, "seconds", "секунд", "01", "f0f"});
  crm.output({OUTPUT_TABL, "date1307", "Год + Месяц + день", "2020.07.04", "f0f"});
  crm.output({OUTPUT_TABL, "time1307", "Час + минуты + секунды", "2020.07.04", "f0f"});
  crm.output({OUTPUT_TABL, "timeformat", "timeFormat", "10/16", "f0f"});
  crm.output({OUTPUT_HR, "1px", "-3px 10% 0"});

  // График
  //crm.chart({[Тип], ["ID"], ["Заголовок"], ["[Массив заголовков]"], ["[Данные]"], ["цвет в HEX формате"], ["высота графика"]});
  crm.chart({CHART_L, "rssi", "WiFi RSSI", "[1,2,3,4,5,6]",  "[1,5,3,2,6,3]", "#00dd00", "250"});

  // Плитки / Карточки
  // Переключатель
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"], ["Значок"], ["цвет в HEX формате"], [Новая группа]});
  // Значок указывается из списка icon.pdf, в формате &#[CODE];  без 0
  crm.card({CARD_CHECKBOX, "card1", "Motor", "false", "&#xf2c5;", "aaa"});
  // График столбцы
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["[Массив заголовков]"], ["[Данные]"], ["цвет в HEX формате"], [Новая группа]]});
  crm.card({CARD_CHART_B, "rssiraw", "WiFi RSSI RAW", "",  "", "#dddd00"});
  // Кнопка
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"], ["Значок"], ["Цвет"], [Новая группа]});
  crm.card({CARD_BUTTON, "card3", "Door 3", (st3 ? "Open" : "Close"), "&#xe802;", "0ab", true});
  crm.card({CARD_BUTTON, "card4", "Door 4", (st4 ? "Open" : "Close"), "&#xe802;", "#a0b"});
  crm.card({CARD_BUTTON, "card5", "Door 5", (st5 ? "Open" : "Close"), "&#xe805;", "#0ab"});

  // Для отображения значков в текстовых полях заключаем их в <z></z>
  // Пример <z>&#xf1c9;</z>
  String txt = F("<z>&#xf1c9;</z> In computer science, an array data structure, or simply an array, is a data structure consisting of a collection of elements (values or variables), each identified by at least one array index or key. An array is stored such that the position of each element can be computed from its index tuple by a mathematical formula.[1][2][3] The simplest type of data structure is a linear array, also called one-dimensional array.");

  // Текстовое поле справка
  //crm.output({[Тип], ["ID"], "", ["ТЕКСТ"], ["цвет в HEX формате"]});
  crm.output({OUTPUT_TEXT, "t1", "", txt, "#ff5"});

  // Кнопки
  // crm.input({[Тип], ["ID"], ["Заголовок / значок"], ["Внутренние отступы, смотри: padding html"], ["r - вряд"], ["Размер"]});
  crm.input({INPUT_BUTTON, "b1", "&#xe816;", "10px 11px 10px 15px", "r", "55"});
  crm.input({INPUT_BUTTON, "b2", "&#xe811;", "10px", "r", ""});
  crm.input({INPUT_BUTTON, "b3", "&#xe812;", "10px 30px 10px 15px", "r", "45"});
  crm.input({INPUT_BUTTON, "b4", "&#xe815;", "30px 11px 10px 15px", "r", "35"});


  crm.page("Settings");
  // Поле выбора (селект)
  // crm.select({["ID"], ["Заголовок / значок"], ["Значение по умолчанию"], ["Значения {{A:1},{B:2},{N:n}}] });
  crm.select({"select1", "Elements", "0", {{"Hide", "0"}, {"Show", "1"}}});
  // Получить значение из конфига
  // crm.var(["ID переменной"])
  if (crm.var("select1").toInt() > 0) {
    // Поля ввода даты времени
    // crm.input({[Тип], ["ID"], ["Заголовок"]});
    crm.input({INPUT_DATE, "date1", "Date"});
    crm.input({INPUT_TIME, "time1", "Time"});
    crm.input({INPUT_DATETIME, "datatime1", "Date & Time"});

    // Поле ввода текста и цифр, поддерживает паттерн, смотри в интернете.
    // crm.input({[Тип], ["ID"], ["Заголовок / значок"], ["Значение по умолчанию"], ["паттерн, смотри в инете"]});
    crm.input({INPUT_TEXT, "input1", "Text (pattern)", "145", "[0-9]{1,8}"});
    crm.input({INPUT_TEXT, "input2", "Output template", "Температура %T1"});
    crm.input({INPUT_NUMBER, "num1", "Only number", "123"});
  }

  // Поле ввода адреса электронной почты, цвета, переключателя (чекбокс).
  // crm.input({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"] });
  crm.input({INPUT_EMAIL, "email", "Your mail", "test@mail.ru"});
  crm.input({INPUT_COLOR, "input3", "Color", "#FF22FF"});
  crm.input({INPUT_CHECKBOX, "chk1", "Button Reboot", "false"});
  
  // Смотри выше ^
  crm.output({OUTPUT_TEXT, "t11", "", txt, "#5f5"});

  // Ползунок
  // crm.range({["ID"], ["Заголовок"], ["Значение по умолчанию"], ["MIN"], ["MAX"], [Шаг], ["Единицы измерения"]});
  crm.range({"range1", "Volume", 12, 0, 84, 1});
  crm.range({"range2", "Brightness", 52, 0, 84, 1, " lux"});
  if (crm.var("chk1") == "true") crm.input({INPUT_BUTTON, "reboot", "&#xe810;", "8px 9px 8px 14px", "row", "50"});

  crm.page("Wi-Fi");
  // форма с полями для WiFi
  crm.wifiForm(WIFI_AP, "ESP-Gateway");
  crm.input({INPUT_BUTTON, "reboot", "REBOOT"});
}