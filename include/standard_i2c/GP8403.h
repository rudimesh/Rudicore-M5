#pragma once
#include <M5Stack.h>
#include <Wire.h>

/* --- GP8403 I2C 0x5F ---------------------------------------------------------------------------
* ">GP8403.Initialize()"
* ">GP8403.SetValue()"        // 0- 4095
* ">GP8403.SetVoltage()"      // 0-5000 or 0-10000 (milliVolt) depending on >GP8403.SetVoltageRange()
* ">GP8403.SetChannel()"            // 0 or 1
* ">GP8403.SetVoltageRange()"       // 5 or 10
* 
*   https://wiki.dfrobot.com/SKU_DFR0971_2_Channel_I2C_0_10V_DAC_Module
*/

byte GP8403_i2c_address = 0x5f;                         // all switches ON
int def_GP8403_output_range = 5;                        // default output range 0-5V
int GP8403_output_range = def_GP8403_output_range;
int GP8403_channel = 0;                                 // current output channel

// PaHUB port used for this device
extern int GP8403_port;

GP8403 _gp8403 = GP8403(GP8403_i2c_address);

bool GP8403_connected()
{
  // check unit is connected
  Wire.beginTransmission(GP8403_i2c_address);
  if ( Wire.endTransmission() != 0) return false;
  return true;
}

// Convert a voltage (millivolts) to a value between 0 and 4095
int GP8403_ConvertToValue(int milliVoltage)
{
  if (milliVoltage <= GP8403_output_range * 1000)       // Ignore if voltage exceeds the selected output range (5 V or 10 V)
  {
    float mv = float(milliVoltage);
    float range = float(GP8403_output_range*1000);
    int val = mv / range * 4095.0; 
    return val;
  }
  else
  {
    LastError("No valid parameter value found");
    return -1;      
  }
}


String GP8403_SetValue(int v)
{
  if ((v>=0)  && (v<4096))
  {
    if (GP8403_channel == 0) _gp8403.setOutput(GP8403::OUT_0, v);
    if (GP8403_channel == 1) _gp8403.setOutput(GP8403::OUT_1, v);
    return " ";
  }
  else 
  {
    LastError("Parameter value out of range");
    return "-";      
  }
}

String GP8403_Channel(int val)
{
  if ((val == 0) || (val == 1)) 
  {
    GP8403_channel = val;
    return String(val);
  }
  else 
  {
    LastError("No valid parameter value found");
    return "-";
  }
}



String GP8403_OutputRange(int val)
{
  if ((val == 5) || (val == 10)) 
  {
    if (val == 5) _gp8403.setVoltageRange(GP8403::V_5);
    if (val == 10) _gp8403.setVoltageRange(GP8403::V_10);
    GP8403_output_range = val;
    return " ";
  }
  else 
  {
    LastError("No valid parameter value found");
    return "-";
  }
}

bool GP8403_init()
{
  if (PaHUB_active && GP8403_port >= 0) selectPaHUBChannel(GP8403_port);
  GP8403_active = GP8403_connected();
  if (GP8403_active) GP8403_OutputRange(def_GP8403_output_range);
  return GP8403_active;
}


String DAC_GP8403()
{
  if (PaHUB_active && GP8403_port >= 0) selectPaHUBChannel(GP8403_port);
  String val;
  int v;
  
  if (!GP8403_active) GP8403_init();
  if (!GP8403_active) 
  {
    LastError("GP8403 device not found");
    return "-";
  }

  if (command == "SETVOLTAGE")  
  {
    param_val = GetParameterValue("VALUEONLY", UpperCase);
    int milliVoltage = param_val.toInt();
    v = GP8403_ConvertToValue(milliVoltage);
    val=GP8403_SetValue(v);
    return val;
  }


  if (command == "SETVALUE")  
  {
    param_val = GetParameterValue("VALUEONLY", UpperCase);
    v = param_val.toInt();
    if ((v>=0)  && (v<4096))
    {
      val=GP8403_SetValue(v);
      return val;
    }
    return "-";
  }

  if (command == "SETCHANNEL")  
  {
    param_val = GetParameterValue("VALUEONLY", UpperCase);
    v = param_val.toInt();
    val = GP8403_Channel(v);
    return val;
  }



  if (command == "SETVOLTAGERANGE")  
  {
    param_val = GetParameterValue("VALUEONLY", UpperCase);
    v = param_val.toInt();
    val = GP8403_OutputRange(v);
    return val;
  }

  if (command == "INITIALIZE")  
  {
    if (!GP8403_init())
    {
      LastError("GP8403 not found");
      return "-";
    }
    return " ";
  }
  LastError("No valid command found");
  return "-";
}
