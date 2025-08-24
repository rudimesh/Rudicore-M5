#pragma once

#include "driver/dac.h"

/* --- Speaker virtual unit (I2C addr D3) -----------------------------------
 * Command strings (prefix: Speaker):
 *   >Speaker.Beep()
 *   >Speaker.Upload(SampleSize=1-1000)        // immediately send bytes
 *   >Speaker.Play(SamplingRate=1-100000, NumberOfTimes=0-1000)
 *   >Speaker.Stop()
 */

// Simple beep
inline void Beep()
{
  tone(25,1000,100);
}

// Analog wave generator

#define WG_NumberOfRepsDefault  0
static unsigned int WG_NumberOfReps = WG_NumberOfRepsDefault;
static unsigned int WG_NumberOfRepsMax = 100000;
#define WG_SampleSizeMax 1000
static unsigned int WG_SampleSize = WG_SampleSizeMax;
#define WG_SamplingRateDefault 10000
static unsigned int WG_SamplingRate = WG_SamplingRateDefault;
static unsigned int WG_SamplingRateMax = 100000;
static unsigned int WG_Pointer = 0;
static byte WG_buf[WG_SampleSizeMax];

static inline void WG_MuteDAC()
{
  dac_output_disable(DAC_CHANNEL_1); // disable DAC on GPIO25
  pinMode(25, INPUT_PULLDOWN);       // make it a quiet digital pin
}

static bool WG_UploadByte(unsigned int sample_size)
{
  WG_Pointer = sample_size;
  if (BT_Active) BT.readBytes(WG_buf, sample_size);
  else Serial.readBytes(WG_buf, sample_size);
  return true;
}

static bool WG_Play(int rate, int reps)
{
  FlushInput();
  uint32_t start = micros();
  uint32_t ttw = 1000000UL / rate;
  int r = 0;
  do
  {
    if ((Serial.available()) || (BT.available())) return false;
    for (int n = 0; n < (int)WG_Pointer; n++)
    {
      while (start + ttw > micros());
      start = micros();
      dacWrite(25, WG_buf[n]);
    }
    r++;
  } while ((reps == 0) || (r < reps)); // infinite reps if zero 
  return true;
}

String Speaker()
{
  if (command == "BEEP") { Beep(); return ""; }

  if (command == "UPLOAD")
  {
    param_val = GetParameterValue("SAMPLESIZE", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      WG_SampleSize = param_val.toInt();
      if ((WG_SampleSize > WG_SampleSizeMax) || (WG_SampleSize <= 0))
      {
        LastError("SampleSize out of range.");
        return "-";
      }
      if (WG_UploadByte(WG_SampleSize)) return " ";
    }
  }

  if (command == "STOP")
  {
    WG_MuteDAC();
    return " ";
  }

  if (command == "PLAY")
  {
    param_val = GetParameterValue("SAMPLINGRATE", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      WG_SamplingRate = param_val.toInt();
      if ((WG_SamplingRate > WG_SamplingRateMax) || (WG_SamplingRate <= 0))
      {
        LastError("WG_SamplingRate out of range.");
        return "-";
      }
    }

    param_val = GetParameterValue("NUMBEROFTIMES", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      WG_NumberOfReps = param_val.toInt();
      if ((WG_NumberOfReps > WG_NumberOfRepsMax) || (WG_NumberOfReps < 0))
      {
        LastError("WG_NumberOfReps out of range.");
        return "-";
      }
    }

    if (WG_Play(WG_SamplingRate, WG_NumberOfReps)) {
    WG_MuteDAC();
    return " ";
    }

    WG_MuteDAC();
    LastError("Waveform repetitions interrupted by user.");
    return "-";
  }

  LastError("No valid command found");
  return "-";
}
