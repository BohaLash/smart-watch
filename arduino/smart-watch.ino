#include "BluetoothSerial.h"
#include "Arduino.h"
#include "Vector.h"

#include "FS.h"
#include "SPIFFS.h"
//#include "esp_sleep.h"
//#include <esp_wifi.h>
//#include "string.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

int i, j, k;
int n = 0;
char c;
int x, y, d = 0;
bool avaitDelayTiming = false;
bool b = false;
bool dspl = true;
//int scrl = 0;
int limit;
int scale = 1;
unsigned long long lastTime = 0;
unsigned long long currentTime = 0;
int l = 0;

int next = 0;
int nextBuf = 0;

String folders[9] = {"Math", "Phythics", "Chemistry", "Biology", "Geogaphy", "History", "Ukrainian", "English", "Others"};

String menu[9] = {"Files", "Calcul", "Browze", "Settin"};

//int cc[4][30] = {
//  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
//  {25,},
//  {},
//  {},
//}

bool ang = false;
String calcSymb[32] = {"", "!", "^", "7", "8", "9", "/", "", "sin", "%", "$", "4", "5", "6", "x", "", "tan", "n", "(", "1", "2", "3", "-", "", "log", "ln", ")", "0", ".", "", "+"};
String calcStr = "12312312";
String calcAnsw = "31232";

String calc[2][4] = {
  {
    "DEG !  ^  7 8 9  / CE",
    "sin %  $  4 5 6  x AC",
    "tan n  (  1 2 3  -   ",
    "log ln )  0 .    + = "
  },
  {
    "RAD !  ^  7 8 9  / CE",
    "sin %  $  4 5 6  x AC",
    "tan n  (  1 2 3  -   ",
    "log ln )  0 .    + = "
  },
};

File myFile;

const char* myFilePath = "/new_file.txt";


void (*swipeHandler) ();
void (*selectionHandler) ();
void (*onTouch) ();
void (*currentPage) ();
struct pageArgs {
  int scroll;
  int index;
  int select;
};
pageArgs page;

void (*swipeHandlerBuf) ();
void (*selectionHandlerBuf) ();
void (*onTouchBuf) ();
void (*currentPageBuf) ();
pageArgs pageBuf;



void setup() {
//  esp_wifi_stop();
  Serial.begin(9600);

  SerialBT.begin("ESP32test");
  Serial.println("The device started, now you can pair it with bluetooth!");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.drawCircle(0, 0, 1, SSD1306_INVERSE);
  display.display();

  if (!SPIFFS.begin(true)) {
    Serial.println("Error while mounting SPIFFS");
    return;
  }

  openClockPage();
}



void highlight(int i, int j) {
  if (i*8 + j != 23 && i*8 + j != 29)
    switch (j) {
      case 0:
        display.fillRoundRect(0,               24 + i*10, 19, 9, 1, SSD1306_INVERSE);
        break;
      case 1:
        display.fillRoundRect(24,              24 + i*10, 13, 9, 1, SSD1306_INVERSE);
        break;
      case 2:
        display.fillRoundRect(41,              24 + i*10, 9,  9, 1, SSD1306_INVERSE);
        break;
      case 6:
        display.fillRoundRect(101,             24 + i*10, 9,  9, 1, SSD1306_INVERSE);
        break;
      case 7:
        display.fillRoundRect(114,             24 + i*10, 13, 9, 1, SSD1306_INVERSE);
        break;
      default:
        display.fillRoundRect(58 + (j - 3)*12, 24 + i*10, 11, 9, 1, SSD1306_INVERSE);
        break;
    }
}



// Icons
// ----------------------

void drawFilesIcon(int x, int y) {
  display.drawRoundRect(x, y, 20, 20, 3, SSD1306_WHITE);
  for (k = 0; k < 3; ++k)
    display.drawLine(x + 4, y + 4 + 3*k, x + 20 - 5, y + 4 + 3*k, SSD1306_WHITE);
  display.drawLine(x + 4, y + 4 + 9, x + 20 - 9, y + 4 + 9, SSD1306_WHITE);

  display.drawPixel(x + 17, y + 19, SSD1306_BLACK);
  display.drawPixel(x + 18, y + 18, SSD1306_BLACK);
  display.drawPixel(x + 19, y + 17, SSD1306_BLACK);
  
  display.drawPixel(x + 15, y + 15, SSD1306_WHITE);
  display.drawPixel(x + 16, y + 14, SSD1306_WHITE);
  display.drawPixel(x + 14, y + 16, SSD1306_WHITE);
  display.drawPixel(x + 17, y + 14, SSD1306_WHITE);
  display.drawPixel(x + 14, y + 17, SSD1306_WHITE);
  display.drawPixel(x + 18, y + 14, SSD1306_WHITE);
  display.drawPixel(x + 14, y + 18, SSD1306_WHITE);
  
  display.drawPixel(x + 18, y + 17, SSD1306_WHITE);
  display.drawPixel(x + 17, y + 18, SSD1306_WHITE);
  
}

void drawCalculIcon(int x, int y) {
  display.drawRoundRect(x, y, 20, 20, 3, SSD1306_WHITE);
}

void drawBrowseIcon(int x, int y) {
  display.drawCircle(x + 10, y + 10, 10, SSD1306_WHITE);
  display.drawLine(x + 2, y + 5, x + 20 - 2, y + 5, SSD1306_WHITE);
  display.drawLine(x + 2, y + 15, x + 20 - 2, y + 15, SSD1306_WHITE);
  display.setCursor(x + 2, y + 6);
  display.write("www");
}

