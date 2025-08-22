#pragma once
/* --- UltraSonic I2C (0x57) 30–150 cm (±1 mm), RCWL-9600 chipset -------------------------------------

 * >UltraSonic.GetValue(Distance)
 */

#define UltraSonic_address 0x57

// PaHUB port used for this device
extern int UltraSonic_port;

bool UltraSonic_connected()
{
  // check that unit is connected
  Wire.beginTransmission(UltraSonic_address);
  if ( Wire.endTransmission() != 0) return false;
  return true;
}

bool UltraSonic_init()
{
  if (PaHUB_active && UltraSonic_port >= 0) selectPaHUBChannel(UltraSonic_port);
  if (UltraSonic_connected()) UltraSonic_active = true; else UltraSonic_active = false;
  return UltraSonic_active;
}

// Return raw distance in millimeters
int UltraSonic_distance()
{
  uint32_t data;
  Wire.beginTransmission(0x57);
  Wire.write(0x01);
  Wire.endTransmission();
  delay(120);
  Wire.requestFrom(0x57,3);
  data  = Wire.read();data <<= 8;
  data |= Wire.read();data <<= 8;
  data |= Wire.read();
  data = data / 1000;  
  // check if out of range
  if (data < 20) data = 0;
  if (data > 1500) data = 1500;
  return data;
}

String UltraSonic()     // UltraSonic unit handler
{
  if (PaHUB_active && UltraSonic_port >= 0) selectPaHUBChannel(UltraSonic_port);
  if (!UltraSonic_active) UltraSonic_init();
  if (!UltraSonic_active)
  {
    LastError("Device not found");
    return "-";
  }
  
   if (command == "GETVALUE")
  {
    if (GetParameterValue("DISTANCE", UpperCase) == "NOVAL") return String(UltraSonic_distance());
  }
  
  LastError("No valid command found");
  return "-";
}

// --------------------------------------------------------------------------------------------------------
