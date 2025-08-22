// UI and menu rendering helpers
#pragma once
#include "standard_i2c/MLX90640.h"
#include "Icons.h"
#include "virtual_i2c/System.h"

// Display power toggle
static inline void DisplayOff()
{
  if (display_active)
  {
    display_active = false;
    M5.Lcd.sleep();
    M5.Lcd.setBrightness(0);
  }
  else
  {
    M5.Lcd.wakeup();
    M5.Lcd.setBrightness(200);
    display_active = true;
  }
}

// Show IMU data on display
void IMUDemo()
{
  
  float Xg, Yg, Zg;
  float Xa, Ya, Za;

  int bg = ez.screen.background();
  int fg = TFT_BLACK;
  int xAxisColor = LIGHTGREY;
  int yValueColor = BLACK;

  
  M5.IMU.Init();
  
  MPU6886_active = true;
  
  ez.screen.clear();
  ez.header.show("Status IMU...");
  ez.buttons.show(" # Main # Hold");
  
  #define x_start 10
  int x = x_start;
  #define y_start 60
  int y = y_start;
  M5.Lcd.setFont(&FreeSans9pt7b);
  M5.Lcd.setTextDatum(BL_DATUM);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(fg, bg);
  M5.Lcd.setTextPadding(50);
  int RowDist = M5.Lcd.fontHeight();
  #define ColDist 50

  // Static text
  M5.Lcd.setFont(&FreeSansBold9pt7b);
  M5.Lcd.drawString("Acceleration (10^-3 * g)" , x, y);
  M5.Lcd.setFont(&FreeSans9pt7b); 
  M5.Lcd.drawString("X:", x+ColDist, y+=RowDist); 
  M5.Lcd.drawString("Y:", x+ColDist, y+=RowDist);
  M5.Lcd.drawString("Z:", x+ColDist, y+=RowDist);

  M5.Lcd.setFont(&FreeSansBold9pt7b);
  int xo = x+90;
  int yo = y+7;
  M5.Lcd.drawString("Rotation (  /s)" , x, y+=RowDist); 
  M5.Lcd.drawCircle(xo,yo,3,fg);
  M5.Lcd.drawCircle(xo,yo,2,fg);
  
  M5.Lcd.setFont(&FreeSans9pt7b);   
  M5.Lcd.drawString("X:", x+ColDist, y+=RowDist); 
  M5.Lcd.drawString("Y:", x+ColDist, y+=RowDist);
  M5.Lcd.drawString("Z:", x+ColDist, y+=RowDist);


  M5.Lcd.setFont(&FreeSansBold9pt7b);
  int xg = 60;
  int xidx = 0;
  int xgmax = 155;
  int yzero = RowDist/2 -1;
  long int prevtime = millis();
  do
  {
    if ((Serial.available()) || (BT.available())) return;
    x = x_start+ ColDist + 40;
    y = y_start;

    M5.IMU.setGyroFsr(M5.IMU.GFS_250DPS);
    M5.IMU.getGyroData(&Xg,&Yg,&Zg);
    M5.IMU.getAccelData(&Xa,&Ya,&Za);
    Xa = 1000 * Xa;
    Ya = 1000 * Ya;
    Za = 1000 * Za;

    if (prevtime+1000 < millis())
    {
      prevtime = millis();
      M5.Lcd.drawFastVLine(x+xg+xidx, y, 160, 0xDEFB);
    }
    else M5.Lcd.drawFastVLine(x+xg+xidx, y, 160, bg);
    
    M5.Lcd.drawFastVLine(x+xg+xidx+1, y, 160, RED);

    M5.Lcd.drawNumber(Xa, x, y+=RowDist); 
    M5.Lcd.drawPixel(xg+x+xidx, y-yzero, xAxisColor);
    M5.Lcd.drawPixel(xg+x+xidx, y-map(Xa, -1000,1000, 0, 20), yValueColor);
    
    M5.Lcd.drawNumber(Ya, x, y+=RowDist);
    M5.Lcd.drawPixel(xg+x+xidx, y-yzero, xAxisColor);
    M5.Lcd.drawPixel(xg+x+xidx, y-map(Ya, -1000,1000, 0, 20), yValueColor);
    
    M5.Lcd.drawNumber(Za, x, y+=RowDist);
    M5.Lcd.drawPixel(xg+x+xidx, y-yzero, xAxisColor);
    M5.Lcd.drawPixel(xg+x+xidx, y-map(Za, -1000,1000, 0, 20), yValueColor);

    y+=RowDist;
    M5.Lcd.drawNumber(Xg,      x, y+=RowDist); 
    M5.Lcd.drawPixel(xg+x+xidx, y-yzero, xAxisColor);
    M5.Lcd.drawPixel(xg+x+xidx, y-map(Xg, -200,200, 0, 20), yValueColor);

    M5.Lcd.drawNumber(Yg,      x, y+=RowDist);
    M5.Lcd.drawPixel(xg+x+xidx, y-yzero, xAxisColor);                            // show x-axis
    M5.Lcd.drawPixel(xg+x+xidx, y-map(Yg, -200,200, 0, 20), yValueColor);

    M5.Lcd.drawNumber(Zg,      x, y+=RowDist);
    M5.Lcd.drawPixel(xg+x+xidx, y-yzero, xAxisColor);                            // show x-axis
    M5.Lcd.drawPixel(xg+x+xidx, y-map(Zg, -200,200, 0, 20), yValueColor);

    if (xidx < xgmax) xidx++; else xidx = 0;

    // check the 'Hold' button
    delay(60);
    if (M5.BtnC.wasPressed())
    {
      delay(1000);
      do
      {
        if ((Serial.available()) || (BT.available())) return;
        delay(100);
      } while (!M5.BtnC.read());
    } 

    button = ez.buttons.poll();
  } while (button != "Main");
  return;
}

