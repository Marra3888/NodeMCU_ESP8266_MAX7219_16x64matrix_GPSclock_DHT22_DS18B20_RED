//Data 25/04/2020

//#include <Arduino.h>
//#include <Wire.h>
//#include <SPI.h>
#include <ESP8266WiFi.h>


//-----------------------------------R e a d  G P S-----------------------------------------------------------------
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Timezone.h>
#include <Time.h>

//static const int RXPin = 2, TXPin = 0;//D4 - RX, D3 - TX
#define RXPin D9 //D4 //3
#define TXPin D10 //D3 //1

// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
// Central European Time
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 180}; // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 2, 120};   // Central European Standard Time
Timezone CE(CEST, CET);
TimeChangeRule *tcr;
//#define Plus2

static const uint32_t GPSBaud = 9600;
uint8_t h, m, s, dow, satelit;
uint8_t d, mm;
uint16_t y;

//----------------------------------Д а т ч и к и-------------------------------------------------------------------
//#include <BMP180MI.h>
#include <BMP180I2C.h>
#include <DHT.h>
//-----------Humidity------------------------------------
#define DHTPIN D3     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
  float hum = 0;
  float t = 0;
  float f = 0;
//---------------Pressure--------------------------------------
#define I2C_ADDRESS 0x77
BMP180I2C bmp180(I2C_ADDRESS);


//------------------------------two pins for two sensors DS18B20-----------------------------------------------------
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS_1 D4
#define ONE_WIRE_BUS_2 D5

OneWire oneWire_in(ONE_WIRE_BUS_1);
OneWire oneWire_out(ONE_WIRE_BUS_2);

DallasTemperature sensor_inhouse(&oneWire_in);
DallasTemperature sensor_outhouse(&oneWire_out);


//--------------------------------------------------------IR c o n t r o l------------------------------------------
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
#define kRecvPin D7

IRrecv irrecv(kRecvPin);

decode_results results;

//-------------------------------------В ы в о д   и н ф о р м а ц и и   н а   э к р а н---------------------------------------------------
#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 16

//#if ESP8266
#define DIN_PIN D8 //D8 - 15
#define CS_PIN  D6 //D6 - 12
#define CLK_PIN D0 //D7 - 13, D5 - 14
// HARDWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, CLK_PIN, CS_PIN, 16);
// SOFTWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

//---------------------------------------------Ш р и ф т ы---------------------------------------------------------
#include "Font_Data.h"
#include "font5_7_Vadim4.h"
#include "font5_7_Vadim3.h"
#include "Small_Font.h"
//#include <PS2Keyboard.h>

//#undef SDA                     //delete dafault SDA pin number
//#undef SCL                     //delete dafault SCL pin number

//#define SDA           D2        //assign new SDA pin to GPIO1/D2/0TX for all slaves on i2c bus
//#define SCL           D1        //assign new SCL pin to GPIO3/D7/0RX for all slaves on i2c bus

//const int DataPin = 8;
//const int IRQpin =  3;
//PS2Keyboard keyboard;

static byte _mode = 0;
bool _mode6;
//byte INTENSITY_Value = 0;
float Temp_out, Temp_in, Press;
static bool flasher = false;  // Проходящий секунд мигалкой
int scrollSpeed = 50;    // значение задержки кадра по умолчанию
static byte contrast = 0; 
//int timeSinceLastRead = 0; 

// Global message buffers shared by Serial and Scrolling functions
#define BUF_SIZE  75
#define MAX_MESG  75
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Hello! Enter new message?" };
bool curMessageAvailable = false;
bool newMessageAvailable = true;
bool ModeAAvailable = false;
bool ModeBAvailable = false;
bool ModeCAvailable = false;
bool ModeHAvailable = false;
bool ModeMAvailable = false;
bool ModeSAvailable = false;
bool ModeDowAvailable = false;
bool ModeDayAvailable = false;
bool ModeMonAvailable = false;
bool ModeYerAvailable = false;
bool ModeTemAvailable = false;


// Global variables
char szTime[9];    // mm:ss\0
char szMesgAll[MAX_MESG+1] = "";
char szMesg[MAX_MESG-50] = "";
char szMes[MAX_MESG+1] = "";
char szMeD[MAX_MESG+1] = "";
char szMeM[MAX_MESG+1] = "";
char szMeY[MAX_MESG+1] = "";
char szsecond[4];    // ss
//char szTime1[] = "0123";
char szMesgDate[MAX_MESG+1] = "";

uint8_t degC[] = { 6, 3, 3, 0, 14, 17, 17 }; // Deg C
uint8_t degF[] = { 6, 3, 3, 0, 31, 5, 1 }; // Deg F
char messagePressureLetter[] = {132, 160, 162, 171, 165, 173, 168, 165, '\0'};//Давление
char messageHumidityLetter[] = {130, 171, 160, 166, 173, 174, 225, 226, 236, '\0'};//Влажность
char messageInLetter[] = {130, ' ', 164, 174, 172, 165, '\0'};//В доме
char messageOutLetter[] = {141, 160, ' ', 227, 171, 168, 230, 165, '\0'};//На улице
char In[] = {127, '\0'};
char Out[] = {126, '\0'};
char IN[] = {255, '\0'};
char OUT[] = {254, '\0'};

#define RUS

//------------------------------------------------П у л ь т   R O M S A T-------------------------------------------------------------------------------------------
/*
#define K0_0  0x807FC837
#define K1_0  0x807F00FF
#define K2_0  0x807FE01F
#define K3_0  0x807F609F
#define K4_0  0x807F20DF
#define K5_0  0x807FD02F
#define K6_0  0x807F50AF
#define K7_0  0x807F10EF
#define K8_0  0x807FF00F
#define K9_0  0x807F708F
#define CH_up_0  0x807FE817
#define CH_down_0  0x807F58A7
#define VOLUME_MINUS_0  0x807F42BD
#define VOLUME_PLUS_0  0x807F827D
#define TIME0  0x807F28D7
#define MENU1_0  0x807FDA25
#define PAGE_up_0  0x807F02FD
#define PAGE_down_0  0x807F7887
#define OK  0x807FA857
#define POWER 0x807F807F
#define MUTE  0x807F48B7
#define SUBT  0x807F18E7
#define TTX   0x807F9867
#define FAV   0x807FAA55
#define AUDIO 0x807FC03F
#define ASPECT  0x807FEA15
#define VFORMAT 0x807F6A95
#define PAGE_PLUS 0x807F02FD
#define PAGE_MINUS  0x807F7887
#define MENU  0x807FDA25
#define EXIT  0x807F6897
#define INFO  0x807F728D
#define EPG   0x807F5AA5
#define RECALL  0x807F08F7
#define TV_RADIO  0x807F906F
#define _RED  0x807F8A75
#define _GREEN  0x807F4AB5
#define _YELLOW 0x807FCA35
#define _BLUE   0x807F2AD5
#define TIMER   0x807F28D7
#define REC     0x807F30CF
#define _LEFT   0x807FA25D
#define _RIGHT  0x807F38C7
#define _LEFT_STOP  0x807FF807
#define _RIGHT_STOP 0x807FE21D
#define _PLAY_PAUSE 0x807F22DD
#define _STOP       0x807F629D
*/

//-------------------------------------------П у л ь т   R e m o t e  C o n t r o l-------------------------------------------------------------------------------------------
/*
#define CH_up_0  0x20DF807F
#define CH_down_0  0x20DF609F
#define VOLUME_MINUS_0  0x20DFE01F
#define VOLUME_PLUS_0  0x20DFC03F
#define OK  0x20DF807F
#define MENU1_0  0x20DF10EF
//#define EXIT  0x807F6897
//#define POWER 0x20DF20DF
#define EXIT  0x20DF20DF
#define MOON  0x20DFA05F
#define Bright 0x20DF906F
#define Dim 0x20DF906F

#define K0_0  0x807FC837
#define K1_0  0x807F00FF
#define K2_0  0x807FE01F
#define K3_0  0x807F609F
#define K4_0  0x807F20DF
#define K5_0  0x807FD02F
#define K6_0  0x807F50AF
#define K7_0  0x807F10EF
#define K8_0  0x807FF00F
#define K9_0  0x807F708F
#define TIME0  0x807F28D7
#define PAGE_up_0  0x807F02FD
#define PAGE_down_0  0x807F7887
*/

