#pragma once
/* --- Pb.HUB -------------------------------------------------------------------------------------

 * >PbHUB.Initialize()
 * >PbHUB.AnalogRead(Port=)
 * >PbHUB.DigitalRead(Port= , DataLine= )
 * >PbHUB.AnalogWrite(Port= , DataLine= , Value= )
 * >PbHUB.DigitalWrite(Port= , DataLine= , Value= )
 */
 
PortHub PbHUB;

uint8_t HUB_ADDR[6]={HUB1_ADDR,HUB2_ADDR,HUB3_ADDR,HUB4_ADDR,HUB5_ADDR,HUB6_ADDR}; 
#define PbHUB_ADDR  0x61
#define def_PbHUB_Port HUB1_ADDR
#define def_PbHUB_DataLine "A"
int PbHUB_Port = def_PbHUB_Port;
String PbHUB_DataLine = def_PbHUB_DataLine;
int PbHUB_Val = 0;

// PaHUB port used for this device
extern int PbHUB_port;

bool PbHUB_connected()
{
  // check HUB is connected
  Wire.beginTransmission(PbHUB_ADDR);
  if (Wire.endTransmission() != 0) return false;
  return true;
}

bool PbHUB_init()
{
  if (PaHUB_active && PbHUB_port >= 0) selectPaHUBChannel(PbHUB_port);
  if (PbHUB_connected())
  {
    PbHUB.begin();
    PbHUB_active = true;
  }
  else PbHUB_active = false;
  return PbHUB_active;
}

void PbHUB_GetParameters()
{
  String param_val;
  param_val = GetParameterValue("PORT", UpperCase);
  if ((param_val != "NOPARAM") && (param_val != "NOVAL"))   // if no parameter or value? Don't change
  {    
    unsigned int PortVal = param_val.toInt();
    if ((PortVal >=0) && (PortVal <= 5)) 
      PbHUB_Port = HUB_ADDR[PortVal];                       // Match the user-port with the translation table
    else
    {
      LastError("Value out of range error, set to default");
    }
  }

  param_val = GetParameterValue("DATALINE", UpperCase);
  if ((param_val != "NOPARAM") && (param_val != "NOVAL"))   // if no parameter or value? Don't change
  { 
    if ((param_val == "A") || (param_val == "B")) 
      PbHUB_DataLine = param_val;
    else
    {
      LastError("Value out of range error, set to default");
    }
  }

  param_val = GetParameterValue("VALUE", UpperCase);
  if ((param_val != "NOPARAM") && (param_val != "NOVAL"))   // if no parameter or value? Don't change
  {    
    unsigned int Val = param_val.toInt();
    if ((Val >=0) && (Val <= 255)) 
      PbHUB_Val = Val;
    else
    {
      LastError("Value out of range error, set to default");
    }
  }
}


String PbdotHUB()
{
  if (PaHUB_active && PbHUB_Port >= 0) selectPaHUBChannel(PbHUB_Port);

  if (!PbHUB_active) PbHUB_init();
  if (!PbHUB_active)
  {
    LastError("Device not found");
    return "-";
  }

  if (command == "INITIALIZE")
  {
    PbHUB_active = false;
    if (PbHUB_init()) return "";
    else
    {
      LastError("Device not found");
      return "-";      
    }
  }
  

  if (command == "ANALOGREAD")
  {
    PbHUB_GetParameters();                                // parse the global parameters
    return String(PbHUB.hub_a_read_value(PbHUB_Port));
  }

  if (command == "DIGITALREAD")
  {
    PbHUB_GetParameters();
    if (PbHUB_DataLine == "A") return String(PbHUB.hub_d_read_value_A(PbHUB_Port)); 
    if (PbHUB_DataLine == "B") return String(PbHUB.hub_d_read_value_B(PbHUB_Port));
    LastError("No valid DataLine value found (A or B)");
    return "-";
  }
    
  if (command == "ANALOGWRITE")
  {
    PbHUB_GetParameters();                                // parse the global parameters
    if (PbHUB_Val < 0) PbHUB_Val = 0;
    if (PbHUB_Val > 255) PbHUB_Val = 255;
    
    if (PbHUB_DataLine == "A") PbHUB.hub_a_wire_value_A(PbHUB_Port, PbHUB_Val);  //
    if (PbHUB_DataLine == "B") PbHUB.hub_a_wire_value_B(PbHUB_Port, PbHUB_Val);  //     
    return "";
  }

  if (command == "DIGITALWRITE")
  {
    PbHUB_GetParameters();                                // parse the global parameters
    if (PbHUB_Val < 0) PbHUB_Val = 0;
    if (PbHUB_Val > 1) PbHUB_Val = 1;
    
    if (PbHUB_DataLine == "A") PbHUB.hub_d_wire_value_A(PbHUB_Port, PbHUB_Val);  //
    if (PbHUB_DataLine == "B") PbHUB.hub_d_wire_value_B(PbHUB_Port, PbHUB_Val);  //     
    return "";
  }



  LastError("No valid command found");
  return "-";
}


//------------------------------------------------------------------------------------------------

