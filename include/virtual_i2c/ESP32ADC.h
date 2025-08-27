#pragma once

/* --- ESP32 ADC (virtual I2C addr D2) ---------------------------------------
 * Command strings:
 *  >ESP32ADC.ConfigureBufferedRead(Port=35|36|Both, SamplingRate=, SampleSize=, TriggerEvent=IGNORE|LOW|HIGH|CHANGE|RISING|FALLING)
 *  >ESP32ADC.GetValue(Port=35|36|Both)
 *  >ESP32ADC.GetEnvelope(Duration=1000, Port=35|36|Both)
 *  >ESP32ADC.AcquireToBuffer()
 *  >ESP32ADC.GetBuffer(Format=Byte|String)
 *  >ESP32ADC.GetAvailableSampleCount()
 *  >ESP32ADC.GetStream()
 *  >ESP32ADC.SetAttenuation(Factor=0dB|2.5dB|6dB|11dB)
 *  >ESP32ADC.Initialize()
 */

#define ESP_ADC1Port 35
#define ESP_ADC2Port 36
#define ESP_TriggerPort 5

static bool ESP_ADC1 = true;
static bool ESP_ADC2 = false;
static bool ESP_Both = false;
static bool ESP_MarkChannelTwo = false;

// Max allowed duration for envelope measurement (ms)
#define ESP_EnvelopeMeasurementDurationMax  100000

#define ESP_SamplingRateDefault 1000
static unsigned long ESP_SamplingRate = ESP_SamplingRateDefault;
#define ESP_SamplingRateMax 100000

#define ESP_SampleSizeDefault 1000
static unsigned long ESP_SampleSize = ESP_SampleSizeDefault;
#define ESP_SampleSizeMax data_buf_size

#define ESP_AttenuationDefault  "11DB"
static String ESP_Attenuation = ESP_AttenuationDefault;

static const String ESP_TriggerEvents[]  =  {"IGNORE", "LOW", "HIGH", "CHANGE", "RISING", "FALLING"};
#define Ignore 0
#define Low 1
#define High 2
#define Change 3
#define Rising 4
#define Falling 5
static int ESP_DefaultTriggerEvent = Ignore;
static int ESP_TriggerEvent = ESP_DefaultTriggerEvent;

static bool ESP_Byte = false;
// Number of samples written by the last AcquireToBuffer()
static volatile unsigned long ESP_AvailableSampleCount = 0;

static bool ESP32_ADC_Trigger(int tp, int event)
{
  pinMode(tp, INPUT_PULLUP);
  switch (event)
  {
    case Ignore:  return true;
    case Low:     while (digitalRead(tp) == HIGH) {}; return true;
    case High:    while (digitalRead(tp) == LOW) {};  return true;
    case Change:  { bool initial = digitalRead(tp); while (digitalRead(tp) == initial); return true; }
    case Rising:  while (digitalRead(tp) == HIGH) {}; while (digitalRead(tp) == LOW) {};  return true;
    case Falling: while (digitalRead(tp) == LOW) {};  while (digitalRead(tp) == HIGH) {}; return true;
    default: return false;
  }
}

static void ESP32_GetStream()
{
  int ADC_GPIO = ESP_ADC1Port;
  if (ESP_ADC2) ADC_GPIO = ESP_ADC2Port;
  unsigned long time_target;
  uint32_t sample_period_micro = 1000000 / ESP_SamplingRate;
  uint16_t ADC_val, ADC_val_2;
  ESP32_ADC_Trigger(ESP_TriggerPort, ESP_TriggerEvent);

  if (ESP_Both)
  {
    do
    {
      time_target = micros() + sample_period_micro;
      ADC_val   = analogRead(ESP_ADC1Port);
      ADC_val_2 = analogRead(ESP_ADC2Port);
      bitSet(ADC_val_2, 12); // Mark channel 2
      // Stream always as bytes: Ch1 (MSB,LSB) then Ch2 (MSB,LSB)
      write2serial((ADC_val >> 8) & 0xff);
      write2serial( ADC_val       & 0xff);
      write2serial((ADC_val_2 >> 8) & 0xff);
      write2serial( ADC_val_2       & 0xff);
      while (micros() < time_target) {};
    } while ((!Serial.available()) && (!BT.available()));
  }
  else
  {
    do
    {
      time_target = micros() + sample_period_micro;
      ADC_val = analogRead(ADC_GPIO);
      // Stream always as bytes: MSB, LSB
      write2serial((ADC_val >> 8) & 0xff);
      write2serial( ADC_val       & 0xff);
      while (micros() < time_target) {};
    } while ((!Serial.available()) && (!BT.available()));
  }
}

