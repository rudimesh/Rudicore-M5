#pragma once
/* --- Faces Encoder Module (0x5E) -------------------------------------------
 * >FacesEncoder.GetValue(Encoder|Button)
 * >FacesEncoder.SetLED(Index=0-11, R=0-255, G=0-255, B=0-255)
 * >FacesEncoder.Initialize()
 */

int FACES_ENCODER_ADDR=0x5E;

// PaHUB port used for this device
extern int FacesEncoder_port;

bool FacesEncoder_connected()
{
  return i2c_device_connected(FACES_ENCODER_ADDR);
}

bool FacesEncoder_init()
{
  if (PaHUB_active && FacesEncoder_port >= 0) selectPaHUBChannel(FacesEncoder_port);
  FacesEncoder_active = FacesEncoder_connected();
  return FacesEncoder_active;
}

String FacesEncoder()
{
  if (PaHUB_active && FacesEncoder_port >= 0) selectPaHUBChannel(FacesEncoder_port);
  if (!FacesEncoder_active) FacesEncoder_init();
  if (!FacesEncoder_active)
  {
    LastError("Faces Encoder not found");
    return "-";
  }

  if (command == "GETVALUE")
  {
    int increment = 0;
    uint8_t button = 0;

    Wire.requestFrom(FACES_ENCODER_ADDR, 3);
    if (Wire.available() >= 2)
    {
      increment = Wire.read();
      button    = Wire.read();
    }

    if (GetParameterValue("ENCODER", UpperCase) == "NOVAL")
    {
      if (increment > 127) increment = -(256 - increment);
      return String(increment);
    }
    if (GetParameterValue("BUTTON", UpperCase) == "NOVAL")
    {
      return String(button == 0 ? 0 : 1);
    }
    LastError("No parameter found");
    return "-";
  }

  if (command == "SETLED")
  {
    int idx = 0, r = 0, g = 0, b = 0;
    String val;
    val = GetParameterValue("INDEX", UpperCase);
    if ((val != "NOPARAM") && (val != "NOVAL")) idx = val.toInt();
    val = GetParameterValue("R", UpperCase);
    if ((val != "NOPARAM") && (val != "NOVAL")) r = val.toInt();
    val = GetParameterValue("G", UpperCase);
    if ((val != "NOPARAM") && (val != "NOVAL")) g = val.toInt();
    val = GetParameterValue("B", UpperCase);
    if ((val != "NOPARAM") && (val != "NOVAL")) b = val.toInt();

    Wire.beginTransmission(FACES_ENCODER_ADDR);
    Wire.write(idx);
    Wire.write(r);
    Wire.write(g);
    Wire.write(b);
    Wire.endTransmission();
    return "";
  }

  if (command == "INITIALIZE")
  {
    if (FacesEncoder_init()) return ""; else return "-";
  }

  LastError("No valid command found");
  return "-";
}

// -----------------------------------------------------------------------------