// Help text
String HelpText()
{
  if (command == "HELP")
  {
      print2serial("Example: >Speaker.Beep()");
      return "";
  }

  return "As of now, only '>help.help()' is implemented";
}

// Static text for sensor screen
void display_sensor_show_static_text()
{
  ez.canvas.clear();
  ez.header.show("M5Stack name: "+Friendly_Name );
  M5.Lcd.setTextColor(TFT_BLACK, ez.screen.background());
  M5.Lcd.setFont(&FreeSans9pt7b);
  display_sensor_static_text_redraw = false;
}

// Coordinates for sensor information
#define X_LABEL       5
#define X_VALUE       100
#define Y_SENSOR      40
#define Y_COMMAND     60
#define Y_PARAMETER   80
#define Y_RESULT      100
#define Y_LAST_ERROR  180
#define Y_BATTERY     180
#define Y_LISTENS_TO  200

// Update function declarations
void update_sensor(bool force = false);
void update_command(bool force = false);
void update_parameter(bool force = false);
void update_result(bool force = false);
void update_last_error(bool force = false);
void update_battery(bool force = false);
void update_listens_to(bool force = false);
void update_connection_icon(bool force = false);
void update_battery_icon(bool force = false);
extern unsigned long LastTransferTime;

void draw_bt_icon(int x, int y, uint16_t color)
{
   M5.Lcd.drawXBitmap(x, y, epd_bitmap_bt_icon, bt_icon_width, bt_icon_height, color);
}

void draw_usb_icon(int x, int y, uint16_t color)
{
  M5.Lcd.drawXBitmap(x, y, epd_bitmap_usb_icon,usb_icon_width, usb_icon_height, color);
}
void draw_charge_icon(int x, int y, uint16_t color)
{
  M5.Lcd.drawXBitmap(x, y, epd_bitmap_charge_icon, charge_icon_width, charge_icon_height, color);
}