static void ESP32_GetBuffer(bool as_bytes)
{
  unsigned long samples = ESP_SampleSize;
  if (ESP_Both) samples = samples + samples;
  for (int i = 0; i < samples; i++)
  {
    if (as_bytes)
    {
      write2serial((data_buf[i] >> 8) & 0xff);
      write2serial( data_buf[i]       & 0xff);
    }
    else
    {
      print2serial(String(data_buf[i]));
    }
  }
}

// port_mode: 1=ADC1(35), 2=ADC2(36), 3=Both
static String ESP32_ADC_GetEnvelope(unsigned long duration_ms = 1000, int port_mode = 1)
{
  String ReturnValue;
  int ADC_GPIO = ESP_ADC1Port;
  if (port_mode == 2) ADC_GPIO = ESP_ADC2Port;
  unsigned long ttm = duration_ms;
  ESP32_ADC_Trigger(ESP_TriggerPort, ESP_TriggerEvent);

  if (port_mode == 3)
  {
    pinMode(ESP_ADC1Port, INPUT);
    pinMode(ESP_ADC2Port, INPUT);
    uint16_t Ch1Max = 0, Ch1Min = 4095, Ch1Val = 0;
    uint16_t Ch2Max = 0, Ch2Min = 4095, Ch2Val = 0;
    int goal = millis() + ttm;
    do
    {
      Ch1Val = analogRead(ESP_ADC1Port);
      Ch2Val = analogRead(ESP_ADC2Port);
      Ch1Max = max(Ch1Max, Ch1Val);
      Ch1Min = min(Ch1Min, Ch1Val);
      Ch2Max = max(Ch2Max, Ch2Val);
      Ch2Min = min(Ch2Min, Ch2Val);
    } while (millis() < goal);
    ReturnValue = String(Ch1Min) + "," + String(Ch1Max) + "," + String(Ch2Min) + "," + String(Ch2Max);
    return ReturnValue;
  }
  else
  {
    pinMode(ADC_GPIO, INPUT);
    uint16_t PrevMax = 0, PrevMin = 4095, Val = 0;
    int goal = millis() + ttm;
    do
    {
      Val = analogRead(ADC_GPIO);
      PrevMax = max(PrevMax, Val);
      PrevMin = min(PrevMin, Val);
    } while (millis() < goal);
    ReturnValue = String(PrevMin) + "," + String(PrevMax);
    return ReturnValue;
  }
}

static uint32_t ESP32_ADC_Read_Data()
{
  int ADC_GPIO = ESP_ADC1Port;
  if (ESP_ADC2) ADC_GPIO = ESP_ADC2Port;
  unsigned long samples = ESP_SampleSize;
  if (ESP_Both) samples = samples + samples;
  if (samples > data_buf_size) samples = data_buf_size;
  uint32_t sample_period_micro = 1000000 / ESP_SamplingRate;

  data_buf_pointer_write = 0;
  unsigned long time_target;
  ESP32_ADC_Trigger(ESP_TriggerPort, ESP_TriggerEvent);
  unsigned long total_time = micros();

  if (!ESP_Both)
  {
    if (ESP_SamplingRate == 100000)
    {
      do
      {
        data_buf[data_buf_pointer_write++] = analogRead(ADC_GPIO);
      } while (data_buf_pointer_write < samples);
    }
    if (ESP_SamplingRate != 100000)
    {
      do
      {
        time_target = micros() + sample_period_micro;
        data_buf[data_buf_pointer_write++] = analogRead(ADC_GPIO);
        while (micros() < time_target) {};
      } while (data_buf_pointer_write < samples);
    }
  }
  else
  {
    do
    {
      time_target = micros() + sample_period_micro;
      data_buf[data_buf_pointer_write++] = analogRead(ESP_ADC1Port);
      data_buf[data_buf_pointer_write++] = analogRead(ESP_ADC2Port);
      while (micros() < time_target) {};
    } while (data_buf_pointer_write < samples);
  }
  // Record how many samples were captured
  ESP_AvailableSampleCount = samples;
  return micros() - total_time;
}

