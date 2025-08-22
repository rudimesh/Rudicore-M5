#pragma once
/* --- VOC/eCO2 SGP30 gas sensor (I2C) ---------------------------------------------------------------------------
* ">SGP30.Initialize()"
* ">SGP30.TVOC()"
* ">SGP30.ECO2()"
* ">SGP30.BASELINE-TVOC()"
* ">SGP30.BASELINE-ECO2()"
* ">SGP30.H2()"
* ">SGP30.ETHANOL()"
 */



Adafruit_SGP30 sgp;

// PaHUB port used for this device
extern int SGP30_port;

bool SGP30_init()
{
  if (PaHUB_active && SGP30_port >= 0) selectPaHUBChannel(SGP30_port);
  if (sgp.begin())
  {
    SGP30_active = true;
    return true;
  }
  SGP30_active = false;
  return false;
}


String SGP30_calibrate()
{
// not implemented yet
  return "";
}

String SGP30_tvoc()
{
  if (!sgp.IAQmeasure()) return "-";
  return String(sgp.TVOC);
}

String SGP30_eco2()
{
  if (!sgp.IAQmeasure()) return "-";
  return String(sgp.eCO2);
}

String SGP30_base_tvoc()
{
  uint16_t TVOC_base, eCO2_base;
  if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) return "-"; //Failed to get baseline readings
  return String(TVOC_base);
}

String SGP30_base_eco2()
{
  uint16_t TVOC_base, eCO2_base;
  if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) return "-"; //Failed to get baseline readings
  return String(eCO2_base);
}

String SGP30_h2()
{
  if (!sgp.IAQmeasureRaw()) return "-";
  return String(sgp.rawH2);
}

String SGP30_ethanol()
{
  if (!sgp.IAQmeasureRaw()) return "-";
  return String(sgp.rawEthanol);
}

/*
 * Warm up the sensor by taking repeated measurements for several minutes.
 * See: https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor?view=all
 */
// Main SGP30 CO2 sensor function
String SGP30()
{
  if (PaHUB_active && SGP30_port >= 0) selectPaHUBChannel(SGP30_port);
  if (!SGP30_active) SGP30_init(); // ensure the sensor is initialized
  
  if (command == "TVOC") return String(SGP30_tvoc());         //>SGP30.TVOC()
  if (command == "ECO2") return String(SGP30_eco2());         //>SGP30.ECO2()
  if (command == "BASELINE-TVOC") return String(SGP30_base_tvoc()); //>SGP30.BASELINE-TVOC()
  if (command == "BASELINE-ECO2") return String(SGP30_base_eco2()); //>SGP30.BASELINE-ECO2()
  if (command == "H2") return String(SGP30_h2());             //>SGP30.H2()
  if (command == "ETHANOL") return String(SGP30_ethanol());   //>SGP30.ETHANOL()

  
  if (command == "INITIALIZE")                                      //>SGP30.INIT()
  {
    if (SGP30_init()) return ""; else return "-";
  }
  LastError("No valid command found");
  return "-";
}



