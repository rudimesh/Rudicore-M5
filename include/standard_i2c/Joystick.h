#pragma once
/* --- Joystick Unit (0x52) ----------------------------------------------------
 * >Joystick.GetValue(X|Y|Button)
 * >Joystick.GetValues()
 * >Joystick.Initialize()
 */

#define JOYSTICK_ADDR 0x52

// PaHUB port used for this device
extern int Joystick_port;

bool Joystick_connected()
{
  return i2c_device_connected(JOYSTICK_ADDR);
}

bool Joystick_init()
{
  if (PaHUB_active && Joystick_port >= 0) selectPaHUBChannel(Joystick_port);
  Joystick_active = Joystick_connected();
  return Joystick_active;
}

String Joystick()
{
  if (PaHUB_active && Joystick_port >= 0) selectPaHUBChannel(Joystick_port);
  if (!Joystick_active) Joystick_init();
  if (!Joystick_active)
  {
    LastError("Joystick not found");
    return "-";
  }

  uint8_t x=0, y=0, b=0;
  Wire.requestFrom(JOYSTICK_ADDR, 3);
  if (Wire.available() == 3) {
    x = Wire.read();
    y = Wire.read();
    b = Wire.read();
  } else {
    LastError("Read error");
    return "-";
  }

  if (command == "GETVALUE") {
    if (GetParameterValue("X", UpperCase) == "NOVAL") return String(x);
    if (GetParameterValue("Y", UpperCase) == "NOVAL") return String(y);
    if (GetParameterValue("BUTTON", UpperCase) == "NOVAL") return String(b);
    LastError("No parameter found");
    return "-";
  }

  if (command == "GETVALUES") {
    return String(x) + "," + String(y) + "," + String(b);
  }

  if (command == "INITIALIZE") {
    if (Joystick_init()) return "";
    LastError("Joystick not found");
    return "-";
  }

  LastError("No valid command found");
  return "-";
}

// -----------------------------------------------------------------------------