static int ESP_decode_event(String event)
{
  int TriggerVal;
  event.toUpperCase();
  for (int x = 0; x <= 5; x++)
  {
    if (event.equals(ESP_TriggerEvents[x]))
    {
      TriggerVal = x;
      return TriggerVal;
    }
  }
  return ESP_DefaultTriggerEvent;
}

static bool ESP32_ADC_init()
{
  adcAttachPin(ESP_ADC1Port);
  adcAttachPin(ESP_ADC2Port);
  analogSetAttenuation(ADC_11db);
  analogReadResolution(12);
  ESP32_ADC_active = true;
  return true;
}

static void ESP_Set_Port()
{
  String param_val;
  param_val = GetParameterValue("Port", UpperCase);
  if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
  {
    if (param_val == "BOTH")
    {
      ESP_Both = true; 
      ESP_ADC1 = false;
      ESP_ADC2 = false;
      param_val = "";
    }

    int temp = param_val.toInt();
    if (temp == 35)
    {
      ESP_ADC1 = true;
      ESP_ADC2 = false;
      ESP_Both = false;
    }
    if (temp == 36)
    {
      ESP_ADC1 = false;
      ESP_ADC2 = true;
      ESP_Both = false;
    }
    if (!ESP_Both && !ESP_ADC1 && !ESP_ADC2)
    {
      LastError("No valid port found, set to default (ADC Port 35)");
      ESP_ADC1 = true;
    }
  }
}

