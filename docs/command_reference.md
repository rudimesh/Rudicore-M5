# Rudicore-M5 Command Reference

Send commands over USB serial or Bluetooth as lines starting with `>`, for example `>SYSTEM.BTName()`.

## System (virtual, prefix `SYSTEM`)
- `>SYSTEM.Restart()`
- `>SYSTEM.BTName()`
- `>SYSTEM.FriendlyName()`
- `>SYSTEM.BTAddress()`
- `>SYSTEM.Echo(Text=)`
- `>SYSTEM.GetConnectionType()`
- `>SYSTEM.GetBatteryLevel()`
- `>SYSTEM.GetChargingStatus()`
- `>SYSTEM.DisplayOn()`
- `>SYSTEM.DisplayOff()`
- `>SYSTEM.ScanI2C()`
- `>SYSTEM.LastError(Clear)`
- `>SYSTEM.Ports()`

## Speaker (virtual, prefix `Speaker`)
- `>Speaker.Beep()`
- `>Speaker.Upload(SampleSize=1-1000)` then stream bytes
- `>Speaker.Play(SamplingRate=1-100000, NumberOfTimes=0-100000)`
- `>Speaker.Stop()`

## ESP32 ADC (virtual, prefix `ESP32ADC`)
- `>ESP32ADC.Configure(Port=35|36|Both, Attenuation=0dB|2.5dB|6dB|11dB, MarkChannelTwo=false, MeasuringInterval=, SamplingRate=, SampleSize=, TriggerEvent=Ignore|Low|High|Change|Rising|Falling, Format=Byte|String)`
- `>ESP32ADC.GetConfiguration(BufferSize)`
- `>ESP32ADC.GetValue()`
- `>ESP32ADC.GetEnvelope()`
- `>ESP32ADC.AcquireToBuffer()`
- `>ESP32ADC.GetBuffer()`
- `>ESP32ADC.GetStream()`

## GPIO (virtual, prefix `M5GPIO`)
- `>M5GPIO.SetValue(Port=2|5|16|17|25|26, Value=0|1)`
- `>M5GPIO.GetValue(Port=2|5|16|17|26)`

## DAQ (virtual, prefix `DAQ`)
- `>DAQ.GET_DIGITAL_STATE(Port=1|2)`
- `>DAQ.SET_DIGITAL_STATE(Port=1|2, State=0|1)`
- `>DAQ.GET_ANALOG_VOLTAGE(Port=1|2)`
- `>DAQ.SET_ANALOG_VOLTAGE(Port=1|2, Voltage=0..10)`
- `>DAQ.GET_PHOTOGATE(Port=1|2)`
- `>DAQ.GET_FORCE()`

## ADS1100 ADC (prefix `ADC`)
- `>ADC.GetValue()`
- `>ADC.GetNTCTemp()`
- `>ADC.Configure(Gain=1|2|4|8, SamplingRate=8|16|32|128)`
- `>ADC.Initialize()`

## ADXL345 (prefix `ADXL345`)
- `>ADXL345.Initialize()`
- `>ADXL345.GetValue(X|Y|Z)`
- `>ADXL345.GetValues()`

## AMeter (prefix `AMeter`)
- `>AMeter.GetValue(Current|RawADC)`
- `>AMeter.Initialize()`

## BMM150 (prefix `BMM150`)
- `>BMM150.Calibrate()`
- `>BMM150.GetValue(x|y|z|Angle)`

## ENVII (prefix `ENVII`)
- `>ENVII.GetTemperature()`
- `>ENVII.GetHumidity()`
- `>ENVII.GetPressure()`
- `>ENVII.Initialize()`

## ExtEncoder (prefix `ExtEncoder`)
- `>ExtEncoder.GetEncoder()`
- `>ExtEncoder.GetMeter(String)`
- `>ExtEncoder.GetPerimeter()`
- `>ExtEncoder.GetPulsePerRound()`
- `>ExtEncoder.SetPerimeter(Perimeter=)`
- `>ExtEncoder.SetPulsePerRound(PulsePerRound=)`
- `>ExtEncoder.SetZTriggerMode(Mode=0..2)`
- `>ExtEncoder.GetTurns()`
- `>ExtEncoder.SetTurns(Turns=)`
- `>ExtEncoder.Reset()`
- `>ExtEncoder.GetFirmware()`
- `>ExtEncoder.GetI2CAddress()`
- `>ExtEncoder.Initialize()`

## FacesEncoder (prefix `FacesEncoder`)
- `>FacesEncoder.GetValue(Encoder|Button)`
- `>FacesEncoder.SetLED(Index=0-11, R=0-255, G=0-255, B=0-255)`
- `>FacesEncoder.Initialize()`

## GP8403 DAC (prefix `GP8403`)
- `>GP8403.Initialize()`
- `>GP8403.SetValue(ValueOnly=0..4095)`
- `>GP8403.SetVoltage(ValueOnly=millivolt)` (0–5000 or 0–10000 depending on range)
- `>GP8403.SetChannel(ValueOnly=0|1)`
- `>GP8403.SetVoltageRange(ValueOnly=5|10)`

