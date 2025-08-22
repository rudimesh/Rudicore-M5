/*
 * Rudicore-M5 firmware entry point
 * - Initializes M5Stack, Bluetooth, UI, and unit drivers
 * - Parses serial/BT commands and dispatches to unit handlers
 * See README.md for an overview and docs/ for command reference.
 */


 // IMU selection for M5Stack
#define M5STACK_MPU6886

#include <M5Stack.h>
#include "Version.h"
#include <Wire.h>
#include <M5ez.h>
#include "BluetoothSerial.h"
#include "ST_HW_HC_SR04.h"
#include "Adafruit_TCS34725.h"
#include "SHT3X.h"
#include "Adafruit_BMP280.h"
#include "Adafruit_Sensor.h"
#include <Adafruit_MCP4725.h>
#include "Adafruit_VL53L0X.h"
#include "Rudicore_TCA9548A.h"
#include "TinyGPS++.h"
#include "SoftwareSerial.h"         
#include "SdsDustSensor.h"          
#include "Adafruit_SGP30.h"
#include "HX711.h"
#include "porthub.h"
#include "Preferences.h"
#include "M5_BMM150.h"
#include "M5_BMM150_DEFS.h"
#include "M5_KMeter.h"
#include "ESP32AnalogRead.h"        
#include "ADS1100.h"                
#include "DFRobot_OxygenSensor.h"   
#include "INA.h"                    
#include "GP8403.h"                 
#include "LTR390.h"                 
#include "ADXL345.h"                
#include "UNIT_SCALES.h"            
#include "Splash.h"

#define debug false
#define RAL3020_RGB565    M5.Lcd.color565(227, 0, 11)
#define POPPY_RGB565      M5.Lcd.color565(255, 66, 75)
#define LADYBUG_RGB565    M5.Lcd.color565(190, 49, 26)
#define BERRY_RGB565      M5.Lcd.color565(143, 32, 17)
#define MAROON_RGB565     M5.Lcd.color565(115, 14, 4)
#define MAHOGANY_RGB565   M5.Lcd.color565(74, 0, 4)

BluetoothSerial BT;
bool BT_Active = false;
String BT_Address;
String BT_Name;
String Friendly_Name;
int FriendlyNameOffset = 0;

// Buffer for storing sensor data
#define data_buf_size 20000
uint16_t data_buf[data_buf_size];
uint16_t data_buf_pointer_read=0;
uint16_t data_buf_pointer_write=0;

#define baudrate 115200             // 
#define M5STACKFIRE_SPEAKER_PIN 25

// result of testing the existence of sensors in init()
bool MPU6886_active = false;     
bool Hall_active = false;           
bool ESP32_HALL_active = false; 
bool ESP32_ADC_active = false;
bool UltraSonic_active = false;     
bool HC_SR04_active = false; 
bool JSN_SR04T_active = false; 
bool Color_active = false; 
bool ADS1100_active = false; 
bool MCP4725_active = false; 
bool TOF_active = false; 
bool PaHUB_active = false; 
bool M5_Speaker_active = false; 
bool GPS_active = false;
bool ENVII_active = false; 
bool SGP30_active = false;
bool Laser_Rx_active = false;
bool SDS011_active = false;
bool display_active = true;         
bool HX711_active = false;          
bool PbHUB_active = false;          
bool NCir_active = false;           
bool KMeter_active = false;         
bool SEN0322_active = false;        
bool MLX90640_active = false;       
bool INA_active = false;            
bool GP8403_active = false;         
bool UV_active = false;         
bool ADXL345_active = false;        
bool M5_Miniscale_active = false;   
bool Joystick_active = false;       
bool FacesEncoder_active = false;   
bool VMeter_active = false;         
bool AMeter_active = false;         
bool ExtEncoder_active = false;     

bool display_sensor_static_text_redraw;   

String QR_String = "www.github.com/rudimesh/rudicore-M5";
String Last_Error = "None";
String ErrorText = "None";

long int DisplayRefreshInterval = 150;
long int show_time;
unsigned long LastTransferTime = 0;

const String cr = (String)char(13);
String button;
String readString;
String LastSensor;
String LastCommand;
String LastParameter;
String LastResult;
String last_commandstring;

// Parsed command fields
String sensor;
String command;
String param;
String param_key, param_val;
char UpperCase = 'U';

#include "Tools.h"
#include "MenuDisplay.h"

