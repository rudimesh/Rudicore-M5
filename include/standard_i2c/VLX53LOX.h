#pragma once
/* --- TOF (VLX53LOX) ----------------------------------------------------------------------------------

 * >TOF.GetDistance()
 * >TOF.AcquireToBuffer(MeasuringInterval = , SamplingRate =)
 * >TOF.Configure(Sensitivity = LongRange|HighAccuracy|Default, MeasuringInterval = SamplingRate = , IncludeTimestamps=true|false)
 * >TOF.Initialize()   only when behind active HUB
 * >TOF.GetConfiguration()                
 * >TOF.GetBuffer[IncludeTimeStamps=true|false)
 * 
 * Note: TOF and COLOR share the same I2C address
 */
#define VL53L0X_address 0x29

// PaHUB port used for this device
extern int VLX53LOX_port;

#define tof_buffer_size 550
unsigned int tof_buf[tof_buffer_size];
unsigned int tof_timestamp[tof_buffer_size];
uint16_t VLX53LOX_number_of_readings;
bool VLX53LOX_TimeStamp = false;                          // if true, store a timestamp in the next cell after every tof distance datapoint
unsigned long VLX53LOX_MeasuringIntervalMax = 100000;     // max 100 seconds 
unsigned long VLX53LOX_MeasuringIntervalDefault = 1000;   // no parameters: buffer 1 sec.
unsigned long VLX53LOX_MeasuringInterval = VLX53LOX_MeasuringIntervalDefault;
unsigned int VLX53LOX_SamplingRateMax = 50;
unsigned int VLX53LOX_SamplingRateDefault = 33;
unsigned int VLX53LOX_SamplingRate = VLX53LOX_SamplingRateDefault;
bool VLX53LOX_IncludeTimestamps = false;
String VLX53LOX_Sensitivity = "Default";


Adafruit_VL53L0X lox = Adafruit_VL53L0X();

uint32_t VL53L0X_Read_Data(unsigned long MeasureTime, unsigned int SamplingRate)
{
  unsigned int buffer_index = 0;
  unsigned long GoalTime = millis() + MeasureTime;            // calculate timeout
  unsigned int SampleTime = 1000/SamplingRate;                // calculate time interval between samples
  unsigned int GoalSampleTime = millis() + SampleTime;
  unsigned long start_time = millis();
  do
  {
    tof_timestamp[buffer_index] = millis() - start_time;    // store time since start of the burst.
    
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);                         // pass in 'true' to get debug data printout
    tof_buf[buffer_index++] = measure.RangeMilliMeter;          // store distance

    while (millis() < GoalSampleTime);                        // wait before reading the next sample
    GoalSampleTime = millis() + SampleTime;

  } while ((millis() < GoalTime) && (buffer_index < tof_buffer_size));   // prevent buffer overflow
  return buffer_index;
}

// Initialize
bool VLX53LOX_init()
{

  // check that device is connected
  lox.begin();
  if (!i2c_device_connected(VL53L0X_address))
  {
    LastError("TOF Unit not found");
    TOF_active = false;
  }
  else 
  {
    TOF_active = true;
    VLX53LOX_Sensitivity = "Default";
    lox.configSensor(lox.VL53L0X_SENSE_DEFAULT);
  }

  return TOF_active;
}

/*
 * Output format is always a string. If timestamps are included the format is "timestamp, value".
*/
// GetBuffer
bool GetBuffer(bool IncludeTimestamps, unsigned int NumberOfSamples)
{
  for (int i = 0; i < NumberOfSamples; i++)
  {
    if (IncludeTimestamps)
      print2serial(String(tof_timestamp[i]) + ", " + String(tof_buf[i]));
    else
      print2serial(String(tof_buf[i]));
  }
  print2serial("EOF");
  return true;
}