void update_sensor(bool force)
{
  static String prevSensor = "";
  if (!force && LastSensor == prevSensor) return;
  prevSensor = LastSensor;
  M5.Lcd.setTextPadding(M5.Lcd.width());
  M5.Lcd.setFont(&FreeSansBold9pt7b);
  M5.Lcd.drawString("Sensor:", X_LABEL, Y_SENSOR);
  M5.Lcd.setFont(&FreeSans9pt7b);
  M5.Lcd.drawString(LastSensor, X_VALUE, Y_SENSOR);
}

void update_command(bool force)
{
  static String prevCommand = "";
  if (!force && LastCommand == prevCommand) return;
  prevCommand = LastCommand;
  M5.Lcd.setTextPadding(M5.Lcd.width());
  M5.Lcd.setFont(&FreeSansBold9pt7b);
  M5.Lcd.drawString("Command:", X_LABEL, Y_COMMAND);
  M5.Lcd.setFont(&FreeSans9pt7b);
  M5.Lcd.drawString(LastCommand, X_VALUE, Y_COMMAND);
}

void update_parameter(bool force)
{
  static String prevParameter = "";
  if (!force && LastParameter == prevParameter) return;
  prevParameter = LastParameter;
  M5.Lcd.setTextPadding(M5.Lcd.width());
  M5.Lcd.setFont(&FreeSansBold9pt7b);
  M5.Lcd.drawString("Parameter:", X_LABEL, Y_PARAMETER);
  M5.Lcd.setFont(&FreeSans9pt7b);
  M5.Lcd.drawString(LastParameter.substring(0,22) + " ...", X_VALUE, Y_PARAMETER);
}

void update_result(bool force)
{
  static String prevResult = "";
  static String prevSensor = "";
  static String prevCommand = "";
  if (!force && LastResult == prevResult && LastSensor == prevSensor && LastCommand == prevCommand)
    return; // if nothing changed do nothing
  prevResult = LastResult;
  prevSensor = LastSensor;
  prevCommand = LastCommand;
  M5.Lcd.setTextPadding(M5.Lcd.width());
  M5.Lcd.setFont(&FreeSansBold9pt7b);
  M5.Lcd.drawString("Result:", X_LABEL, Y_RESULT);
  if ((LastSensor == "MLX90640") && (LastCommand == "GET_IMAGE"))
  {
    M5.Lcd.drawString(" ", X_VALUE, Y_RESULT);
    MLX90640_draw_image(X_VALUE, Y_RESULT);
  }
  else
  {
    MLX90640_draw_white_image(X_VALUE, Y_RESULT);
    M5.Lcd.setFont(&FreeSans9pt7b);
    M5.Lcd.drawString(LastResult, X_VALUE, Y_RESULT);
  }
}

void update_last_error(bool force)
{
  static String prevError = "";
  if (!force && ErrorText == prevError) return;
  prevError = ErrorText;
  M5.Lcd.setTextPadding(M5.Lcd.width());
  M5.Lcd.setFont(&FreeSansBold9pt7b);
  M5.Lcd.drawString("Last Error:", X_LABEL, Y_LAST_ERROR);
  M5.Lcd.setFont(&FreeSans9pt7b);
  M5.Lcd.drawString(ErrorText, X_VALUE, Y_LAST_ERROR);
}

void update_battery(bool force)
{
  static int prevLevel = -1;
  static bool prevCharging = false;
  int level = M5.Power.getBatteryLevel();
  bool charging = M5.Power.isCharging();
  if (!force && level == prevLevel && charging == prevCharging) return;
  prevLevel = level;
  prevCharging = charging;
  M5.Lcd.setTextPadding(M5.Lcd.width());
  M5.Lcd.setFont(&FreeSansBold9pt7b);
  M5.Lcd.drawString("Battery:", X_LABEL, Y_BATTERY);
  M5.Lcd.setFont(&FreeSans9pt7b);
  M5.Lcd.drawString(String(level)+"%", X_VALUE, Y_BATTERY);
  if (charging)  M5.Lcd.drawString(", Charging ", X_VALUE+50, Y_BATTERY);
}

