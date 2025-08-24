#pragma once

/* --- System virtual unit (I2C addr D4) ------------------------------------
 * Prefix: SYSTEM
 * Commands:
 *   >SYSTEM.Restart()
 *   >SYSTEM.BTName()
 *   >SYSTEM.FriendlyName()
 *   >SYSTEM.BTAddress()
 *   >SYSTEM.Echo(Text=)
 *   >SYSTEM.GetConnectionType()
 *   >SYSTEM.GetBatteryLevel()
 *   >SYSTEM.GetChargingStatus()
 *   >SYSTEM.DisplayOn()
 *   >SYSTEM.DisplayOff()
 *   >SYSTEM.ScanI2C()
 *   >SYSTEM.LastError(Clear)
 *   >SYSTEM.Ports()                     // list remembered I2C ports
 */

static inline void System_HardReset()
{
  ESP.restart();
}

String System()
{
  // Restart
  if (command == "RESTART")
  {
    delay(2000);
    System_HardReset();
    return "";
  }

  // Connection info and echo
  if (command == "BTNAME")         return BT_Name;
  if (command == "FRIENDLYNAME")   return Friendly_Name;
  if (command == "BTADDRESS")      return BT_Address;
  if (command == "ECHO")
  {
    param_val = GetParameterValue("TEXT");
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))  return param_val;
    return "-";
  }
  if (command == "GETCONNECTIONTYPE")
  {
    if (BT_Active) return "Bluetooth"; else return "USB-Serial";
  }

  // Power
  if (command == "GETBATTERYLEVEL")   return String(M5.Power.getBatteryLevel());
  if (command == "GETCHARGINGSTATUS") return M5.Power.isCharging() ? String("Charging") : String("Not Charging");

  // Display
  if (command == "DISPLAYOFF")
  {
    display_active = false;
    M5.Lcd.sleep();
    M5.Lcd.setBrightness(0);
    return "";
  }
  if (command == "DISPLAYON")
  {
    M5.Lcd.wakeup();
    M5.Lcd.setBrightness(200);
    display_active = true;
    return "";
  }

  // Test / diagnostics
  if (command == "SCANI2C")
  {
    String i2c_addresses = i2c_scanner();
    return i2c_addresses;
  }
  if (command == "LASTERROR")
  {
    if (GetParameterValue("CLEAR", UpperCase) == "NOVAL")
    {
      ErrorText = "None";
      Last_Error = "None";
    }
    return Last_Error;
  }
  if (command == "PORTS")
  {
    return port_list();
  }

  LastError("No valid command found");
  return "-";
}

// Persistent settings (friendly name offset and device type)
// Define global Preferences instance and helpers here so they are
// available to main and other units.
Preferences prefs;

inline int FriendlyNameOffsetLoad()
{
  int val = 0;
  // Open read-write to auto-create namespace after flash erase
  if (prefs.begin("friendly", false))
  {
    if (prefs.isKey("offset"))
    {
      val = prefs.getInt("offset");
    }
    else
    {
      val = 0; // default
      prefs.putInt("offset", val); // initialize to avoid future NOT_FOUND
    }
    prefs.end();
  }
  return val;
}

inline void FriendlyNameOffsetSave(int val)
{
  prefs.begin("friendly", false);
  prefs.putInt("offset", val);
  prefs.end();
}

inline String DeviceTypeLoad()
{
  String val = "Gray";
  // Open read-write to auto-create namespace after flash erase
  if (prefs.begin("friendly", false))
  {
    if (prefs.isKey("device_type"))
    {
      val = prefs.getString("device_type");
    }
    else
    {
      val = "Gray"; // default
      prefs.putString("device_type", val); // initialize to avoid future NOT_FOUND
    }
    prefs.end();
  }
  return val;
}

inline void DeviceTypeSave(const String &val)
{
  prefs.begin("friendly", false);
  prefs.putString("device_type", val);
  prefs.end();
}

// -----------------------------------------------------------------------------
// Internal Virtual I2C mask persistence
// Bit mask to control visibility of virtual I2C devices in ScanI2C()
// bit0: D0 (DAQ), bit1: D1 (GPIO), bit2: D2 (ESP32ADC), bit3: D3 (Speaker)
// Note: System (D4) is always visible and not maskable.

inline uint8_t VirtualI2CMaskLoad()
{
  uint8_t mask = 0; // default: show all virtual devices
  // Open read-write to auto-create namespace after flash erase
  if (prefs.begin("i2cmask", false))
  {
    if (prefs.isKey("mask"))
    {
      mask = prefs.getUChar("mask");
    }
    else
    {
      mask = 0;
      prefs.putUChar("mask", mask); // initialize to avoid future NOT_FOUND
    }
    prefs.end();
  }
  return mask;
}

inline void VirtualI2CMaskSave(uint8_t mask)
{
  prefs.begin("i2cmask", false);
  prefs.putUChar("mask", mask);
  prefs.end();
}