// Main function
String VLX53LOX()
{

  if (PaHUB_active && VLX53LOX_port >= 0) selectPaHUBChannel(VLX53LOX_port);
  // check TOF is connected
  if (!TOF_active) VLX53LOX_init();
  if (!TOF_active) return "-";  
  
  if (command == "INITIALIZE")
  {
    if (VLX53LOX_init()) return ""; else return "-";
  } 

  if (command == "GETDISTANCE")
  {
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);                      
    if (measure.RangeStatus != 4)                          
    {
      return String(measure.RangeMilliMeter);               
    }
    else 
    {
      LastError("TOF out of range");
      return "-";
    }
  }

  if (command == "GETCONFIGURATION")
  {
    print2serial("MeasuringInterval = " + String(VLX53LOX_MeasuringInterval));
    print2serial("SamplingRate = " + String(VLX53LOX_SamplingRate));
    print2serial("IncludeTimeStamps =  " + String(VLX53LOX_IncludeTimestamps));
    print2serial("Sensitivity =  " + String(VLX53LOX_Sensitivity));
    return "";
  }


  if (command == "CONFIGURE")
  {
    String param_val;

    param_val = GetParameterValue("SENSITIVITY", UpperCase);
      if (param_val == "HIGHACCURACY")
      {
        VLX53LOX_Sensitivity = "HighAccuracy";
        lox.configSensor(lox.VL53L0X_SENSE_HIGH_ACCURACY);
      }
      
      if (param_val == "LONGRANGE")
      {
        VLX53LOX_Sensitivity = "LongRange";
        lox.configSensor(lox.VL53L0X_SENSE_LONG_RANGE);
      }
      
      if (param_val == "DEFAULT")
      {
        VLX53LOX_Sensitivity = "Default";
        lox.configSensor(lox.VL53L0X_SENSE_DEFAULT);
      }
    
    param_val = GetParameterValue("MEASURINGINTERVAL", UpperCase);
    VLX53LOX_MeasuringInterval = param_val.toInt();
    if ((VLX53LOX_MeasuringInterval >= VLX53LOX_MeasuringIntervalMax) || (VLX53LOX_MeasuringInterval <= 0)) 
    {
      LastError("MeasuringInterval out of range, set to default");
      VLX53LOX_MeasuringInterval = VLX53LOX_MeasuringIntervalDefault;
    }

    param_val = GetParameterValue("SAMPLINGRATE", UpperCase);
    VLX53LOX_SamplingRate = param_val.toInt();
    if ((VLX53LOX_SamplingRate >= VLX53LOX_SamplingRateMax) || (VLX53LOX_SamplingRate <= 0)) 
    {
      LastError("SamplingRate out of range, set to default");
      VLX53LOX_SamplingRate = VLX53LOX_SamplingRateDefault;
    }

    param_val = GetParameterValue("INCLUDETIMESTAMPS", UpperCase);
    if ((param_val == "TRUE") || (param_val == "1")) VLX53LOX_IncludeTimestamps = true;
    else VLX53LOX_IncludeTimestamps = false;

    return "";
  }


  if (command == "ACQUIRETOBUFFER")
  {
    // keep previous settings if no parameter or parameter value is available
    param_val = GetParameterValue("MEASURINGINTERVAL", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      VLX53LOX_MeasuringInterval = param_val.toInt();
      if ((VLX53LOX_MeasuringInterval >= VLX53LOX_MeasuringIntervalMax) || (VLX53LOX_MeasuringInterval <= 0)) 
      {
        LastError("MeasuringInterval out of range, set to default");
        VLX53LOX_MeasuringInterval = VLX53LOX_MeasuringIntervalDefault;
      }
    }

    // keep previous settings if no parameter or parameter-value is available. Set to default if param-val is out of range
    param_val = GetParameterValue("SAMPLINGRATE", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      VLX53LOX_SamplingRate = param_val.toInt();
      if ((VLX53LOX_SamplingRate >= VLX53LOX_SamplingRateMax) || (VLX53LOX_SamplingRate <= 0)) 
      {
        LastError("SamplingRate out of range, set to default");
        VLX53LOX_SamplingRate = VLX53LOX_SamplingRateDefault;
      }
    }
    
    VLX53LOX_number_of_readings = VL53L0X_Read_Data(VLX53LOX_MeasuringInterval, VLX53LOX_SamplingRate);   // read tof and store in buffer
    return String(VLX53LOX_number_of_readings);
  }

  if (command == "GETBUFFER")
  { 
    param_val = GetParameterValue("INCLUDETIMESTAMPS", UpperCase);
    if ((param_val == "TRUE") || (param_val == "1")) VLX53LOX_IncludeTimestamps = true;
    else VLX53LOX_IncludeTimestamps = false;

    if (GetBuffer(VLX53LOX_IncludeTimestamps, VLX53LOX_number_of_readings)) return "";
    else 
    {
      LastError("Error sending buffer");
      return "-";
    }
  }
  LastError("No valid command found");
  return "-";
}
  
// --------------------------------------------------------------------------------------------------------
