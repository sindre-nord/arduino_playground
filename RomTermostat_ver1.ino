#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

//#include <Wire.h>
// #include <SHT2x.h>

#define temp_sensor_pin 2
 
//OneWire oneWire(temp_sensor_pin);
OneWire tempsensor(temp_sensor_pin);
DallasTemperature sensors(&tempsensor);

float temp = 0.0;
float prev_temp;
char temp_chr[10];
String temp_str;
String prev_temp_str;
float set_temp = 24.0;
float prev_set_temp = 24.0;
String set_temp_str;
String prev_set_temp_str;
int paa = 0;
float fukt = 0;
float prev_fukt = 0;
String fukt_str;
String prev_fukt_str;


//Arduino Uno/Nano Pins:
//GND = GND
//VCC = 5v
//DC = 8
//RES = 9
//CS = 10
//SDA = 11
//SCL = 13

#define TFT_SCLK   13  
#define TFT_MOSI   11  
#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
 
const int buttonVELGPin = A0;
int buttonVELGState= 0;
int lastbuttonVELGState = 0;

const int buttonPLUSSPin = A2;
int buttonPLUSSState= 0;
int lastbuttonPLUSSState = 0;

const int buttonMINUSPin = A1;
int buttonMINUSState= 0;
int lastbuttonMINUSState = 0;

int inp;
int loop_cnt = -1;

#define paa 1
#define av  0

int rele_status = av;

void setup(void)
{
//  pinMode(4, OUTPUT);
//  Wire.begin();
  Serial.begin(9600);
  sensors.begin();
  tft.initR(INITR_GREENTAB); //Initializes ST7735.
  //Use of GREENTAB rather than BLACKTAB avoids static at the bottom and right-hand side of the display.
  //Available Colours: BLACK, BLUE (Displays Red), RED (Displays Blue), GREEN, CYAN (Displays Yellow), MAGENTA, YELLOW (Displays Cyan) and WHITE.
  tft.fillScreen(ST7735_BLACK);
//  tft.drawLine(0, 140, 127, 140, ST7735_WHITE);
//  tft.drawCircle(40, 140, 5, ST7735_CYAN);
//  tft.fillTriangle(64, 135, 59, 145, 69, 145, ST7735_MAGENTA);
//  tft.drawTriangle(64, 135, 59, 145, 69, 145, ST7735_WHITE);
//  tft.fillCircle(88, 140, 5, ST7735_YELLOW);
  loadData();
}

void loop()
{
   if ( loop_cnt < 0 ) {
      temp = getTemp();
//      fukt = SHT2x.GetHumidity();
//      Serial.print("Fuktighet: ");
//      Serial.println(fukt);
      Serial.print("Temperatur: ");
      Serial.println(temp);
//   Serial.println();
    }
    loop_cnt++;
    if ( loop_cnt > 10000 ) {
      loop_cnt = -1;
    }
    sjekk_knapp();

    if (temp != prev_temp || set_temp != prev_set_temp ) {
      skrivskjerm();
      styr_rele();
    }

  if ( set_temp != prev_set_temp ) {
    storeData();
  }
 
  prev_temp = temp;
  prev_set_temp = set_temp;
//  prev_fukt = fukt;
}

void skrivskjerm() {
    prev_temp_str = dtostrf(prev_temp,1,1,temp_chr);
    temp_str = dtostrf(temp,1,1,temp_chr);
    prev_set_temp_str = dtostrf(prev_set_temp,1,1,temp_chr);
    set_temp_str = dtostrf(set_temp,1,1,temp_chr);
//    fukt_str = dtostrf(fukt,1,0,temp_chr);
//    prev_fukt_str = dtostrf(prev_fukt,1,0,temp_chr);

    tft.setTextSize(4);
    tft.setCursor(15, 20);
    tft.setTextColor(ST7735_BLACK);
    tft.print(prev_temp_str);
    tft.setCursor(15, 20);
    tft.setTextColor(ST7735_GREEN);
    tft.print(temp_str);
 
    tft.setCursor(15, 70);
    tft.setTextColor(ST7735_BLACK);
    tft.print(prev_set_temp_str);
    tft.setCursor(15, 70);
    if (rele_status == av)
      tft.setTextColor(ST7735_RED);
    else
      tft.setTextColor(ST7735_BLUE);
    tft.print(set_temp_str);

//    tft.setCursor(15, 90);
//    tft.setTextColor(ST7735_BLACK);
//    tft.print(prev_fukt_str);
//    tft.setCursor(15, 90);
//    tft.setTextColor(ST7735_GREEN);
//    tft.print(fukt_str);
}

void styr_rele() {
  if ( temp < set_temp-0.5 ) {
    rele_status = paa;
    analogWrite(3,0);
  } else if ( temp > set_temp ) {
    rele_status = av;
    analogWrite(3,255);
  }
}

float getTemp () {
    float tmp = 0.0; 
    sensors.requestTemperatures();
    tmp = sensors.getTempCByIndex(0);
    //tmp = round(tmp*10)/10.0;
    return tmp;
}


void sjekk_knapp () {
  int buttomhighlimit = 200;

  buttonPLUSSState = analogRead(buttonPLUSSPin);
  //Serial.print("A1: ");Serial.println(buttonPLUSSState);
  if (buttonPLUSSState > buttomhighlimit) buttonPLUSSState = HIGH; else buttonPLUSSState = LOW;
  if (buttonPLUSSState != lastbuttonPLUSSState) {
    if (buttonPLUSSState == HIGH) {
      set_temp = set_temp + 0.5;    
      Serial.println("Pluss");
    }
  }
  lastbuttonPLUSSState = buttonPLUSSState;
  buttonMINUSState = analogRead(buttonMINUSPin);
  //Serial.print("A2: ");Serial.println(buttonMINUSState);

  if (buttonMINUSState > buttomhighlimit) buttonMINUSState = HIGH; else buttonMINUSState = LOW;
  if (buttonMINUSState != lastbuttonMINUSState) {
    if (buttonMINUSState == HIGH ) {
      set_temp = set_temp - 0.5;
      Serial.println("Minus");
    }
  }
  lastbuttonMINUSState = buttonMINUSState;

  buttonVELGState = analogRead(buttonVELGPin);
  //Serial.print("A3: ");Serial.println(buttonVELGState);
  if (buttonVELGState > buttomhighlimit) buttonVELGState = HIGH; else buttonVELGState = LOW;
  if (buttonVELGState != lastbuttonVELGState) {
    if (buttonVELGState == HIGH) {
      Serial.println("Velg");
    }
  }
  lastbuttonVELGState = buttonVELGState;
}

 

void storeData() {
  unsigned int baseAddr = 0;  // Adress of EEPROM
  int n;

  Serial.println("Skriver data til minne");
  n = EEPROM_writeAnything(baseAddr, set_temp);  // Keep the set_temp value in EEPROM
  // baseAddr = baseAddr + n;
}

void loadData() {
  unsigned int baseAddr = 0;  // Adress of EEPROM
  int n;
 
  Serial.println("Leser data fra minne");
  n = EEPROM_readAnything(baseAddr, set_temp);
  if ( isnan(set_temp) ) set_temp = 23.0;
  Serial.print("Start verdi Temp: ");
  Serial.print(set_temp,1);
  Serial.print(" ");
  Serial.print(n);
  Serial.println(" bytes");
}

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
   const byte* p = (const byte*)(const void*)&value;
   int i;
   for (i = 0; i < sizeof(value); i++)
       EEPROM.write(ee++, *p++);
   return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    int i;
    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    return i;
}