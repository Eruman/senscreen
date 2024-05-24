///////////////
// Sketch: Calibrate - Calibrate TFT SPFD5408 Touch
// Author: Joao Lopes F. - joaolopesf@gmail.com
//
// Versions:
//    - 0.9.0  First beta - July 2015
//    - 0.9.1  Rotation for Mega
// Comments:
//    Show the calibration parameters to put in your code
//    Please use a small like the eraser on a pencil for best results
//
//    Code for buttons, based on Adafruit arduin_o_phone example
///////////////

// library SPFD5408

#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>     // Touch library

// Calibrates value
#define SENSIBILITY 300
#define MINPRESSURE 10
#define MAXPRESSURE 1000

//These are the pins for the shield!
#define YP A1
#define XM A2
#define YM 7
#define XP 6

/*
  //Macros replaced by variables
  #define TS_MINX 150
  #define TS_MINY 120
  #define TS_MAXX 920
  #define TS_MAXY 940
*/
short TS_MINX = 150;
short TS_MINY = 75;
short TS_MAXX = 955;
short TS_MAXY = 845;

// Init TouchScreen:

TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSIBILITY);

// LCD Pin

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4 // Optional : otherwise connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000 // 0
#define BLUE    0x001F // 31
#define RED     0xF800 // 63488
#define GREEN   0x07E0 // 2016
#define CYAN    0x07FF // 2047
#define MAGENTA 0xF81F // 63519
#define YELLOW  0xFFE0 // 65504
#define WHITE   0xFFFF // 65535

// Init LCD

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Dimensions

uint16_t width = 0;
uint16_t height = 0;

// Buttons

#define BUTTONS 3
#define BUTTON_CLEAR 0
#define BUTTON_SHOW 1
#define BUTTON_RESET 2

//#define debug  // Раскомментировать для отладочной информации

Adafruit_GFX_Button buttons[BUTTONS];

uint16_t buttons_y = 0;

//-- Setup

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "bitlash.h"

/*
#define MORSE_BUF_LEN 80
byte morse_buffer_tail;
byte morse_buffer_head;
byte morse_buffer[MORSE_BUF_LEN];
*/

// function handler for "tone()" bitlash function
//
//  arg 1: pin
//  arg 2: frequency
//  arg 3: duration (optional)
//
numvar func_tone(void) {
  if (getarg(0) == 2) tone(getarg(1), getarg(2));
  else tone(getarg(1), getarg(2), getarg(3));
  return 0;
}

numvar func_tft_setCursor(void) {
  if (getarg(0) == 2) tft.setCursor(getarg(1), getarg(2));
  return 0;
}

numvar func_tft_setTextSize(void) {
  if (getarg(0) == 1) tft.setTextSize(getarg(1));
  return 0;
}

numvar func_tft_setTextColor(void) {
  if (getarg(0) == 1) tft.setTextColor(getarg(1));
  return 0;
}

numvar func_notone(void) {
  noTone(getarg(1));
  return 0;
}

void screenPut(byte c) {
  tft.print((char)c);
  //Serial.print((char)c);
}

numvar func_printd(void) {
  tft.print(getarg(1));  return 0; // keep the compiler happy
}

numvar func_printm(void) {
  // route Bitlash output to custom function
  setOutputHandler(&screenPut);
  //tft.print(getarg(0));
  func_printf_handler(1, 2); // format=arg(1), optional args start at 2

  // restore normal output
  resetOutputHandler();
  return 0; // keep the compiler happy
}

numvar func_printmln(void) {
  // route Bitlash output to custom function
  setOutputHandler(&screenPut);
  func_printf_handler(1, 2); // format=arg(1), optional args start at 2

  // restore normal output
  resetOutputHandler();
  tft.println();
  return 0; // keep the compiler happy
}

numvar func_tft_fillScreen(void) {
  if (getarg(0) == 1) tft.fillScreen(getarg(1));
  if (getarg(0) == 0) tft.fillScreen(0);
  showCalibration();
  return 0;
}

numvar func_tft_fillRect(void) {
  if (getarg(0) == 5) tft.fillRect(getarg(1), getarg(2), getarg(3), getarg(4), getarg(5));
  if (getarg(0) == 4) tft.fillRect(getarg(1), getarg(2), getarg(3), getarg(4), 0);
  return 0;
}