## Joystick (prefix `Joystick`)
- `>Joystick.GetValue(X|Y|Button)`
- `>Joystick.GetValues()`
- `>Joystick.Initialize()`

## KMeter (prefix `KMeter`)
- `>KMeter.GetValue(Temperature)`
- `>KMeter.GetValue(InternalTemperature)`

## Mini Scales (prefix `Scales`)
- `>Scales.GetValue(Weight|GapValue|RawADC)`
- `>Scales.GetValues()`
- `>Scales.Initialize()`
- `>Scales.SetValue(Gap=)`
- `>Scales.Tare()`

## MCP4725 DAC (prefix `DAC`)
- `>DAC.SetValue(ValueOnly=0..4095)`
- `>DAC.SetVoltage(ValueOnly=millivolt)`
- `>DAC.Initialize()`

## MLX90614 IR temp (prefix `NCir`)
- `>NCir.GetTemperature()`
- `>NCir.Initialize()`

## MLX90640 Thermal (prefix `MLX90640`)
- `>MLX90640.Get_Image()` returns 768 bytes followed by frame number

## MPU6886 IMU (prefix `MPU6886`)
- `>MPU6886.GetAccelerometerValues()`
- `>MPU6886.GetGyroscopeValues()`
- `>MPU6886.ConfigureAccelerometer(Scalefactor=2g|4g|8g|16g)`
- `>MPU6886.ConfigureGyroscope(Scalefactor=250dps|500dps|1000dps|2000dps)`
- `>MPU6886.AcquireToAccelerometerBuffer(MeasuringInterval=, SamplingRate=)`
- `>MPU6886.AcquireToGyroscopeBuffer(MeasuringInterval=, SamplingRate=)`
- `>MPU6886.GetAccelerometerBuffer()`
- `>MPU6886.GetGyroscopeBuffer()`
- `>MPU6886.Initialize()`

## PaHUB (prefix `PaHUB`)
- `>PaHUB.Initialize()`
- `>PaHUB.SetPort(Port=0..5)`
- `>PaHUB.Disable()`

## PbHUB (prefix `PbHUB`)
- `>PbHUB.Initialize()`
- `>PbHUB.AnalogRead(Port=)`
- `>PbHUB.DigitalRead(Port=, DataLine=A|B)`
- `>PbHUB.AnalogWrite(Port=, DataLine=A|B, Value=0..255)`
- `>PbHUB.DigitalWrite(Port=, DataLine=A|B, Value=0|1)`

## Roller (prefix `Roller`)
- `>Roller.SetMode(Mode=1..4)`
- `>Roller.SetSpeed(Speed=-120000..120000)`
- `>Roller.SetPos(Pos=-2147483648..2147483647)`
- `>Roller.SetOutput(Output=0|1)`
- `>Roller.GetSpeed()`
- `>Roller.GetPos()`
- `>Roller.GetSysStatus()`
- `>Roller.GetErrorCode()`
- `>Roller.GetOutputStatus()`

## SGP30 gas (prefix `SGP30`)
- `>SGP30.Initialize()`
- `>SGP30.TVOC()`
- `>SGP30.ECO2()`
- `>SGP30.BASELINE-TVOC()`
- `>SGP30.BASELINE-ECO2()`
- `>SGP30.H2()`
- `>SGP30.ETHANOL()`

## Color sensor TCS34725 (prefix `Color`)
- `>Color.GetValue(Red|Green|Blue|Clear)`
- `>Color.GetValues()`
- `>Color.Configure(Gain=1|4|16|60, IntegrationTime=2|24|50|101|154|300|614)`
- `>Color.Initialize()`

## UltraSonic (prefix `UltraSonic`)
- `>UltraSonic.GetValue(Distance)`

## Time-of-Flight VL53L0X (prefix `TOF`)
- `>TOF.GetDistance()`
- `>TOF.AcquireToBuffer(MeasuringInterval=, SamplingRate=)`
- `>TOF.Configure(Sensitivity=LongRange|HighAccuracy|Default, MeasuringInterval=, SamplingRate=, IncludeTimestamps=true|false)`
- `>TOF.Initialize()` (only when behind active hub)
- `>TOF.GetConfiguration()`
- `>TOF.GetBuffer(IncludeTimeStamps=true|false)`

## VMeter (prefix `VMeter`)
- `>VMeter.GetValue(Voltage|RawADC)`
- `>VMeter.Initialize()`

## INA226 (prefix `INA226`)
- `>INA226.Initialize()`
- `>INA226.GetValues()` (BusVoltage mV, Current mA)

## LTR390 UV (prefix `UV`)
- `>UV.Initialize()`
- `>UV.GetALS()` (Lux)
- `>UV.GetUVS()` (UV index)

## Oxygen SEN0322 (prefix `SEN0322`)
- `>SEN0322.GetValue(AverageSample=1..100)`
- `>SEN0322.Calibrate(OxygenConcentration=0..100)`

---

Note: Some units share I2C addresses (e.g., TOF and Color). Use PaHUB to multiplex if both are required.