void drawSettinIcon(int x, int y) {
  display.drawCircle(x + 10, y + 10, 10, SSD1306_WHITE);
}


void drawIcon(int index, int x, int y) {
  switch (index) {
    case 0:
      drawFilesIcon(x, y);
      break;
    case 1:
      drawCalculIcon(x, y);
      break;
    case 2:
      drawBrowseIcon(x, y);
      break;
    case 3:
      drawSettinIcon(x, y);
      break;
  }
}



// Controls
// --------------------------------

void pass() {
  
}

void touch() {
  onTouch();
  display.drawCircle(x, y, 1, SSD1306_INVERSE);
  for (i = 2; i < 5; ++i) {
    display.drawCircle(x, y, i, SSD1306_INVERSE);
    display.display();
    display.drawCircle(x, y, i, SSD1306_INVERSE);
  }
  display.drawCircle(x, y, 1, SSD1306_INVERSE);
  display.display();
}

void scrollUp() {
  Serial.println("\nscroll up");
  if (currentPage == drawClockPage)
    currentTime ++;
  page.scroll ++;
  handlePageChange();
}

void scrollDown() {
  Serial.println("\nscroll down");
  if (page.scroll) {
    if (currentPage == drawClockPage)
      currentTime --;
    page.scroll --;
    handlePageChange();
  }
}



// Cursor Handlers
// ------------------------------------

void handleTextCursor() {
  
}

void handleFilesCursor() {
  if (x > 119 && y < 10) {
    if (page.select != -1) {
      if (page.select >= 0 && 24 + page.select*10 - page.scroll*5 > 20) 
        display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_BLACK);
      page.select = -1;
      onTouch = openFoldersPage;
      display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
    }
    return;
  }
  if (x > 119 && 19 < y && y < 25) {
    if (page.select != -2) {
      if (page.select >= 0 && 24 + page.select*10 - page.scroll*5 > 20) 
        display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_BLACK);
      else if (page.select == -1) 
        display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
      page.select = -2;
      onTouch = scrollDown;
    }
    return;
  }
  if (x > 119 && y > 55) {
    if (page.select != -3) {
      if (page.select >= 0 && 24 + page.select*10 - page.scroll*5 > 20) 
        display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_BLACK);
      else if (page.select == -1) 
        display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
      page.select = -3;
      onTouch = scrollUp;
    }
    return;
  }
  if (y > 20) {
    for (i = 0; i < limit; ++i)
      if (20 + i * 10 - page.scroll*5 < y && y < 30 + i * 10 - page.scroll*5) {
          if (page.select != i) {
            if (page.select >= 0 && 24 + page.select*10 - page.select*5 > 20)
              display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_BLACK);
            else if (page.select == -1) 
              display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
            page.select = i;
            next = page.index * 10 + page.select;
            onTouch = openTextPage;
            display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_WHITE);
          }
          return;
        }
  }
  if (page.select != -5) {
    if (page.select >= 0 && 24 + page.select*10 - page.select*5 > 20)
      display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_BLACK);
    else if (page.select == -1) 
      display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
    page.select = -5;
    onTouch = pass;
  }
}

void handleFoldersCursor() {
  if (x > 119 && y < 10) {
    if (page.select != -1) {
      if (page.select >= 0) 
        display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_BLACK);
      page.select = -1;
      onTouch = openMenuPage;
      display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
    }
    return;
  }
  if (x > 119 && 19 < y && y < 25) {
    if (page.select != -2) {
      if (page.select >= 0) 
        display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_BLACK);
      else if (page.select == -1) 
        display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
      page.select = -2;
      onTouch = scrollDown;
    }
    return;
  }
  if (x > 119 && y > 55) {
    if (page.select != -3) {
      if (page.select >= 0) 
        display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_BLACK);
      else if (page.select == -1) 
        display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
      page.select = -3;
      onTouch = scrollUp;
    }
    return;
  }
  for (i = 0; i < 2; ++i)
    for (j = 0; j < 3; ++j)
      if (3 + 35*j < x && x < 3 + 35*j + 25   &&   3 + i * 40 - page.scroll*5 < y && y < 3 + i * 40 - page.scroll*5 + 35) {
        if (page.select != i*3 + j) {
          if (page.select >= 0)
            display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_BLACK);
          else if (page.select == -1) 
            display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
          page.select = i*3 + j;
          next = i*3 + j;
          onTouch = openFilesPage;
          display.drawRoundRect(1 + 35*j, 1 + 40*i - page.scroll*5, 29, 39, 6, SSD1306_WHITE);
        }
        return;
      }
  if (page.select != -5) {
    if (page.select >= 0) 
      display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_BLACK);
    else if (page.select == -1) 
      display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
    page.select = -5;
    onTouch = pass;
  }
}