void update_listens_to(bool force)
{
  static bool prevBT = !BT_Active;
  if (!force && BT_Active == prevBT) return;
  prevBT = BT_Active;
  M5.Lcd.setTextPadding(M5.Lcd.width());
  M5.Lcd.setFont(&FreeSansBold9pt7b);
  M5.Lcd.drawString("Listens to:", X_LABEL, Y_LISTENS_TO);
  M5.Lcd.setFont(&FreeSans9pt7b);
  if (BT_Active)  M5.Lcd.drawString("Bluetooth", X_VALUE, Y_LISTENS_TO);
  else M5.Lcd.drawString("USB-Serial", X_VALUE, Y_LISTENS_TO);
}

void update_battery_icon(bool force)
{
  static int prevLevel = -1;
  static bool prevCharging = false;
  int level = M5.Power.getBatteryLevel();
 
  bool charging = M5.Power.isCharging();
  
  if (!force && level == prevLevel && charging == prevCharging) return;
  prevLevel = level;
  prevCharging = charging;

  int x = 320 - 46;
  M5.Lcd.fillRect(x, 1, 23, 23, ez.theme->header_bgcolor);

  int bx = x + 3;
  int by = 6;
  int bw = 16;
  int bh = 11;

  M5.Lcd.drawRect(bx, by, bw, bh, TFT_WHITE);
  M5.Lcd.drawRect(bx + bw, by + 3, 2, bh - 6, TFT_WHITE);
  M5.Lcd.fillRect(bx+1, by+1, bw-2, bh-2, TFT_BLACK);

  if (charging) {
    uint16_t col = TFT_GREEN;
    int cx = bx + (bw - charge_icon_width)/2;
    int cy = by + (bh - charge_icon_height)/2;
    draw_charge_icon(cx, cy, col);
  } else {
    int bars = (level + 24) / 25; // show 1-4 bars for 25%-100%
    int barWidth = 2;
    int space = 1;
    int start = bx + 2;
    for (int i = 0; i < 4; i++) {
      int x0 = start + i * (barWidth + space);
      uint16_t color = (i < bars) ? TFT_WHITE : ez.theme->header_bgcolor;
      M5.Lcd.fillRect(x0, by + 2, barWidth, bh - 4, color);
    }
  }
}

void update_connection_icon(bool force)
{
  static bool prevBT = !BT_Active;
  static bool prevFlash = false;
  bool flashing = (millis() - LastTransferTime < 200) && ((millis() / 150) % 2);
  if (!force && BT_Active == prevBT && flashing == prevFlash) {
    return;
  }
  prevBT = BT_Active;
  prevFlash = flashing;

  
  M5.Lcd.fillRect(320-23, 1, 23, 23, ez.theme->header_bgcolor);

  uint16_t col;
  if (BT_Active)
    col = flashing ? TFT_BLUE : TFT_WHITE;
  else
    col = flashing ? TFT_YELLOW : TFT_WHITE;

  if (BT_Active) draw_bt_icon(320-23, 3, col);
  else draw_usb_icon(320-23, 3, col);
}

// Show dynamic sensor values; redraw static text when necessary
void Sensor_show()
{
  bool force = display_sensor_static_text_redraw;
  if (display_sensor_static_text_redraw) display_sensor_show_static_text();

  M5.Lcd.setTextDatum(TL_DATUM);
  M5.Lcd.setFont(&FreeSans9pt7b);
  M5.Lcd.setTextColor(TFT_BLACK, ez.screen.background());
  M5.Lcd.setTextSize(1);

  update_sensor(force);
  update_command(force);
  update_parameter(force);
  update_result(force);
  update_last_error(force);
  update_connection_icon(force);
  update_battery_icon(force);
}

void Sensor_showtime()
{
  if (show_time < millis())
  {
    Sensor_show();
    show_time = millis() + DisplayRefreshInterval;
  }
  update_connection_icon();
  
}



