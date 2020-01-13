#include <Arduino.h>

//for TFTLCD
#include <Adafruit_GFX.h> //and possible .cpp variant
//#include <Adafruit_ILI9341.h>
#include <Adafruit_TFTLCD.h> //https://learn.adafruit.com/adafruit-2-dot-8-color-tft-touchscreen-breakout-v2/spi-wiring-and-test
//#include <TouchScreen.h>
//#include <UTFT.h> //http://www.rinkydinkelectronics.com/library.php?id=51

//for DHT22 sensor
#include <DHT.h>
#include <DHT_U.h> // or alternatively; #include <Adafruit_Sensor.h>

//for DS3231 RTC
#include <RtcDS3231.h>

//universal inclusions
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>

// generic definitions
// *******************************************************************************************************************************************************************************

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// TFTLCD stuff
// *******************************************************************************************************************************************************************************

//old definitions
#define LCD_RESET A4 // LCD reset pin is connected to A4
#define LCD_CS    A3 // LCD chip select pin is connected to A3
#define LCD_CD    A2 // LCD command/data pin is connected to A2
#define LCD_WR    A1 // LCD write pin is connected to A1
#define LCD_RD    A0 // LCD read pin is connected to A0
// these are all SPI (Serial Peripheral Interface) pins
// since it is a shield, the pins of the TFT plug directly into the TFT

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// *******************************************************************************************************************************************************************************

/*INCREDIBLY IMPORTANT
In order to use the TFTLCD library, you MUST use version 1.5.3 or LOWER of the adafruit GFX library
like such: --- https://forum.arduino.cc/index.php?topic=622880.0
implement in the platformio.ini file in the project itself like so:
lib_deps =
  # Using a library name (ones that can be installed off the internet)
  Adafruit GFX Library@<=1.5.3
*/

// *******************************************************************************************************************************************************************************





void setup() {
  Serial.begin(9600);
  Serial.println("ILI9341 Test!");

  tft.reset();
  delay(1700);
  uint16_t identifier = tft.readID();
  Serial.print("TFT Identifier == ");
  Serial.println(identifier);
  delay(100);
  tft.begin(identifier);
  tft.setRotation(1);
  //splashScreen();
}

void loop() {
  delay(3000);
  tft.fillScreen(BLACK);
  delay(3000);
  tft.fillScreen(WHITE);
  Serial.println("filled");
}