String ESP_ADC()
{
  if (!ESP32_ADC_active) ESP32_ADC_init();
  if (!ESP32_ADC_active)
  {
    LastError("ESP32 ADC not ready");
    return "-";
  }

  if (command == "INITIALIZE")
  {
    if (ESP32_ADC_init()) return "";
    LastError("ESP32 ADC not ready");
    return "-";
  }

  if (command == "CONFIGUREBUFFEREDREAD")
  {
    String param_val;
    ESP_Set_Port();

    param_val = GetParameterValue("SAMPLINGRATE", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      ESP_SamplingRate = param_val.toInt();
      if ((ESP_SamplingRate > ESP_SamplingRateMax) || (ESP_SamplingRate <= 0))
      {
        LastError("SamplingRate out of range, set to default");
        ESP_SamplingRate = ESP_SamplingRateDefault;
      }
    }

    param_val = GetParameterValue("SAMPLESIZE", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      ESP_SampleSize = param_val.toInt();
      if ((ESP_SampleSize > ESP_SampleSizeMax) || (ESP_SampleSize <= 0))
      {
        LastError("SampleSize out of range, set to default");
        ESP_SampleSize = ESP_SampleSizeDefault;
      }
    }

    param_val = GetParameterValue("TRIGGEREVENT", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      ESP_TriggerEvent = ESP_decode_event(param_val);
    }

    
    return "";
  } 

  if (command == "SETATTENUATION")
  {
    String val = GetParameterValue("FACTOR", UpperCase);
    if ((val != "NOPARAM") && (val != "NOVAL"))
    {
      if ((val == "0DB") || (val == "2.5DB") || (val == "6DB") || (val == "11DB"))
      {
        ESP_Attenuation = val;
        if      (val == "0DB")   analogSetAttenuation(ADC_0db);
        else if (val == "2.5DB") analogSetAttenuation(ADC_2_5db);
        else if (val == "6DB")   analogSetAttenuation(ADC_6db);
        else if (val == "11DB")  analogSetAttenuation(ADC_11db);
        return String("");
      }
      else
      {
        LastError("SetAttenuation: invalid Factor; using default 11dB");
      }
    }
    // If invalid or missing, set default
    ESP_Attenuation = ESP_AttenuationDefault;
    analogSetAttenuation(ADC_11db);
    return String("");
  }

  if (command == "GETVALUE")
  {
    // Per-call Port override: 35 (ADC1), 36 (ADC2), or BOTH
    int desired_mode = 1; // 1=ADC1(35), 2=ADC2(36), 3=Both
    String p = GetParameterValue("PORT", UpperCase);
    if ((p != "NOPARAM") && (p != "NOVAL"))
    {
      if (p == "BOTH") desired_mode = 3;
      else {
        int pv = p.toInt();
        if (pv == 36) desired_mode = 2; else if (pv == 35) desired_mode = 1;
      }
    }

    // Save current config and set temporary sampling mode
    unsigned long samples_save = ESP_SampleSize;
    bool save_ESPBoth = ESP_Both;
    bool save_ADC1 = ESP_ADC1;
    bool save_ADC2 = ESP_ADC2;

    ESP_SampleSize = 1;
    if (desired_mode == 3) { ESP_Both = true; ESP_ADC1 = false; ESP_ADC2 = false; }
    else if (desired_mode == 2) { ESP_Both = false; ESP_ADC1 = false; ESP_ADC2 = true; }
    else { ESP_Both = false; ESP_ADC1 = true; ESP_ADC2 = false; }

    ESP32_ADC_Read_Data();

    // Build return string based on requested port(s)
    String out;
    if (desired_mode == 3)
    {
      out = String(data_buf[0]) + "," + String(data_buf[1]);
    }
    else
    {
      out = String(data_buf[0]);
    }

    // Restore configuration
    ESP_SampleSize = samples_save;
    ESP_Both = save_ESPBoth;
    ESP_ADC1 = save_ADC1;
    ESP_ADC2 = save_ADC2;
    return out;
  }

  if (command == "GETENVELOPE")
  {
    // Optional parameter: Duration in milliseconds (default 1000)
    unsigned long duration_ms = 1000;
    String param_val = GetParameterValue("DURATION", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      unsigned long parsed = param_val.toInt();
      if ((parsed > 0) && (parsed <= ESP_EnvelopeMeasurementDurationMax)) duration_ms = parsed;
      else LastError("Duration out of range, using default 1000");
    }
    // Optional parameter: Port to measure (35, 36, or BOTH); default 35
    int port_mode = 1;
    String p = GetParameterValue("PORT", UpperCase);
    if ((p != "NOPARAM") && (p != "NOVAL"))
    {
      if (p == "BOTH") port_mode = 3;
      else
      {
        int pv = p.toInt();
        if (pv == 36) port_mode = 2;
        else if (pv == 35) port_mode = 1;
        else { LastError("Invalid Port for GetEnvelope; using 35"); port_mode = 1; }
      }
    }
    return ESP32_ADC_GetEnvelope(duration_ms, port_mode);
  }

  if (command == "ACQUIRETOBUFFER")
  {
    int32_t time = ESP32_ADC_Read_Data();
    return String(time);
  }

  if (command == "GETBUFFER")
  {
    // Optional per-call format override
    bool as_bytes = ESP_Byte; // default to configured format
    String f = GetParameterValue("FORMAT", UpperCase);
    if ((f != "NOPARAM") && (f != "NOVAL"))
    {
      if (f == "BYTE")  as_bytes = true;
      else if (f == "STRING")  as_bytes = false;
    }
    ESP32_GetBuffer(as_bytes);
    // Reset available-sample count after reading from the buffer
    ESP_AvailableSampleCount = 0;
    return String("");
  }

  if (command == "GETAVAILABLESAMPLECOUNT")
  {
    return String(ESP_AvailableSampleCount);
  }

  if (command == "GETSTREAM")
  {
    print2serial("");
    FlushInput();
    ESP32_GetStream();
    return String("");
  }

  LastError("No valid command found");
  return "-";
}