void handleCalculCursor() {
  if (x > 119 && y < 10) {
    if (page.select != -1) {
      if (page.select >= 0) 
        highlight(page.select/8, page.select%8);
      page.select = -1;
      onTouch = openMenuPage;
      display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
    }
    return;
  }
  for (i = 0; i < 4; ++i)
    for (j = 0; j < 8; ++j)
      switch (j) {
        case 0:
          if (0 < x && x < 19   &&   24 + i*10 < y && y < 24 + i*10 + 9) {
            if (page.select != i*8 + j) {
              if (page.select >= 0)
                highlight(page.select/8, page.select%8);
              else if (page.select == -1) 
                display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
              if (i*8 + j != 23 && i*8 + j != 29) {
                page.select = i*8 + j;
                next = page.select;
                onTouch = calculHandler;
                display.fillRoundRect(0, 24 + i*10, 19, 9, 1, SSD1306_INVERSE);
              }
            }
            return;
          }
          break;
        case 1:
          if (24 < x && x < 24 + 13   &&   24 + i*10 < y && y < 24 + i*10 + 9) {
            if (page.select != i*8 + j) {
              if (page.select >= 0)
                highlight(page.select/8, page.select%8);
              else if (page.select == -1) 
                display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
              page.select = i*8 + j;
              next = page.select;
              onTouch = calculHandler;
              display.fillRoundRect(24, 24 + i*10, 13, 9, 1, SSD1306_INVERSE);
            }
            return;
          }
          break;
        case 2:
          if (41 < x && x < 41 + 9   &&   24 + i*10 < y && y < 24 + i*10 + 9) {
            if (page.select != i*8 + j) {
              if (page.select >= 0)
                highlight(page.select/8, page.select%8);
              else if (page.select == -1) 
                display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
              page.select = i*8 + j;
              next = page.select;
              onTouch = calculHandler;
              display.fillRoundRect(41, 24 + i*10, 9,  9, 1, SSD1306_INVERSE);
            }
            return;
          }
          break;
        case 6:
          if (101 < x && x < 101 + 9   &&   24 + i*10 < y && y < 24 + i*10 + 9) {
            if (page.select != i*8 + j) {
              if (page.select >= 0)
                highlight(page.select/8, page.select%8);
              else if (page.select == -1) 
                display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
              page.select = i*8 + j;
              next = page.select;
              onTouch = calculHandler;
              display.fillRoundRect(101, 24 + i*10, 9,  9, 1, SSD1306_INVERSE);
            }
            return;
          }
          break;
        case 7:
          if (114 < x && x < 114 + 13   &&   24 + i*10 < y && y < 24 + i*10 + 9) {
            if (page.select != i*8 + j) {
              if (page.select >= 0)
                highlight(page.select/8, page.select%8);
              else if (page.select == -1) 
                display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
              if (i*8 + j != 23) {
                page.select = i*8 + j;
                next = page.select;
                onTouch = calculHandler;
                display.fillRoundRect(114, 24 + i*10, 13, 9, 1, SSD1306_INVERSE);
              } else {
                page.select = -5;
                onTouch = pass;
              }
            }
            return;
          }
          break;
        default:
          if (58 + (j - 3)*12 < x && x < 58 + (j - 3)*12 + 9   &&   24 + i*10 < y && y < 24 + i*10 + 9) {
            if (page.select != i*8 + j) {
              if (page.select >= 0)
                highlight(page.select/8, page.select%8);
              else if (page.select == -1) 
                display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
              if (i*8 + j != 29) {
                page.select = i*8 + j;
                next = page.select;
                onTouch = calculHandler;
                display.fillRoundRect(58 + (j - 3)*12, 24 + i*10, 11, 9, 1, SSD1306_INVERSE);
              } else {
                page.select = -5;
                onTouch = pass;
              }
            }
            return;
          }
          break;
      }
  if (page.select != -5) {
    if (page.select >= 0) 
      highlight(page.select/8, page.select%8);
    else if (page.select == -1) 
      display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
    page.select = -5;
    onTouch = pass;
  }
}

void handleMenuCursor() {
  if (x > 119 && y < 10) {
    if (page.select != -1) {
      if (page.select >= 0) 
        display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_BLACK);
      page.select = -1;
      onTouch = openClockPage;
      display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
    }
    return;
  }
  if (x > 119 && 19 < y && y < 25) {
    if (page.select != -2) {
      if (page.select >= 0) 
        display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_BLACK);
      else if (page.select == -1) 
        display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
      page.select = -2;
      onTouch = scrollDown;
    }
    return;
  }
  if (x > 119 && y > 55) {
    if (page.select != -3) {
      if (page.select >= 0) 
        display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_BLACK);
      else if (page.select == -1) 
        display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
      page.select = -3;
      onTouch = scrollUp;
    }
    return;
  }
  for (i = 0; i < 2; ++i)
    for (j = 0; j < 2; ++j)
      if (5 + 60*j < x && x < 5 + 60*j + 45   &&   3 + 50*i - page.scroll*5 < y && y < 3 + 50*i - page.scroll*5 + 45) {
        if (page.select != i*2 + j) {
          if (page.select >= 0)
            display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_BLACK);
          else if (page.select == -1) 
            display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
          page.select = i*2 + j;
          next = i*2 + j;
          onTouch = menuSelector;
          display.drawRoundRect(5 + 60*j, 3 + 50*i - page.scroll*5, 45, 45, 5, SSD1306_WHITE);
        }
        return;
      }
  if (page.select != -5) {
    if (page.select >= 0) 
      display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_BLACK);
    else if (page.select == -1) 
      display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
    page.select = -5;
    onTouch = pass;
  }
}

void handleClockCursor () {
  
}



// Swipe Handler
// ----------------------------------

