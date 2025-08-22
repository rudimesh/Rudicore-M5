#pragma once

#include "standard_i2c/PaHUB.h"
#include <stdlib.h>

// Known I2C addresses (scanned)

static const uint8_t KnownI2CAddresses[] = {
    0x10, // BMM150 magnetometer
    0x26, // M5Stack Mini Scales Unit
    0x29, // M5Stack Color sensor TCS34725 / M5Stack ToF sensor VL53L0X
    0x33, // M5Stack Thermal unit (MLX90640) 
    0x40, // INA226 current/voltage monitor
    0x48, // M5Stack ADC unit / M5Stack A Meter (ADS1100)
    0x49, // M5Stack V Meter
    0x51, // M5Stack A Meter EEPROM
    0x52, // M5Stack Joystick Unit
    0x53, // M5Stack ACCEL unit (ADXL345) / LTR390 / M5Stack V Meter EEPROM
    0x57, // M5 Ultrasonic Unit
    0x58, // SGP30 gas sensor
    0x59, // M5Stack External encoder unit
    0x5A, // M5Stack NCIR unit (MLX90614)
    0x5E, // M5Stack Faces Encoder
    0x5F, // DFRobot GP8403 DAC
    0x60, // M5Stack DAC unit (MCP4725)
    0x61, // M5Stack PbHUB
    0x64, // M5Stack Roller Unit
    0x66, // M5Stack K Meter thermocouple
    0x68, // IMU (MPU9250 etc.)
    0x70, // M5Stack PaHUB multiplexer
    0x76  // M5Stack ENVII Unit (BMP280)
};

static const size_t NumKnownI2CAddresses = sizeof(KnownI2CAddresses) / sizeof(KnownI2CAddresses[0]);

// Virtual I2C tokens: D0=custom DAQ board, D1=ESP32 GPIO, D2=ESP32ADC, D3=Speaker, D4=System

// Forward declaration for reading the configured device type
String DeviceTypeLoad();
// Forward declaration for the Virtual I2C mask
uint8_t VirtualI2CMaskLoad();

// Port memory for I2C units
extern int M5_Miniscale_port;
extern int VLX53LOX_port;
extern int TCS34725_port;
extern int MLX90640_port;
extern int ADS1100_port;
extern int AMeter_port;
extern int VMeter_port;
extern int Joystick_port;
extern int UV_port;
extern int ADXL345_port;
extern int UltraSonic_port;
extern int FacesEncoder_port;
extern int SGP30_port;
extern int MLX90614_port;
extern int ExtEncoder_port;
extern int GP8403_port;
extern int MCP4725_port;
extern int KMeter_port;
extern int ENVII_port;
extern int PbHUB_port;
extern int Roller_port;

// Initialize remembered ports to "not found"
int M5_Miniscale_port = -1;
int VLX53LOX_port     = -1;
int TCS34725_port     = -1;
int MLX90640_port     = -1;
int ADS1100_port      = -1;
int AMeter_port       = -1;
int VMeter_port       = -1;
int Joystick_port     = -1;
int UV_port           = -1;
int ADXL345_port      = -1;
int UltraSonic_port   = -1;
int FacesEncoder_port = -1;
int SGP30_port        = -1;
int MLX90614_port     = -1;
int ExtEncoder_port   = -1;
int GP8403_port       = -1;
int MCP4725_port      = -1;
int KMeter_port       = -1;
int ENVII_port        = -1;
int PbHUB_port        = -1;
int Roller_port       = -1;

// Helpers

inline bool i2c_device_connected(int address)
{
    Wire.beginTransmission(address);
    return Wire.endTransmission() == 0;
}

static bool list_has_address(String list, String token)
{
    list.toUpperCase();
    token.toUpperCase();
    String haystack = "," + list + ",";
    String needle   = "," + token + ",";
    return haystack.indexOf(needle) != -1;
}

static String pop_list_token(String &list)
{
    int comma = list.indexOf(',');
    String token;
    if (comma == -1)
    {
        token = list;
        list  = "";
    }
    else
    {
        token = list.substring(0, comma);
        list  = list.substring(comma + 1);
    }
    return token;
}

static void remember_unit_port(uint8_t address, int port)
{
    switch (address)
    {
        case 0x26: M5_Miniscale_port = port; break;
        case 0x29: VLX53LOX_port = port; break;
        case 0xF2: TCS34725_port = port; break;
        case 0x33: MLX90640_port = port; break;
        case 0x48: ADS1100_port = port; AMeter_port = port; break;
        case 0x49: VMeter_port  = port; break;
        case 0x52: Joystick_port = port; break;
        case 0x53: UV_port = port; ADXL345_port = port; break;
        case 0x57: UltraSonic_port = port; break;
        case 0x58: SGP30_port = port; break;
        case 0x59: ExtEncoder_port = port; break;
        case 0x5A: MLX90614_port = port; break;
        case 0x5E: FacesEncoder_port = port; break;
        case 0x5F: GP8403_port = port; break;
        case 0x60: MCP4725_port = port; break;
        case 0x61: PbHUB_port   = port; break;
        case 0x66: KMeter_port = port; break;
        case 0x64: Roller_port  = port; break;
        case 0x76: ENVII_port  = port; break;
        default: break;
    }
}

