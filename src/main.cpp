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

// HC-12 variable definitions
// *******************************************************************************************************************************************************************************
#define TXpin 10
#define RXpin 11
SoftwareSerial HC12(TXpin, RXpin); // TX pin on HC-12, RX pin on HC-12
#define setPin 22

char HCinput;
String HCinputString;
float DataBuffer[7]; //7 elements in array

// UI variable definitions
// *******************************************************************************************************************************************************************************
bool OutputFlag = false;
bool mainWrittenFlag = false;
bool menuWrittenFlag = false;
bool mainScreenFlag = true;
bool menuScreenFlag = false;
bool TFTWrittenFlag = false;
uint16_t connectionCheck = 1000;
bool connectedFlag = false;

// DHT22 variable definitions
// *******************************************************************************************************************************************************************************
float innerTemp;
float innerHumid;

// graphics variable definitions
// *******************************************************************************************************************************************************************************
uint16_t xPos;
uint16_t yPos;
uint16_t width;
uint16_t height;
uint16_t radius;
uint8_t charsize;
uint16_t bitmap;
uint16_t displWidth;
uint16_t displHeight;

uint16_t mainRectWidth;
uint16_t mainRectHeight;
uint16_t mainRectYSepDist;
uint16_t mainRectXSepDist;
uint16_t mainRectStartY;
uint16_t mainRectTextBuffer;
uint16_t statusBarYStart;
uint16_t statusBarWidth;

int charWidth = 6;
int charHeight = 8;
// all other text sizes are multiples of this (6w and 8h)
// for example, size 2 text is 12 by 16
// size 3 is 18 by 21
// etc.

// functions below
// *******************************************************************************************************************************************************************************
// *******************************************************************************************************************************************************************************
// *******************************************************************************************************************************************************************************
// *******************************************************************************************************************************************************************************



// UI/GFX/LCD functions
// *******************************************************************************************************************************************************************************

void smallWhiteText(uint16_t x, uint16_t y) {
  tft.setCursor(x, y);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
}

void mediumWhiteText(uint16_t x, uint16_t y) {
  tft.setCursor(x, y);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
}

void mainRectangle(uint16_t x, uint16_t y) {
  tft.drawRect(x,y,mainRectWidth,mainRectHeight, WHITE);
}

void menuRectangle(uint16_t x, uint16_t y) {
  tft.drawRect(x,y,8*(2*charWidth)+(2*mainRectTextBuffer),2*charHeight+2*mainRectTextBuffer,WHITE);
}

void displayMenu() {
  tft.fillScreen(BLACK);

  smallWhiteText(4,4);
  tft.print("SETTINGS");

  menuRectangle(4,mainRectStartY);
  mediumWhiteText(4+mainRectTextBuffer,mainRectStartY+mainRectTextBuffer);
  tft.print("Channel");

  menuRectangle(4,mainRectStartY+mainRectHeight+mainRectYSepDist);
  mediumWhiteText(4+mainRectTextBuffer,mainRectStartY+mainRectHeight+mainRectYSepDist+mainRectTextBuffer);
  tft.print("Power");

  menuRectangle(4,mainRectStartY+(2*mainRectHeight)+(2*mainRectYSepDist));
  mediumWhiteText(4+mainRectTextBuffer,mainRectStartY+(2*mainRectHeight)+(2*mainRectYSepDist)+mainRectTextBuffer);
  tft.print("S.Delay");

  uint16_t menuBigRectStartX = 4+mainRectTextBuffer+8*(2*charWidth)+(2*mainRectTextBuffer)+mainRectXSepDist;
  tft.drawRect(menuBigRectStartX,mainRectStartY,displWidth-(menuBigRectStartX+mainRectXSepDist),displHeight-((displHeight-statusBarYStart)+mainRectStartY+(3*mainRectYSepDist)),WHITE);

  tft.fillRect(displWidth-(4*(2*charWidth)+8),displHeight-((2*charHeight)+8),4*(2*charWidth)+8,2*charHeight+8,WHITE);
  tft.setCursor(displWidth-(4*(2*charWidth)+4),displHeight-((2*charHeight)+4));
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("Home");

  menuWrittenFlag = true;
}