void handleTextSwipes() {
  if (b) {
    if (n < -1 && page.index%10 < limit - 1) {
      next = page.index/10*10 + page.index%10 + 1;
      openTextPage();
    } else if (n > 1 && page.index%10 > 0) {
      next = page.index/10*10 + page.index%10 - 1;
      openTextPage();
    }
  } else {
    if (page.scroll - n <= 0)
      page.scroll = 0;
    else
      page.scroll -= n;
    currentPage();
  }
}

void handleFilesSwipes() {
  if (limit == 0) {
    if (page.select >= 0)
      display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_BLACK);
    page.select = -1;
    onTouch = openFoldersPage;
    display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
    display.display();
  } else if (page.select < -1) {
    page.scroll = 0;
    page.select = 0;
    next = page.index*10;
    onTouch = openTextPage;
    dspl = false;
    currentPage();
    dspl = true;
    display.drawCircle(1, 24, 1, SSD1306_WHITE);
    display.display();
  } else if (page.select == -1) {
    if ((b && n < 0) || (!b && n > 0)) {
      page.select = 0;
      next = page.index*10;
      onTouch = openTextPage;
      page.scroll = 0;
      dspl = false;
      currentPage();
      dspl = true;
      display.drawCircle(1, 24, 1, SSD1306_WHITE);
      display.display();
    }
  } else {
    if (b) {
      if (n > 0) {
        display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_BLACK);
        page.select = -1;
        onTouch = openFoldersPage;
        display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
        display.display();
      }
    } else {
      if (page.select + n < 0) {
        if (page.select != 0) {
          display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_BLACK);
          page.select = 0;
          next = page.index*10;
          onTouch = openTextPage;
          page.scroll = 0;
          dspl = false;
          currentPage();
          dspl = true;
          display.drawCircle(1, 24, 1, SSD1306_WHITE);
          display.display();
        }
      } else if (page.select + n >= limit) {
        if (page.select != limit - 1 && limit > 0) {
          display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_BLACK);
          page.select = limit - 1;
          next = page.index*10 + limit - 1;
          onTouch = openTextPage;
          page.scroll = page.select*2 - 7;
          if (page.scroll < 0)
            page.scroll = 0;
          dspl = false;
          currentPage();
          dspl = true;
          display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_WHITE);
          display.display();
        }
      } else {
        display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_BLACK);
        page.select += n;
        next = page.index*10 + page.select;
        onTouch = openTextPage;
        if (page.scroll < page.select*2 - 7)
          page.scroll = page.select*2 - 7;
        else if (page.scroll > page.select*2)
          page.scroll = page.select*2;
        
        dspl = false;
        currentPage();
        dspl = true;
        display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_WHITE);
        display.display();
      }
    }
  }
}

void handleFoldersSwipes() {
  if (page.select < -1) {
    page.scroll = 0;
    page.select = 0;
    next = 0;
    onTouch = openFilesPage;
    dspl = false;
    currentPage();
    dspl = true;
    display.drawRoundRect(1, 1, 29, 39, 6, SSD1306_WHITE);
    display.display();
  } else if (page.select == -1) {
    if ((b && n < 0) || (!b && n > 0)) {
      page.select = 0;
      next = 0;
      onTouch = openFilesPage;
      page.scroll = 0;
      dspl = false;
      currentPage();
      dspl = true;
      display.drawRoundRect(1, 1, 29, 39, 6, SSD1306_WHITE);
      display.display();
    }
  } else {
    if (b) {
      if (page.select / 3 < (page.select + n)/3) {
        display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_BLACK);
        page.select = -1;
        onTouch = openMenuPage;
        display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
        display.display();
      } else if (page.select / 3 > (page.select + n)/3 || page.select + n < 0) {
        if (page.select % 3) {
          display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_BLACK);
          page.select -= page.select % 3;
          next = page.select;
          onTouch = openFilesPage;
          display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_WHITE);
          display.display();
        }
      } else {
        display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_BLACK);
        page.select += n;
        next = page.select;
        onTouch = openFilesPage;
        display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_WHITE);
        display.display();
      }
    } else {
      if (page.select + n*3 < 0) {
        if (page.select >= 3) {
          page.select = page.select % 3;
          next = page.select;
          onTouch = openFilesPage;
          page.scroll = (1 + 40*(page.select/3))/5 - 4;
          if (page.scroll < 0)
            page.scroll = 0;
          dspl = false;
          currentPage();
          dspl = true;
          display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_WHITE);
          display.display();
        }
      } else if (page.select + n*3 >= 9) {
        if (page.select < 9 - 3) {
          page.select = 9 - 3 + page.select % 3;
          next = page.select;
          onTouch = openFilesPage;
          page.scroll = (1 + 40*(page.select/3))/5 - 4;
          if (page.scroll < 0)
            page.scroll = 0;
          dspl = false;
          currentPage();
          dspl = true;
          display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_WHITE);
          display.display();
        }
      } else {
        page.select += n*3;
        next = page.select;
        onTouch = openFilesPage;
        page.scroll = (1 + 40*(page.select/3))/5 - 4;
        if (page.scroll < 0)
            page.scroll = 0;
        dspl = false;
        currentPage();
        dspl = true;
        display.drawRoundRect(1 + 35*(page.select%3), 1 + 40*(page.select/3) - page.scroll*5, 29, 39, 6, SSD1306_WHITE);
        display.display();
      }
    }
  }
}

