#pragma once
/* --- SEN0322 oxygen I2C module -------------------------------------------------
 * >SEN0322.GetValue(AverageSamples = 1-100)          // Returns oxygen concentration (%). Optionally average N samples (default: 1).
 * >SEN0322.Calibrate(OxygenConcentration = 0-25)     // Calibrates the sensor at a known oxygen concentration.
 */

#define DEF_COLLECT_NUMBER    1             // Number of samples to average (1–100)
#define MAX_COLLECT_NUMBER    100
#define Oxygen_IICAddress ADDRESS_3     // default
#define OXYGEN_CONECTRATION 20.9  // The current concentration of oxygen in the air.
#define OXYGEN_MV           0     // The value marked on the sensor.
unsigned int CollectNumber = DEF_COLLECT_NUMBER;  // Number of samples used for averaging

DFRobot_OxygenSensor oxygen;

bool SEN0322_connected()
{
  // Check that the unit is connected
  Wire.beginTransmission(Oxygen_IICAddress);
  if ( Wire.endTransmission() != 0) return false;
  return true;
}

bool SEN0322_init()
{
  SEN0322_active = false;
  if (SEN0322_connected())
  {
    if (oxygen.begin(Oxygen_IICAddress))
      SEN0322_active = true;
  }
  return SEN0322_active;
}


String SEN0322()
{
  SEN0322_init();
  if (!SEN0322_active)
  {
    LastError("SEN0322 Oxygen not found");
    return "-";
  }

  if (command == "GETVALUE")
  {
    param_val = GetParameterValue("AVERAGESAMPLE", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL")) // New value available; apply only when in range
    {
      unsigned int CollectNumber = param_val.toInt();
      if ((CollectNumber > 0) && (CollectNumber <= MAX_COLLECT_NUMBER))
      {
        float OxyF = oxygen.getOxygenData(CollectNumber);
        String v = String(OxyF, 2);
        return v;
      }
      else
      {
        LastError("No valid parameter value found");
        return "-";
      }
    }
    LastError("No valid parameter found");
    return "-";
  }

  if (command == "CALIBRATE")
  {
    param_val = GetParameterValue("OXYGENCONCENTRATION", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL")) // New value available; apply only when in range
    {
      unsigned int OC = param_val.toFloat();
      if ((OC > 0) && (OC <= 100))
      {
        oxygen.calibrate(OC, OXYGEN_MV);
        return "";
      }
      else
      {
        LastError("No valid parameter value found");
        return "-";
      }
    }
    LastError("No valid parameter found");
    return "-";
  }
  LastError("No valid command found");
  return "-";
}

// -----------------------------------------------------------------------------