//-------------------------------------------П у л ь т   H O P F E N-------------------------------------------------------------------------------------------
#define GREEN_CH_up_0  0xB590D52A
#define GREEN_CH_down_0  0xB5903DC2
#define GREEN_VOLUME_MINUS_0  0xB590F708
#define GREEN_VOLUME_PLUS_0  0xB590D728
#define GREEN_OK  0xB590F50A
#define GREEN_MENU1_0  0xB59035CA
#define GREEN_EXIT  0xB59005FA
#define GREEN_MOON  0xB590AF50
#define GREEN_Bright 0xB5909768
#define GREEN_Dim 0xB590D728
#define GREEN_ON 0xB5905FA0
#define GREEN_OFF 0xB59015EA

#define RED_CH_up_0  0x5EDD52A
#define RED_CH_down_0  0x5ED3DC2
#define RED_VOLUME_MINUS_0  0x5EDF708
#define RED_VOLUME_PLUS_0  0x5EDD728
#define RED_OK  0x5EDF50A
#define RED_MENU1_0  0x5ED35CA
#define RED_EXIT  0x5ED05FA
#define RED_MOON  0x5EDAF50
#define RED_Bright 0x5ED9768
#define RED_Dim 0x5EDD728
#define RED_ON 0x5ED5FA0
#define RED_OFF 0x5ED15EA

#define K0_0  0x807FC837
#define K1_0  0x807F00FF
#define K2_0  0x807FE01F
#define K3_0  0x807F609F
#define K4_0  0x807F20DF
#define K5_0  0x807FD02F
#define K6_0  0x807F50AF
#define K7_0  0x807F10EF
#define K8_0  0x807FF00F
#define K9_0  0x807F708F
#define TIME0  0x807F28D7
//#define PAGE_up_0  0x807F02FD
//#define PAGE_down_0  0x807F7887


//===================================================================================================
void(* resetFunc) (void) = 0;//объявить функцию сброса по адресу 0
//---------------------------------------------------------------------------------------------------

