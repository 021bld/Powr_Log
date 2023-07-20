/*************************************************************
 * Power_Logger - Powr_Log.ino
 * 
 * Description:
 *    This Arduino sketch serves as a power logging application using the INA219 current sensor. 
 *    It displays the readings on a 16x2 LCD screen and saves them to SD card files for logging purposes. 
 *    The original concept and code were inspired by GreatScott's "Make Your Own Power Meter/Logger" instructable, which provided the foundation for this project.
 *    
 *         
 * Author: Yoganath P
 * Contact: 021BLD - 021bld.lab@gmail.com
 * 
 * Preferred Links:
 *   - GitHub: https://github.com/021bld
 *   - YouTube: https://www.youtube.com/@021_BLD
 *   - Instagram: https://www.instagram.com/021_bld
 * 
 ************************************************************/

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal.h>
//#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SdFat.h>
SdFat SD;

//#define OLED_RESET 4
//Adafruit_SSD1306 display(OLED_RESET);
Adafruit_INA219 ina219;
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned long previousMillis = 0;
unsigned long interval = 1000;
const int chipSelect = 10;
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float energy = 0;
File TimeFile;
File VoltFile;
File CurFile;


void setup() {
  SD.begin(chipSelect);
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    lcd.begin(16, 2);
  //Serial.begin(9600);
  ina219.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    ina219values();
    currentMillis = currentMillis/1000;

    TimeFile = SD.open("TIME.txt", FILE_WRITE);
    if (TimeFile) {
      TimeFile.println(currentMillis);
      TimeFile.close();
    }

    VoltFile = SD.open("VOLT.txt", FILE_WRITE);
    if (VoltFile) {
      VoltFile.println(loadvoltage);
      VoltFile.close();
    }

    CurFile = SD.open("CUR.txt", FILE_WRITE);
    if (CurFile) {
      CurFile.println(current_mA);
      CurFile.close();
    }
    displaydata();
  }
}

void displaydata() {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("V");
  lcd.setCursor(1, 0);
  lcd.print(loadvoltage);
  
  lcd.setCursor(0, 1);
  lcd.print("mA");
  lcd.setCursor(2, 1);
  lcd.print(current_mA);
  
  
  lcd.setCursor(8, 0);
  lcd.print(loadvoltage * current_mA);
  lcd.setCursor(14, 0);
  lcd.print("mW");
  
  lcd.setCursor(8, 1);
  lcd.print(energy);
  lcd.setCursor(13, 1);
  lcd.print("mWh");
  lcd.display();

  /*Serial.print(loadvoltage);
  Serial.println(" V");
  Serial.print(current_mA);  
  Serial.println(" mA");
  Serial.print(loadvoltage * current_mA);
  Serial.println(" mW");
  Serial.print(energy);
  Serial.println(" mWh");
  Serial.println("\n");*/

  }

void ina219values() {
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  energy = energy + loadvoltage * current_mA / 3600;
}
