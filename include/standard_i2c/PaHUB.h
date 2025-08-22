#pragma once
/* --- Pa.HUB (TCA9548A) ------------------------------------------------------------------------

 * >PaHUB.Initialize() (Only if you would cascade them, not recommended, not supported)
 * >PaHUB.SetPort(port = )  ( 0 - 5)   
 * >PaHUB.Disable()
 */
#define PaHub_I2C_ADDRESS  0x70
// Timeout in milliseconds for I2C operations
constexpr uint32_t PAHUB_TIMEOUT_MS = 50;

// Currently selected port (0–5) on the hub; -1 means unknown/none selected
int PaHUBport = -1;
#define PortNrMin 0
#define PortNrMax 6

void LastError(String error); // Forward declarations
String GetParameterValue(String parameter, char UpperCase);

// Write a value to the TCA9548A control register and verify it by reading back
inline bool writePaHUB(uint8_t value, uint32_t timeout = PAHUB_TIMEOUT_MS)
{
  Wire.beginTransmission(PaHub_I2C_ADDRESS);
  Wire.write(value);
  if (Wire.endTransmission() != 0) return false;

  unsigned long start = millis();
  while (Wire.requestFrom((uint8_t)PaHub_I2C_ADDRESS, (uint8_t)1, (uint8_t)true) != 1)
  {
    if (millis() - start > timeout) return false;
    delay(1);
  }
  while (!Wire.available())
  {
    if (millis() - start > timeout) return false;
    delay(1);
  }
  return Wire.read() == value;
}

inline bool selectPaHUBChannel(uint8_t channel)
{
  if (channel >= PortNrMax) return false;
  // Skip re-writing if we're already on the requested channel
  if ((int)channel == PaHUBport) return true;
  if (!writePaHUB(1 << channel)) return false;
  PaHUBport = channel;
  return true;
}


bool PaHUBconnected()
{
  PaHUB_active = false;
  // Check that the hub is connected
  Wire.beginTransmission(PaHub_I2C_ADDRESS);
  if (Wire.endTransmission() != 0) return false;
  PaHUB_active = true;
  return true;
}

String PaHUBdisable()
{
  // Check that the hub is connected
  if (!PaHUBconnected()) return "-";

  // disable HUB
  if (!writePaHUB(0)) return "-";
  // No channel is active after disable; mark as unknown
  PaHUBport = -1;
  return "";
}

bool TCA9548A_init()
{
  if (!PaHUBconnected()) return false;
  // Only initialize; disable hub function
  PaHUBdisable();
  return true;
}

String PaHUB()
{
  if (!PaHUB_active) TCA9548A_init();
  if (!PaHUB_active) 
  {
    LastError("PAHub not found");
    return "-";
  }
  
  if (command == "SETPORT") 
  {
    String param_val;
    param_val = GetParameterValue("PORT", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))   // if no parameter or value? Don't change
    {    
      unsigned int PortVal = param_val.toInt();
      if ((PortVal >= PortNrMin) && (PortVal < PortNrMax))
      {
        if (selectPaHUBChannel(PortVal)) return "";
        LastError("PaHUB returns error");
        return "-";
      }
      LastError("Port value out of range (0-5)");
      return "-";
    }
    LastError("No parameter or value found");
    return "-";
  }

  if (command == "DISABLE") 
  {
    return PaHUBdisable();
  }


  if (command == "INITIALIZE") 
  {
    if (TCA9548A_init()) return "";
    LastError("PAHub not found");
    return "-";
  }

  LastError("No valid command found");
  return "-";             // No valid command found
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static uint8_t PaHUBchannels = 0;      // bit mask of currently enabled channels

inline void beginPaHUB(TwoWire &inWire = Wire)
{
  inWire.begin();
}

inline void writePaHUBRaw(uint8_t data)
{
  Wire.beginTransmission(PaHub_I2C_ADDRESS);
  Wire.write(data);
  Wire.endTransmission(true);
}

inline uint8_t readPaHUBRaw()
{
  Wire.requestFrom((uint8_t)PaHub_I2C_ADDRESS, (uint8_t)1, (uint8_t)true);
  if (!Wire.available()) return 255;
  return Wire.read();
}

inline void openChannel(uint8_t channel)
{
  uint8_t mask = 1 << channel;
  PaHUBchannels |= mask;
  writePaHUBRaw(PaHUBchannels);
  PaHUBport = channel;
}

inline void closeChannel(uint8_t channel)
{
  uint8_t mask = 1 << channel;
  PaHUBchannels &= ~mask;
  writePaHUBRaw(PaHUBchannels);
}

inline void closeAll()
{
  PaHUBchannels = 0x00;
  writePaHUBRaw(PaHUBchannels);
  // No channel active -> unknown
  PaHUBport = -1;
}

inline void openAll()
{
  PaHUBchannels = 0xFF;
  writePaHUBRaw(PaHUBchannels);
}

inline void writeRegister(uint8_t value)
{
  PaHUBchannels = value;
  writePaHUBRaw(PaHUBchannels);
}

inline uint8_t readRegister()
{
  return readPaHUBRaw();
}

// Ensure the hub is on the currently selected channel before talking to
// downstream devices. This helper is safe to call even if the hub is not
// present.
inline void ensurePaHUBPort()
{
  if (PaHUB_active)
  {
    selectPaHUBChannel(PaHUBport);
  }
}