// Store port information for all addresses in a list
static void remember_unit_ports(String addresses, int port)
{
    String tmp = addresses;
    while (tmp.length() > 0)
    {
        String token = pop_list_token(tmp);
        token.toUpperCase();
        char *endptr = nullptr;
        uint16_t addr = strtol(token.c_str(), &endptr, 16);
        if (endptr != token.c_str()) remember_unit_port(addr, port);
    }
}

// Special address processing

// Replace address combinations by synthetic tokens
static String apply_special_combinations(String addresses, String prefix)
{
    bool has53 = false;
    bool has49 = false;
    bool has48 = false;
    bool has51 = false;

    String tmp = addresses;
    while (tmp.length() > 0)
    {
        String token = pop_list_token(tmp);
        int dot = token.indexOf('.');
        if (dot != -1) token = token.substring(dot + 1);
        token.toUpperCase();
        if (token == "53") has53 = true;
        if (token == "49") has49 = true;
        if (token == "48") has48 = true;
        if (token == "51") has51 = true;
    }

    tmp = addresses;
    addresses = "";
    while (tmp.length() > 0)
    {
        String token = pop_list_token(tmp);
        String addr_token  = token;
        String token_prefix = "";
        int dot = token.indexOf('.');
        if (dot != -1)
        {
            token_prefix = token.substring(0, dot + 1);
            addr_token   = token.substring(dot + 1);
        }
        addr_token.toUpperCase();

        if (has53 && has49 && (addr_token == "53" || addr_token == "49")) continue;
        if (has48 && has51 && (addr_token == "48" || addr_token == "51")) continue;

        if (addresses.length() > 0) addresses += ",";
        addresses += token_prefix + addr_token;
    }

    if (has53 && has49)
    {
        if (addresses.length() > 0) addresses += ",";
        addresses += prefix + "F0";
    }
    if (has48 && has51)
    {
        if (addresses.length() > 0) addresses += ",";
        addresses += prefix + "F1";
    }
    return addresses;
}

// Prefix all comma separated addresses with "port." when port >= 0
static String prefix_addresses(String addresses, int port)
{
    if (port < 0) return addresses;
    String result = "";
    String tmp = addresses;
    while (tmp.length() > 0)
    {
        String token = pop_list_token(tmp);
        if (result.length() > 0) result += ",";
        result += String(port) + "." + token;
    }
    return result;
}

// Subtract all addresses from 'remove' out of 'source'
static String subtract_addresses(String source, String remove)
{
    String result = "";
    String tmp = source;
    while (tmp.length() > 0)
    {
        String token = pop_list_token(tmp);
        if (!list_has_address(remove, token))
        {
            if (result.length() > 0) result += ",";
            result += token;
        }
    }
    return result;
}

// Scanning helpers

// Detect hub and close all channels
static bool prepare_hub()
{
    if (!PaHUBconnected()) return false;
    closeAll();
    return true;
}

// Scan the active bus for known addresses
static String scan_raw_bus()
{
    String addresses = "";
    bool color_detected = false;
    bool tof_detected   = false;

    for (size_t idx = 0; idx < NumKnownI2CAddresses; ++idx)
    {
        int address = KnownI2CAddresses[idx];
        if (address == 0x70) continue;
        if (!i2c_device_connected(address))
        {
            delay(10);
            continue;
        }

        if (address == 0x29)
        {
            Wire.beginTransmission(0x29);
            Wire.write(0xC0);
            if (Wire.endTransmission(false) == 0 && Wire.requestFrom((uint8_t)0x29, (uint8_t)2))
            {
                uint8_t id0 = Wire.read();
                uint8_t id1 = Wire.read();
                if (id0 == 0xEE && id1 == 0xAA) tof_detected = true;
                else color_detected = true;
            }
            else
            {
                color_detected = true;
            }
        }
        else
        {
            if (addresses.length() > 0) addresses += ",";
            addresses += String(address, HEX);
        }
        delay(10);
    }

    if (tof_detected)
    {
        if (addresses.length() > 0) addresses += ",";
        addresses += String(0x29, HEX);
    }
    if (color_detected)
    {
        if (addresses.length() > 0) addresses += ",";
        addresses += "F2";
    }
    return addresses;
}