void handleCalculSwipes() {
  if (page.select < -1) {
    page.select = 0;
    next = 0;
    onTouch = openFilesPage;
    display.fillRoundRect(0, 24, 19, 9, 1, SSD1306_INVERSE);
    display.display();
  } else if (page.select == -1) {
    if ((b && n < 0) || (!b && n > 0)) {
      display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_BLACK);
      page.select = 0;
      next = 0;
      onTouch = openFilesPage;
      display.fillRoundRect(0, 24, 19, 9, 1, SSD1306_INVERSE);
      display.display();
    }
  } else {
    if (b) {
      if (page.select / 8 < (page.select + n)/8) {
        highlight(page.select / 8, page.select % 8);
        page.select = page.select - page.select % 8;
        next = page.select;
        onTouch = openFilesPage;
        display.fillRoundRect(0, 24 + (page.select / 8)*10, 19, 9, 1, SSD1306_INVERSE);
        display.display();
      } else if (page.select / 8 > (page.select + n)/8 || page.select + n < 0) {
        highlight(page.select / 8, page.select % 8);
        page.select = page.select - page.select % 8 + 7;
        Serial.print(page.select);
        if (page.select == 23) {
          page.select = 22;
          display.fillRoundRect(101, 24 + (page.select / 8)*10, 9,  9, 1, SSD1306_INVERSE);
        } else
          display.fillRoundRect(114, 24 + (page.select / 8)*10, 13, 9, 1, SSD1306_INVERSE);
        next = page.select;
        onTouch = openFilesPage;
        display.display();
      } else {
        highlight(page.select / 8, page.select % 8);
        page.select += n;
        next = page.select;
        onTouch = openFilesPage;
        highlight(page.select / 8, page.select % 8);
        display.display();
      }
    } else {
      if (page.select + n*8 < 0) {
        highlight(page.select / 8, page.select % 8);
        page.select = -1;
        onTouch = openMenuPage;
        display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
        display.display();
      } else if (page.select + n*8 >= 32) {
        if (page.select < 32 - 8) {
          highlight(page.select / 8, page.select % 8);
          page.select = 32 - 8 + page.select % 8;
          next = page.select;
          onTouch = openFilesPage;
          highlight(3, page.select % 8);
          display.display();
        }
      } else {
        highlight(page.select / 8, page.select % 8);
        page.select += n*8;
        next = page.select;
        onTouch = openFilesPage;
        highlight(page.select / 8, page.select % 8);
        display.display();
      }
    }
  }
}

void handleBrowserSwipes() {
  
}

void handleSettinSwipes() {
  
}

void handleMenuSwipes() {
  if (page.select < -1) {
    page.scroll = 0;
    page.select = 0;
    next = 0;
    onTouch = openFoldersPage;
    dspl = false;
    currentPage();
    dspl = true;
    display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_WHITE);
    display.display();
  } else if (page.select == -1) {
    if ((b && n < 0) || (!b && n > 0)) {
      page.select = 0;
      next = 0;
      onTouch = openFoldersPage;
      page.scroll = 0;
      dspl = false;
      currentPage();
      dspl = true;
      display.drawRoundRect(5, 3, 45, 45, 5, SSD1306_WHITE);
      display.display();
    }
  } else {
    if (b) {
      if (page.select / 2 < (page.select + n)/2) {
        display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_BLACK);
        page.select = -1;
        onTouch = openClockPage;
        display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
        display.display();
      } else if (page.select / 2 > (page.select + n)/2 || page.select + n < 0) {
        if (page.select % 2) {
          display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_BLACK);
          page.select -= page.select % 2;
          next = page.select;
          onTouch = menuSelector;
          display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_WHITE);
          display.display();
        }
      } else {
        display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_BLACK);
        page.select += n;
        next = page.select;
        onTouch = menuSelector;
        display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_WHITE);
        display.display();
      }
    } else {
      if (page.select + n*2 < 0) {
        if (page.select >= 2) {
          page.select = page.select % 2;
          next = page.select;
          onTouch = menuSelector;
          page.scroll = (5 + 50*(page.select/2))/5 - 4;
          if (page.scroll < 0)
            page.scroll = 0;
          dspl = false;
          currentPage();
          dspl = true;
          display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_WHITE);
          display.display();
        }
      } else if (page.select + n*2 >= 4) {
        if (page.select < 4 - 2) {
          page.select = 4 - 2 + page.select % 2;
          next = page.select;
          onTouch = menuSelector;
          page.scroll = (5 + 50*(page.select/2))/5 - 4;
          if (page.scroll < 0)
            page.scroll = 0;
          dspl = false;
          currentPage();
          dspl = true;
          display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_WHITE);
          display.display();
        }
      } else {
        page.select += n*2;
        next = page.select;
        onTouch = menuSelector;
        page.scroll = (5 + 50*(page.select/2))/5 - 4;
        if (page.scroll < 0)
            page.scroll = 0;
        dspl = false;
        currentPage();
        dspl = true;
        display.drawRoundRect(5 + 60*(page.select%2), 3 + 50*(page.select/2) - page.scroll*5, 45, 45, 5, SSD1306_WHITE);
        display.display();
      }
    }
  }
}

void handleClockSwipes() {
  
}



// File Managment
// ----------------------------------------------

void sendTree() {
  myFile = SPIFFS.open("/tree.txt", FILE_READ);
  while(myFile.available()) {
    SerialBT.write(myFile.read());
  }
  myFile.close();
}

