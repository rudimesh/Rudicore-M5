#pragma once
/* --- ADC ADS1100 ----------------------------------------------------------------------------------

 * >ADC.GetValue()
 * >ADC.GetNTCTemp() for custom NTC mod
 * >ADC.Configure(Gain=,  SamplingRate= )
 * >ADC.Initialize()
 */
ADS1100 ads;
#define default_ADS1100_Gain  GAIN_ONE
#define default_ADS1100_Rate  RATE_8

// PaHUB port used for this device
extern int ADS1100_port;

bool ADS1100_init()
{
  if (PaHUB_active && ADS1100_port >= 0) selectPaHUBChannel(ADS1100_port);
  ADS1100_active = true;
  ads.getAddr_ADS1100(ADS1100_DEFAULT_ADDRESS);   

  // check ADC is connected
  if (!i2c_device_connected(ADS1100_DEFAULT_ADDRESS))
  {
    LastError("ADC Unit not found");
    ADS1100_active = false;
    return false;
  }

  // The ADC gain (PGA), device operating mode and data rate
  // can be changed via the following functions
    ads.setGain(default_ADS1100_Gain);
  // ads.setGain(GAIN_ONE);       // 1x gain(default)
  // ads.setGain(GAIN_TWO);       // 2x gain
  // ads.setGain(GAIN_FOUR);      // 4x gain
  // ads.setGain(GAIN_EIGHT);     // 8x gain

  ads.setMode(MODE_CONTIN);       // Continuous conversion mode (default)
  // ads.setMode(MODE_SINGLE);    // Single-conversion mode
  
    ads.setRate(default_ADS1100_Rate);
  // ads.setRate(RATE_8);         // 8SPS,  16bit resolution (default)
  // ads.setRate(RATE_16);        // 16SPS, 15bit resolution          
  // ads.setRate(RATE_32);        // 32SPS, 14bit resolution 
  // ads.setRate(RATE_1288);      // 128SPS, 12bit resolution (1288: bug in library)

  ads.setOSMode(OSMODE_SINGLE);   // Set to start a single-conversion

  ads.begin();
  ads.Measure_Differential();     // First read may return a stale value; perform a dummy read to flush
  return ADS1100_active;
}



// support for a modded M5Stack ADC Unit connected to a 10K NTC temperature sensor
String ADS1100_temp()
{
  // set gain and rate to use the maximum accuracy
  ads.setGain(GAIN_FOUR);
  ads.setRate(RATE_8);
  ads.Measure_Differential();                 // First read after changing gain may be stale; perform a dummy read to flush

  float voltage=((float)ads.Measure_Differential()/32768.0)*3.3;
  float R1 = 10000;
  float R_shunt=400000; // input impedance ADC
  float R_tot = R1 / (3.3 / voltage - 1.0); // measured resistance shunt and NTC in parallel
  float R2= 1/((1/R_tot)-(1/R_shunt)); // actual thermistor resistance only
  
  float logR2, T, Tc; // Tf
  float c1 = 1.118219133e-03, c2 = 2.365819078e-04, c3 = 0.7269448288e-07;
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15; 
  return String(Tc,1);
}

bool ADS1100_configure_Gain(String param_val)
{
  bool ok = false;
  
  if (param_val == "NOVAL")
  {
    ads.setGain(default_ADS1100_Gain);          // No value: set to default
    return false;                               // No parameter value found
  }
  
  if (param_val == "NOPARAM") return true;     // No valid parameter found, return with no changes

  int val = param_val.toInt();
  switch (val)
  {
    case 1:
      ads.setGain(GAIN_ONE);
      ok = true;
      break;
    case 2:
      ads.setGain(GAIN_TWO);  
      ok = true;
      break;
    case 4:
      ads.setGain(GAIN_FOUR);  
      ok = true;
      break;
    case 8:
      ads.setGain(GAIN_EIGHT);
      ok = true;
      break;
    default:
      ok = false;
      break;
  }
  return ok;                                  // Return true on success; false if the value is invalid
}

bool ADS1100_configure_Rate(String param_val)
{
  bool ok = false;
  if (param_val == "NOVAL")
  {
    ads.setRate(default_ADS1100_Rate);          // No value: set to default
    return false;                               // No parameter value found
  }
  
  if (param_val == "NOPARAM") return true;     // No valid parameter found, return with no changes

  int val = param_val.toInt();
  switch (val)
  {
    case 8:
      ads.setRate(RATE_8);
      ok = true;
      break;
    case 16:
      ads.setRate(RATE_16);  
      ok = true;
      break;
    case 32:
      ads.setRate(RATE_32);  
      ok = true;
      break;
    case 128:
      ads.setRate(RATE_1288);
      ok = true;
      break;
    default:
      ok = false;
      break;
  }
  return ok;                                  // Return true on success; false if the value is invalid
}


// Main ADS1100 function
String ADC_ADS1100()
{
  if (PaHUB_active && ADS1100_port >= 0) selectPaHUBChannel(ADS1100_port);
  // check ADC is connected
  if (!ADS1100_active) ADS1100_init();
  if (!ADS1100_active) 
  {
    return "-";
  }  

  if (command == "GETVALUE") return String(ads.Measure_Differential());
  if (command == "GETNTCTEMP") return String(ADS1100_temp());
  if (command == "CONFIGURE")
  {
    String param_val;
    param_val = GetParameterValue("GAIN", UpperCase);
    bool result_Gain = ADS1100_configure_Gain(param_val);

    param_val = GetParameterValue("SAMPLINGRATE", UpperCase);
    bool result_Rate = ADS1100_configure_Rate(param_val);

    if ((result_Gain) && (result_Rate)) 
    {
      ads.Measure_Differential();        // First read may return a stale value; perform a dummy read to flush
      return "";    
    }
  }
  
  if (command == "INITIALIZE")
  {
    if (ADS1100_init()) return ""; 
    else return "-";
  }
  LastError("No valid command found");
  return "-";                           // No valid command found
}
// --------------------------------------------------------------------------------------------------------