void mainGUI() {
  //1st column
  smallWhiteText(4,4);
  tft.print("OUTSIDE");
    
  smallWhiteText((displWidth - (mainRectWidth+mainRectYSepDist)+1),4);
  tft.print("INSIDE");
    

  mainRectangle(4,mainRectStartY);
  smallWhiteText(8,mainRectStartY+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("DHT *C");

  mainRectangle(4,mainRectStartY+mainRectHeight+mainRectYSepDist);
  smallWhiteText(8,mainRectStartY+mainRectHeight+mainRectYSepDist+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("DHT RH%");

  mainRectangle(4,mainRectStartY+(2*mainRectHeight)+(2*mainRectYSepDist));
  smallWhiteText(8,mainRectStartY+(2*mainRectHeight)+(2*mainRectYSepDist)+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("BMP hPa");

  mainRectangle(4,mainRectStartY+(3*mainRectHeight)+(3*mainRectYSepDist));
  smallWhiteText(8,mainRectStartY+(3*mainRectHeight)+(3*mainRectYSepDist)+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("Lux");

  // 2nd column
  mainRectangle(4+mainRectWidth+mainRectXSepDist,mainRectStartY);
  smallWhiteText(8+mainRectWidth+mainRectXSepDist,mainRectStartY+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("BMP *C");

  mainRectangle(4+mainRectWidth+mainRectXSepDist,mainRectStartY+mainRectHeight+mainRectYSepDist);
  smallWhiteText(8+mainRectWidth+mainRectXSepDist,mainRectStartY+mainRectHeight+mainRectYSepDist+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("Volts");

  mainRectangle(4+mainRectWidth+mainRectXSepDist,mainRectStartY+(2*mainRectHeight)+(2*mainRectYSepDist));
  smallWhiteText(8+mainRectWidth+mainRectXSepDist,mainRectStartY+(2*mainRectHeight)+(2*mainRectYSepDist)+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("Soil RH%");

  mainRectangle(4+mainRectWidth+mainRectXSepDist,mainRectStartY+(3*mainRectHeight)+(3*mainRectYSepDist));
  smallWhiteText(8+mainRectWidth+mainRectXSepDist,mainRectStartY+(3*mainRectHeight)+(3*mainRectYSepDist)+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("?");



  // separator line (is actually a rectangle so it has width > 1)
  // doesn't need to be fillRect as it has a width of 2
  tft.drawRect(
    (4+(2*mainRectWidth)+(mainRectXSepDist)+(mainRectXSepDist/2))-1,
    4,
    2,
    (4*mainRectHeight)+(3*mainRectYSepDist)+(mainRectStartY)-4,
    WHITE
  );

  /*
  tft.drawRect(
    4+(2*mainRectWidth)+mainRectXSepDist+(mainRectXSepDist/2)-1,
    mainRectStartY+(2*mainRectHeight)+mainRectYSepDist+(mainRectYSepDist/2)-1,
    mainRectWidth+(mainRectXSepDist/2),
    2,
    WHITE
  );
  */

  // 3rd column
  mainRectangle(4+(2*mainRectWidth)+(2*mainRectXSepDist),mainRectStartY);
  smallWhiteText(8+(2*mainRectWidth)+(2*mainRectXSepDist),mainRectStartY+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("*C inner");

  mainRectangle(4+(2*mainRectWidth)+(2*mainRectXSepDist),mainRectStartY+mainRectHeight+mainRectYSepDist);
  smallWhiteText(8+(2*mainRectWidth)+(2*mainRectXSepDist),mainRectStartY+mainRectHeight+mainRectYSepDist+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("%RH inner");

  mainRectangle(4+(2*mainRectWidth)+(2*mainRectXSepDist),mainRectStartY+(2*mainRectHeight)+(2*mainRectYSepDist));
  smallWhiteText(8+(2*mainRectWidth)+(2*mainRectXSepDist),mainRectStartY+(2*mainRectHeight)+(2*mainRectYSepDist)+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("????");

  mainRectangle(4+(2*mainRectWidth)+(2*mainRectXSepDist),mainRectStartY+(3*mainRectHeight)+(3*mainRectYSepDist));
  smallWhiteText(8+(2*mainRectWidth)+(2*mainRectXSepDist),mainRectStartY+(3*mainRectHeight)+(3*mainRectYSepDist)+(mainRectHeight-2*(mainRectTextBuffer)));
  tft.print("?????");

  tft.fillRect(displWidth-(4*(2*charWidth)+8),displHeight-((2*charHeight)+8),4*(2*charWidth)+8,2*charHeight+8,WHITE);
  tft.setCursor(displWidth-(4*(2*charWidth)+4),displHeight-((2*charHeight)+4));
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("Menu");
  //tft.drawBitmap(displWidth-35,displHeight-35,settingsIcon,20,20,WHITE);

  mainWrittenFlag = true;
}


void splashScreen() {
  tft.fillScreen(BLACK);
  int wait1;
  int wait2;

  int numLetters;
  numLetters = 6;

  int mRectHeight;
  int mRectWidth;
  int mRect_yBuffer; //y buffer for the main rectangles
  int mRect_xSideBuffer; //x buffer for the first and last rectangles
  int mRect_adjacentBuffer; //x buffer between rectangles
  int mRect_xPositions[numLetters]; //6 = number of elements in array, since there are 6 letters, we have 6 places
                           //at 5 since 0 is an index as well (0-5 = 6)

  int mainText_xBuffer;
  int mainText_yBuffer;
  char mainTextLetters[numLetters] = {'T', 'e', 't', 'r', 'O', 'S'};

  int subText_xPosition[2]; //2 since there are 2 lines of subtext
  int subText_yPosition[2];
  int subText_xyBuffer;
  int subText_textSpacingBuffer;

  int sloganText_xPosition;
  int sloganText_yPosition;
  int sloganText_xBuffer;
  int sloganText_yBuffer;

  // variable definitions
  // ************************************************************************************
  wait1 = 70;
  wait2 = 50;

  mRect_xSideBuffer = 14; //placeholder value
  mRect_adjacentBuffer = 10;

  mRectWidth = 40;
  mRectHeight = 60;

  mainText_xBuffer = 12;
  mainText_yBuffer = 20;

  sloganText_yBuffer = 8;
  sloganText_xBuffer = 14;

  subText_xyBuffer = 4; // only need one buffer value wince the values for x and y are going to be identical
  subText_textSpacingBuffer = 4;


  // splash screen UI code
  // ************************************************************************************
  // ************************************************************************************
  // ************************************************************************************
  // draw main encapsulating rectangle
  tft.drawRect(0, 0, displWidth, displHeight, WHITE);

  // text background rectangles
  mRect_yBuffer = (displHeight/2) - (mRectHeight/2);
  for(int i=0; i<(numLetters-2); i++) {
    mRect_xPositions[i] = mRect_xSideBuffer + (i*mRect_adjacentBuffer) + (i*mRectWidth);

    //drawing and calculating in same loop
    tft.fillRect(mRect_xPositions[i], mRect_yBuffer, mRectWidth, mRectHeight, BLUE);
    delay(wait1);
  }

  for(int i=(numLetters-2); i<numLetters; i++) {
    mRect_xPositions[i] = mRect_xSideBuffer + (i*mRect_adjacentBuffer) + (i*mRectWidth);

    //drawing and calculating in same loop
    tft.fillRect(mRect_xPositions[i], mRect_yBuffer, mRectWidth, mRectHeight, GREEN);
    delay(wait1);
  }

  // main (header) text
  // ************************************************************************************
  tft.setTextSize(5);
  tft.setTextColor(WHITE);

  for(int i=0; i<numLetters; i++) {
    tft.setCursor(
      mRect_xPositions[i] + mainText_xBuffer,
      mRect_yBuffer + mainText_yBuffer
    );
    tft.print(mainTextLetters[i]);
    delay(wait2);
  }
  delay(700);

  // slogan text
  // ************************************************************************************
  tft.setTextSize(1);

  sloganText_xPosition = displWidth - ((charWidth * 44) + mRect_xSideBuffer + sloganText_xBuffer);;
  sloganText_yPosition = (mRect_yBuffer + mRectHeight + sloganText_yBuffer);

  tft.setCursor(sloganText_xPosition, sloganText_yPosition);
  tft.print("Blurring the line between useful and useless"); //44 chars

  // copyright text
  // ************************************************************************************
  tft.setTextSize(1);

  subText_xPosition[0] = displWidth - ((charWidth * 12) + subText_xyBuffer); //as there are 12 characters in "TetrOS ALPHA"
  subText_xPosition[1] = displWidth - ((charWidth * 21) + subText_xyBuffer); //as there are 21 characters in "Copyright 2020 TetrOS"

  subText_yPosition[0] = displHeight - ((charHeight * 2) + subText_xyBuffer + subText_textSpacingBuffer); // 2 lines from the bottom
  subText_yPosition[1] = displHeight - ((charHeight * 1) + subText_xyBuffer); // 1 line from the bottom

  tft.setCursor(subText_xPosition[0], subText_yPosition[0]);
  tft.print("TetrOS ALPHA");

  tft.setCursor(subText_xPosition[1], subText_yPosition[1]);
  tft.print("Copyright 2020 TetrOS");
}

// DS3231 functions
// *******************************************************************************************************************************************************************************

// DHT22 functions
// *******************************************************************************************************************************************************************************


/*void getTime() {
  currentTime = rtc.getTimeStr(1); // 0 = hh:mm:ss, 1 = hh:mm
  DOW = rtc.getDOWStr(1); // 0 = full english day of week, 1 = abbreviated form
  Date = rtc.getDateStr(1,1,'/'); // year format, overall format, separator
  int dateLen = Date.length();
  Date.remove((dateLen-3));
}*/

/*void displayTime() {
  tft.fillRect(8,statusBarYStart,statusBarWidth,(2*charHeight),BLACK);
  // clears out stale date/time data by making it black then rewriting
  // accompanied by annoying data flash on screen
  
  tft.setTextSize(2);
  tft.setCursor(8,statusBarYStart);
  tft.setTextColor(WHITE);
  tft.print(currentTime); // includes leading zeroes
  tft.print(" ");
  tft.print(DOW);
  tft.print(" ");
  tft.print(Date);
  // code to update it every minute
  // ONLY EVERY MINUTE
  // use micros() or something
  // doesn't need to be exact
}*/

/*void innerVars() {
  innerTemp = dht.readTemperature();
  innerHumid = dht.readHumidity();
  // For column 3 (inside column)
  tft.fillRect(4+(2*mainRectWidth)+(2*mainRectXSepDist)+mainRectTextBuffer,mainRectStartY+mainRectTextBuffer,4*(2*charWidth),(2*charHeight),BLACK);
  mediumWhiteText(4+(2*mainRectWidth)+(2*mainRectXSepDist)+mainRectTextBuffer,mainRectStartY+mainRectTextBuffer);
  tft.print(innerTemp);
  //tft.print("00.00");
  // inside DHT temperature


  tft.fillRect(4+(2*mainRectWidth)+(2*mainRectXSepDist)+mainRectTextBuffer,mainRectStartY+mainRectHeight+mainRectYSepDist+mainRectTextBuffer,4*(2*charWidth),(2*charHeight),BLACK);
  mediumWhiteText(4+(2*mainRectWidth)+(2*mainRectXSepDist)+mainRectTextBuffer,mainRectStartY+mainRectHeight+mainRectYSepDist+mainRectTextBuffer);
  tft.print(innerHumid);
  //tft.print("00.00");
  // inside DHT humidity
}*/





















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

  displWidth = tft.width();
  displHeight = tft.height();
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
  //splashScreen();
}

void loop() {
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  splashScreen();
  delay(9000);

  Serial.println("looped");
}