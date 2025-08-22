#pragma once
/* --- VMeter Unit ------------------------------------------------------------
 * >VMeter.GetValue(Voltage|RawADC)
 * >VMeter.Initialize()
 */

#include "M5_ADS1115.h"

#define M5_UNIT_VMETER_I2C_ADDR             0x49
#define M5_UNIT_VMETER_EEPROM_I2C_ADDR      0x53
#define M5_UNIT_VMETER_PRESSURE_COEFFICIENT 0.015918958F

// PaHUB port used for this device
extern int VMeter_port;

ADS1115 VMeter;

float VMeter_resolution = 0.0;
float VMeter_calibration = 0.0;

bool VMeter_init()
{
  if (PaHUB_active && VMeter_port >= 0) selectPaHUBChannel(VMeter_port);
  VMeter_active = false;
  if (!VMeter.begin(&Wire, M5_UNIT_VMETER_I2C_ADDR, 21, 22, 400000U))
  {
    LastError("VMeter not found");
    return false;
  }
  VMeter.setEEPROMAddr(M5_UNIT_VMETER_EEPROM_I2C_ADDR);
  VMeter.setMode(ADS1115_MODE_SINGLESHOT);
  VMeter.setRate(ADS1115_RATE_8);
  VMeter.setGain(ADS1115_PGA_512);

  VMeter_resolution = VMeter.getCoefficient() / M5_UNIT_VMETER_PRESSURE_COEFFICIENT;
  VMeter_calibration = VMeter.getFactoryCalibration();
  VMeter_active = true;
  return true;
}

String VMeter_unit()
{
  if (PaHUB_active && VMeter_port >= 0) selectPaHUBChannel(VMeter_port);
  if (!VMeter_active) VMeter_init();
  if (!VMeter_active) return "-";

  if (command == "GETVALUE")
  {
    if (GetParameterValue("RAWADC", UpperCase) == "NOVAL")
    {
      int16_t adc_raw = VMeter.getSingleConversion();
      return String(adc_raw);
    }
    // default Voltage
    int16_t adc_raw = VMeter.getSingleConversion();
    float voltage = adc_raw * VMeter_resolution * VMeter_calibration;
    return String(voltage, 2);
  }

  if (command == "INITIALIZE")
  {
    if (VMeter_init()) return ""; else return "-";
  }

  LastError("No valid command found");
  return "-";
}

// -----------------------------------------------------------------------------
