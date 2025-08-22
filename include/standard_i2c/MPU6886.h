#pragma once

/*
 * --- MPU6886 Command Strings ---
 *
 * >MPU6886.GetAccelerometerValues()
 * >MPU6886.GetGyroscopeValues()
 * >MPU6886.ConfigureAccelerometer(Scalefactor=2g|4g|8g|16g)
 * >MPU6886.ConfigureGyroscope(Scalefactor=250dps|500dps|1000dps|2000dps)
 * >MPU6886.AcquireToAccelerometerBuffer(MeasuringInterval = , SamplingRate =)
 * >MPU6886.AcquireToGyroscopeBuffer(MeasuringInterval = , SamplingRate =)
 * >MPU6886.GetAccelerometerBuffer()
 * >MPU6886.GetGyroscopeBuffer()
 * >MPU6886.Initialize()
 */

#define IMU_MeasuringIntervalMax  100000            // stop sampling after max 100sec
#define IMU_SamplingRateMax 1000                    // to be determined...
#define NumberOfDecimals 4                          // resolution
int IMU_MeasuringIntervalDefault = 10000;
int IMU_SamplingRateDefault = 10;
int IMU_MeasuringInterval = IMU_MeasuringIntervalDefault;
int IMU_SamplingRate = IMU_SamplingRateDefault;
uint16_t IMU_number_of_readings = 0;
bool AccelBufferFilled = false;
bool  GyroBufferFilled = false;
MPU6886::Gscale selected_fsr;

// Buffer to store during AcquireToBuffer. A timestamp is also recorded
#define IMUBufferSize 1000                // max 1000 samples (see struct)
typedef struct
{
  float x, y, z;
  unsigned long TimeStamp;
} IMUBuffer;

IMUBuffer IMUBuf[IMUBufferSize];

// initialize internal IMU. Return true if ready.
bool MPU6886_init()
{
  Wire.beginTransmission(0x68);   // check if IMU is available
  if (Wire.endTransmission() != 0) 
  {
    MPU6886_active = false;
    return false;
  }
  M5.IMU.Init();

  // Extra register-level init to ensure gyro is fully awake and configured
  // Minimal register map for MPU6886
  const uint8_t MPU_ADDR       = 0x68;
  const uint8_t REG_PWR_MGMT_1 = 0x6B;
  const uint8_t REG_CONFIG     = 0x1A;
  const uint8_t REG_SMPLRT_DIV = 0x19;
  const uint8_t REG_GYRO_CFG   = 0x1B;
  const uint8_t REG_ACCEL_CFG  = 0x1C;
  const uint8_t REG_ACCEL_CFG2 = 0x1D;
  

  auto i2cWrite1 = [&](uint8_t reg, uint8_t val){
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
    delay(1);
  };

  // Wake and set clock source
  i2cWrite1(REG_PWR_MGMT_1, 0x00);  // wake up
  delay(10);
  i2cWrite1(REG_PWR_MGMT_1, 0x01);  // set clock source

  // Basic filters and sample rate
  i2cWrite1(REG_CONFIG, 0x01);      // DLPF
  i2cWrite1(REG_SMPLRT_DIV, 0x01);  // ~500 Hz
  i2cWrite1(REG_ACCEL_CFG2, 0x00);

  // Default full-scale ranges (can be changed via Configure* commands)
  i2cWrite1(REG_ACCEL_CFG, 0x10);   // +/- 8g
  i2cWrite1(REG_GYRO_CFG,  0x18);   // +/- 2000 dps

  // Also configure M5 IMU wrappers to sane defaults
  M5.IMU.setAccelFsr(M5.IMU.AFS_8G);
  selected_fsr=M5.IMU.GFS_2000DPS;
  MPU6886_active = true;
  return true;
}

// Collect the data from accelerometer or gyro and store in struct
void IMU_GetSample(char d, int b)
{
  if (d == 'g') {
    M5.IMU.setGyroFsr(selected_fsr);
    M5.IMU.getGyroData (&IMUBuf[b].x,&IMUBuf[b].y,&IMUBuf[b].z);
  }
  else
    M5.IMU.getAccelData(&IMUBuf[b].x,&IMUBuf[b].y,&IMUBuf[b].z);
}

// AcquireToBuffer
uint32_t IMU_Read_Stream(unsigned long MeasureTime, unsigned int SamplingRate, char dev)
{
  unsigned int b = 0;                                         // pointer array
  unsigned long GoalTime = millis() + MeasureTime;            // calculate timeout
  unsigned int SampleTime = 1000000/SamplingRate;             // calculate time between samples in microsec
  unsigned long start_time = micros();
  unsigned int GoalSampleTime = start_time + SampleTime;      // 
  do
  {
    IMUBuf[b].TimeStamp = micros() - start_time;              // store time since start of the burst in microsec.    

    IMU_GetSample(dev, b);                                    // Get sample and store in struct array

    b++;
    while (micros() < GoalSampleTime);                        // wait before reading the next sample
    GoalSampleTime =micros() + SampleTime;

  } while ((millis() < GoalTime) && (b < IMUBufferSize));     // prevent buffer overflow
  return b;                                                   // return the number of samples in buffer
}

