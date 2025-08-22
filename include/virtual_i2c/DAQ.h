#pragma once

/* --- DAQ virtual unit ---------------------------------------------------------------------------
 * This is a virtual I2C device that is present when the Service menu
 * Device Type is set to "DAQ". It's implemented to support an in-house built
 * DAQ interface console.
 *
 * Implemented command strings:
 *   >DAQ.GET_DIGITAL_STATE(port=1|2)
 *   >DAQ.SET_DIGITAL_STATE(port=1|2,state=0|1)
 *   >DAQ.GET_ANALOG_VOLTAGE(port=1|2)
 *   >DAQ.SET_ANALOG_VOLTAGE(port=1|2,voltage=[0,10])
 *   >DAQ.GET_PHOTOGATE(port=1|2)
 *   >DAQ.GET_FORCE()
 */


static int DAQ_parse_port()
{
  String p = GetParameterValue("PORT", 'U');
  if (p == "NOPARAM" || p == "NOVAL") return -1;
  int port = p.toInt();
  if (port != 1 && port != 2) return -1;
  return port;
}

static int DAQ_parse_state()
{
  String s = GetParameterValue("STATE", 'U');
  if (s == "NOPARAM" || s == "NOVAL") return -1;
  int state = s.toInt();
  if (state != 0 && state != 1) return -1;
  return state;
}

static bool DAQ_get_digital(int gpio)
{
  pinMode(gpio, INPUT);
  return digitalRead(gpio) ? true : false;
}

static String DAQ_get_digital_state()
{
  int port = DAQ_parse_port();
  if (port == -1) { LastError("No valid parameter value found"); return "-"; }
  int gpio = (port == 1) ? 5 : 2; // port1->GPIO5, port2->GPIO2
  bool val = DAQ_get_digital(gpio);
  return String(val ? 1 : 0);
}

static String DAQ_set_digital_state()
{
  int port = DAQ_parse_port();
  int state = DAQ_parse_state();
  if (port == -1 || state == -1) { LastError("No valid parameter value found"); return "-"; }
  int gpio = (port == 1) ? 16 : 17; // port1->GPIO16, port2->GPIO17
  pinMode(gpio, OUTPUT);
  digitalWrite(gpio, state ? HIGH : LOW);
  return String(state);
}

static float DAQ_read_analog_voltage_for_port(int port)
{
  int gpio = (port == 1) ? 36 : 35; // port1->GPIO36, port2->GPIO35
  int raw = analogRead(gpio);       // 0..4095 
  // Map 0..4095 -> -10..10
  float v = ((float)raw / 4095.0f) * 20.0f - 10.0f;
  return v;
}

static String DAQ_get_analog_voltage()
{
  int port = DAQ_parse_port();
  if (port == -1) { LastError("No valid parameter value found"); return "-"; }
  float v = DAQ_read_analog_voltage_for_port(port);
  return String(v, 3);
}

static String DAQ_set_analog_voltage()
{
  int port = DAQ_parse_port();
  if (port == -1) { LastError("No valid parameter value found"); return "-"; }
  String sval = GetParameterValue("VOLTAGE", 'U');
  if (sval == "NOPARAM" || sval == "NOVAL") { LastError("No valid parameter value found"); return "-"; }
  float v = sval.toFloat();
  if (v < 0.0f) v = 0.0f;
  if (v > 10.0f) v = 10.0f;
  int gpio = (port == 1) ? 25 : 26; // DAC pins
  int dac = (int)roundf((v / 10.0f) * 255.0f); // 0..10 -> 0..255
  if (dac < 0) dac = 0; if (dac > 255) dac = 255;
  dacWrite(gpio, (uint8_t)dac);
  return String(v, 3);
}

static String DAQ_get_photogate()
{
  // Same mapping as GET_DIGITAL_STATE
  return DAQ_get_digital_state();
}

static String DAQ_get_force()
{
  // Return raw ADC value on GPIO36
  int raw = analogRead(36);
  return String(raw);
}

String DAQ()
{
  pinMode(5, INPUT_PULLUP);       
  pinMode(2, INPUT_PULLUP);       
  pinMode(16, OUTPUT);  
  pinMode(17, OUTPUT);  
  pinMode(35, INPUT);  
  pinMode(36, INPUT);  
  pinMode(25, OUTPUT);  
  pinMode(26, OUTPUT);  

  if (command == "GET_DIGITAL_STATE")   return DAQ_get_digital_state();
  if (command == "SET_DIGITAL_STATE")   return DAQ_set_digital_state();
  if (command == "GET_ANALOG_VOLTAGE")  return DAQ_get_analog_voltage();
  if (command == "SET_ANALOG_VOLTAGE")  return DAQ_set_analog_voltage();
  if (command == "GET_PHOTOGATE")       return DAQ_get_photogate();
  if (command == "GET_FORCE")           return DAQ_get_force();

  LastError("No valid command found");
  return "-";
}