// Scan for local devices (in front of the hub)
static String scan_local_bus()
{
    String devices = scan_raw_bus();
    remember_unit_ports(devices, -1);
    return devices;
}

// Scan a single PaHUB port
static String scan_hub_port(int port, String local_devices)
{
    if (!selectPaHUBChannel(port))
        return "";

    String found = scan_raw_bus();

    remember_unit_ports(found, port);

    String processed = apply_special_combinations(found, "");

    processed = subtract_addresses(processed, local_devices);

    processed = prefix_addresses(processed, port);

    closeAll();

    return processed;
}

// Public API

void clear_active_states()
{

 MPU6886_active = false;     
 Hall_active = false;           
 ESP32_HALL_active = false; 
 ESP32_ADC_active = false;
 UltraSonic_active = false;     
 HC_SR04_active = false; 
 JSN_SR04T_active = false; 
 Color_active = false; 
 ADS1100_active = false; 
 MCP4725_active = false; 
 TOF_active = false; 
 PaHUB_active = false; 
 M5_Speaker_active = false; 
 GPS_active = false;
 ENVII_active = false; 
 SGP30_active = false;
 Laser_Rx_active = false;
 SDS011_active = false;
 display_active = true;         
 HX711_active = false;          
 PbHUB_active = false;          
 NCir_active = false;           
 KMeter_active = false;         
 SEN0322_active = false;        
 MLX90640_active = false;       
 INA_active = false;            
 GP8403_active = false;         
 UV_active = false;         
 ADXL345_active = false;        
 M5_Miniscale_active = false;   
 Joystick_active = false;       
 FacesEncoder_active = false;   
 VMeter_active = false;         
 AMeter_active = false;         
 ExtEncoder_active = false;   
}


static String i2c_scanner()
{
    clear_active_states();
    bool hub_found = prepare_hub();

    String local_raw       = scan_local_bus();
    String local_processed = apply_special_combinations(local_raw, "");
    String result          = local_processed;

    uint8_t vmask = VirtualI2CMaskLoad();

    if (hub_found)
    {
        for (int port = 0; port < 6; ++port)
        {
            String port_list = scan_hub_port(port, local_processed);
            if (port_list.length() == 0) continue;
            if (result.length() > 0) result += ",";
            result += port_list;
        }
        closeAll();
    }

    String dtype = DeviceTypeLoad();
    if (dtype.equalsIgnoreCase("DAQ"))
    {
        if ((vmask & 0x01) == 0) {
          if (result.length() > 0) result += ",";
          result += "D0";
        }
    }

    if (dtype.equalsIgnoreCase("Gray"))
    {
        if ((vmask & 0x02) == 0) {
          if (result.length() > 0) result += ",";
          result += "D1";
        }
    }

    if (dtype.equalsIgnoreCase("Core Basic"))
    {
        if ((vmask & 0x02) == 0) {
          if (result.length() > 0) result += ",";
          result += "D1";
        }
    }

    if ((vmask & 0x04) == 0) {
      if (result.length() > 0) result += ",";
      result += "D2";
    }

    if ((vmask & 0x08) == 0) {
      if (result.length() > 0) result += ",";
      result += "D3";
    }

    if (result.length() > 0) result += ",";
    result += "D4";


    return result;
}

// Return remembered port numbers for all supported units
static String port_list()
{
    String ports = "";
    ports += "M5_Miniscale=" + String(M5_Miniscale_port) + "\n";
    ports += "VLX53LOX="     + String(VLX53LOX_port)     + "\n";
    ports += "TCS34725="     + String(TCS34725_port)     + "\n";
    ports += "MLX90640="     + String(MLX90640_port)     + "\n";
    ports += "ADS1100="      + String(ADS1100_port)      + "\n";
    ports += "AMeter="       + String(AMeter_port)       + "\n";
    ports += "VMeter="       + String(VMeter_port)       + "\n";
    ports += "Joystick="     + String(Joystick_port)     + "\n";
    ports += "LTR390="       + String(UV_port)           + "\n";
    ports += "ADXL345="      + String(ADXL345_port)      + "\n";
    ports += "UltraSonic="   + String(UltraSonic_port)   + "\n";
    ports += "SGP30="        + String(SGP30_port)        + "\n";
    ports += "MLX90614="     + String(MLX90614_port)     + "\n";
    ports += "GP8403="       + String(GP8403_port)       + "\n";
    ports += "MCP4725="      + String(MCP4725_port)      + "\n";
    ports += "KMeter="       + String(KMeter_port)       + "\n";
    ports += "PbHUB="        + String(PbHUB_port)        + "\n";
    ports += "Roller="       + String(Roller_port)       + "\n";
    ports += "FacesEncoder=" + String(FacesEncoder_port) + "\n";
    ports += "ENVII="        + String(ENVII_port);
    return ports;
}
