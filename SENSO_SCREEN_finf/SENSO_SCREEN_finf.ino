#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

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
short TS_MINX=150;
short TS_MINY=75;
short TS_MAXX=955;
short TS_MAXY=845;

int   last_X;
int   last_Y;

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

 
void setup(void) {
  Serial.begin(115200);
  //n4_api(0, loop_scr);
  //n4_setup(code);

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
  setup2();
}

//// -- Loop
//
//void loop()
//{
//  //n4_run();
//}

//void loop_scr() {
void loop() {
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
  tft.fillCircle(p.x, p.y, 3, BLUE);
  
  last_X = p.x;
  last_Y = p.y;

  // Show touch screen point (TSPOINT)
  showTouched(p);
  
  // Buttons

  // Go thru all the buttons, checking if they were pressed
  
  for (uint8_t b=0; b<BUTTONS; b++) {
    
    if (buttons[b].contains(p.x, p.y)) {

        // Action

        switch  (b) {
          case BUTTON_CLEAR:
              // Clear
              showCalibration();
              break;
              
          case BUTTON_SHOW:
              // Clear
              showResults();
              tft.println();
              tft.println("Touch to proceed");
              waitOneTouch();
              showCalibration();
              break;
              
          case BUTTON_RESET:
              // New calibration
              calibrate_TS();
              waitOneTouch();
              showCalibration();
              break;
        }
    }
  }

}

// Calibration of Touch Screen (resistive)

void calibrate_TS(void) {
}
// wait 1 touch to return the point 

TSPoint waitOneTouch() {
 
  TSPoint p;
  
  do {
    p= ts.getPoint(); 
    pinMode(XM, OUTPUT); //Pins configures again for TFT control
    pinMode(YP, OUTPUT);
    // **************************************************
    if (Serial.available() > 0) {
      unsigned ch = Serial.read();
      Serial.print((char)ch);
      feed_char(ch);
    }
    
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE)) ;
  
  return p;
}

// Draw a border

void drawBorder () {

  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;

  tft.fillScreen(RED);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);
  
}

// Show a screen of calibration

void showCalibration() {
  
  // Clear
  
  tft.fillScreen(BLACK);
  tft.setTextSize (1);

  // Header
  
  tft.fillRect(0, 0, width, 10, RED);

  tft.setCursor (40, 0);
  tft.setTextColor(WHITE);
  tft.println("*** Test of calibration ***");

  // Footer

  TSPoint p; // Only for show initial values
  p.x=0;
  p.y=0;
  p.z=0;
  
  showTouched(p);

  // Buttons

  for (uint8_t i=0; i<3; i++) {
      buttons[i].drawButton();
  }
  
}

// Show the coordinates     

void showTouched(TSPoint p) {

  uint8_t w = 40; // Width
  uint8_t h = 10; // Heigth
  uint8_t x = (width - (w*2)); // X
  uint8_t y = 11; // Y
  
  tft.fillRect(x, y, w*2, h, WHITE); // For cleanup

  tft.drawRect(x, y, w, h, RED); // For X
  tft.drawRect(x+w+2, y, w*2, h, RED); // For Y

  tft.setTextColor(BLACK);
  tft.setCursor(x+2, y + 1);
  tft.print("X: ");
  showValue(p.x);
  
  tft.setCursor(x+2+w+2, y + 1);
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

void showResults() {

  tft.fillScreen(BLACK);
  
  // Header
  
  tft.fillRect(0, 0, width, 10, RED);

  tft.setCursor (40, 0);
  tft.setTextColor(WHITE);
  tft.println("*** Results of calibration ***");

  // Results
  
  tft.setCursor(5, 30);
  tft.setTextSize(2);
  tft.println("After calibration: ");
  tft.print("TS_MINX= ");tft.println(TS_MINX);
  tft.print("TS_MINY= ");tft.println(TS_MINY);
  tft.println();
  tft.print("TS_MAXX= ");tft.println(TS_MAXX);
  tft.print("TS_MAXY= ");tft.println(TS_MAXY);

}

// Initialize buttons

void initializeButtons() {

  uint16_t x = 40;
  uint16_t y = height - 20;
  uint16_t w = 75;
  uint16_t h = 20;
  
  uint8_t spacing_x = 5;
  
  uint8_t textSize = 1;

  char buttonlabels[3][20] = {"Clear", "Show set`s", "Function"};
  uint16_t buttoncolors[15] = {RED, BLUE, RED};

  for (uint8_t b=0; b<3; b++) {
    buttons[b].initButton(&tft,                           // TFT object
                  x+b*(w+spacing_x),  y,                  // x, y,
                  w, h, WHITE, buttoncolors[b], WHITE,    // w, h, outline, fill, 
                  buttonlabels[b], textSize);             // text
  }

  // Save the y position to avoid draws
  
  buttons_y = y;
  
}

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
