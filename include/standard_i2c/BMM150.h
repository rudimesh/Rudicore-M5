#pragma once

/* --- BMM150 THREE-AXIS GEOMAGNETIC SENSOR ---------------------------------
 * Command strings:
 * >BMM150.Calibrate()                     // Flip and rotate for calibration
 * >BMM150.GetValue(x|y|z|Angle)         // Return magnetometer x,y,z or Angle
 */

// Beep is implemented in virtual_units/Speaker.h; forward declare here
void Beep();

struct bmm150_dev dev;
bmm150_mag_data mag_offset;
bmm150_mag_data mag_max;
bmm150_mag_data mag_min;

#define TimeToCalibrate 10000                 // 10 seconds flip and rotate device to calibrate

int8_t bmm150_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *read_data, uint16_t len)
{
  if (M5.I2C.readBytes(dev_id, reg_addr, len, read_data))
  {
    return BMM150_OK;
  }
  else
  {
    return BMM150_E_DEV_NOT_FOUND;
  }
}

int8_t bmm150_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *read_data, uint16_t len)
{
  if (M5.I2C.writeBytes(dev_id, reg_addr, read_data, len))
  {
    return BMM150_OK;
  }
  else
  {
    return BMM150_E_DEV_NOT_FOUND;
  }
}

int8_t bmm150_initialization()
{
  int8_t rslt = BMM150_OK;

  dev.dev_id = 0x10;                 // Device address setting
  dev.intf = BMM150_I2C_INTF;        // SPI or I2C interface setup
  dev.read = bmm150_i2c_read;        // Read bus pointer
  dev.write = bmm150_i2c_write;      // Write bus pointer
  dev.delay_ms = delay;

  // Set the initial max range
  mag_max.x = -2000;
  mag_max.y = -2000;
  mag_max.z = -2000;

  // Set the initial min range
  mag_min.x = 2000;
  mag_min.y = 2000;
  mag_min.z = 2000;

  rslt = bmm150_init(&dev);              // Memory chip ID
  dev.settings.pwr_mode = BMM150_NORMAL_MODE;
  rslt |= bmm150_set_op_mode(&dev);      // Set sensor power mode
  dev.settings.preset_mode = BMM150_PRESETMODE_ENHANCED;
  rslt |= bmm150_set_presetmode(&dev);   // Set the preset mode
  return rslt;
}

extern Preferences prefs;

void bmm150_offset_save()
{
  prefs.begin("bmm150", false);
  prefs.putBytes("offset", (uint8_t *)&mag_offset, sizeof(bmm150_mag_data));
  prefs.end();
}

void bmm150_offset_load()
{
  // Open read-write to auto-create namespace after flash erase
  if (prefs.begin("bmm150", false))
  {
    if (prefs.isKey("offset"))
    {
      prefs.getBytes("offset", (uint8_t *)&mag_offset, sizeof(bmm150_mag_data));
    }
    else
    {
      // Default to zero offsets and initialize to avoid future NOT_FOUND
      mag_offset.x = 0;
      mag_offset.y = 0;
      mag_offset.z = 0;
      prefs.putBytes("offset", (uint8_t *)&mag_offset, sizeof(bmm150_mag_data));
    }
    prefs.end();
  }
}

void bmm150_calibrate(uint32_t calibrate_time)
{
  uint32_t calibrate_timeout = millis() + calibrate_time;

  int dly = 100;
  int delta = calibrate_time / dly / 100;
  int progress = 0;

  while (calibrate_timeout > millis())
  {
    bmm150_read_mag_data(&dev); // read magnetometer data from registers
    if (dev.data.x)
    {
      mag_min.x = (dev.data.x < mag_min.x) ? dev.data.x : mag_min.x;
      mag_max.x = (dev.data.x > mag_max.x) ? dev.data.x : mag_max.x;
    }
    if (dev.data.y)
    {
      mag_max.y = (dev.data.y > mag_max.y) ? dev.data.y : mag_max.y;
      mag_min.y = (dev.data.y < mag_min.y) ? dev.data.y : mag_min.y;
    }
    if (dev.data.z)
    {
      mag_min.z = (dev.data.z < mag_min.z) ? dev.data.z : mag_min.z;
      mag_max.z = (dev.data.z > mag_max.z) ? dev.data.z : mag_max.z;
    }
    progress = progress + delta;
    M5.Lcd.progressBar(20, 160, 280, 40, progress);
    delay(dly);
  }

  mag_offset.x = (mag_max.x + mag_min.x) / 2;
  mag_offset.y = (mag_max.y + mag_min.y) / 2;
  mag_offset.z = (mag_max.z + mag_min.z) / 2;
  bmm150_offset_save();
}

// display_sensor_static_text_redraw is defined in main.cpp
extern bool display_sensor_static_text_redraw;

String BMM150()
{
  if (bmm150_initialization() != BMM150_OK)
  {
    LastError("BMM150: Error initializing");
    return "-";
  }

  bmm150_offset_load();

  if (command == "GETVALUE")
  {
    bmm150_read_mag_data(&dev);
    float head_dir = atan2(dev.data.x - mag_offset.x, dev.data.y - mag_offset.y) * 180.0 / M_PI;

    if (GetParameterValue("X") == "NOVAL") return String(dev.data.x);
    if (GetParameterValue("Y") == "NOVAL") return String(dev.data.y);
    if (GetParameterValue("Z") == "NOVAL") return String(dev.data.z);
    if (GetParameterValue("ANGLE") == "NOVAL") return String(head_dir);
    LastError("No valid parameter found");
    return "-"; // No valid parameter value found
  }

  if (command == "CALIBRATE")
  {
    ez.screen.clear();
    M5.Lcd.setFont(&FreeSans9pt7b);
    M5.Lcd.setTextDatum(BC_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("Flip and Rotate for 10sec...", 160, 120);

    bmm150_calibrate(TimeToCalibrate);
    Beep();                                   // Generate a 'Beep' when ready
    display_sensor_static_text_redraw = true; // complete redraw of the main window 'Sensor info...'
    return "";
  }

  LastError("No valid command found");
  return "-";
}
