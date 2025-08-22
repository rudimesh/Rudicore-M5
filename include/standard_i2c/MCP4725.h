#pragma once
/* --- DAC MCP4725 ----------------------------------------------------------------------------------

 * >DAC.SetValue()      (Sets the output voltage to a fraction of source vref.)
 * >DAC.SetVoltage()    (Calculate the output value)
 * >DAC.Initialize()
 */
#define MCP4725_address 0x60    // 0x60 or 0x61 depending on ADDR pin
Adafruit_MCP4725 dac;

// PaHUB port used for this device
extern int MCP4725_port;
 
bool MCP4725_init()
{
  if (PaHUB_active && MCP4725_port >= 0) selectPaHUBChannel(MCP4725_port);
  bool active = dac.begin(MCP4725_address);
  dac.setVoltage(0, false);       // Set output to 0. Parameters: val, storeflag. Storeflag stores the val in EEPROM (max. 20.000 times)
  MCP4725_active = active;
  return active;
}

// Main MCP4725 function
String MCP4725()
{
  if (PaHUB_active && MCP4725_port >= 0) selectPaHUBChannel(MCP4725_port);
  if (!MCP4725_active) MCP4725_init();
  if (!MCP4725_active) return "-";

  if (command == "SETVALUE")
  {
    String param_val;
    param_val = GetParameterValue("VALUEONLY", UpperCase);
    unsigned int v = param_val.toInt();
    v = min(v, 4095);
    dac.setVoltage(v, false);
    return "";
  }

  if (command == "SETVOLTAGE")
  {
    String param_val;
    param_val = GetParameterValue("VALUEONLY", UpperCase);
    unsigned int v = param_val.toInt() / 1.2;
    v = min(v, 4095);
    dac.setVoltage(v, false);
    return "";
  }

  
  if (command == "INITIALIZE")
  {
    if (MCP4725_init()) return ""; else return "-";
  }
  LastError("No valid command found");
  return "-";
}
// --------------------------------------------------------------------------------------------------------