void help_dev_usage()
{
  String DeviceUsage = 
  "To use this device, setup a serial connection over USB or BT and send queries like:"+ cr +
  ">Speaker.beep()"  + cr +
  "A python wrapper library is available and can be installed with:" + cr +
  "pip install rudipy" + cr +
  "For a complete guide, go over to:" + cr +
  "www.github.com/rudimesh/rudicore-M5 or" + cr +
  "www.github.com/rudimesh/rudipy";
  ez.textBox("Device Usage...", DeviceUsage, true, "up#Done#down");
}

void help_dev_info()
{
  String DeviceInfo = "Firmware: Rudicore-M5 v" + Firmware_Version + cr + 
  "(C) 2025 Radboud University" + cr +
  "Authors: Thieu Asselbergs" + cr +
  "               Hans Zondag." + cr +
  "This device contains open-source software. "+cr + 
  "Licenses and source code available at: " + cr +
  "www.github.com/rudimesh/rudicore-M5";
  
  ez.textBox("Device Info...", DeviceInfo, true, "up#Done#down");
}

void menu_help_main()
{
  ezMenu help_main("Help Main...");
  help_main.txtSmall();
  help_main.addItem("Device info", help_dev_info);
  help_main.addItem("Device usage", help_dev_usage);
  help_main.addItem("Back", Sensor_showtime);
  help_main.upOnFirst("last|up");
  help_main.downOnLast("first|down");
  help_main.run();
}

// Fill status window 
void show_status()
{
  ez.header.show("System status...");
  ez.canvas.font(&FreeSans9pt7b);
  ez.canvas.lmargin(25);
  ez.canvas.println("");
  ez.canvas.println("Firmware: " + String(Firmware_Version));

  ez.canvas.println("Baudrate: " + String(baudrate));
  ez.canvas.println("BT Name: " + BT_Name);
  ez.canvas.println("Friendly name: " + Friendly_Name);
  ez.canvas.println("Device type: " + DeviceTypeLoad());
  ez.canvas.println("BT Address: " + BT_Address);
  ez.canvas.println("I2C devices at: " + i2c_scanner() + " (HEX)");
}
 
// GPIO information
typedef struct
{
  int GPIO;
  String Name;
  char Type;
  unsigned int Value;
  unsigned long StopOverload;
  unsigned long StopUnderload;
}  GPIODef;

// GPIO ports to show
#define pins  4
GPIODef Stat[pins] = 
{
  {02, "G2", 'D', 0, 0, 0},
  {05, "G5", 'D', 0, 0, 0},
  {35, "AD", 'A', 0, 0, 0},
  {36, "AD", 'A', 0, 0, 0}
}; 

// Configure digital GPIOs as INPUT_PULLUP
void SetToInputPullUp()
{
  for (int x=0;x<pins;x++)
  {
    if (Stat[x].Type == 'D') pinMode(Stat[x].GPIO, INPUT_PULLUP);    
  }
}

// Configure analog GPIOs as INPUT
void InitializeAnalogIns()
{
  for (int x=0;x<pins;x++)
  {
    if (Stat[x].Type == 'A') pinMode(Stat[x].GPIO, INPUT);    
  }
}


