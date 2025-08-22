#pragma once
/* --- Ext Encoder Unit ------------------------------------------------------
 * Prefix: ExtEncoder
 * >ExtEncoder.GetEncoder()
 * >ExtEncoder.GetMeter(String) // default mm
 * >ExtEncoder.GetPerimeter()
 * >ExtEncoder.GetPulsePerRound()
 * >ExtEncoder.SetPerimeter(Perimeter=)
 * >ExtEncoder.SetPulsePerRound(PulsePerRound=)
 * >ExtEncoder.SetZTriggerMode(Mode=0-2)
 * >ExtEncoder.GetTurns()
 * >ExtEncoder.SetTurns(Turns=)
 * >ExtEncoder.Reset()
 * >ExtEncoder.GetFirmware()
 * >ExtEncoder.GetI2CAddress()
 * >ExtEncoder.Initialize()
 */

#define EXT_ENCODER_ADDR 0x59

#define EXT_ENCODER_ENCODER_REG        0x00
#define EXT_ENCODER_METER_REG          0x10
#define EXT_ENCODER_METER_STR_REG      0x20
#define EXT_ENCODER_RESET_REG          0x30
#define EXT_ENCODER_PERIMETER_REG      0x40
#define EXT_ENCODER_PULSE_PER_ROUND_REG 0x50
#define EXT_ENCODER_TURNS_REG          0x60
#define EXT_ENCODER_Z_TRIGGER_MODE_REG 0x70
#define EXT_ENCODER_FW_VERSION_REG     0xFE
#define EXT_ENCODER_I2C_ADDRESS_REG    0xFF

// PaHUB port used for this device
extern int ExtEncoder_port;

bool ExtEncoder_connected()
{
  return i2c_device_connected(EXT_ENCODER_ADDR);
}

bool ExtEncoder_init()
{
  if (PaHUB_active && ExtEncoder_port >= 0) selectPaHUBChannel(ExtEncoder_port);
  ExtEncoder_active = ExtEncoder_connected();
  return ExtEncoder_active;
}

String ExtEncoder_write_uint32(uint8_t reg, uint32_t value)
{
  Wire.beginTransmission(EXT_ENCODER_ADDR);
  Wire.write(reg);
  Wire.write((uint8_t *)&value, 4);
  Wire.endTransmission();
  return "";
}

int32_t ExtEncoder_read_int32(uint8_t reg)
{
  int32_t value = 0;
  Wire.beginTransmission(EXT_ENCODER_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((int)EXT_ENCODER_ADDR, 4);
  if (Wire.available() >= 4)
  {
    for (int i = 0; i < 4; i++)
    {
      ((uint8_t *)&value)[i] = Wire.read();
    }
  }
  return value;
}

uint32_t ExtEncoder_read_uint32(uint8_t reg)
{
  return (uint32_t)ExtEncoder_read_int32(reg);
}

uint8_t ExtEncoder_read_byte(uint8_t reg)
{
  uint8_t value = 0;
  Wire.beginTransmission(EXT_ENCODER_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((int)EXT_ENCODER_ADDR, 1);
  if (Wire.available() >= 1)
  {
    value = Wire.read();
  }
  return value;
}

String ExtEncoder_read_string(uint8_t reg)
{
  char buf[10];
  Wire.beginTransmission(EXT_ENCODER_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((int)EXT_ENCODER_ADDR, 9);
  int i = 0;
  while (Wire.available() && i < 9)
  {
    buf[i++] = Wire.read();
  }
  buf[i] = '\0';
  return String(buf);
}

String ExtEncoder()
{
  if (PaHUB_active && ExtEncoder_port >= 0) selectPaHUBChannel(ExtEncoder_port);
  if (!ExtEncoder_active) ExtEncoder_init();
  if (!ExtEncoder_active)
  {
    LastError("ExtEncoder not found");
    return "-";
  }

  if (command == "GETENCODER")
  {
    int32_t value = ExtEncoder_read_int32(EXT_ENCODER_ENCODER_REG);
    return String(value);
  }

  if (command == "GETMETER")
  {
    if (GetParameterValue("STRING", UpperCase) == "NOVAL")
    {
      return ExtEncoder_read_string(EXT_ENCODER_METER_STR_REG);
    }
    int32_t value = ExtEncoder_read_int32(EXT_ENCODER_METER_REG);
    return String(value);
  }

  if (command == "SETPERIMETER")
  {
    param_val = GetParameterValue("PERIMETER", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      uint32_t val = param_val.toInt();
      return ExtEncoder_write_uint32(EXT_ENCODER_PERIMETER_REG, val);
    }
    LastError("No parameter or value found");
    return "-";
  }

  if (command == "GETPERIMETER")
  {
    uint32_t value = ExtEncoder_read_uint32(EXT_ENCODER_PERIMETER_REG);
    return String(value);
  }

  if (command == "SETPULSEPERROUND")
  {
    param_val = GetParameterValue("PULSEPERROUND", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      uint32_t val = param_val.toInt();
      return ExtEncoder_write_uint32(EXT_ENCODER_PULSE_PER_ROUND_REG, val);
    }
    LastError("No parameter or value found");
    return "-";
  }

  if (command == "GETPULSEPERROUND")
  {
    uint32_t value = ExtEncoder_read_uint32(EXT_ENCODER_PULSE_PER_ROUND_REG);
    return String(value);
  }

  if (command == "SETZTRIGGERMODE")
  {
    param_val = GetParameterValue("MODE", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      uint8_t mode = param_val.toInt();
      if (mode <= 2)
      {
        Wire.beginTransmission(EXT_ENCODER_ADDR);
        Wire.write(EXT_ENCODER_Z_TRIGGER_MODE_REG);
        Wire.write(mode);
        Wire.endTransmission();
        return "";
      }
      LastError("Mode-Value out of range error");
      return "-";
    }
    LastError("No parameter or value found");
    return "-";
  }

  if (command == "RESET")
  {
    Wire.beginTransmission(EXT_ENCODER_ADDR);
    Wire.write(EXT_ENCODER_RESET_REG);
    Wire.write((uint8_t)1);
    Wire.endTransmission();
    return "";
  }

  if (command == "GETTURNS")
  {
    int32_t value = ExtEncoder_read_int32(EXT_ENCODER_TURNS_REG);
    return String(value);
  }

  if (command == "SETTURNS")
  {
    param_val = GetParameterValue("TURNS", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      uint32_t val = param_val.toInt();
      return ExtEncoder_write_uint32(EXT_ENCODER_TURNS_REG, val);
    }
    LastError("No parameter or value found");
    return "-";
  }

  if (command == "GETFIRMWARE")
  {
    uint8_t value = ExtEncoder_read_byte(EXT_ENCODER_FW_VERSION_REG);
    return String(value);
  }

  if (command == "GETI2CADDRESS")
  {
    uint8_t value = ExtEncoder_read_byte(EXT_ENCODER_I2C_ADDRESS_REG);
    return String(value);
  }

  if (command == "INITIALIZE")
  {
    if (ExtEncoder_init()) return ""; else return "-";
  }

  LastError("No valid command found");
  return "-";
}

// -----------------------------------------------------------------------------
