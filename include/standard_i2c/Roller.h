#pragma once
/* --- Roller module --------------------------------------------------------------------------------
    >Roller.SetMode(Mode= 1-4)         // default mode = 1 (Speed Mode)
    >Roller.SetSpeed(Speed= -120000 - 120000) // default speed = 0
    >Roller.SetPos(Pos= -2147483648 - 2147483647) // default pos = 0
    >Roller.SetOutput(Output= 0-1)       // default output = 0 (disabled), 1 = enabled
    >Roller.GetSpeed()
    >Roller.GetPos()
    >Roller.GetSysStatus()
    >Roller.GetErrorCode()
    >Roller.GetOutputStatus()
*/

#define ROLLER_ADDR 0x64 // default address
#define def_Roller_Mode 1
#define def_Roller_Speed 0
#define def_Roller_Pos 0
#define def_Roller_Output 0
uint8_t Roller_Mode = def_Roller_Mode;
int32_t Roller_Speed = def_Roller_Speed;
int32_t Roller_Pos = def_Roller_Pos;
uint8_t Roller_Output = def_Roller_Output;

// PaHUB port used for this device
extern int Roller_port;

// --- Roller Registers ---
#define I2C_ADDR (0x64)
#define I2C_OUTPUT_REG (0x00)
#define I2C_MODE_REG (0x01)
#define I2C_POS_RANGE_PROTECT_REG (0x0A)
#define I2C_RESET_STALLED_PROTECT_REG (0x0B)
#define I2C_SYS_STATUS_REG (0x0C)
#define I2C_ERROR_CODE_REG (0x0D)
#define I2C_KEY_SWTICH_MODE_REG (0x0E)
#define I2C_STALL_PROTECTION_REG (0x0F)
#define I2C_ID_REG (0x10)
#define I2C_BPS_REG (0x11)
#define I2C_RGB_BRIGHTNESS_REG (0x12)
#define I2C_POS_MAX_CURRENT_REG (0x20)
#define I2C_RGB_REG (0x30)
#define I2C_VIN_REG (0x34)
#define I2C_TEMP_REG (0x38)
#define I2C_DIAL_COUNTER_REG (0x3C)
#define I2C_SPEED_REG (0x40)
#define I2C_SPEED_MAX_CURRENT_REG (0x50)
#define I2C_SPEED_READBACK_REG (0x60)
#define I2C_SPEED_PID_REG (0x70)
#define I2C_POS_REG (0x80)
#define I2C_POS_READBACK_REG (0x90)
#define I2C_POS_PID_REG (0xA0)
#define I2C_CURRENT_REG (0xB0)
#define I2C_CURRENT_READBACK_REG (0xC0)
#define I2C_SAVE_FLASH_REG (0xF0)
#define I2C_FIRMWARE_VERSION_REG (0xFE)
#define START_ANGLE_CAL_REG (0xF1)
#define UPDATE_ANGLE_CAL_REG (0xF2)
#define GET_ANGLE_BUSY_REG (0xF3)


bool Roller_connected()
{
  // check HUB is connected
  return i2c_device_connected(ROLLER_ADDR);
}


String Roller_write_byte(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(ROLLER_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
  return "";
}

String Roller_write_int32(uint8_t reg, int32_t value) {
  Wire.beginTransmission(ROLLER_ADDR);
  Wire.write(reg);
  Wire.write((uint8_t *)&value, 4); // Write 4 bytes of the int32_t
  Wire.endTransmission();
  return "";
}

int32_t Roller_read_int32(uint8_t reg) {
  int32_t value = 0;
  Wire.beginTransmission(ROLLER_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false); // Repeated start

  Wire.requestFrom((int)ROLLER_ADDR, 4);
  if (Wire.available() == 4) {
    for (int i = 0; i < 4; i++) {
      ((uint8_t *)&value)[i] = Wire.read();
    }
  }
  return value;
}

uint8_t Roller_read_byte(uint8_t reg) {
  uint8_t value = 0;
  Wire.beginTransmission(ROLLER_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false); // Repeated start

  Wire.requestFrom((int)ROLLER_ADDR, 1);
  if (Wire.available() == 1) {
    value = Wire.read();
  }
  return value;
}


String Roller()
{
  if (PaHUB_active && Roller_port >= 0) selectPaHUBChannel(Roller_port);
  if (!Roller_connected())
  {
    LastError("No Roller device available");
    return "-";
  }

  if (command == "SETMODE")
  {
    param_val = GetParameterValue("MODE", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))   // if no parameter or value? Don't change
    {
      unsigned int Val = param_val.toInt();
      if ((Val >= 1) && (Val <= 4))
        Roller_Mode = Val;
      else
      {
        LastError("Mode-Value out of range error, keep old value");
      }
    }
    return Roller_write_byte(I2C_MODE_REG, Roller_Mode);
  }

  if (command == "SETSPEED")
  {
    param_val = GetParameterValue("SPEED", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))   // if no parameter or value? Don't change
    {
      int Val = param_val.toInt();
      Roller_Speed = Val; // No range check for speed for now, refer to doc for limits
    }
    return Roller_write_int32(I2C_SPEED_REG, Roller_Speed);
  }

    if (command == "SETPOS")
  {
    param_val = GetParameterValue("POS", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))   // if no parameter or value? Don't change
    {
      int Val = param_val.toInt();
      Roller_Pos = Val; // No range check for pos for now, refer to doc for limits
    }
    return Roller_write_int32(I2C_POS_REG, Roller_Pos);
  }

  if (command == "SETOUTPUT")
  {
    param_val = GetParameterValue("OUTPUT", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))   // if no parameter or value? Don't change
    {
      unsigned int Val = param_val.toInt();
      if ((Val >= 0) && (Val <= 1))
        Roller_Output = Val;
      else
      {
        LastError("Output-Value out of range error, keep old value");
      }
    }
    return Roller_write_byte(I2C_OUTPUT_REG, Roller_Output);
  }


  if (command == "GETSPEED")
  {
      int32_t speed = Roller_read_int32(I2C_SPEED_READBACK_REG);
      return String(speed);
  }

  if (command == "GETPOS")
  {
      int32_t pos = Roller_read_int32(I2C_POS_READBACK_REG);
      return String(pos);
  }

  if (command == "GETSYSSTATUS")
  {
      uint8_t status = Roller_read_byte(I2C_SYS_STATUS_REG);
      return String(status);
  }

  if (command == "GETERRORCODE")
  {
      uint8_t error = Roller_read_byte(I2C_ERROR_CODE_REG);
      return String(error);
  }

  if (command == "GETOUTPUTSTATUS")
  {
      uint8_t output_status = Roller_read_byte(I2C_OUTPUT_REG);
      return String(output_status);
  }


  return ""; 
}
// -------------------------------------------------------------------------------------------------