void menu_show_GPIO()
{
  SetToInputPullUp();
  ez.screen.clear();
  ez.header.show("Status GPIO...");
  ez.buttons.show(" # Main # IMU");

  #define ProgressBarColor 0x09F1 // color of the progressbar can't be changed
  int column = 4;
  #define x_start 10
  int x = x_start;
  int temp;
  #define y_start_tab 42
  int y = y_start_tab;
  #define ColDist 50
  int RowDist;
  int bg = ez.screen.background();
  int fg = TFT_BLACK;
  #define Overload 4000
  #define Underload 1
  #define HoldTime 1000

  // text above the column
  String ColumnHeader[column] = {{"IO#"}, {"ID"}, {"A/D"}, {"Value"}};

  M5.Lcd.setFont(&FreeSansBold9pt7b);
  M5.Lcd.setTextDatum(BL_DATUM);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(fg, bg);
  M5.Lcd.setTextPadding(ColDist-10);       // erase previous text (in pixels)
  RowDist = M5.Lcd.fontHeight();

  for (int c=0; c<4; c++)
  {
    M5.Lcd.drawString(ColumnHeader[c], x, y);
    x+=ColDist;
  }

  M5.Lcd.setFont(&FreeSans9pt7b);
  y = y_start_tab;
  y+=RowDist;
  x = x_start;
  for (int i=0; i<pins; i++)
  {
    M5.Lcd.drawNumber(Stat[i].GPIO, x, y); 
    M5.Lcd.drawString(String(Stat[i].Name), x+=ColDist, y);
    M5.Lcd.drawString(String(Stat[i].Type), x+=ColDist, y);
    M5.Lcd.drawNumber(Stat[i].Value, x+=ColDist, y);
    y+=RowDist;
    temp = x;
    x = x_start;
  }

  y = y_start_tab;
  x = x_start-2;

  for (int r=0; r<pins; r++)
  {
    for (int c=0; c<column; c++)
    {
      M5.Lcd.drawRect(x+c*ColDist-3, y+r*RowDist, ColDist+1, RowDist+1, TFT_LIGHTGREY);
    }
  }


  M5.Lcd.setFont(&FreeSansBold9pt7b);
  unsigned int v;
  bool firsttime = true;
  while (true) 
  {
    if ((Serial.available()) || (BT.available())) return;
    M5.Lcd.setTextDatum(BL_DATUM);
    x = temp;
    y = y_start_tab + RowDist;
    for (int i=0; i<pins; i++)
    {
      unsigned int prev_val = Stat[i].Value;
      if (Stat[i].Type == 'D') 
        Stat[i].Value = digitalRead(Stat[i].GPIO);
      else
        Stat[i].Value = analogRead(Stat[i].GPIO);
        write2serial(Stat[i].Value);

      if ((prev_val != Stat[i].Value) || (firsttime))
      {
        M5.Lcd.setTextColor(fg, bg);
        M5.Lcd.drawNumber(Stat[i].Value, x, y);

        int xBar = x+ColDist+15;
        int yBar = y-RowDist+5;
        int wBar = 80;
        int hBar = 10;
        int xOverload = xBar+wBar +2;
        int yOverload = yBar;
        int wOverload = 10;
        int hOverload = 10;
        int wUnderload = 10;
        int hUnderload = 10;
        int xUnderload = xBar-wUnderload -2;
        int yUnderload = yBar;


        // Draw Progress Bar for Analog GPIO
        if (Stat[i].Type == 'A') 
        {
          v = map(Stat[i].Value,0,4095,0,100);
          M5.Lcd.fillRect(   xBar,yBar,wBar,hBar,bg);
          M5.Lcd.progressBar(xBar,yBar,wBar-1,hBar,v);
          
          // turn on/off right RED overload indicator when value > 'Overload' 
          if (Stat[i].Value > Overload)
          {
            Stat[i].StopOverload = millis() + HoldTime;                   // save current time
            M5.Lcd.fillRect(xOverload,yOverload,wOverload,hOverload,TFT_RED);
          }
          else
          {
            if ((Stat[i].StopOverload < millis()) && (Stat[i].StopOverload != 0))    // reset overload indicator after HoldTime and NOT 0
            {
              M5.Lcd.fillRect(xOverload,yOverload,wOverload,hOverload,bg);
              M5.Lcd.drawRect(xOverload,yOverload,wOverload,hOverload,ProgressBarColor);
              Stat[i].StopOverload = 0;
            }
          }
          
          // turn on/off left RED overload indicator when value <= 'Underload'
          if (Stat[i].Value <= Underload)
          {
            Stat[i].StopUnderload = millis() + HoldTime;                   // save current time
            M5.Lcd.fillRect(xUnderload,yUnderload,wUnderload,hUnderload,TFT_RED);
          }
          else
          {
            if ((Stat[i].StopUnderload < millis()) && (Stat[i].StopUnderload != 9999))    // reset overload indicator after HoldTime and NOT 0
            {
              M5.Lcd.fillRect(xUnderload,yUnderload,wUnderload,hUnderload,bg);
              M5.Lcd.drawRect(xUnderload,yUnderload,wUnderload,hUnderload,ProgressBarColor);
              Stat[i].StopUnderload = 9999;
            }
          }
 
        }
        else
        // Draw digital indicator
        {
          if (Stat[i].Value > 0)
            M5.Lcd.fillRect(xBar,yBar,wBar,hBar,ProgressBarColor);
          else
            {
              M5.Lcd.fillRect(xBar,yBar,wBar,hBar,bg);                            
              M5.Lcd.drawRect(xBar,yBar,wBar,hBar,ProgressBarColor);               
            }
        }        
      }
      y+=RowDist;
    }
    firsttime = false;
    button = ez.buttons.poll();
    if (button == "Main") 
    {
      return;
    }
    if (button == "IMU") 
    {
      IMUDemo();
      break;
    }
  }
}