void deleteFile(int folder, int index) {
  String s = "";
  char c;
  myFile = SPIFFS.open("/tree.txt", FILE_READ);
  while(myFile.available()) {
    c = myFile.read();
    s += c;
  }
  myFile.close();
  int buf1 = 0, buf2 = 0;
  String newTree;
  for (i = 0; i < s.length(); ++i) {
    if (buf1 == folder && buf2 == index) {
      newTree = s.substring(0, i);
      while (!(s[i] == ' ' || s[i] == '/')) i++;
      newTree += s.substring(i + 1, s.length());
      break;
    } else if (s[i] == '/') {
      buf1++;
      buf2 = 0;
    } else if (s[i] == ' ') buf2++;
  }
  myFile = SPIFFS.open("/tree.txt", FILE_WRITE);
  if (myFile.print(newTree)) {
   Serial.println(newTree);  
  }
  myFile.close();
  s = "/";
  s += folder;
  s += index;
  s += ".txt";
  if (SPIFFS.remove(s)) {
     Serial.println("File successfully deleted");
  } else {
    Serial.print("No such file");
  }
  sendTree();
}

void createFile(int folder, int l) {
  String s = "", fname = "untitled";
  char c;
  bool b = true;
  for (i = 0; i < l; ++i) {
    c = SerialBT.read();
    s += c;
    if (s[i] == '@' && b) {
      fname = s.substring(0, i);
      b = false;
    }
  }
  String tree = "";
  myFile = SPIFFS.open("/tree.txt", FILE_READ);
  while(myFile.available()) {
    c = myFile.read();
    tree += c;
  }
  myFile.close();
  int buf1 = 0, buf2 = 0;
  String newTree;
  for (i = 0; i < tree.length(); ++i) {
    if (tree[i] == '/') {
      buf1++;
      if (buf1 == folder + 1) {
        newTree = tree.substring(0, i);
        newTree += fname;
        newTree += ' ';
        newTree += tree.substring(i, tree.length());
        break;
      } else {
        buf2 = 0;
      }
    } else if (tree[i] == ' ') buf2++;
  }
  myFile = SPIFFS.open("/tree.txt", FILE_WRITE);
  if (myFile.print(newTree)) {
   Serial.println(newTree);  
  }
  myFile.close();
  fname = "/";
  fname += folder;
  fname += buf2;
  fname += ".txt";
  myFile = SPIFFS.open(fname, FILE_WRITE);
  if (myFile.print(s)) {
   Serial.println(s);  
  }
  myFile.close();
  sendTree();
}

void sendFile(int folder, int index) {
  SerialBT.write('@');
  String s = "/";
  s += folder;
  s += index;
  s += ".txt";
  myFile = SPIFFS.open(s, FILE_READ);
  while(myFile.available()) {
    SerialBT.write(myFile.read());
  }
  myFile.close();
  SerialBT.write('@');
}



// Pages Drawing
// --------------------------------------------------------------

void drawTextPage() {
  char c;
  display.clearDisplay();

  display.setTextSize(scale + 1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0 - page.scroll*8);
  display.cp437(true); // Use full 256 char 'Code Page 437' font

  Serial.println(page.index);
  Serial.println("/" + String(page.index) + ".txt");

  String s = "";
  if (page.index < 10)
    s = "0";

  Serial.println(page.index);
  Serial.println("/" + s + String(page.index) + ".txt");

  myFile = SPIFFS.open("/" + s + String(page.index) + ".txt", FILE_READ);
//  if (!myFile.available()) {
//    next = page.index/10;
//    openFilesPage();
//    return;
//  }
  while(myFile.available()) {
    c = myFile.read();
    if (c == '@') {
      display.write('\n');
      display.setTextSize(scale);
      display.write('\n');
    } else
      display.write(c);
  }
  myFile.close();
  display.display();
}

void drawFilesPage() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true); // Use full 256 char 'Code Page 437' font
  display.clearDisplay();

  for (i = 0; i < 3; ++i) {
    display.drawPixel(122 + i, 20 + i, SSD1306_WHITE);
    display.drawPixel(122 - i, 20 + i, SSD1306_WHITE);

    display.drawPixel(122 + i, 60 - i, SSD1306_WHITE);
    display.drawPixel(122 - i, 60 - i, SSD1306_WHITE);
  }
  String tree = "";
  myFile = SPIFFS.open("/tree.txt", FILE_READ);
  while(myFile.available()) {
    c = myFile.read();
    tree += c;
  }
  myFile.close();
  j = 0;
  k = 0;
  display.setCursor(5, 20 - page.scroll*5);
  for (i = 0; i < tree.length(); ++i) {
     if (j == page.index && tree[i] == ' ') {
      k++;
      display.setCursor(5, 20 + k * 10 - page.scroll*5);
      if (page.select == k)
        display.drawCircle(1, 24 + page.select*10 - page.scroll*5, 1, SSD1306_WHITE);
    } else if (tree[i] == '/') {
      j++;
      if (j > page.index) {
        limit = k;
        break;
      }
    } else if (j == page.index) {
      display.write(tree[i]);
    }
  }

  display.fillRect(0, 0, 64, 20, SSD1306_BLACK);

  for (i = 0; i < folders[page.index].length(); ++i) {
    display.setCursor(i*7, 0);
    display.write(folders[page.index][i]);
    display.setCursor(1 + i*7, 0);
    display.write(folders[page.index][i]);
  }
  display.setCursor(120, 1);
  display.write("x");

  if (page.select == -1)
    display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
  
  display.setCursor(0, 10);
  for (i = 0; i < 11; ++i)
    display.write("- ");
  display.display();
}

