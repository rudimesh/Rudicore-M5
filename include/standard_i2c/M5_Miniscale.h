#pragma once
/* --- M5_Miniscale() i2c (0x26) Mini Scales Unit (loadcell) ---------------------------------------------------------------------------

 * >Scales.GetValue( Weight | GapValue | RawADC)
 * >Scales.GetValues() returns Weight, GapValue, and RawADC in a comma-separated list
 * >Scales.Initialize()
 * >Scales.SetValue(Gap = float) 
 * >Scales.Tare()
 * 
 * https://github.com/m5stack/M5Unit-Miniscale
 */
 
UNIT_SCALES scales;

#define M5_Miniscale_address 0x26

// PaHUB port used for this device
extern int M5_Miniscale_port;

bool M5_Miniscale_connected()
{
  // check that unit is connected
  if (!scales.begin(&Wire, 21, 22, M5_Miniscale_address)) return false;
  return true;
}

bool M5_Miniscale_init()
{
  
  if (M5_Miniscale_connected()) M5_Miniscale_active = true; else M5_Miniscale_active = false;
  return M5_Miniscale_active;
}


String M5_Miniscale()
{
  if (PaHUB_active && M5_Miniscale_port >= 0) selectPaHUBChannel(M5_Miniscale_port);
  if (!M5_Miniscale_active) M5_Miniscale_init();
  if (!M5_Miniscale_active)
  {
    LastError("Device Miniscale not found");
    return "-";
  }
  
  if (command == "GETVALUE")
  {
    if (GetParameterValue("WEIGHT", UpperCase) == "NOVAL") return String(scales.getWeight(),2);
    if (GetParameterValue("GAPVALUE", UpperCase) == "NOVAL") return String(scales.getGapValue(),2);
    if (GetParameterValue("RAWADC", UpperCase) == "NOVAL") return String(scales.getRawADC());
    LastError("No parameter found");
    return "-";
  }

  if (command == "GETVALUES")       // return all values in a comma separated list
  {
    int   r=scales.getRawADC();
    float w=scales.getWeight();
    float g=scales.getGapValue();

    String scalevalues = String(w,2) + ',' + String(g,2) + ',' + String(r);
    return scalevalues;
  }

  if (command == "SETVALUE")
  {
    param_val = GetParameterValue("GAP", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      scales.setGapValue(atof(param_val.c_str()));
      return "";
    }
  }

  if (command == "TARE") 
  {
    scales.setOffset();
    return "";
  }


  if (command == "INITIALIZE") 
  {
    if (M5_Miniscale_init()) return ""; else return "-";
  }
  
  LastError("No valid command found");
  return "-";
}


// --------------------------------------------------------------------------------------------------------