// Forward declarations
void menu_show_status();
void menu_service_main();
void menu_show_GPIO();
bool menu_service_devicetype(ezMenu *callingMenu = nullptr);
bool menu_service_i2cmask(ezMenu *callingMenu = nullptr);

void refresh_name(int index=0)
{
  ez.canvas.clear();
    ez.canvas.font(&FreeSans9pt7b);
    ez.canvas.lmargin(25);
    ez.canvas.println("");
    ez.canvas.println("");
    ez.canvas.println("Use Prev/Next to select a name:");
    ez.canvas.println("");
    ez.canvas.font(&FreeSansBold9pt7b);
    ez.canvas.println(String("    ") + FriendlyNameList[index]);

}


bool menu_service_friendlyname(ezMenu *callingMenu = nullptr)
{
  ez.screen.clear();
  ez.header.show("Friendly name...");
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_BT);
  int index = (baseMac[5] + FriendlyNameOffset) % NumFriendlyNames;
  refresh_name(index);

  while (true)
  {
    ez.buttons.show("Prev # Save # Next");
    if ((Serial.available()) || (BT.available())) return false;
    button = ez.buttons.poll();

    if (button == "Next") {
      index = (index + 1) % NumFriendlyNames;
      refresh_name(index);
    }
    if (button == "Prev") {
      index = (index + NumFriendlyNames - 1) % NumFriendlyNames;
      refresh_name(index);
    }
    if (button == "Save")
    {
      int offset = (index - baseMac[5]) % NumFriendlyNames;
      if (offset < 0) offset += NumFriendlyNames;
      FriendlyNameOffset = offset;
      FriendlyNameOffsetSave(FriendlyNameOffset);
      Friendly_Name = FriendlyNameConstruct();
      display_sensor_static_text_redraw = true;
      return false; // close menu and return to main screen
    }
  }

  return true; // keep menu open
}



bool menu_service_devicetype(ezMenu *callingMenu)
{
  // Options for device type selection
  static const char *options[] = {"DAQ", "Gray", "Core Basic"};
  const int nopts = sizeof(options) / sizeof(options[0]);

  ez.screen.clear();
  ez.header.show("Device Type...");

  // Load current value and map to index
  String current = DeviceTypeLoad();
  int index = 0;
  for (int i = 0; i < nopts; i++)
    if (current.equalsIgnoreCase(options[i])) { index = i; break; }

  auto refresh_view = [&](int idx){
    ez.canvas.clear();
    ez.canvas.font(&FreeSans9pt7b);
    ez.canvas.lmargin(25);
    ez.canvas.println("");
    ez.canvas.println("");
    ez.canvas.println("Use Prev/Next to select type:");
    ez.canvas.println("");
    ez.canvas.font(&FreeSansBold9pt7b);
    ez.canvas.println(String("    ") + options[idx]);
  };

  refresh_view(index);

  while (true)
  {
    ez.buttons.show("Prev # Save # Next");
    if ((Serial.available()) || (BT.available())) return false;
    button = ez.buttons.poll();

    if (button == "Next") {
      index = (index + 1) % nopts;
      refresh_view(index);
    }
    if (button == "Prev") {
      index = (index + nopts - 1) % nopts;
      refresh_view(index);
    }
    if (button == "Save")
    {
      DeviceTypeSave(String(options[index]));
      display_sensor_static_text_redraw = true;
      return false; // close and return to previous menu/screen
    }
  }

  return true; // keep menu open
}



