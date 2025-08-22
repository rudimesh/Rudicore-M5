#pragma once

/* --- ESP32 ADC (virtual I2C addr D2) ---------------------------------------
 * Command strings:
 *  >ESP32ADC.Configure(Port= , Attenuation= , MarkChannelTwo=false,
 *                      MeasuringInterval=, SamplingRate=, SampleSize=,
 *                      TriggerEvent=, Format=Byte|String)
 *  >ESP32ADC.GetConfiguration(BufferSize)
 *  >ESP32ADC.GetConfiguration()
 *  >ESP32ADC.GetValue()
 *  >ESP32ADC.GetEnvelope()
 *  >ESP32ADC.AcquireToBuffer()
 *  >ESP32ADC.GetBuffer()
 *  >ESP32ADC.GetStream()
 */

#define ESP_ADC1Port 35
#define ESP_ADC2Port 36
#define ESP_TriggerPort 5

static bool ESP_ADC1 = true;
static bool ESP_ADC2 = false;
static bool ESP_Both = false;
static bool ESP_MarkChannelTwo = false;

#define ESP_MeasuringIntervalDefault  1000
static unsigned long ESP_MeasuringInterval = ESP_MeasuringIntervalDefault;
#define ESP_MeasuringIntervalMax  100000

#define ESP_SamplingRateDefault 1000
static unsigned long ESP_SamplingRate = ESP_SamplingRateDefault;
#define ESP_SamplingRateMax 100000

#define ESP_SampleSizeDefault 1000
static unsigned long ESP_SampleSize = ESP_SampleSizeDefault;
#define ESP_SampleSizeMax data_buf_size

#define ESP_AttenuationDefault  "11db"
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
      if (ESP_MarkChannelTwo) bitSet(ADC_val_2, 12);
      if (ESP_Byte)
      {
        write2serial((ADC_val >> 8) & 0xff);
        write2serial( ADC_val       & 0xff);
        write2serial((ADC_val_2 >> 8) & 0xff);
        write2serial( ADC_val_2       & 0xff);
      }
      else
      {
        print2serial(String(ADC_val));
        print2serial(String(ADC_val_2));
      }
      while (micros() < time_target) {};
    } while ((!Serial.available()) && (!BT.available()));
  }
  else
  {
    do
    {
      time_target = micros() + sample_period_micro;
      ADC_val = analogRead(ADC_GPIO);
      if (ESP_Byte)
      {
        write2serial((ADC_val >> 8) & 0xff);
        write2serial( ADC_val       & 0xff);
      }
      else
      {
        print2serial(String(ADC_val));
      }
      while (micros() < time_target) {};
    } while ((!Serial.available()) && (!BT.available()));
  }
}

static void ESP32_GetBuffer()
{
  unsigned long samples = ESP_SampleSize;
  if (ESP_Both) samples = samples + samples;
  for (int i = 0; i < samples; i++)
  {
    if (ESP_Byte)
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

static String ESP32_ADC_GetEnvelope()
{
  String ReturnValue;
  int ADC_GPIO = ESP_ADC1Port;
  if (ESP_ADC2) ADC_GPIO = ESP_ADC2Port;
  unsigned long ttm = ESP_MeasuringInterval;
  ESP32_ADC_Trigger(ESP_TriggerPort, ESP_TriggerEvent);

  if (ESP_Both)
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

  if (command == "CONFIGURE")
  {
    String param_val;
    ESP_Set_Port();

    param_val = GetParameterValue("MEASURINGINTERVAL", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      ESP_MeasuringInterval = param_val.toInt();
      if ((ESP_MeasuringInterval > ESP_MeasuringIntervalMax) || (ESP_MeasuringInterval <= 0))
      {
        LastError("MeasuringInterval out of range, set to default");
        ESP_MeasuringInterval = ESP_MeasuringIntervalDefault;
      }
    }

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

    param_val = GetParameterValue("FORMAT", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      if (param_val == "BYTE") ESP_Byte = true; else ESP_Byte = false;
    }

    param_val = GetParameterValue("ATTENUATION", UpperCase);
    if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
    {
      if ((param_val == "0DB") || (param_val == "2.5DB") || (param_val == "6DB") || (param_val == "11DB"))
      {
        ESP_Attenuation = param_val;
        if      (param_val == "0DB")   analogSetAttenuation(ADC_0db);
        else if (param_val == "2.5DB") analogSetAttenuation(ADC_2_5db);
        else if (param_val == "6DB")   analogSetAttenuation(ADC_6db);
        else if (param_val == "11DB")  analogSetAttenuation(ADC_11db);
      }
      else
      {
        LastError("No correct value found, set to default");
        ESP_Attenuation = ESP_AttenuationDefault;
        analogSetAttenuation(ADC_11db);
      }
    }
    return "";
  } // end CONFIGURE

  if (command == "GETCONFIGURATION")
  {
    if (GetParameterValue("BUFFERSIZE", UpperCase) == "NOVAL")
    {
      return String(data_buf_size);
    }
    String ExPorts;
    if (ESP_Both) ExPorts = "Both"; else
      if (ESP_ADC1) ExPorts = "35"; else ExPorts = "36";
    String result = "Ports in use = " + ExPorts + "\n";
    result = result + "Attenuation = " + ESP_Attenuation + "\n";
    result = result + "MeasuringInterval = " + String(ESP_MeasuringInterval) + "\n";
    result = result + "SamplingRate = " + String(ESP_SamplingRate) + "\n";
    result = result + "SampleSize = " + String(ESP_SampleSize) + "\n";
    result = result + "TriggerEvent = " + ESP_TriggerEvents[ESP_TriggerEvent] + "\n";
    result = result + "OutputFormat = "; if (ESP_Byte) result = result + "Byte"; else result = result + "String";
    return result;
  }

  if (command == "GETVALUE")
  {
    unsigned long samples_save = ESP_SampleSize;
    bool save_ESPBoth = ESP_Both;
    ESP_SampleSize = 1;
    ESP_Both = false;
    ESP32_ADC_Read_Data();
    ESP_SampleSize = samples_save;
    ESP_Both = save_ESPBoth;
    return String(data_buf[0]);
  }

  if (command == "GETENVELOPE")
  {
    return ESP32_ADC_GetEnvelope();
  }

  if (command == "ACQUIRETOBUFFER")
  {
    int32_t time = ESP32_ADC_Read_Data();
    return String(time);
  }

  if (command == "GETBUFFER")
  {
    ESP32_GetBuffer();
    return String("");
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