void drawFoldersPage() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  display.setCursor(120, 1);
  display.clearDisplay();
  display.write("x");
  if (page.select == -1)
    display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
  for (i = 0; i < 3; ++i) {
    display.drawPixel(122 + i, 20 + i, SSD1306_WHITE);
    display.drawPixel(122 - i, 20 + i, SSD1306_WHITE);

    display.drawPixel(122 + i, 60 - i, SSD1306_WHITE);
    display.drawPixel(122 - i, 60 - i, SSD1306_WHITE);
  }
  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      display.drawRoundRect(3 + 35*j, 3 + i * 40 - page.scroll*5, 25, 35, 5, SSD1306_WHITE);
      if (page.select == i*3 + j)
        display.drawRoundRect(1 + 35*j, 1 + i * 40 - page.scroll*5, 29, 39, 6, SSD1306_WHITE);
      for (k = 0; k < folders[3*i + j].length(); ++k) {
        if (!(k % 3)) display.setCursor(3 + 35*j + 4, 3 + i * 40 + 4 + (k/3)*10 - page.scroll*5);
        display.write(folders[3*i + j][k]);
      }
    }
  }
  if (dspl)
    display.display();
}

void drawCalculPage() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  display.setCursor(120, 1);
  display.clearDisplay();
  display.write("x");
  
//  display.setCursor(0, 0);

  for (i = 0; i < 4; ++i) {
    display.setCursor(1, i*10 + 25);
    display.print(calc[ang][i]);
  }

  display.drawLine(53, 25, 53, 64, SSD1306_WHITE);
  display.drawLine(95, 25, 95, 64, SSD1306_WHITE);

  if (page.select == -1)
    display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
  else 
    highlight(page.select/8, page.select%8);

//  display.setTextSize(2);
//  display.setCursor(0, 0);
//  display.print("123123\n");
//  display.setTextSize(1);
//  display.print("123123\n");
  next = 15;
  calculHandler();
  
  if (dspl)
    display.display();
}

void drawMenuPage() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  display.setCursor(120, 1);
  display.clearDisplay();
  display.write("x");
  if (page.select == -1)
    display.drawRoundRect(117, 0, 11, 11, 3, SSD1306_WHITE);
  for (i = 0; i < 3; ++i) {
    display.drawPixel(122 + i, 20 + i, SSD1306_WHITE);
    display.drawPixel(122 - i, 20 + i, SSD1306_WHITE);

    display.drawPixel(122 + i, 60 - i, SSD1306_WHITE);
    display.drawPixel(122 - i, 60 - i, SSD1306_WHITE);
  }
  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 2; ++j) {
      if (page.select == i*2 + j)
        display.drawRoundRect(5 + 60*j, 3 + i * 50 - page.scroll*5, 45, 45, 5, SSD1306_WHITE);
      drawIcon(2*i + j, 5 + 60*j + 12,  3 + i * 50 + 6 - page.scroll*5);
      display.setCursor(5 + 60*j + 2 + 3*(7 - menu[2*i + j].length()), 3 + i * 50 + 31 - page.scroll*5);
      display.print(menu[2*i + j]);
      }
  }
  if (dspl)
    display.display();
}

void drawClockPage() {
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.print(String(page.scroll/60) + ':' + String(page.scroll%60));
  if (dspl)
    display.display();
}



// Open Page
// --------------------------

void openTextPage() {
  currentPage = drawTextPage;
  page.scroll = 0;
  page.index = next;
  page.select = -5;
  onTouch = openFilesPage;
  next = page.index / 10;
  selectionHandler = handleTextCursor;
  swipeHandler = handleTextSwipes;
  handlePageChange();
}

void openFilesPage() {
  currentPage = drawFilesPage;
  page.scroll = 0;
  page.index = next;
  page.select = -5;
  selectionHandler = handleFilesCursor;
  swipeHandler = handleFilesSwipes;
  handlePageChange();
}

void openFoldersPage() {
  currentPage = drawFoldersPage;
  page.scroll = 0;
  page.index = next;
  page.select = -5;
  selectionHandler = handleFoldersCursor;
  swipeHandler = handleFoldersSwipes;
  handlePageChange();
}

void openCalculPage() {
  currentPage = drawCalculPage;
  page.scroll = 0;
  page.index = next;
  page.select = -5;
  selectionHandler = handleCalculCursor;
  swipeHandler = handleCalculSwipes;
  handlePageChange();
}

void openMenuPage() {
  currentPage = drawMenuPage;
  page.scroll = 0;
  page.index = next;
  page.select = -5;
  selectionHandler = handleMenuCursor;
  swipeHandler = handleMenuSwipes;
  handlePageChange();
}

void openClockPage() {
  currentPage = drawClockPage;
  page.scroll = currentTime;
  page.index = next;
  page.select = -5;
  next = 0;
  onTouch = openMenuPage;
  selectionHandler = handleClockCursor;
  swipeHandler = handleClockSwipes;
  handlePageChange();
}



void handlePageChange() {
  dspl = false;
  currentPage();
  dspl = true;
  display.drawCircle(x, y, 1, SSD1306_INVERSE);
  display.display();
}



// Logic 
// ----------------

