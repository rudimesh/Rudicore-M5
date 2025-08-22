#pragma once
/* --- LTR390 i2c 0x53 -----------------------------------------------------------
 * Prefix: UV
 * >UV.Initialize()
 * >UV.GetALS()       // set gain to 3, 18‑bit; returns ambient light in Lux (1 decimal)
 * >UV.GetUVS()       // set gain to 18, 20‑bit; returns UV index
 *
 * NOTE: https://github.com/levkovigor/LTR390
 */

#include <LTR390.h>

byte LTR390_i2c_address = 0x53;
LTR390 ltr390(LTR390_i2c_address);

// PaHUB port used for this device
extern int UV_port;

bool UV_connected()
{
  // check unit is connected
  Wire.beginTransmission(LTR390_i2c_address);
  if ( Wire.endTransmission() != 0) return false;
  return true;
}

bool UV_init()
{
  if (PaHUB_active && UV_port >= 0) selectPaHUBChannel(UV_port);
  UV_active = UV_connected();
  if (UV_active)
  {
    ltr390.init();
  }
  return UV_active;
}

String UV()
{
  if (PaHUB_active && UV_port >= 0) selectPaHUBChannel(UV_port);
  String val;
  int v;
  
  if (!UV_active) UV_init();
  if (!UV_active)
  {
    LastError("LTR390device not found");
    return "-";
  }

  // for ambient light value in Lux, set the Gain to 3 and resolution to 18
  if (command == "GETALS")  
  {
    ltr390.setGain(LTR390_GAIN_3);                   //Recommended for Lux - x3
    ltr390.setResolution(LTR390_RESOLUTION_18BIT);   //Recommended for Lux - 18-bit   
    ltr390.setMode(LTR390_MODE_ALS);
    while (!ltr390.newDataAvailable()) delay(10);
    val = String(ltr390.getLux(),1);
    return val;
  }

  // for a result corresponding to the UV-index, set the Gain to 18 and resolution to 20
  if (command == "GETUVS")  
  {
    ltr390.setGain(LTR390_GAIN_18);                 //Recommended for UV-index - x18
    ltr390.setResolution(LTR390_RESOLUTION_20BIT);  //Recommended for UV-index - 20-bit   
    ltr390.setMode(LTR390_MODE_UVS);
    while (!ltr390.newDataAvailable()) delay(10);
    val = String(ltr390.getUVI());
    return val;
  }

  if (command == "INITIALIZE")  
  {
    if (!UV_init())
    {
      LastError("LTR390 not found");
      return "-";
    }
    return " ";
  }
  LastError("No valid command found");
  return "-";
}
