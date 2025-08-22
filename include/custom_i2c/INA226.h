#pragma once
/* --- INA226 i2c 0x40 ---------------------------------------------------------
 * ">INA226.Initialize()"
 * ">INA226.GetValues()"     // result: BusVoltage (mV), Current (mA) in string format
 *
 */

INA_Class INA;

bool INA_init()
{
  uint8_t deviceNumber = INA.begin(20,2000);              // 20Amp, 2000 microOhm (0.002ohm). Returns '1' when unit is found
  if (deviceNumber > 0)
  {
    // all settings can be found in INA.h
    INA_active = true;
    INA.setAveraging(64, deviceNumber);                   // Average each reading 64 times
    INA.setBusConversion(8244, deviceNumber);             // Maximum conversion time 8.244ms
    INA.setShuntConversion(8244, deviceNumber);           // Maximum conversion time 8.244ms
    INA.setMode(INA_MODE_CONTINUOUS_BOTH, deviceNumber);  // Bus/shunt measured continuously
    INA.alertOnConversion(true, deviceNumber);            // Make alert pin go low on finish
  }
  else INA_active = false;

  return  INA_active;
}

String INA226_module()
{
  if (!INA_active) INA_init();
  if (!INA_active)
  {
    LastError("INA226 module not found");
    return "-";
  }

  if (command == "GETVALUES")
  {
    String c = (String(INA.getBusMilliVolts(0))+"," + String(INA.getBusMicroAmps(0) / 1000.0));       // get values from device '0'
    return c;
  }

  if (command == "INITIALIZE")
  {
    INA_init();
    if (!INA_active)
    {
      LastError("INA226 module not found");
      return "-";
    }
    return " ";
  }
  LastError("No valid command found");
  return "-";
}

// -----------------------------------------------------------------------------