bool menu_service_i2cmask(ezMenu *callingMenu)
{
  // Bit mapping: 0=D0 (DAQ), 1=D1 (GPIO), 2=D2 (ESP32ADC), 3=D3 (Speaker)
  // System (D4) is always visible and not maskable
  const char *names[4] = {"D0: DAQ", "D1: GPIO", "D2: ESP32ADC", "D3: Speaker"};

  uint8_t mask = VirtualI2CMaskLoad();
  int index = 0;

  auto draw_view = [&](int sel){
    ez.screen.clear();
    ez.header.show("Internal I2C Mask...");
    ez.canvas.font(&FreeSans9pt7b);
    ez.canvas.lmargin(20);
    ez.canvas.println("");
    ez.canvas.println("Select and Toggle to hide/show:");
    ez.canvas.println("");

    for (int i = 0; i < 4; ++i)
    {
      bool hidden = (mask & (1 << i)) != 0;
      String line = String((i == sel) ? "> " : "  ");
      line += names[i];
      line += String("  [") + (hidden ? "Hidden" : "Shown") + "]";
      if (i == sel) ez.canvas.font(&FreeSansBold9pt7b);
      else ez.canvas.font(&FreeSans9pt7b);
      ez.canvas.println(line);
    }
  };

  draw_view(index);

  while (true)
  {
    ez.buttons.show("Back # Toggle # Next");
    if ((Serial.available()) || (BT.available())) return false;
    button = ez.buttons.poll();

    if (button == "Back")
    {
      // leave without further changes
      return false;
    }
    if (button == "Next")
    {
      index = (index + 1) % 4;
      draw_view(index);
    }
    if (button == "Toggle")
    {
      mask ^= (1 << index);
      VirtualI2CMaskSave(mask);
      draw_view(index);
    }
  }

  return true;
}


void menu_show_status()
{
  ez.screen.clear();
  show_status();
  ez.buttons.show("IO Status#Service#Back");

  while (true) 
  {
    if ((Serial.available()) || (BT.available())) return;
    button = ez.buttons.poll();
    if (button == "Back") 
    {
      return;
    }
    if (button == "Refresh") 
    {
      menu_show_status();
      return;
    }
    if (button == "IO Status")
    {
      menu_show_GPIO();
      return;
    }
    if (button == "Service")
    {
      menu_service_main();
      return;
    }
  }
}


void menu_service_main()
{
  ezMenu svc("Service...");
  svc.txtSmall();
  svc.addItem("Device Type", NULL, menu_service_devicetype);
  svc.addItem("Friendly name", NULL, menu_service_friendlyname);
  svc.addItem("Internal I2C Mask", NULL, menu_service_i2cmask);
  svc.addItem("Back", menu_show_status);
  svc.upOnFirst("last|up");
  svc.downOnLast("first|down");
  svc.run();
}


// Read menu buttons
void read_button()
{
  ez.buttons.show("DisplayOff # M5 Status # Help");
  button = ez.buttons.poll();
  
  if (debug) Serial.println(button);

  if (button != "") display_sensor_static_text_redraw = true; 

  if (button == "DisplayOff") DisplayOff();
  if (button == "M5 Status") menu_show_status();
  if (button == "Help") menu_help_main();
}
