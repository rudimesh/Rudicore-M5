// Core utilities: error reporting, I/O, parsing, and BT helpers
#pragma once
#include "I2CScanner.h"

// Error reporting
void LastError(String error)
{
  ErrorText = error;
  Last_Error = "Error: '" + error;
}

// Output selection (USB serial vs Bluetooth)
extern unsigned long LastTransferTime;

void note_data_transfer()
{
  LastTransferTime = millis();
}

void print2serial(String data)
{
  LastResult = data;
  note_data_transfer();
  if (BT_Active) BT.println(data);
  else Serial.println(data);
}

void write2serial(uint8_t data)
{
  note_data_transfer();
  if (BT_Active) BT.write(data);
  else Serial.write(data);
}

// Busy-wait until delay has expired
void non_blocking_delay(long delay)
{
  long goal = millis() + delay;
  while (goal > millis());
}

// Parameter parsing from global 'param' string
String GetParameterValue(String parameter, char UpperCase = 'l')
{
  String val;
  parameter.toUpperCase();

  param.trim();

  if (parameter == "VALUEONLY") return param;
  
  String _param = param;
  _param.toUpperCase();

  int lenght_param = _param.length();
  if (lenght_param == 0) return "NOPARAM";
  
  int index_parameter = _param.indexOf(parameter);
  if (index_parameter == -1) return "NOPARAM";
  
  _param = param;
  _param = _param.substring(index_parameter, lenght_param);

  int index_comma = _param.indexOf(',');
  if (index_comma > 0) _param =  _param.substring(parameter.length(), index_comma);

  int index_issign = _param.indexOf('=');
  if (index_issign >= 0) 
  {
    _param =  _param.substring(index_issign+1, _param.length());
    _param.trim();
    if (_param.length() == 0) return "NOVAL";

    if ((UpperCase == 'U') || (UpperCase == 'u')) _param.toUpperCase();
    return _param;
  }
  else return "NOVAL";
}

// Debug print of parsed fields
void show_parsed_input()
{
  Serial.print("Captured command String is : ");
  Serial.println(readString);
  Serial.print("sensor : ");
  Serial.println(sensor);
  Serial.print("command : ");
  Serial.println(command);
  Serial.print("parameter(s) : ");
  Serial.println(param);
  Serial.println("---------------------------------");
}

// Parse command string: ">device.command(parameters)"
void parse_input()
{
  int index_1 = readString.indexOf('.');
  sensor = readString.substring(0, index_1);
  int index_2 = readString.indexOf('(', index_1+1 );
  command = readString.substring(index_1+1, index_2);
  int index_3 = readString.indexOf(')', index_2+1 );
  param = readString.substring(index_2+1, index_3);

  sensor.toUpperCase();
  command.toUpperCase();

  LastSensor = sensor;
  LastCommand = command;
  LastParameter = param;

  if (debug) show_parsed_input();

  last_commandstring = readString;
  readString = "";
}

// Read user input from USB serial or Bluetooth
void read_user_input() 
{
  sensor = "";
  command = "";
  param = "";

  char c=' ';
  
  while ((Serial.available()) || (BT.available()))
  {
    show_time = millis() + DisplayRefreshInterval;
    if (Serial.available())
    {
      c = Serial.read();
      BT_Active = false;
      note_data_transfer();
    }
    if (BT.available())
    {
      c = BT.read();
      BT_Active = true;
      note_data_transfer();
    }
    
    if (c == '>')
    {
      readString="";
      sensor="";
      command="";
      param=""; 
      LastSensor = sensor;
      LastCommand = command;
      LastParameter = param;
    }
    else
      readString += c;
    
    if (c == ')')
    { 
      parse_input();
      return;
    } 
  }
}

// Discard pending characters in both serial buffers
void FlushInput()
{
  Serial.flush();  
  BT.flush();
}

// Construct Bluetooth device name and set globals
char * BTNameConstruct()
{
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_BT);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  BT_Address = baseMacChr;

  char * BTName = (char *) malloc (24);
  strcpy(BTName, "Rudicore-M5_");
  char c_tmp[10];
  sprintf(c_tmp, "%02X:%02X:%02X", baseMac[3], baseMac[4], baseMac[5]);
  strcat(BTName, c_tmp);
  BT_Name = String(BTName);

  return BTName;
}

// Friendly device names
static const char *FriendlyNameList[] = {
    "Ada", "Alan", "Albert", "Alexander", "Alexis", "Alfred", "Andreas", "Antoine", "Anton", "Antonie", "Archimedes", "Arthur", "Augustin", "Barbara", "Benjamin", "Bernhard", "Blaise", "Brian", "Buckminster", "Carl", "Carol", "Charles", "Christiaan", "Christiane", "Claude",
    "Daniel", "David", "Dian", "Dorothy", "Edward", "Edwin", "Elizabeth", "Emma", "Enrico", "Erik", "Ernst", "Ernest", "Erwin", "Francis", "Frank", "Frederik", "Fritz", "Galileo", "Gaspard", "George", "Gerard", "Gertrude", "Gerty", "Gottfried", "Grace", "Gregor", "Gustav",
    "Hans", "Hendrik", "Henri", "Hermann", "Hooke", "Humphry", "Irene", "Isaac", "Jack", "Jacques", "James", "Jane", "Jean", "Jennifer", "Johannes", "John", "Jonas", "Joseph", "Josiah", "Julian", "Katherine", "Karl", "Kjeld", "Klaas", "Konrad", "Leon", "Leonardo", "Lev", "Linus",
    "Lise", "Louis", "Macy", "Maarten", "Margaret", "Marie", "Mary", "Maurice", "Max", "Milo", "Michael", "Nikola", "Nikolaas", "Neil", "Niels", "Norbert", "Olivier", "Otto", "Paul", "Peter", "Pierre", "Pythagoras", "Rachel", "Reinier", "Richard", "Rita", "Rafael", "Robert",
    "Roger", "Rosalinde", "Rudolf", "Severo", "Sheldon", "Stephanie", "Stefan", "Steven", "Thomas", "Tycho", "Vera", "Vincent", "Walter", "Werner", "Wim", "Wilhelm", "Wilbert", "William", "Wolfgang"
};

static const size_t NumFriendlyNames = sizeof(FriendlyNameList) / sizeof(FriendlyNameList[0]);

String FriendlyNameConstruct()
{
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_BT);

  uint8_t last_mac_byte = baseMac[5];
  int name_index = (last_mac_byte + FriendlyNameOffset) % NumFriendlyNames;
  return String(FriendlyNameList[name_index]);
}
