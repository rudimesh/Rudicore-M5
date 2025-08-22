#pragma once

/* --- ESP32 GPIO virtual unit (I2C addr D1) -------------------------------
 * Command strings:
 *   >M5GPIO.SetValue(Port=[2|5|16|17|25|26], Value=0|1)
 *   >M5GPIO.GetValue(Port=[2|5|16|17|26])
 *
 * This virtual unit mirrors the ESP32 GPIO controls ,the I2C scanner 
 * exposes it at virtual address D1.
 */

static unsigned int GPIO_Port = 2;
static bool GPIO_Value = true;

static void GPIO_GetParameters()
{
  String pv;
  pv = GetParameterValue("PORT", UpperCase);
  if ((pv != "NOPARAM") && (pv != "NOVAL"))
  {
    unsigned int p = pv.toInt();
    if ((p == 2) || (p == 5) || (p == 16) || (p == 17) || (p == 25) || (p == 26))
      GPIO_Port = p;
    else
      LastError("GPIO# out of range, no changes");
  }

  pv = GetParameterValue("VALUE", UpperCase);
  if ((pv != "NOPARAM") && (pv != "NOVAL"))
  {
    unsigned int v = pv.toInt();
    if ((v == 0) || (v == 1))
      GPIO_Value = v;
    else
      LastError("Value out of range error, no changes");
  }
}

String M5GPIO()
{
  GPIO_GetParameters();

  if (command == "GETVALUE")
  {
    pinMode(GPIO_Port, INPUT_PULLUP);
    return String(digitalRead(GPIO_Port));
  }
  if (command == "SETVALUE")
  {
    pinMode(GPIO_Port, OUTPUT);
    digitalWrite(GPIO_Port, GPIO_Value);
    return "";
  }

  LastError("No valid command found");
  return "-";
}
