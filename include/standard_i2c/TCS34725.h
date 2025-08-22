#pragma once
/* --- Color sensor (TCS34725) ----------------------------------------------------------------------------------

 * Color sensor RGB unit
 * Command strings:
 * >Color.GetValue(Red)
 * >Color.GetValue(Green)
 * >Color.GetValue(Blue)
 * >Color.GetValue(Clear)
 * >Color.GetValues()
 * >Color.Configure(Gain=1|4|16|60, IntegrationTime=2(=2.4ms)|24|50|101|154|300|614 )
 * >Color.Initialize()
 * 
 * Note: TOF and COLOR share the same I2C address
 */

// our RGB -> eye-recognized gamma color
byte gammatable[256];
#define default_Color_Gain TCS34725_GAIN_4X
#define default_Color_IntegrationTime TCS34725_INTEGRATIONTIME_50MS

// PaHUB port used for this device
extern int TCS34725_port;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// initialize, return true if ready.
bool Color_init()
{
  if (PaHUB_active && TCS34725_port >= 0) selectPaHUBChannel(TCS34725_port);
  Color_active = false;
  if (tcs.begin())
  {
    uint16_t clear, red, green, blue;
    tcs.setIntegrationTime(default_Color_IntegrationTime);
    tcs.setGain(default_Color_Gain);
    tcs.getRawData(&red, &green, &blue, &clear);
    Color_active = true;
  }
  return Color_active;
}

bool Color_configure_IntegrationTime(String param_val)
{
  bool ok = false;
  if (param_val == "NOVAL")
  {
    tcs.setIntegrationTime(default_Color_IntegrationTime);   // No value: set to default
    return false;                                       // No parameter value found
  }
  
  if (param_val == "NOPARAM") return true;             // No valid parameter found, return with no changes

  int val = param_val.toInt();
  switch (val)
  {
    case 2:
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_2_4MS);
      ok = true;
      break;
    case 24:
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);  
      ok = true;
      break;
    case 50:
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_50MS);  
      ok = true;
      break;
    case 101:
     tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_101MS);
      ok = true;
      break;
    case 154:
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_154MS);  
      ok = true;
      break;
    case 300:
      tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_300MS);  
      ok = true;
      break;
    case 614:
     tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_614MS);
      ok = true;
      break;
    default:
      ok = false;
      break;
  }
  return ok;            // Return true on success; false if the value is invalid
}

bool Color_configure_Gain(String param_val)
{
  bool ok = false;
  if (param_val == "NOVAL")
  {
    tcs.setGain(default_Color_Gain);            // No value: set to default
    return false;                               // No parameter value found
  }
  
  if (param_val == "NOPARAM") return true;     // No valid parameter found, return with no changes

  int val = param_val.toInt();
  switch (val)
  {
    case 1:
      tcs.setGain(TCS34725_GAIN_1X);
      ok = true;
      break;
    case 4:
      tcs.setGain(TCS34725_GAIN_4X);  
      ok = true;
      break;
    case 16:
      tcs.setGain(TCS34725_GAIN_16X);  
      ok = true;
      break;
    case 60:
      tcs.setGain(TCS34725_GAIN_60X);
      ok = true;
      break;
    default:
      ok = false;
      break;
  }
  return ok;                                  // Return true on success; false if the value is invalid
}

String Color()
{
  if (PaHUB_active && TCS34725_port >= 0) selectPaHUBChannel(TCS34725_port);
  String param_val;
  uint16_t clear, red, green, blue;

  if (!Color_active) Color_init();
  if (!Color_active) 
  {
    LastError("Color Unit not found");
    return "-";
  }
  
  tcs.getRawData(&red, &green, &blue, &clear);  // First call also initializes the device
  
  if (command == "GETVALUE")
  {
    if (GetParameterValue("RED", UpperCase) == "NOVAL") return String(red);
    if (GetParameterValue("GREEN", UpperCase) == "NOVAL") return String(green);
    if (GetParameterValue("BLUE", UpperCase) == "NOVAL") return String(blue);
    if (GetParameterValue("CLEAR", UpperCase) == "NOVAL") return String(clear);
    LastError("No valid parameter found");
    return "-";                                // No valid parameter provided
  }

  if (command == "GETVALUES") return  String(red) + "," + String(green) + "," + String(blue) + "," + String(clear);

  if (command == "INITIALIZE")
  {
    if (Color_init()) return ""; 
    else 
    {
      LastError("Color Unit not found");
      return "-";
    }
  }
  
  if (command == "CONFIGURE") 
  {
    String param_val;
    param_val = GetParameterValue("GAIN", UpperCase);
    bool result_gain = Color_configure_Gain(param_val);

    param_val = GetParameterValue("INTEGRATIONTIME", UpperCase);
    bool result_IntegrationTime = Color_configure_IntegrationTime(param_val);
    if ((result_gain) && (result_IntegrationTime)) return "";
  }  
  
  LastError("No valid command found");
  return "-";             // No valid command found
}
// ----------------------------------------------------------------------------------------------
