#pragma once
/* --- AMeter Unit ------------------------------------------------------------
 * >AMeter.GetValue(Current|RawADC)
 * >AMeter.Initialize()
 */

#include "M5_ADS1115.h"

#define M5_UNIT_AMETER_I2C_ADDR             0x48
#define M5_UNIT_AMETER_EEPROM_I2C_ADDR      0x51
#define M5_UNIT_AMETER_PRESSURE_COEFFICIENT 0.05F

// PaHUB port used for this device
extern int AMeter_port;

ADS1115 Ameter;

float AMeter_resolution = 0.0;
float AMeter_calibration = 0.0;

bool AMeter_init()
{
  if (PaHUB_active && AMeter_port >= 0) selectPaHUBChannel(AMeter_port);
  AMeter_active = false;
  if (!Ameter.begin(&Wire, M5_UNIT_AMETER_I2C_ADDR, 21, 22, 400000U))
  {
    LastError("AMeter not found");
    return false;
  }
  Ameter.setEEPROMAddr(M5_UNIT_AMETER_EEPROM_I2C_ADDR);
  Ameter.setMode(ADS1115_MODE_SINGLESHOT);
  Ameter.setRate(ADS1115_RATE_8);
  Ameter.setGain(ADS1115_PGA_512);

  AMeter_resolution = Ameter.getCoefficient() / M5_UNIT_AMETER_PRESSURE_COEFFICIENT;
  AMeter_calibration = Ameter.getFactoryCalibration();
  AMeter_active = true;
  return true;
}

String AMeter_unit()
{
  if (PaHUB_active && AMeter_port >= 0) selectPaHUBChannel(AMeter_port);
  if (!AMeter_active) AMeter_init();
  if (!AMeter_active) return "-";

  if (command == "GETVALUE")
  {
    if (GetParameterValue("RAWADC", UpperCase) == "NOVAL")
    {
      int16_t adc_raw = Ameter.getSingleConversion();
      return String(adc_raw);
    }
    // Default: current
    int16_t adc_raw = Ameter.getSingleConversion();
    float current = adc_raw * AMeter_resolution * AMeter_calibration;
    return String(current, 2);
  }

  if (command == "INITIALIZE")
  {
    if (AMeter_init()) return ""; else return "-";
  }

  LastError("No valid command found");
  return "-";
}

// -----------------------------------------------------------------------------
