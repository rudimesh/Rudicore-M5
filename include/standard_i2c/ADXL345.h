#pragma once
/* --- ADXL345 Accelerometer Unit (i2c 0x53) ------------------------------------
 * >ADXL345.Initialize()
 * >ADXL345.GetValue(X|Y|Z)
 * >ADXL345.GetValues()
 */

#include <ADXL345.h>

#define ADXL345_address 0x53


ADXL345 adxl345(ADXL345_address);

bool ADXL345_connected()
{
  Wire.beginTransmission(ADXL345_address);
  if (Wire.endTransmission() != 0) return false;
  return true;
}

bool ADXL345_init()
{
  ADXL345_active = ADXL345_connected();
  if (ADXL345_active) {
    adxl345.start();
  }
  return ADXL345_active;
}

String ADXL345_sensor()
{
  if (!ADXL345_active) ADXL345_init();
  if (!ADXL345_active) {
    LastError("ADXL345 not found");
    return "-";
  }

  adxl345.update();

  if (command == "GETVALUE") {
    if (GetParameterValue("X", UpperCase) == "NOVAL") return String(adxl345.getX(),3);
    if (GetParameterValue("Y", UpperCase) == "NOVAL") return String(adxl345.getY(),3);
    if (GetParameterValue("Z", UpperCase) == "NOVAL") return String(adxl345.getZ(),3);
    LastError("No parameter found");
    return "-";
  }

  if (command == "GETVALUES") {
    return String(adxl345.getX(),3) + ", " + String(adxl345.getY(),3) + ", " + String(adxl345.getZ(),3);
  }

  if (command == "INITIALIZE") {
    if (ADXL345_init()) return ""; else return "-";
  }

  LastError("No valid command found");
  return "-";
}

// -----------------------------------------------------------------------------