void calculHandler() {
  calcStr += calcSymb[next];

  switch (next) {
    case 0:
      ang = !ang;
      currentPage();
      break;
    case 7:
      calcStr = calcStr.substring(0, calcStr.length() - l);
      break;
    case 8:
      l = 3;
      break;
    case 16:
      l = 3;
      break;
    case 24:
      l = 3;
      break;
    case 25:
      l = 2;
      break;
    case 15:
      calcStr = "";
      calcAnsw = "";
      break;
    case 31:
      calcStr = calcAnsw;
      calcAnsw = "34981/7617";
      break;
    default:
      l = 1;
      break;
  }
  
  Serial.println(calcStr);

  Serial.println(calcStr.substring(max(0, (int)calcStr.length() - 10), calcStr.length()));
  

  display.fillRect(0, 0, 115, 24, SSD1306_BLACK);
  display.setTextSize(2);
  display.setCursor(max(-3, (11*(11 - (int)calcStr.length()) - 15)), 0);
  display.print(calcStr.substring(max(0, (int)calcStr.length() - 10), calcStr.length()));
  display.setTextSize(1);
  display.setCursor(6*(21 - calcAnsw.length()), 16);
  display.print(calcAnsw);

  display.display();
  
}

void menuSelector() {
  switch (next) {
    case 0:
      openFoldersPage();
      break;
    case 1:
      openCalculPage();
      break;
  }
}



void loop() {
  if (millis()/60000 > lastTime) {
    lastTime = millis()/60000;
    currentTime ++;
    if (currentPage == drawClockPage) {
      page.scroll = currentTime;
      handlePageChange();
    }
  }
  if (SerialBT.available()) {
    c = SerialBT.read();
    Serial.print(c);

    switch (c) {
      case 'x':
      display.drawCircle(x, y, 1, SSD1306_INVERSE);
        if (b) n *= -1;
        x += n;
        if (x < 0) x = 0;
        else if (x > SCREEN_WIDTH - 1) x = SCREEN_WIDTH - 1;
        break;
      case 'y':
        if (b) n *= -1;
        y += n;
        if (y < 0) y = 0;
        else if (y > SCREEN_HEIGHT - 1) y = SCREEN_HEIGHT - 1;
        selectionHandler();
        display.drawCircle(x, y, 1, SSD1306_INVERSE);
        display.display();
        break;
      case 'i':
        n = n/200 + 1;
        if (b) n *= -1;
        b = true;
        swipeHandler();
        break;
      case 'j':
        n = n/200 + 1;
        if (b) n *= -1;
        b = false;
        swipeHandler();
        break;
      case 't':
        touch();
        break;
      case 'u':
        scrollDown();
        break;
      case 'd':
        scrollUp();
        break;
      case 's':
        d = n;
        break;
      case 'c':
        currentTime = n;
        if (currentPage == drawClockPage)
          openClockPage();
        break;
      case 'q':
        nextBuf = next;
        pageBuf = page;
        swipeHandlerBuf = swipeHandler;
        selectionHandlerBuf = selectionHandler;
        onTouchBuf = onTouch;
        currentPageBuf = currentPage;
        if (currentPage != drawClockPage)
          openClockPage();
        break;
      case 'o':
        Serial.print("open");
        if (currentPage == drawClockPage) {
          next = nextBuf;
          page = pageBuf;
          swipeHandler = swipeHandlerBuf;
          selectionHandler = selectionHandlerBuf;
          onTouch = onTouchBuf;
          currentPage = currentPageBuf;
          handlePageChange();
//          if (currentPageBuf == drawFilesPage) 
//            openFilesPage();
        }
        break;
      case '?':
        sendTree();
        break;
      case '#':
        deleteFile(n%10, n/10);
        break;
      case '!':
        createFile(n%10, n/10);
        break;
      case 'f':
        sendFile(n%10, n/10);
        break;
      case '\n':
        b = false;
        n = 0;
        break;
      case '-':
        b = true;
        break;
      default:
        n *= 10;
        n += c - '0';
        break;
    }
  }
  
//  if(d > 50) {
//    esp_sleep_enable_timer_wakeup(d);
//    esp_light_sleep_start();
//  }
//  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  delay(d);
}

//Vector< Vector<String> > parseTree() {
//  String s = "";
//  myFile = SPIFFS.open("/tree.txt", FILE_READ);
//  while(myFile.available()) {
//    s += myFile.read();
//  }
//  myFile.close();
//  Vector< Vector<String> > result;
//  Vector<String> v;
//  int buf = 0;
//  for (i = 0; i < s.length(); ++i) {
//    if (s[i] == ' ') {
//        if (i - buf > 0)
//          v.push_back(s.substring(buf, i));
//      buf = i + 1;
//    } else if (s[i] == '/') {
//      v.push_back(s.substring(buf, i));
//      buf = i + 1;
//      result.push_back(v);
//      v.clear();
//    }
//  }
//  return result;
//}




//  String s = "Hello hi dsfsdsf/wow bssdf bohdan/dsfsadfsdf dasd/@";
//  for (i = 0; i < s.length(); ++i)
//    SerialBT.write(s[i]);
//
//  File file = SPIFFS.open("/tree.txt");
//   if(!file){
//     Serial.println("Failed to open file for reading");
//                //  "Не удалось открыть файл для чтения"
//     return;
//   }
   
//   Serial.println("File Content:");
//              //  "Содержимое файла:"
//   while(file.available()){
//    c = file.read();
//    Serial.write(c);
//     SerialBT.write(c);
//     s += c;
//   }
//   file.close();

//  myFile.close();
