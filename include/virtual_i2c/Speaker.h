#pragma once

/* --- Speaker virtual unit (I2C addr D3) -----------------------------------
 * Command strings (prefix: Speaker):
 *   >Speaker.Beep()
 *   >Speaker.Upload(SampleSize=1-1000)        // immediately send bytes
 *   >Speaker.Play(SamplingRate=1-100000, NumberOfTimes=0-1000, Port=25|26|Both)
 *   >Speaker.Stop()
 */

// Simple beep utility, used by other units as well
inline void Beep()
{
  M5.Speaker.setVolume(5);
  M5.Speaker.tone(1000, 100);
  delay(100);
  M5.Speaker.mute();
}

// Wave generator state
#define WG_DACpinDefault 26
static unsigned int WG_DACpin = WG_DACpinDefault;

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
  dacWrite(25, 0);
  dacWrite(26, 0);
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
      if ((WG_DACpin==26) || (WG_DACpin==99)) dacWrite(26, WG_buf[n]);
      if ((WG_DACpin==25) || (WG_DACpin==99)) dacWrite(25, WG_buf[n]);
    }
    r++;
  } while ((reps == 0) || (r < reps));
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

    param_val = GetParameterValue("PORT", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      if (param_val == "25") WG_DACpin = 25;
      if (param_val == "26") WG_DACpin = 26;
      if (param_val == "BOTH") WG_DACpin = 99;
      if ((WG_DACpin !=25) && (WG_DACpin !=26) && (WG_DACpin != 99))
      {
        LastError("Port number incorrect.");
        return "-";
      }
    }

    if (WG_Play(WG_SamplingRate, WG_NumberOfReps)) return " ";
    WG_MuteDAC();
    LastError("Waveform repetitions interrupted by user.");
    return "-";
  }

  LastError("No valid command found");
  return "-";
}
