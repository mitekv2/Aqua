// Управление 4мя реле по времени
// настройка через ком порт
// комманда,значение1,значение2
// 0 - вывод настроек включения выключения.
// запись нового - X,Y,Z 
// X - Номер реле 1-4
// Y - время включения в минутах т.е. часы*60 + минуты  
// Z - время выключения в минутах т.е. часы*60 + минуты 
// 1,600,1000 -  1 реле, включение - 10 часов (600 минут) выключение - 16:40 (1000 минвут)
// 5 - запись в энерго независимую память
// 9 - Вывод текушего времени
// 8 - установка времени 8,часы,минуты




#include <EEPROM.h>
#include <Time.h>
#include <DS1307RTC.h>




int numPins = 4;
int pins[] = { 2, 3, 4, 5 };    // Пины для реле

int rlb[] = {540, 600, 630, 660}; // время включения реле
int rle[] = {1000, 1020, 1000, 1000}; // время выключения реле

int i = 0;

int LastColebration = 0;




void setup() {
  for (int i = 0; i < numPins; i++)
  {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], 1);
    }

  Serial.begin(9600);

for (int i = 0; i < numPins; i++)
{
 rlb[i]= EEPROM.read(i*10)*60+EEPROM.read(i*10+1);
 rle[i]= EEPROM.read(i*10+2)*60+EEPROM.read(i*10+3);
  
   }

  
}



void loop() {



  tmElements_t tm;
  RTC.read(tm);
  // // Корректировка Часов //если =0 часов,15 минут 15 секунд, и последняя колибровка (LastColebration) была не сегодня
  //
  if (tm.Hour == 0 && tm.Minute == 15 && tm.Second == 15  && LastColebration != tm.Day) {
    tm.Second = 10;
    RTC.write(tm);
    LastColebration = tm.Day;   //сохраняем день
  }


  // включение по таймеру

  for (int i = 0; i < numPins; i++)
  {

    if (int(tm.Hour) * 60 + int(tm.Minute) >= rlb[i] && int(tm.Hour) * 60 + int(tm.Minute) <= rle[i]) {
      digitalWrite(pins[i], 0);

    }   // включаем LED}
    else  {
      digitalWrite(pins[i], 1);

    }


  }

 
// Работа С ком портом

  char buffer[25];
  int unitID=0;
  int BTime=0;
  int ETime=0;


  //если есть данные - читаем
  int j = 0;
  if (Serial.available())
  {
    delay(100);

    //загоняем прочитанное в буфер
    while ( Serial.available() && j < 25) {
      buffer[j++] = Serial.read();
    }
    //закрываем массив
    buffer[j++] = '\0';

    if (j > 1) {

      //разбераем его на части отделенные запятой
      sscanf(buffer, "%d,%d,%d", &unitID, &BTime, &ETime);

    }

    Serial.print(unitID);
    Serial.print(":");
    Serial.print(BTime);
    Serial.print(":");
    Serial.println(ETime);


switch (unitID) {
  
  case 0:
  Serial.println("RELE    START   STOP");
           for (int i = 0; i < numPins; i++)
        {
          Serial.print(i+1);
          Serial.print("\t");          
          Serial.print(rlb[i]);
          Serial.print("\t");
          Serial.println(rle[i]);
       

         }
  break; 
  case 9:
         Serial.print("Ok, Time = ");
         Serial.print(tm.Hour);
         Serial.write(':');
         Serial.print(tm.Minute);
         Serial.write(':');
         Serial.println(tm.Second);
  break;      

case 1:
rlb[0]=BTime;
rle[0]=ETime;
break;

case 2:
rlb[1]=BTime;
rle[1]=ETime;
break;

case 3:
rlb[2]=BTime;
rle[2]=ETime;
break;

case 4:
rlb[3]=BTime;
rle[3]=ETime;
break;

case 5:
for (int i = 0; i < numPins; i++)
{
  EEPROM.write(i*10, rlb[i]/60);
  EEPROM.write(i*10+1, rlb[i]-rlb[i]/60*60);
  EEPROM.write(i*10+2, rle[i]/60);
  EEPROM.write(i*10+3, rle[i]-rle[i]/60*60);
   }
   Serial.println("Write to memory");
break;

case 8:
  tm.Hour = BTime;
  tm.Minute = ETime;
  RTC.write(tm);
break;

case 7:
Serial.println("Config via com port");
Serial.println("command,value1,value2");
Serial.println("0 - list reley time on/off.");
Serial.println("1-4 new relay value - X,Y,Z");
Serial.println("X - number of relay 1-4");
Serial.println("Y - time to on in minute"); 
Serial.println("Z - time to off in minute");
Serial.println("5 - write to EEPROM");
Serial.println("7 - this help");
Serial.println("9 - list current time");
Serial.println("8 - set time, 8,hh,mm");


break;

  
  }

  }

  //END SCETCH
}









