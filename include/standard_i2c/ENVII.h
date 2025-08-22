#pragma once
/* --- ENVII ----------------------------------------------------------------------------------
 * Prefix: ENVII
 * >ENVII.GetTemperature()
 * >ENVII.GetHumidity()
 * >ENVII.GetPressure()
 * >ENVII.Initialize()
 */
 
#define BMP280_address 0x76    // I2C address
#define CELSIUS    1
#define KELVIN     2
#define FAHRENHEIT 3

// PaHUB port used for this device
extern int ENVII_port;

Adafruit_BMP280 bme;
SHT3X sht30;

bool BMP280_init()
{
  if (PaHUB_active && ENVII_port >= 0) selectPaHUBChannel(ENVII_port);
  ENVII_active = bme.begin(BMP280_address);
  return ENVII_active;
}

// Main ENVII (BMP280 & SHT3X) function
String BMP280_SHT3X()
{
  if (PaHUB_active && ENVII_port >= 0) selectPaHUBChannel(ENVII_port);
  float tmp = 0.0;
  float hum = 0.0;
  if (!ENVII_active) BMP280_init();
  if (!ENVII_active) 
  {
    LastError("ENVII not found");
    return "-";
  }
  
  if ((command == "GETTEMPERATURE") || (command == "GETHUMIDITY")) 
  {
    if (sht30.get() == 0)                           // Update temperature and humidity only when SHT30 has new data
    {
      tmp = sht30.cTemp;
      hum = sht30.humidity;
    }
  }
  if (command == "GETTEMPERATURE") return String(tmp);
  if (command == "GETHUMIDITY")  return String(hum);
  if (command == "GETPRESSURE") return String(bme.readPressure());
  if (command == "INITIALIZE")
  {
    if (BMP280_init()) return ""; 
    else 
    {
      LastError("ENVII not found");
      return "-";
    }
  }
  LastError("No valid command");
  return "-";
}
// --------------------------------------------------------------------------------------------------------