// Standard M5Stack units
#include "standard_i2c/M5_Miniscale.h"
#include "standard_i2c/MLX90640.h"
#include "standard_i2c/SGP30.h"
#include "standard_i2c/KMeter.h"
#include "standard_i2c/UltraSonic.h"
#include "standard_i2c/PbHUB.h"
#include "standard_i2c/MLX90614.h"
#include "standard_i2c/VLX53LOX.h"
#include "standard_i2c/ENVII.h"
#include "standard_i2c/ADS1100.h"
#include "standard_i2c/MCP4725.h"
#include "standard_i2c/GP8403.h"
#include "standard_i2c/TCS34725.h"
#include "standard_i2c/PaHUB.h"
#include "standard_i2c/Roller.h"
#include "standard_i2c/ADXL345.h"
#include "standard_i2c/Joystick.h"
#include "standard_i2c/ExtEncoder.h"
#include "standard_i2c/FacesEncoder.h"
#include "standard_i2c/VMeter.h"
#include "standard_i2c/AMeter.h"
#include "standard_i2c/MPU6886.h"
#include "standard_i2c/BMM150.h"

//Custom M5Stack units
#include "custom_i2c/INA226.h"
#include "custom_i2c/LTR390.h"
#include "custom_i2c/SEN0322.h"

// Virtual units
#include "virtual_i2c/ESP32ADC.h"
#include "virtual_i2c/GPIO.h"
#include "virtual_i2c/DAQ.h"
#include "virtual_i2c/Speaker.h"
#include "virtual_i2c/System.h"

  void setup() 
  {
  M5.begin(true, false, false, true);
  Serial.begin(115200);
          
  M5.Power.begin();
  // Power-off on USB unplug instead of reboot
  M5.Power.setPowerBoostKeepOn(false);
  M5.Power.setPowerVin(false);
  M5.Power.setPowerBtnEn(true);
  M5.Power.setCharge(true);

  const char *UniqueBTName = BTNameConstruct();
  BT.begin(UniqueBTName);
  FriendlyNameOffset = FriendlyNameOffsetLoad();
  Friendly_Name = FriendlyNameConstruct();
  
  ez.begin();

  showSplash();

  ez.theme->header_bgcolor = RAL3020_RGB565;   
  ez.theme->button_bgcolor_b     = LADYBUG_RGB565;
  ez.theme->button_bgcolor_t = TFT_BLUE;
  ez.canvas.clear();
  ez.canvas.scroll(false);
  ez.header.remove("clock");
  ez.theme->header_height=25;
  ez.theme->header_tmargin = 5;
  M5.Lcd.setTextColor(TFT_BLACK, ez.screen.background());
  
  SetToInputPullUp();
  
  M5.Speaker.begin();
  M5.Speaker.mute();

  while(Serial.read() >= 0) ;
 
  display_sensor_static_text_redraw = true;
  show_status();
  delay(500);

  show_time = millis() + DisplayRefreshInterval;
  }

  void loop() 
  {
  read_user_input();
  ensurePaHUBPort();
  if (sensor == "SYSTEM") print2serial(System());
  if (sensor == "SPEAKER") print2serial(Speaker());
  if (sensor == "COLOR") print2serial(Color());
  if (sensor == "ADC") print2serial(ADC_ADS1100());
  if (sensor == "DAC") print2serial(MCP4725());
  if (sensor == "ENVII") print2serial(BMP280_SHT3X());
  if (sensor == "TOF") print2serial(VLX53LOX());
  if (sensor == "ESP32ADC") print2serial(ESP_ADC());
  if (sensor == "PAHUB") print2serial(PaHUB());
  if (sensor == "NCIR") print2serial(MLX90614());
  if (sensor == "PBHUB") print2serial(PbdotHUB());
  if (sensor == "M5GPIO") print2serial(M5GPIO());
  if (sensor == "BMM150") print2serial(BMM150());
  if (sensor == "KMETER") print2serial(KMeter());
  if (sensor == "SGP30") print2serial(SGP30());
  if (sensor == "SEN0322") print2serial(SEN0322());
  if (sensor == "MLX90640") print2serial(MLX90640());
  if (sensor == "INA226") print2serial(INA226_module());
  if (sensor == "GP8403") print2serial(DAC_GP8403());
  if (sensor == "EXTENCODER") print2serial(ExtEncoder());
  if (sensor == "MPU6886") print2serial(MPU6886());
  if (sensor == "ADXL345") print2serial(ADXL345_sensor());
  if (sensor == "JOYSTICK") print2serial(Joystick());
  if (sensor == "FACESENCODER") print2serial(FacesEncoder());
  if (sensor == "DAQ") print2serial(DAQ());
  if (sensor == "SCALES") print2serial(M5_Miniscale());
  if (sensor == "ROLLER") print2serial(Roller());
  if (sensor == "VMETER") print2serial(VMeter_unit());
  if (sensor == "AMETER") print2serial(AMeter_unit());
  if (sensor == "HELP") print2serial(HelpText());
  if (sensor == "ULTRASONIC") print2serial(UltraSonic());
  if (sensor == "UV") print2serial(UV());

  // for backwards compatibility:
  if (sensor == "M5TOOLS") print2serial(System());
  if (sensor == "M5TEST") print2serial(System());

  if (display_active) Sensor_showtime();
  read_button();
  }
