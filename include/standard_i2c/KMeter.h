#pragma once
/* --- KMeter I2C (0x66) thermocouple meter ---------------------------------------------------------

 * >KMeter.GetValue(Temperature|InternalTemperature)
 * 
 * https://github.com/m5stack/M5Unit-KMeter
 */
 
M5_KMeter Ksensor;

#define KMeter_address 0x66


bool KMeter_connected()
{
  // Check that the unit is connected
  Wire.beginTransmission(KMeter_address);
  if ( Wire.endTransmission() != 0) return false;
  return true;
}

bool KMeter_init()
{
  Ksensor.begin();
  delay(10);
  if (KMeter_connected()) KMeter_active = true; else KMeter_active = false;
  return KMeter_active;
}

String KMeter()
{
  if (!KMeter_active) KMeter_init();
  if (!KMeter_active)
  {
    LastError("Device not found");
    return "-";
  }
  
   if (command == "GETVALUE")
  {
    Ksensor.update();
    delay(10);
    if (GetParameterValue("TEMPERATURE", UpperCase) == "NOVAL") return String(Ksensor.getTemperature(),2);
    if (GetParameterValue("INTERNALTEMPERATURE", UpperCase) == "NOVAL") return String(Ksensor.getInternalTemp(),2);
  }
  
  LastError("No valid command found");
  return "-";
}


// --------------------------------------------------------------------------------------------------------