char *mon2str(uint8_t mon, char *psz, uint8_t len)
// Получить метку от PROGMEM в массив символов
{
  #ifdef RUS
  static const char str[12][9] PROGMEM =
  {
    {159,173,162,160,224,239,'\0'}, {148,165,162,224,160,171,239,'\0'}, {140,160,224,226,160,'\0'}, {128,175,224,165,171,239,'\0'}, {140,160,239,'\0'}, {136,238,173,239,'\0'},
    {136,238,171,239,'\0'}, {128,162,163,227,225,226,160,'\0'}, {145,165,173,226,239,161,224,239,'\0'}, {142,170,226,239,161,224,239,'\0'}, {141,174,239,161,224,239,'\0'}, {132,165,170,160,161,224,239,'\0'}
  };
  #else
  static const char str[][4] PROGMEM =
  {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  #endif
  *psz = '\0';
  mon--;
  if (mon < 12)
  {
    strncpy_P(psz, str[mon], len);
    psz[len] = '\0';
  }
  return(psz);
}
//------------------------------------------------------------------------------------------------

char *dow2str(uint8_t code, char *psz, uint8_t len)
{
  #ifdef RUS
  static const char str[7][12] PROGMEM =
  {
    {130,174,225,170,224,165,225,165,173,236,165,'\0'}, {143,174,173,165,164,165,171,236,173,168,170,'\0'}, {130,226,174,224,173,168,170,'\0'}, {145,224,165,164,160,'\0'},
    {151,165,226,162,165,224,163,'\0'}, {143,239,226,173,168,230,160,'\0'}, {145,227,161,161,174,226,160,'\0'}
  };
  #else
  static const char str[][10] PROGMEM =
  {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
  };
  #endif
  *psz = '\0';
  code--;
  if (code < 7)
  {
    strncpy_P(psz, str[code], len);
    psz[len] = '\0';
  }
  return(psz);
}
//------------------------------------------------------------------------------------------------
/*
void getHour(char *psz, bool f = true)
// Code for reading clock time
{
   if (flasher) sprintf(psz, "@@%c%02d", (f ? ':' : ' '), m);//Конвертирование данных в строку "@@-текст, %с-символ, %02d- d- Целое двойное десятичное число с нулем впереди// sprintf(myStr2,"%5d",x );    125//
                                                              //sprintf(myStr2,"%05d",x );00125//sprintf(myStr2,"Цикл %d закончен",x );Цикл   125 закончен//printf(myStr2,"Цикл %d закончен y= %d",x,y );Цикл   125 закончен y= 34//

  else sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
}
//------------------------------------------------------------------------------------------------

void getMinute(char *psz, bool f = true)
// Code for reading clock time
{
   if (flasher) sprintf(psz, "%02d%c%@@", h, (f ? ':' : ' '));

  else sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
}
//------------------------------------------------------------------------------------------------

void getSec(char *psz)
// Code for reading clock date
{
   if (flasher) sprintf(psz, " ");

  else   sprintf(psz, "%02d", s);
}*/
//------------------------------------------------------------------------------------------------

void getTime(char *psz, bool f = true)
// Code for reading clock time
{
//    RTC.readTime();
//    readGPS();
  sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
}
//-----------------------------------------------------------------------------------------------

void getsecond(char *psz)
// Code for reading clock date
{
  sprintf(psz, "%02d", s);
}
//------------------------------------------------------------------------------------------------

void getDate(char *psz)
// Code for reading clock date
{
  char  szBuf[10];
  sprintf(psz, "%0d %s %04d", d, mon2str(mm, szBuf, sizeof(szBuf)-1), y);
}
//------------------------------------------------------------------------------------------------

void getTemperature()
{
//    delay(100);
//    t = getTemperaturBMP180();
//    t = myHTU21D.readTemperature(HTU21D_TRIGGER_TEMP_MEASURE_NOHOLD);
//      t = myHTU21D.readTemperature();
//    hum = myHTU21D.readHumidity(HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD);
//      hum = myHTU21D.readHumidity();
    
  // read without samples.
//  int temperature = 0;
//  int humidity = 0;
//  int err = SimpleDHTErrSuccess;
//  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) 
//  {
//    delay(100);
//    return;
//  }
//  humidity = myHTU21D.readHumidity(HTU21D_TRIGGER_HUMD_MEASURE_NOHOLD);
//  temperature = myHTU21D.readTemperature(HTU21D_TRIGGER_TEMP_MEASURE_NOHOLD);
/*                  static uint8_t i = 0;
                  if(i++ > 200)
                  {
                    i = 0;
                    hum = dht.readHumidity();
                    delay(500);
                    t = dht.readTemperature();
                    f = (1.8 * t) + 32;                  
                  }
*/
  // Read temperature as Fahrenheit

  // Каждые 2 секунды.
//  if(timeSinceLastRead++ > 300) 
//  {
    // Считывание температуры и влажности около 250 милисекунд!
    // Показания датчика также могут составлять до 2 секунд
//    hum = dht.readHumidity(true); // Влажность
      hum = dht.readHumidity(false); // Влажность
//    t = dht.readTemperature(false, true); // Температура
      t = dht.readTemperature(false, false); // Температура
      f = dht.readTemperature(true);

    // Проверяем, получили ли данные с датчика.
//    if (isnan(hum) || isnan(t)) 
//    {
//      Serial.println("Данных нет! Останавливаем цикл и запускаем по новой");
//      timeSinceLastRead = 0;
//      return;
//    }
//    timeSinceLastRead = 0;
//  }
//  timeSinceLastRead += 100;
}
//-------------------------------------------------------------------------------------------------

void getDow(char *psz)
// Code for reading clock date
{
  if (ModeDowAvailable) 
  {
   if (flasher)  sprintf(psz, " ");

    else  dow2str(dow, szMes, MAX_MESG);
  }
  else dow2str(dow, szMes, MAX_MESG);
}
//--------------------------------------------------------------------------------------------------

void getDay(char *psz)
// Code for reading clock date
{
  if (ModeDayAvailable) 
  {
   if (flasher) sprintf(psz, " ");

  else sprintf(psz, "%02d", d);
  }
  else sprintf(psz, "%02d", d);
}
//---------------------------------------------------------------------------------------------------

void getMon(char *psz)
// Code for reading clock date
{
  if (ModeMonAvailable) 
  {
   if (flasher) sprintf(psz, " ");

  else {
          char  szBuf[4];
          sprintf(psz, "%s", mon2str(mm, szBuf, sizeof(szBuf)-1));
       }
  }
  else {
          char  szBuf[4];
          sprintf(psz, "%s", mon2str(mm, szBuf, sizeof(szBuf)-1));
       }
}
//----------------------------------------------------------------------------------------------------

void getYer(char *psz)
// Code for reading clock date
{
  if (ModeYerAvailable) 
  {
   if (flasher) sprintf(psz, " ");
   else sprintf(psz, "%04d", y);
  }
  else sprintf(psz, "%04d", y);
}
//--------------------------------------------------------------------------------------------------------
float getPressureBMP180()
{ 
  if (!bmp180.measurePressure())
  {
  }     
  do
  {
    delay(10);
  } while (!bmp180.hasValue());
     float value3 = (bmp180.getPressure()/133.3);
//    float value3 = bmp180.getPressure();
     return value3;
} 
//---------------------------------------------------------------------------------------------------------
float getTemperaturBMP180()
{ 
  if (!bmp180.measureTemperature())
  {
  }     
  do
  {
    delay(10);
  } while (!bmp180.hasValue());
     float value4 = bmp180.getTemperature();
     return value4;
} 
//----------------------------------------------------------------------------------------------------------
float getTemperaturDS18B20_in()
{ 
     sensor_inhouse.requestTemperatures(); 
     float value1 = sensor_inhouse.getTempCByIndex(0);
     return value1;
} 
//------------------------------------------------------------------------------------------------------------
float getTemperaturDS18B20_out()
{ 
     sensor_outhouse.requestTemperatures(); 
     float value2 = sensor_outhouse.getTempCByIndex(0);
     return value2;
} 
//----------------------------------------------------------------------------------------------------------
float getTemperaturDS18B20_in_F()
{ 
     sensor_inhouse.requestTemperatures(); 
     float value5 = sensor_inhouse.getTempFByIndex(0);
     return value5;
} 

//===========================================================================================================

void getmodea()
{
// initialise the LED display
  P.begin(4);
//  P.displayShutdown(true);
//  P.displayReset();
  delay(100);
    P.displayReset();
  // Установите зоны для 4 половин дисплея
  // Каждая зона получает свой шрифт, составляющий верхнюю и нижнюю половину каждой буквы
  
  P.setZone(0, 0, MAX_DEVICES - 11);
  P.setZone(1, MAX_DEVICES - 10, MAX_DEVICES - 9);
  P.setZone(2, MAX_DEVICES - 8, MAX_DEVICES - 3);
  P.setZone(3, MAX_DEVICES - 2, MAX_DEVICES - 1);
      
      P.setFont(0, BigFontLower);
//      P.setFont(0, NULL);
      P.setFont(1, Vadim3);
      P.setFont(2, BigFontUpper);
//      P.setFont(2, NULL);
      P.setFont(3, Vadim3);

    P.setZoneEffect(0, true, PA_FLIP_LR);
        P.setZoneEffect(1, true, PA_FLIP_LR);
            P.setZoneEffect(2, true, PA_FLIP_LR);
                P.setZoneEffect(3, true, PA_FLIP_LR);
//  PZ.setZoneEffect(true, PA_FLIP_LR);
  P.displayZoneText(0, szTime, PA_RIGHT, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(1, szsecond, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(2, szTime, PA_RIGHT, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(3, szMesgAll, PA_CENTER, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
  P.addChar('^', degC);
  P.addChar('_', degF);
  P.setIntensity(contrast);
  P.displayClear();
}
//-----------------------------------------------------------------------------------------------------------------

void getmodeb()
{
  P.begin(2);
  // Установите зоны для 4 половин дисплея
  // Каждая зона получает свой шрифт, составляющий верхнюю и нижнюю половину каждой буквы
  P.setZone(0, 0, MAX_DEVICES - 9);
  P.setZone(1, MAX_DEVICES - 8, MAX_DEVICES - 1);
//      P.setFont(0, BigFontLower);
//      P.setFont(1, BigFontUpper);
        P.setZoneEffect(0, true, PA_FLIP_LR);
        P.setZoneEffect(1, true, PA_FLIP_LR);
  P.setIntensity(contrast);
  P.displayClear();
}
//------------------------------------------------------------------------------------------------------------------

void getmodec()
{
  P.begin(2);
  P.setZone(0, 0, MAX_DEVICES - 9);
  P.setZone(1, MAX_DEVICES - 8, MAX_DEVICES - 1);
      P.setFont(0, BigFontLower);
      P.setFont(1, BigFontUpper);
      P.setZoneEffect(0, true, PA_FLIP_LR);
      P.setZoneEffect(1, true, PA_FLIP_LR);
        P.displayZoneText(0, newMessage, PA_LEFT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
        P.displayZoneText(1, newMessage, PA_LEFT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
    P.displayReset(0);
    P.displayReset(1);
    P.setIntensity(contrast);
    P.displayClear();
}
//------------------------------------------------------------------------------------------------------------------

void getmoded()
{
  P.begin(1);
  P.setZone(0, 0, MAX_DEVICES - 9);
      P.setFont(0, NULL);
      P.setZoneEffect(0, true, PA_FLIP_LR);
    P.displayZoneText(0, curMessage, PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
//      P.displayZoneText(0, curMessage, PA_LEFT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
    P.displayReset(0);
    P.setIntensity(contrast);
    P.displayClear();
}
//------------------------------------------------------------------------------------------------------------------

void getmodee()
{
  P.begin(1);
  P.setZone(0, 0, MAX_DEVICES - 1);
      P.setFont(0, NULL);
      P.setZoneEffect(0, true, PA_FLIP_LR);
    P.displayZoneText(0, newMessage, PA_LEFT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
    P.displayReset(0);
    P.setIntensity(contrast);
    P.displayClear();
}
//------------------------------------------------------------------------------------------------------------------

void getmodef()
{
// initialise the LED display
  P.begin(4);
  // Установите зоны для 4 половин дисплея
  // Каждая зона получает свой шрифт, составляющий верхнюю и нижнюю половину каждой буквы
  P.setZone(0, 0, MAX_DEVICES - 14);
  P.setZone(1, MAX_DEVICES - 13, MAX_DEVICES - 11);
  P.setZone(2, MAX_DEVICES - 10, MAX_DEVICES - 9);
  P.setZone(3, MAX_DEVICES - 8, MAX_DEVICES - 1);
      P.setFont(0, NULL);
      P.setFont(1, NULL);
      P.setFont(2, NULL);
      P.setFont(3, NULL);
          P.setZoneEffect(0, true, PA_FLIP_LR);
            P.setZoneEffect(1, true, PA_FLIP_LR);
               P.setZoneEffect(2, true, PA_FLIP_LR);
                  P.setZoneEffect(3, true, PA_FLIP_LR);
  P.displayZoneText(3, szMes, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(0, szMeY, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(2, szMeD, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(1, szMeM, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.setIntensity(contrast);
  P.displayClear();
}
//------------------------------------------------------------------------------------------------------------------
void _mode_()
{
              if(_mode == 8)
              {
                ModeAAvailable = true;
                ModeBAvailable = false;
                ModeTemAvailable = false;
                getmodea();
              }
              else
                  {
                    ModeAAvailable = false;
                    ModeBAvailable = false;
                    ModeTemAvailable = false;
                    P.begin(2);
                    P.setFont(0, Vadim3);
                    P.setFont(1, Vadim3);
                    P.setZone(0, 0, MAX_DEVICES - 9);
                    P.setZone(1, MAX_DEVICES - 8, MAX_DEVICES - 1);

                    if(_mode == 5)
                      {
                          
                          P.displayZoneText(0, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                //         P.displayZoneText(0, szMesgAll, PA_RIGHT, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                //         P.displayZoneText(1, szMesgDate, PA_RIGHT, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                          P.displayZoneText(1, szMesgDate, PA_LEFT, scrollSpeed, 0, PA_PRINT, PA_NO_EFFECT);
                      }
                    if(_mode == 6)
                      {
        //                  P.begin(2);
        //                  P.setFont(0, Vadim3);
        //                  P.setFont(1, Vadim3);
        //                  P.setZone(0, 0, MAX_DEVICES - 9);
        //                  P.setZone(1, MAX_DEVICES - 8, MAX_DEVICES - 1);
                          P.displayZoneText(0, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                //          P.displayZoneText(0, szMesgAll, PA_RIGHT, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                          P.displayZoneText(1, szMesgDate, PA_LEFT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                      }
                    if(_mode == 7)
                      {
        //                  P.begin(2);
        //                  P.setFont(0, Vadim3);
        //                  P.setFont(1, Vadim3);
        //                  P.setZone(0, 0, MAX_DEVICES - 9);
        //                  P.setZone(1, MAX_DEVICES - 8, MAX_DEVICES - 1);
                          
                          P.displayZoneText(0, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                //          P.displayZoneText(0, szMesgAll, PA_RIGHT, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                          P.displayZoneText(1, szMesgDate, PA_LEFT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                      }
                      P.setZoneEffect(0, true, PA_FLIP_LR);
                      P.setZoneEffect(1, true, PA_FLIP_LR);
                      P.addChar('^', degC);
                      P.setIntensity(contrast);
                      P.displayClear(2);
                  }
}
//------------------------------------------------------------------------------------------------------------------

void readkeyboard()
{

/*                else if (results.value == PAGE_PLUS) 
                      {
                        ModeAAvailable = false;
                        curMessageAvailable = false;
                        ModeCAvailable = true;
                        newMessageAvailable = true;
                        getmodec();
                      } 
                      else if (results.value == MENU) 
                            {
                              ModeAAvailable = false;
                              ModeHAvailable = true;
                            } */
//                            readkeys();
                      /*     else if (results.value == GREEN_VOLUME_PLUS_0) 
                                  {
                                    scrollSpeed += 10;
                                    if (scrollSpeed >= 200 )  scrollSpeed = 0;
                                    P.setSpeed(scrollSpeed);
                                  }
                                  else if (results.value == GREEN_VOLUME_MINUS_0) 
                                        {
                                          scrollSpeed -= 10;
                                          if (scrollSpeed < 0 )  scrollSpeed = 200;
                                          P.setSpeed(scrollSpeed);
                                        }
                                        else if (results.value == GREEN_CH_up_0) 
                                              {
                                                contrast++;
                                                if (contrast > 15 )  contrast = 0;
                                                P.setIntensity(contrast);
                                              }
                                              else if (results.value == GREEN_CH_down_0) 
                                                    {
                                                      contrast--;
                                                      if (contrast > 15 )  contrast = 15;
                                                      P.setIntensity(contrast);
                                                    }
      IR_results();
      irrecv.resume(); // Receive the next value  */
//      }//end while
//  }
//   else 
/*      
                    else if (results.value == PAGE_PLUS) 
                          {
                            ModeBAvailable = false;
                            curMessageAvailable = false;
                            ModeCAvailable = true;
                            newMessageAvailable = true;
                            getmodec();
                          } 
                          else if (results.value == PAGE_MINUS) 
                                {
                                  ModeBAvailable = false;
                                  ModeHAvailable = true;
                                  getmodea();
                                }*/
//                                readkeys();
                         /*       else if (results.value == GREEN_VOLUME_PLUS_0) 
                                      {
                                        scrollSpeed += 10;
                                        if (scrollSpeed > 200 )  scrollSpeed = 0;
                                        P.setSpeed(scrollSpeed);
                                      }
                                      else if (results.value == GREEN_VOLUME_MINUS_0) 
                                            {
                                              scrollSpeed -= 10;
                                              if (scrollSpeed < 0 )  scrollSpeed = 200;
                                              P.setSpeed(scrollSpeed);
                                            }
                                            else if (results.value == GREEN_CH_up_0) 
                                                  {
                                                    contrast++;
                                                    if (contrast > 15)  contrast = 0;
                                                    P.setIntensity(contrast);
                                                  }
                                                  else if (results.value == GREEN_CH_down_0) 
                                                        {
                                                          contrast--;
                                                          if (contrast > 15 )  contrast = 15;
                                                          P.setIntensity(contrast);
                                                        }
            IR_results();
            irrecv.resume(); // Receive the next value    */                                              
//            }//end while
//    }
//   else 
//              readkeys();
               /*     else if (results.value == GREEN_VOLUME_PLUS_0) 
                 {
                    scrollSpeed += 10;
                  if (scrollSpeed > 200 )  scrollSpeed = 0;
                  P.setSpeed(scrollSpeed);
                  }
                    else if (results.value == GREEN_VOLUME_MINUS_0) 
                          {
                            scrollSpeed -= 10;
                            if (scrollSpeed < 0 )  scrollSpeed = 200;
                            P.setSpeed(scrollSpeed);
                          }
                            else if (results.value == GREEN_CH_up_0) 
                                  {
                                    contrast++;
                                    if (contrast > 15)  contrast = 0;
                                    P.setIntensity(contrast);
                                  }
                                    else if (results.value == GREEN_CH_down_0) 
                                          {
                                            contrast--;
                                            if (contrast > 15 )  contrast = 15;
                                            P.setIntensity(contrast);
                                          }                                           
              IR_results();
              irrecv.resume(); // Receive the next value*/
//            }
//          }
/*   else if (ModeCAvailable)
    {
        static char *cp = curMessage;
        if(irrecv.decode(&results))
              {
                *cp = results.value;
                if (*cp == OK)
                  {
                    *cp = '\0'; // end the string
                    cp = curMessage;
                    strcpy(newMessage, curMessage);
                    newMessageAvailable = true;
                  } 
                else 
                    if (*cp == EXIT)  resetFunc();
                    else if (*cp == PAGE_PLUS) 
                          {
                              ModeCAvailable = false;
                              newMessageAvailable = false;
                              curMessageAvailable = false;
                              ModeBAvailable = false;
                              ModeAAvailable = true;
                              getmodea();
                          }   
                         else if (*cp == PAGE_MINUS) 
                                {
                                    ModeCAvailable = false;
                                    newMessageAvailable = false;
                                    curMessageAvailable = false;
                                    ModeAAvailable = false;
                                    ModeBAvailable = true;
                                    getmodeb();
                                }  
                              else if (*cp == TV_RADIO) curMessageAvailable = true;
                                   else if (*cp == RECALL) newMessageAvailable = true;
                                        else  // move char pointer to next position
                                             cp++;
                  getmoded();
                  irrecv.resume(); // Receive the next value
              }//end while
  }
   else if (ModeHAvailable)
    {
      if(irrecv.decode(&results))
            {
              if (results.value == OK) 
                {
                  ModeHAvailable = false;
                  ModeAAvailable = true;
                }
              else if (results.value == VOLUME_PLUS_0) 
                    {
                      ModeHAvailable = false;
                      ModeMAvailable = true;
                    }
                    else if (results.value == VOLUME_MINUS_0) 
                          {
                            ModeHAvailable = false;
                            ModeYerAvailable = true;
                            getmodef();
                          }
                      else if (results.value == CH_up_0) 
                            {
//                              RTC.h++;
//                              if (RTC.h >= 24 )  RTC.h = 0;
//                              RTC.writeTime();
                            }
                        else if (results.value == CH_down_0) 
                              {
//                                RTC.h--;
//                                if (RTC.h <= 0 )  RTC.h = 23;
//                                RTC.writeTime();
                              }
            irrecv.resume(); // Receive the next value
            }//end while
    }
   else if (ModeMAvailable)
    {
      if(irrecv.decode(&results))
            {
              if (results.value == OK) 
                {
                  ModeMAvailable = false;
                  ModeAAvailable = true;
                }
                else if (results.value == VOLUME_PLUS_0) 
                      {
                        ModeMAvailable = false;
                        ModeSAvailable = true;
                      }
                      else if (results.value == VOLUME_MINUS_0) 
                            {
                              ModeMAvailable = false;
                              ModeHAvailable = true;
                            }
                            else if (results.value == CH_up_0) 
                                  {
//                                    RTC.m++;
//                                    if (RTC.m >= 60 )  RTC.m = 0;
//                                    RTC.writeTime();
                                  }
                                  else if (results.value == CH_down_0) 
                                        {
//                                          RTC.m--;
//                                          if (RTC.m <= 0 )  RTC.m = 59;
//                                         RTC.writeTime();
                                        }
            irrecv.resume(); // Receive the next value
            }//end while
    }
   else if (ModeSAvailable)
    {
      if(irrecv.decode(&results))
            {
              if (results.value == OK) 
                {
                  ModeSAvailable = false;
                  ModeAAvailable = true;
                  getmodea();
                }
                else if (results.value == VOLUME_PLUS_0) 
                      {
                        ModeSAvailable = false;
                        ModeDowAvailable = true;
                        getmodef();
                      }
                      else if (results.value == VOLUME_MINUS_0) 
                            {
                              ModeSAvailable = false;
                              ModeMAvailable = true;
                              getmodea();
                            }
                            else if (results.value == CH_up_0) 
                                  {
//                                    RTC.s = 0;
 //                                   RTC.writeTime();
                                  }
                                  else if (results.value == CH_down_0) 
                                        {
//                                          RTC.s = 0;
//                                          RTC.writeTime();
                                        }
            irrecv.resume(); // Receive the next value
            }//end while
    }
   else if (ModeDowAvailable)
    {
      if(irrecv.decode(&results))
            {
              if (results.value == OK) 
                {
                  ModeDowAvailable = false;
                  ModeAAvailable = true;
                  getmodea();
                }
                else if (results.value == VOLUME_PLUS_0) 
                      {
                        ModeDowAvailable = false;
                        ModeDayAvailable = true;
                      }
                      else if (results.value == VOLUME_MINUS_0) 
                            {
                              ModeDowAvailable = false;
                              ModeSAvailable = true;
                              getmodea();
                            }
                            else if (results.value == CH_up_0) 
                                  {
//                                    RTC.dow++;
//                                    if (RTC.dow >= 8 )  RTC.dow = 1;
//                                    RTC.writeTime();
                                  }
                                  else if (results.value == CH_down_0) 
                                        {
//                                          RTC.dow--;
//                                          if (RTC.dow <= 0 )  RTC.dow = 7;
//                                          RTC.writeTime();
                                        }
            irrecv.resume(); // Receive the next value
            }//end while
    }
   else if (ModeDayAvailable)
    {
      if(irrecv.decode(&results))
            {
              if (results.value == OK) 
                {
                  ModeDayAvailable = false;
                  ModeAAvailable = true;
                  getmodea();
                }
                else if (results.value == VOLUME_PLUS_0) 
                      {
                        ModeDayAvailable = false;
                        ModeMonAvailable = true;
                      }
                      else if (results.value == VOLUME_MINUS_0) 
                            {
                              ModeDayAvailable = false;
                              ModeDowAvailable = true;
                            }
                            else if (results.value == CH_up_0) 
                                  {
//                                    RTC.dd++;
//                                    if (RTC.dd >= 32 )  RTC.dd = 1;
 //                                   RTC.writeTime();
                                  }
                                  else if (results.value == CH_down_0) 
                                        {
//                                          RTC.dd--;
//                                          if (RTC.dd <= 0 )  RTC.dd = 31;
//                                          RTC.writeTime();
                                        }
            irrecv.resume(); // Receive the next value
            }//end while
    }
   else if (ModeMonAvailable)
    {
      if(irrecv.decode(&results))
            {
              if (results.value == OK) 
                {
                  ModeMonAvailable = false;
                  ModeAAvailable = true;
                  getmodea();
                }
                else if (results.value == VOLUME_PLUS_0) 
                      {
                        ModeMonAvailable = false;
                        ModeYerAvailable = true;
                      }
                      else if (results.value == VOLUME_MINUS_0) 
                            {
                              ModeMonAvailable = false;
                              ModeDayAvailable = true;
                            }
                            else if (results.value == CH_up_0) 
                                  {
//                                    RTC.mm++;
//                                    if (RTC.mm >= 13 ) RTC.mm = 1;
//                                    RTC.writeTime();
                                  }
                                  else if (results.value == CH_down_0) 
                                        {
//                                          RTC.mm--;
//                                          if (RTC.mm <= 0 ) RTC.mm = 12;
//                                          RTC.writeTime();
                                        }
            irrecv.resume(); // Receive the next value
            }//end while
    }
   else if (ModeYerAvailable)
    {
      if(irrecv.decode(&results))
            {
                if (results.value == OK) 
                  {
                    ModeYerAvailable = false;
                    ModeAAvailable = true;
                    getmodea();
                  }
                  else if (results.value == VOLUME_PLUS_0) 
                        {
                          ModeYerAvailable = false;
                          ModeHAvailable = true;
                          getmodea();
                        }
                        else if (results.value == VOLUME_MINUS_0) 
                              {
                                ModeYerAvailable = false;
                                ModeMonAvailable = true;
                              }
                              else if (results.value == CH_up_0) 
                                    {
 //                                     RTC.yyyy++;
 //                                     if (RTC.yyyy >= 2035 )  RTC.yyyy = 2015;
 //                                     RTC.writeTime();
                                    }
                                    else if (results.value == CH_down_0) 
                                          {
 //                                           RTC.yyyy--;
 //                                           if (RTC.yyyy <= 2010 )  RTC.yyyy = 2025;
 //                                           RTC.writeTime();
                                          }
            irrecv.resume(); // Receive the next value
            }//end while
            
    }*/

    
}//end readkeyboard


//=====================================================================================================


void IRRead() 
{
    if (irrecv.decode(&results))
    {
      IR_results();
      irrecv.resume(); // Receive the next value
    }

}

void IR_results()
{
/*        if(results.value == K0_0)  {_mode = 0; _mode_(); P.displayClear();}

        if(results.value == K1_0)  {_mode = 1; _mode_(); P.displayClear();}

        if(results.value == K2_0)  {_mode = 2; _mode_(); P.displayClear();}

        if(results.value == K3_0)  {_mode = 3; _mode_(); P.displayClear();}

        if(results.value == K4_0)  {_mode = 4; _mode_(); P.displayClear();}

        if(results.value == K5_0)  {_mode = 5; _mode_(); P.displayClear();}

        if(results.value == K6_0)  {_mode = 6; _mode_(); P.displayClear();}

        if(results.value == K7_0)  {_mode = 7; _mode_(); P.displayClear();}
        
        if(results.value == K8_0)  {_mode = 8; _mode_(); P.displayClear();} */

        if(results.value == GREEN_ON)  {_mode = 8; _mode_(); P.displayClear();} 

    else    if(results.value == TIME0)  {_mode = 7; _mode6 = false; _mode_(); P.displayClear();}

    else    if(results.value == RED_CH_up_0)  {_mode++; _mode_(); P.displayClear();}

    else    if(results.value == RED_CH_down_0)  {_mode--; _mode_(); P.displayClear();}

    else    if(results.value == GREEN_MENU1_0)  {_mode = 7; _mode6 = true; _mode_(); P.displayClear();}

    else    if(results.value == GREEN_CH_down_0)
              {
                if(contrast > 2)  contrast = 2;
                  else  contrast--;
                  P.setIntensity(contrast);
              }
    else    if(results.value == GREEN_CH_up_0)
              {
                if(contrast > 2)  contrast = 0;
                  else contrast++;
                  P.setIntensity(contrast);
              }
    else    if(results.value == GREEN_MOON)
              {
                contrast = 0;
                P.setIntensity(contrast);
              }
    else    if (results.value == GREEN_VOLUME_PLUS_0) 
                {
                  scrollSpeed += 5;
                  if (scrollSpeed > 200 )  scrollSpeed = 0;
                  P.setSpeed(scrollSpeed);
                }
    else    if (results.value == GREEN_VOLUME_MINUS_0) 
                {
                  scrollSpeed -= 5;
                  if (scrollSpeed < 0 )  scrollSpeed = 200;
                  P.setSpeed(scrollSpeed);
                }
    else    if (ModeAAvailable)
                {
                  if (results.value == GREEN_OK) 
                      {
                      //  Serial.println("GREEN_OK_A");
                        _mode = 8;
                        ModeAAvailable = false;
                        ModeBAvailable = true;
                        ModeTemAvailable = false;
                        getmodeb();
                      }
                  else if (results.value == GREEN_EXIT)  resetFunc();
                }
           else if (ModeBAvailable)
                    {
                      if (results.value == GREEN_OK) 
                        {
                          //  Serial.println("GREEN_OK_B");
                            _mode = 8;
                            ModeAAvailable = false;
                            ModeBAvailable = false;
                            ModeTemAvailable = true;
                            getmodeb();
                        }
                      else if (results.value == GREEN_EXIT)  resetFunc();
                    }
              else if(ModeTemAvailable)
                      {
                        if (results.value == GREEN_OK) 
                          {
                          //  Serial.println("GREEN_OK_Temp");
                            _mode = 8;
                            ModeAAvailable = true;
                            ModeBAvailable = false;
                            ModeTemAvailable = false;
                            getmodea();
                          }
                        else if (results.value == GREEN_EXIT)  resetFunc();
                      }

}

//====================================================================================================

void setup() 
{
    WiFi.mode(WIFI_OFF);
    WiFi.persistent(false); //disable saving wifi config into SDK flash area
    WiFi.softAPdisconnect(false);
    WiFi.disconnect(false);
    WiFi.forceSleepBegin(); //disable swap & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep 

//    Serial.begin(57600);
//    Serial.println("OK");
//  ModeTemAvailable = true;
//  ModeAAvailable = true;
//  getmodea();
//    getmodeb();
//  delay(3000);
  
//  P.begin(MAX_DEVICES);
//    P.setIntensity(contrast);
//    P.displayClear();
    ss.begin(GPSBaud);
    irrecv.enableIRIn();  // Start the receiver
    sensor_inhouse.begin();
    sensor_outhouse.begin();
    Wire.begin(SDA, SCL);
//      Wire.setClock(100000UL);

        bmp180.begin();
        //reset sensor to default parameters.
        bmp180.resetToDefaults();
        //enable ultra high resolution mode for pressure measurements
        bmp180.setSamplingMode(BMP180MI::MODE_UHR);

        dht.begin();


  getmoded();
    delay(100);
  getmoded();
      while (h == 0 && m == 0)
            {
              if(P.displayAnimate())   P.displayZoneText(0, "GPS conecting", PA_LEFT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
              
              while (ss.available())
                    {
                      gps.encode(ss.read());
                    }
                    if (gps.time.isValid() && gps.date.isValid())
                        {
                            h = gps.time.hour();
                            m = gps.time.minute();
                        }
            }
//  readGPS();
//    _mode = 7;
//    _mode_(); 

    _mode = 8;
     ModeBAvailable = false;
     ModeAAvailable = false;
     ModeTemAvailable = true;
     getmodeb();
}
//======================================================================================================

void loop() 
{
  static uint32_t lastTime = 0; // millis() memory
  static uint8_t  _display = 0;  // current display mode
  static uint8_t  cycle_Tem, cycle = 0;  // current display mode

  IRRead();
  readGPS();

  if(_mode < 8)
  {
      P.displayAnimate();

              if(_mode == 0)
                {
                  // Вых температура
                   Temp_out = getTemperaturDS18B20_out();
                   dtostrf(Temp_out, 5, 2, szMesg);
                   strcpy(szMesgAll, "Out ");
                   strcat(szMesgAll, szMesg);
                   strcat(szMesgAll, "^");
                   P.displayZoneText(1, szMesgAll, PA_RIGHT, 0, 0, PA_PRINT, PA_NO_EFFECT);
                   P.displayAnimate();
                    
                   // Вх температура
                   Temp_in = getTemperaturDS18B20_in();
                   dtostrf(Temp_in, 5, 2, szMesg);
                   strcpy(szMesgAll, "In    ");
                   strcat(szMesgAll, szMesg);
                   strcat(szMesgAll, "^");
                   P.displayZoneText(0, szMesgAll, PA_RIGHT, 0, 0, PA_PRINT, PA_NO_EFFECT); 
                   P.displayAnimate();
                }

              if(_mode == 1)
                {
                  //Давление
                  Press = getPressureBMP180();
                  delay(500);
                  dtostrf(Press, 5, 2, szMesg);
                  strcpy(szMesgAll, messagePressureLetter);
                  P.displayZoneText(1, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                  P.displayAnimate();
                  strcpy(szMesgAll, szMesg);
                  strcat(szMesgAll, " mmHg");
                  P.displayZoneText(0, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT); 
                  P.displayAnimate();  
                }

              if(_mode == 2)
              {
                //Влажность
                hum = dht.readHumidity();
                dtostrf(hum, 5, 2, szMesg);
                strcpy(szMesgAll, messageHumidityLetter);
                P.displayZoneText(1, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                P.displayAnimate();
                strcpy(szMesgAll, szMesg);
                strcat(szMesgAll, " %");
                P.displayZoneText(0, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT); 
                P.displayAnimate();
              }

            if(_mode == 3)
              {
                //Вых температура
                Temp_out = getTemperaturDS18B20_out();
                dtostrf(Temp_out, 5, 2, szMesg);
                strcpy(szMesgAll, messageOutLetter);
                P.displayZoneText(1, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                P.displayAnimate();
                strcpy(szMesgAll, szMesg);
                strcat(szMesgAll, "^");
                P.displayZoneText(0, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT); 
                P.displayAnimate();
              }

            if(_mode == 4)
              {
                //Вх температура
                Temp_in = getTemperaturDS18B20_in();
                dtostrf(Temp_in, 5, 2, szMesg);
                strcpy(szMesgAll, messageInLetter);
                P.displayZoneText(1, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                P.displayAnimate();
                strcpy(szMesgAll, szMesg);
                strcat(szMesgAll, "^");
                P.displayZoneText(0, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT); 
                P.displayAnimate();
              }

            if(_mode == 5)
              {
          //      readGPS();
                dow2str(dow, szMesgAll, MAX_MESG);
                P.displayReset(0);
                getDate(szMesgDate);
                if (P.getZoneStatus(1))
                    {  
                      P.setTextEffect(1, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                      P.displayReset(1);
                    }
              }
          
            if(_mode == 6)
              {
         //       readGPS();
                getTime(szMesgAll);
                getDate(szMesgDate);
                P.displayReset(0);

                if (P.getZoneStatus(1))
                    {  
                      P.setTextEffect(1, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                      P.displayReset(1);
                    }
              }

            if(_mode == 7)
              {
         //      readGPS();
               getDate(szMesgDate);
                if (millis() - lastTime >= 1000)
                  {
                    lastTime = millis();
          ///          getsecond(szsecond);
                    getTime(szMesgAll, flasher);
                    flasher = !flasher;
                  } 
                                //  if (P.getZoneStatus(0))
                                //  {      
                                  //  P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);    
                                  //      if (P.getZoneStatus(0))
                                  //        {  
                                  //         P.displayZoneText(0, szMesgAll, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
                                  //            P.displayZoneText(0, szMesgAll, PA_RIGHT, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                                   //         strcpy(szMesgAll, szTime);
                                   //         P.displayAnimate();
                                           P.displayReset(0);
                                   //       }
                                            
                                            
                                      if (P.getZoneStatus(1))
                                          {  
                                            P.setTextEffect(1, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                       //     P.displayZoneText(1, szMesgAll, PA_RIGHT, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                                      //       P.displayZoneText(1, szMesgAll, PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
                                            
                                            P.displayReset(1);
                                      //     P.displayReset(0);
                                    //       P.displayAnimate();
                                            }
              }        
  }        
//================================================================================================================ 
    if(_mode == 8)
      { 
                
        P.displayAnimate();
                                            if (ModeAAvailable)
                                            {
                                          //    readGPS();
                                                  //настроить строку времени, если мы должны
                                                  if (millis() - lastTime >= 500)
                                                  {
                                                    lastTime = millis();
                                                    getsecond(szsecond);
                                                    getTime(szTime, flasher);
                                                    flasher = !flasher;
                                                    P.displayReset(0);
                                                    P.displayReset(1);
                                                    P.displayReset(2);
                                                  } 
                                                  
                                                  if (P.getZoneStatus(3))
                                                  {
                                                    switch (_display)
                                                    {
                                                      case 0: // day of week
                                                              P.setTextEffect(3, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                              _display++;
                                                              dow2str(dow, szMesgAll, MAX_MESG);
                                                        break;
                                                        
                                                      case 1: // Calendar
                                                              P.setTextEffect(3, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                              _display++;
                                                              getDate(szMesgAll);
                                                        break;
                                                
                                                      case 2: // Temperature deg °C
                                                              P.setTextEffect(3, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                              _display++;
                                                              Temp_in = getTemperaturDS18B20_in();
                                                              dtostrf(Temp_in, 3, 1, szMesg);
                                                              strcpy(szMesgAll, IN);
                                                              strcat(szMesgAll, szMesg);
                                                              strcat(szMesgAll, "^");
                                                        break;
                                                
                                                      case 3: // Temperature deg °C
                                                              P.setTextEffect(3, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                              _display++;
                                                              Temp_out = getTemperaturDS18B20_out();
                                                              dtostrf(Temp_out, 3, 1, szMesg);
                                                              strcpy(szMesgAll, OUT);
                                                              strcat(szMesgAll, szMesg);
                                                              strcat(szMesgAll, "^");
                                                        break;
                                                
                                                      case 4: // Relative Humidity
                                                              P.setTextEffect(3, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                              _display++;
                                                              getTemperature();
                                                              dtostrf(hum, 3, 1, szMesgAll);
                                                              strcat(szMesgAll, "% RH");
                                                        break;
                                                      case 5:// Pressure
                                                              P.setTextEffect(3, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                              _display = 0;
                                                              Press = getPressureBMP180();
                                                              dtostrf(Press, 3, 1, szMesgAll);
                                                              strcat(szMesgAll, " mmHg");
                                                        break;
                                                      default:
                                                                _display = 0;
                                                        break;
                                                    }
                                                    P.displayReset(3);
                                                  }
                                            }
                                           else
                                            if (ModeBAvailable)
                                            {
                                        //      readGPS();
                                                  if (P.getZoneStatus(0) && P.getZoneStatus(1))
                                                  {
                                                    switch (cycle)
                                                    {
                                                      case 0: // time
                                                                P.setFont(0, BigFontLower);
                                                                P.setFont(1, BigFontUpper);
                                                                P.displayZoneText(0, szMesg, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_UP, PA_SCROLL_DOWN);
                                                                P.displayZoneText(1, szMesg, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_DOWN, PA_SCROLL_UP);
                                                                cycle++;
                                                                getTime(szMesg);
                                                        break;
                                                        
                                                      case 1: // day of week
                                                                P.setFont(0, BigFontLower);
                                                                P.setFont(1, BigFontUpper);
                                                                P.displayZoneText(0, szMesg, PA_RIGHT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                                P.displayZoneText(1, szMesg, PA_LEFT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                                cycle++;
                                                                dow2str(dow, szMesg, MAX_MESG);
                                                        break;
                                                        
                                                      case 2: // Calendar
                                                                P.setFont(0, BigFontLower);
                                                                P.setFont(1, BigFontUpper);
                                                                P.displayZoneText(0, szMesg, PA_RIGHT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                                P.displayZoneText(1, szMesg, PA_LEFT, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                                cycle++;
                                                                getDate(szMesg);
                                                        break;
                                                        
                                                      case 3: // Temperature deg °C out
                                                                P.setFont(0, BigFontLower);
                                                                P.setFont(1, BigFontUpper);
                                                                cycle++;
                                                                Temp_out = getTemperaturDS18B20_out();
                                                                dtostrf(Temp_out, 3, 1, szMesg);
                                                                strcpy(szMesgAll, Out);
                                                                strcat(szMesgAll, szMesg);
                                                                strcat(szMesgAll, "$");
                                                                P.displayZoneText(0, szMesgAll, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_UP, PA_SCROLL_UP);
                                                                P.displayZoneText(1, szMesgAll, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_DOWN, PA_SCROLL_DOWN);
                                                        break;
                                                
                                                      case 4: // Temperature deg °C in
                                                                P.setFont(0, BigFontLower);
                                                                P.setFont(1, BigFontUpper);
                                                                cycle++;
                                                                Temp_in = getTemperaturDS18B20_in();
                                                                dtostrf(Temp_in, 3, 1, szMesg);
                                                                strcpy(szMesgAll, In);
                                                                strcat(szMesgAll, szMesg);
                                                                strcat(szMesgAll, "$");
                                                                P.displayZoneText(0, szMesgAll, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_UP, PA_SCROLL_UP);
                                                                P.displayZoneText(1, szMesgAll, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_DOWN, PA_SCROLL_DOWN);
                                                        break;
                                                
                                                      case 5: // Temperature deg °F, Pressure
                                                                P.setFont(0, BigFontLower);
                                                                P.setFont(1, BigFontUpper);
                                                                P.displayZoneText(0, szMesg, PA_CENTER, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                                P.displayZoneText(1, szMesg, PA_CENTER, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                                cycle++;
                                                   //             f = getTemperaturDS18B20_in_F();
                                                   //             dtostrf(f, 3, 1, szMesg);
                                                                Press = getPressureBMP180();
                                                                dtostrf(Press, 5, 2, szMesg);
                                                                strcat(szMesg, " mmHg");
                                                   //             strcat(szMesg, "&");
                                                        break;
                                                
                                                      case 6: // Temperature deg RH
                                                                P.setFont(0, BigFontLower);
                                                                P.setFont(1, BigFontUpper);
                                                                P.displayZoneText(0, szMesg, PA_CENTER, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                                P.displayZoneText(1, szMesg, PA_CENTER, scrollSpeed, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                                                                cycle = 0;
                                                                getTemperature();
                                                                dtostrf(hum, 3, 1, szMesg);
                                                                strcat(szMesg, "% RH");
                                                        break;

                                                      default:
                                                                cycle = 0;
                                                        break;
                                                    }
                                                    P.displayReset(0);
                                                    P.displayReset(1);
                                                  }
                                            }
                                           else
                                   /*         if (ModeHAvailable)
                                            {
                                                if (millis() - lastTime >= 250)
                                                {
                                              //    RTC.readTime();
                                              //    readGPS();
                                                  lastTime = millis();
                                                  getsecond(szsecond);
                                                  getHour(szTime, flasher);
                                                  flasher = !flasher;
                                                  P.displayReset(0);
                                                  P.displayReset(1);
                                                  P.displayReset(2);
                                                } 
                                           }
                                           else
                                            if (ModeMAvailable)
                                            {
                                                if (millis() - lastTime >= 250)
                                                {
                                              //    RTC.readTime();
                                              //    readGPS();
                                                  lastTime = millis();
                                                  getsecond(szsecond);
                                                  getMinute(szTime, flasher);
                                                  flasher = !flasher;
                                                  P.displayReset(0);
                                                  P.displayReset(1);
                                                  P.displayReset(2);
                                                } 
                                           }
                                           else
                                            if (ModeSAvailable)
                                            {
                                                if (millis() - lastTime >= 250)
                                                {
                                              //    RTC.readTime();
                                              //    readGPS();
                                                  lastTime = millis();
                                                  getSec(szsecond);
                                                  flasher = !flasher;
                                                  P.displayReset(0);
                                                  P.displayReset(1);
                                                  P.displayReset(2);
                                                } 
                                           }
                                           else
                                            if (ModeDowAvailable)
                                            {
                                                if (millis() - lastTime >= 250)
                                                {
                                              //    RTC.readTime();
                                              //    readGPS();
                                                  lastTime = millis();
                                                      getDow(szMes);
                                                      getYer(szMeY);
                                                      getMon(szMeM);
                                                      getDay(szMeD);
                                                  flasher = !flasher;
                                                  P.displayReset(0);
                                                  P.displayReset(1);
                                                  P.displayReset(2);
                                                  P.displayReset(3);
                                                } 
                                           }
                                           else
                                            if (ModeDayAvailable)
                                            {
                                                if (millis() - lastTime >= 250)
                                                {
                                              //    RTC.readTime();
                                              //    readGPS();
                                                  lastTime = millis();
                                                      getYer(szMeY);
                                                      getMon(szMeM);
                                                      getDay(szMeD);
                                                      getDow(szMes);
                                                  flasher = !flasher;
                                                  P.displayReset(0);
                                                  P.displayReset(1);
                                                  P.displayReset(2);
                                                  P.displayReset(3);
                                                } 
                                           }
                                           else
                                            if (ModeMonAvailable)
                                            {
                                                if (millis() - lastTime >= 250)
                                                {
                                              //    RTC.readTime();
                                              //    readGPS();
                                                  lastTime = millis();
                                                      getYer(szMeY);
                                                      getMon(szMeM);
                                                      getDay(szMeD);
                                                      getDow(szMes);
                                                  flasher = !flasher;
                                                  P.displayReset(0);
                                                  P.displayReset(1);
                                                  P.displayReset(2);
                                                  P.displayReset(3);
                                                } 
                                           }
                                           else
                                            if (ModeYerAvailable)
                                            {
                                                if (millis() - lastTime >= 250)
                                                {
                                              //    RTC.readTime();
                                              //    readGPS();
                                                  lastTime = millis();
                                                      getYer(szMeY);
                                                      getMon(szMeM);
                                                      getDay(szMeD);
                                                      getDow(szMes);
                                                  flasher = !flasher;
                                                  P.displayReset(0);
                                                  P.displayReset(1);
                                                  P.displayReset(2);
                                                  P.displayReset(3);
                                                } 
                                           }
                                           else 
                                            if (ModeCAvailable)
                                            {
                                                if (P.displayAnimate())
                                                {
                                                     if (newMessageAvailable)
                                                      {
                                                        getmodec();
                                                        P.setIntensity(contrast);
                                                        newMessageAvailable = false;
                                                      }
                                                     else
                                                      if (curMessageAvailable)
                                                      {
                                                        getmodee();
                                                        curMessageAvailable = false;
                                                      }
                                                  P.displayReset();
                                                }
                                           }
                                           else*/
                                            if(ModeTemAvailable)
                                            {
                                         //     if (P.displayAnimate())
                                         //     {
                                                        if (P.getZoneStatus(0) && P.getZoneStatus(1))
                                                        {
                                                          switch (cycle_Tem)
                                                                 {
                                                                   case 0: // Temperature deg °C in
                                                                          P.setFont(0, BigFontLower);
                                                                          P.setFont(1, BigFontUpper);
                                                                          cycle_Tem++;
                                                                          Temp_in = getTemperaturDS18B20_in();
                                                                          dtostrf(Temp_in, 3, 1, szMesg);
                                                                          strcpy(szMesgAll, In);
                                                                          strcat(szMesgAll, szMesg);
                                                                          strcat(szMesgAll, "$");
                                                                       //   P.displayZoneText(0, szMesgAll, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_UP_LEFT, PA_SCROLL_DOWN_RIGHT);
                                                                       //   P.displayZoneText(1, szMesgAll, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_DOWN_LEFT, PA_SCROLL_UP_RIGHT);
                                                                       P.displayZoneText(0, szMesgAll, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_DOWN, PA_SCROLL_UP);
                                                                       P.displayZoneText(1, szMesgAll, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_UP, PA_SCROLL_DOWN);
                                                                          cycle++;
                                                                   break;

                                                                   case 1:// Temperature deg °C out
                                                                          P.setFont(0, BigFontLower);
                                                                          P.setFont(1, BigFontUpper);
                                                                          cycle_Tem = 0;
                                                                          Temp_out = getTemperaturDS18B20_out();
                                                                          dtostrf(Temp_out, 3, 1, szMesg);
                                                                          strcpy(szMesgAll, Out);
                                                                          strcat(szMesgAll, szMesg);
                                                                          strcat(szMesgAll, "$");
                                                                          P.displayZoneText(0, szMesgAll, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_DOWN, PA_SCROLL_UP);
                                                                          P.displayZoneText(1, szMesgAll, PA_CENTER, scrollSpeed, 3000, PA_SCROLL_UP, PA_SCROLL_DOWN);
                                                                   break;

                                                                   default:
                                                                          cycle_Tem = 0;
                                                                    break;
                                                                 }
                                                        }
                                        //      }                           
                                            }
//      readkeyboard();
      }//end if _mode8
          if(_mode >= 9 && _mode < 10)  {_mode = 0; _mode_();}

          if(_mode >= 254)  {_mode = 8; _mode_();}

}

// =======================================================================
void readGPS()
{
//  unsigned long start = millis();
//  do 
//  {
//    while (ss.available())
//      gps.encode(ss.read());
//  } while (millis() - start < ms);
/*
    while (ss.available())
    {
      gps.encode(ss.read());
    }
    if (gps.time.isValid() && gps.date.isValid()) 
      {
        setTime(gps.time.hour(), gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
      }
//      const unsigned timeLength = 6;
//      char timeValue[timeLength];
#ifdef Plus3
      time_t utc = now();
      time_t local = CE.toLocal(utc, &tcr);
#endif
//  snprintf(timeValue, timeLength, "%02d:%02d", hour(local), minute(local));
#ifdef Plus3
      h = hour(local);
#endif
#ifdef Plus2
      h = hour(CE.toLocal(now()));
#endif
//      h = gps.time.hour();
      m = gps.time.minute();

//      m = minute(local);
      s = gps.time.second();
      satelit = gps.satellites.value();
      d = gps.date.day();
      mm = gps.date.month();
      y = gps.date.year();
#ifdef Plus3
      dow = weekday(local);
#endif
#ifdef Plus2
      dow = weekday(CE.toLocal(now()));
#endif*/

  static uint32_t jj = 0;
      if (millis() - jj >= 200)
        {
            jj = millis();
//            jj = 0;
            while (ss.available())
                        {
                          gps.encode(ss.read());
                          if (gps.time.isValid() && gps.date.isValid())
                          {
                              setTime(gps.time.hour(), gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
                              y = year();
                              mm = month();
                              d = day();
                              h = hour(CE.toLocal(now()));
                              m = minute();
                              s = second();
                    //          Формула расчета Кима Ларсона  W = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) mod 7  http://baike.baidu.com/view/739374.htm
                    //                                        int iWeek = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;
                    //          WoTag = (Tag + 2 * Monat + 3 * (Monat + 1) / 5 + Jahr + Jahr / 4 - Jahr / 100 + Jahr / 400) % 7;
                              dow = weekday() - 1;
                          }
                        }
                    
        }
}
// =========================================================================