bool IMUGetBuffer(unsigned int NumberOfSamples)
{
  String r;
  for (int b = 0; b < NumberOfSamples; b++)
  {
    r =      String(IMUBuf[b].TimeStamp) + ", ";
    r = r +  String(IMUBuf[b].x,NumberOfDecimals) + ", " + String(IMUBuf[b].y,NumberOfDecimals) + ", " + String(IMUBuf[b].z,NumberOfDecimals);
    print2serial(r);
  }
  return true;
}

static void MPU6886_ensure_init()
{
  if (!MPU6886_active) MPU6886_init();
}

static String MPU6886_configure_accel()
{
  String sf = GetParameterValue("SCALEFACTOR");
  sf.toUpperCase();
  if (sf == "NOVAL" || sf == "NOPARAM") { LastError("No parameter value found"); return "-"; }
  if (sf == "2G")  { M5.IMU.setAccelFsr(M5.IMU.AFS_2G); return ""; }
  if (sf == "4G")  { M5.IMU.setAccelFsr(M5.IMU.AFS_4G); return ""; }
  if (sf == "8G")  { M5.IMU.setAccelFsr(M5.IMU.AFS_8G); return ""; }
  if (sf == "16G") { M5.IMU.setAccelFsr(M5.IMU.AFS_16G); return ""; }
  LastError("No valid parameter value found");
  return "-";
}

static String MPU6886_configure_gyro()
{
  String sf = GetParameterValue("SCALEFACTOR");
  sf.toUpperCase();
  if (sf == "NOVAL" || sf == "NOPARAM") { LastError("No parameter value found"); return "-"; }
  if (sf == "250DPS")  { selected_fsr=M5.IMU.GFS_250DPS; return ""; }
  if (sf == "500DPS")  { selected_fsr=M5.IMU.GFS_500DPS; return ""; }
  if (sf == "1000DPS") { selected_fsr=M5.IMU.GFS_1000DPS; return ""; }
  if (sf == "2000DPS") { selected_fsr=M5.IMU.GFS_2000DPS; return ""; }
  LastError("No valid parameter value found");
  return "-";
}

static String MPU6886_acquire_to_buffer(char dev)
{
  param_val = GetParameterValue("MEASURINGINTERVAL", UpperCase);
  if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
  {
    IMU_MeasuringInterval = param_val.toInt();
    if ((IMU_MeasuringInterval > IMU_MeasuringIntervalMax) || (IMU_MeasuringInterval <= 0)) 
    {
      LastError("MeasuringInterval out of range, set to default");
      IMU_SamplingRate = IMU_SamplingRateDefault;
    }
  }

  param_val = GetParameterValue("SAMPLINGRATE", UpperCase);
  if ((param_val != "NOPARAM") && (param_val != "NOVAL"))
  {
    IMU_SamplingRate = param_val.toInt();
    if ((IMU_SamplingRate > IMU_SamplingRateMax) || (IMU_SamplingRate <= 0)) 
    {
      LastError("SamplingRate out of range, set to default");
      IMU_SamplingRate = IMU_SamplingRateDefault;
    }
  }

  IMU_number_of_readings = IMU_Read_Stream(IMU_MeasuringInterval, IMU_SamplingRate, dev);
  GyroBufferFilled = (dev == 'g');
  AccelBufferFilled = (dev == 'a');
  return String(IMU_number_of_readings);
}

static String MPU6886_get_values(char dev)
{
  IMU_GetSample(dev, 0);
  return String(IMUBuf[0].x,NumberOfDecimals) + ", " + String(IMUBuf[0].y,NumberOfDecimals) + ", " + String(IMUBuf[0].z,NumberOfDecimals);
}

static String MPU6886_get_buffer(char dev)
{
  if (IMU_number_of_readings == 0) return "-";
  if ((dev == 'g' && GyroBufferFilled) || (dev == 'a' && AccelBufferFilled))
  {
    IMUGetBuffer(IMU_number_of_readings);
    return " ";
  }
  return "-";
}

String MPU6886()
{
  MPU6886_ensure_init();

  if (command == "INITIALIZE")                return MPU6886_init() ? String("") : String("-");

  if (command == "GETACCELEROMETERVALUES")    return MPU6886_get_values('a');
  if (command == "GETGYROSCOPEVALUES")        return MPU6886_get_values('g');

  if (command == "CONFIGUREACCELEROMETER")    return MPU6886_configure_accel();
  if (command == "CONFIGUREGYROSCOPE")        return MPU6886_configure_gyro();

  if (command == "ACQUIRETOACCELEROMETERBUFFER") return MPU6886_acquire_to_buffer('a');
  if (command == "ACQUIRETOGYROSCOPEBUFFER")     return MPU6886_acquire_to_buffer('g');

  if (command == "GETACCELEROMETERBUFFER")    return MPU6886_get_buffer('a');
  if (command == "GETGYROSCOPEBUFFER")        return MPU6886_get_buffer('g');

  LastError("No valid command found");
  return "-";
}