numvar func_translator() {
  while (!Serial.available()) {}
  while (true) {
    while (Serial.available()) {
      byte c = Serial.read();
      tft.print((char)c);
    }
    unsigned long startTime = millis(); // Запоминаем время начала ожидания
    while (!Serial.available()) {
      if (millis() - startTime > 3000) {
        return 0; // Возвращаемся из функции, если прошло более 1 секунды без поступления символов
      }
    }
  }
}


void setup(void) {
  initBitlash(115200);    // must be first to initialize serial port
  addBitlashFunction("tone", (bitlash_function) func_tone);
  addBitlashFunction("notone", (bitlash_function) func_notone);
  addBitlashFunction("printd", (bitlash_function) func_printd);
  addBitlashFunction("printm", (bitlash_function) func_printm);
  addBitlashFunction("printmln", (bitlash_function) func_printmln);
  addBitlashFunction("setcursor", (bitlash_function) func_tft_setCursor);
  addBitlashFunction("textsize", (bitlash_function) func_tft_setTextSize);
  addBitlashFunction("settextcolor", (bitlash_function) func_tft_setTextColor);
  addBitlashFunction("cls", (bitlash_function) func_tft_fillScreen);
  addBitlashFunction("fillrect", (bitlash_function) func_tft_fillRect);
  addBitlashFunction("trans", (bitlash_function) func_translator);

  // Inicialize the controller
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(-1); // Need for the Mega, please changed for your choice or rotation initial
  width = tft.width() - 1;
  height = tft.height() - 1;

  // Debug
#ifdef debug
  Serial.println(F("TFT LCD test"));
  Serial.print("TFT size is ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());
#endif

  // UI
  initializeButtons();

  // Border
  drawBorder();

  // Calibration
  showCalibration();

}

// -- Loop
void loop()
{
  // Test of calibration
  TSPoint p;

  // Wait a touch
  digitalWrite(13, HIGH);
  p = waitOneTouch();
  digitalWrite(13, LOW);

  // Map of values
  //  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  //  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  p.x = mapXValue(p);
  p.y = mapYValue(p);

  // Draw a point
  //tft.fillCircle(p.x, p.y, 2, BLUE);

  // Show touch screen point (TSPOINT)
  showTouched(p);
}

// Calibration of Touch Screen (resistive)
void calibrate_TS(void) {
}


// wait 1 touch to return the point
TSPoint waitOneTouch() {
  TSPoint p;
  do {
    p = ts.getPoint();
    pinMode(XM, OUTPUT); //Pins configures again for TFT control
    pinMode(YP, OUTPUT);
    runBitlash();
  } while ((p.z < MINPRESSURE ) || (p.z > MAXPRESSURE)) ;
  assignVar('X'-'A', mapXValue(p));  
  assignVar('Y'-'A', mapYValue(p));  
  return p;
}

// Draw a border
void drawBorder () {}

// Show a screen of calibration
void showCalibration() {}

// Show the coordinates
void showTouched(TSPoint p) {
  uint8_t w = 40; // Width
  uint8_t h = 10; // Heigth
  uint8_t x = (width - (w * 2)); // X
  uint8_t y = 11; // Y

  tft.fillRect(x, y, w * 2, h, WHITE); // For cleanup
  tft.drawRect(x, y, w, h, RED); // For X
  tft.drawRect(x + w + 2, y, w * 2, h, RED); // For Y
  tft.setTextColor(BLACK);
  tft.setCursor(x + 2, y + 1);
  tft.print("X: ");
  showValue(p.x);
  tft.setCursor(x + 2 + w + 2, y + 1);
  tft.print("Y: ");
  showValue(p.y);
}

// Show a value of TSPoint
void showValue (uint16_t value) {
  if (value < 10)
    tft.print("00");
  if (value < 100)
    tft.print("0");
  tft.print(value);
}

// Show results of calibration
void showResults() {}

// Initialize buttons
void initializeButtons() {}

// Map the coordinate X
uint16_t mapXValue(TSPoint p) {
  uint16_t x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  //Correct offset of touch. Manual calibration
  //x+=1;
  return x;

}

// Map the coordinate Y
uint16_t mapYValue(TSPoint p) {
  uint16_t y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  //Correct offset of touch. Manual calibration
  //y-=2;
  return y;
}
