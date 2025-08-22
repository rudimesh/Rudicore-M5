#pragma once
/* --- NCir (MLX90614) ---------------------------------------------------------------------------

 * >NCir.GetTemperature()
 * >NCir.Initialize()
 */

#define MLX90614_default_address 0x5A
int MLX90614_address = MLX90614_default_address;

// PaHUB port used for this device
extern int MLX90614_port;

bool MLX90614_connected()
{
  // Check that the unit is connected
  Wire.beginTransmission(MLX90614_address);
  if ( Wire.endTransmission() != 0) return false;
  return true;
}

bool MLX90614_init()
{
  if (PaHUB_active && MLX90614_port >= 0) selectPaHUBChannel(MLX90614_port);
  Wire.beginTransmission(MLX90614_address); // Send start condition and device address (default 0x5A)
  Wire.write(0x07);                         // Read temperature register (object)
  Wire.endTransmission(false);
  Wire.requestFrom(MLX90614_address, 2);    // Read two bytes
  Wire.read();
  Wire.read();
    
  if (MLX90614_connected()) NCir_active = true; else NCir_active = false;
  return NCir_active;
}

String MLX90614_TEMP()
{
  Wire.beginTransmission(MLX90614_address);
  Wire.write(0x07);
  Wire.endTransmission(false);
  Wire.requestFrom(MLX90614_address, 2);
  uint16_t result = Wire.read();
  result |= Wire.read() << 8;
  float temperature = result * 0.02 - 273.15;
  return String(temperature);           
}



String MLX90614()
{
  if (PaHUB_active && MLX90614_port >= 0) selectPaHUBChannel(MLX90614_port);
  if (!NCir_active) MLX90614_init();
  if (!NCir_active)
  {
    LastError("Device not found");
    return "-";
  }
  
  if (command == "GETTEMPERATURE")  return String(MLX90614_TEMP());
  if (command == "INITIALIZE")
  {
    if (MLX90614_init()) return ""; 
    else 
    {
      LastError("Device not found");
      return "-";
    }
  }
  LastError("No valid command found");
  return "-";
}

// --------------------------------------------------------------------------------------------------------




 
