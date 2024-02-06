/*
   -------------------------------------------------------------------
   EmonESP Serial to Emoncms gateway
   -------------------------------------------------------------------
   Adaptation of Chris Howells OpenEVSE ESP Wifi
   by Trystan Lea, Glyn Hudson, OpenEnergyMonitor

   Modified to use with the CircuitSetup.us Split Phase Energy Meter by jdeglavina

   All adaptation GNU General Public License as below.

   -------------------------------------------------------------------

   This file is part of OpenEnergyMonitor.org project.
   EmonESP is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.
   EmonESP is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with EmonESP; see the file COPYING.  If not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "emonesp.h"
#include "energy_meter.h" // Energy meters config default values
#include "mqtt.h" // MQTT config default values
#include "config.h"
#include "esp_wifi.h"

// Wifi Network Strings
String esid = "";
String epass = "";

// Web server authentication (leave blank for none)
String www_username = "";
String www_password = "";

// MQTT Settings
String mqtt_server = "";
String mqtt_topic = "";
String mqtt_user = "";
String mqtt_pass = "";
String mqtt_feed_prefix = "";

// Calibration Settings
unsigned short voltage_cal = 0;
unsigned short voltage2_cal = 0;
unsigned short freq_cal = 0;
unsigned short gain_cal[NUM_BOARDS] = { 0 };
String ct_name[NUM_CHANNELS] = { };
unsigned short ct_cal[NUM_CHANNELS] = { 0 };
float cur_mul[NUM_CHANNELS] = { 0.0 };
float pow_mul[NUM_CHANNELS] = { 0.0 };

//Configuration flags
struct config_flags_t config_flags = { 0 };

Preferences preferences;

// -------------------------------------------------------------------
// Load saved preferences
// -------------------------------------------------------------------
void config_load_settings()
{
  char name[12];
  
  // Load WiFi values
  preferences.begin("wifi", true);
  esid = preferences.getString("wifi_essid","");
  epass = preferences.getString("wifi_pass","");
  preferences.end();

  // MQTT settings
  preferences.begin("mqtt", true);
  mqtt_server = preferences.getString("mqtt_server",MQTT_SERVER_DEFAULT);
  mqtt_topic = preferences.getString("mqtt_topic",MQTT_TOPIC_DEFAULT);
  mqtt_feed_prefix = preferences.getString("mqtt_feed_prefix","");
  mqtt_user = preferences.getString("mqtt_user","");
  mqtt_pass = preferences.getString("mqtt_pass","");
  // Configuration flags
  *((uint16_t *)&config_flags) = preferences.getUShort("config_flags", 0);
  preferences.end();

  // Calibration settings
  preferences.begin("calibration", true);
  voltage_cal = preferences.getUShort("voltage_cal", VOLTAGE_GAIN_DEFAULT);
  voltage2_cal = preferences.getUShort("voltage2_cal", VOLTAGE_GAIN_DEFAULT);
  freq_cal = preferences.getUShort("freq_cal", LINE_FREQ_DEFAULT);
  for (int i = 0; i < NUM_CHANNELS; i++)
  {
    sprintf(name, "ct%d_name", i+1);
    ct_name[i] = preferences.getString(name,"ct" + String(i+1));
    sprintf(name, "ct%d_cal", i+1);
    ct_cal[i] = preferences.getUShort(name, CURRENT_GAIN_DEFAULT);
    sprintf(name, "cur%d_mul", i+1);
    cur_mul[i] = preferences.getFloat(name, 1.0);
    sprintf(name, "pow%d_mul", i+1);
    pow_mul[i] = preferences.getFloat(name, 1.0);
  }
  for (int i = 0; i < NUM_BOARDS; i++)
  {
    sprintf(name, "gain%d_cal", i);
    gain_cal[i] = preferences.getUShort(name, PGA_GAIN_DEFAULT);
  }
  preferences.end();

  preferences.begin("admin", false);
  www_username = preferences.getString("admin_user");
  www_password = preferences.getString("admin_pass");
  preferences.end();
}

void config_save_mqtt(String server, String topic, String prefix, String user, String pass, bool json)
{
  config_flags.mqtt_json = json;
  mqtt_server = server;
  mqtt_topic = topic;
  mqtt_feed_prefix = prefix;
  mqtt_user = user;
  mqtt_pass = pass;

  preferences.begin("mqtt", false);
  preferences.putString("mqtt_server",mqtt_server);
  preferences.putString("mqtt_topic",mqtt_topic);
  preferences.putString("mqtt_feed_prefix",mqtt_feed_prefix);
  preferences.putString("mqtt_user",mqtt_user);
  preferences.putString("mqtt_pass",mqtt_pass);
  preferences.putUShort("config_flags",*((uint16_t *)&config_flags));
  preferences.end();
}

//for CircuitSetup 6 channel energy meter
void config_save_cal(AsyncWebServerRequest * request)
{
  char req[12];

  preferences.begin("calibration", false);

  voltage_cal = request->arg("voltage_cal").toInt();
  voltage2_cal = request->arg("voltage2_cal").toInt();
  freq_cal = request->arg("freq_cal").toInt();

  preferences.putUShort("voltage_cal",voltage_cal);
  preferences.putUShort("voltage2_cal",voltage2_cal);
  preferences.putUShort("freq_cal",freq_cal);

  for (int i = 0; i < NUM_CHANNELS; i++)
  {
    sprintf(req, "ct%d_name", i+1);
    ct_name[i] = request->arg(req);
    preferences.putString(req,ct_name[i]);

    sprintf(req, "ct%d_cal", i+1);
    ct_cal[i] = request->arg(req).toInt();
    preferences.putUShort(req, ct_cal[i]);

    sprintf(req, "cur%d_mul", i+1);
    cur_mul[i] = request->arg(req).toFloat();
    preferences.putFloat(req, cur_mul[i]);

    sprintf(req, "pow%d_mul", i+1);
    pow_mul[i] = request->arg(req).toFloat();
    preferences.putFloat(req, pow_mul[i]);
  }

  for (int i = 0; i < NUM_BOARDS; i++)
  {
    byte pgaA, pgaB, pgaC;

    sprintf(req, "gain%d_cal", i*NUM_INPUTS+1);
    pgaA = request->arg(req).toInt() >> 1;
    sprintf(req, "gain%d_cal", i*NUM_INPUTS+2);
    pgaB = request->arg(req).toInt() >> 1;
    sprintf(req, "gain%d_cal", i*NUM_INPUTS+3);
    pgaC = request->arg(req).toInt() >> 1;

    gain_cal[i] = pgaA | (pgaB << 2) | (pgaC << 4);

    sprintf(req, "gain%d_cal", i*NUM_INPUTS+4);
    pgaA = request->arg(req).toInt() >> 1;
    sprintf(req, "gain%d_cal", i*NUM_INPUTS+5);
    pgaB = request->arg(req).toInt() >> 1;
    sprintf(req, "gain%d_cal", i*NUM_INPUTS+6);
    pgaC = request->arg(req).toInt() >> 1;

    gain_cal[i] |= (pgaA << 8) | (pgaB << 10) | (pgaC << 12);

    sprintf(req, "gain%d_cal", i);
    preferences.putUShort(req, gain_cal[i]);
  }

  preferences.end();
}

void config_save_admin(String user, String pass)
{
  www_username = user;
  www_password = pass;

  preferences.begin("admin", false);
  preferences.putString("admin_user",user);
  preferences.putString("admin_pass",pass);
  preferences.end();
}

void config_save_wifi(String qsid, String qpass)
{
  esid = qsid;
  epass = qpass;
  preferences.begin("wifi", false);
  preferences.putString("wifi_essid",qsid);
  preferences.putString("wifi_pass",qpass);
  preferences.end();
}

void config_reset()
{
  preferences.begin("calibration", false);
  preferences.clear();
  preferences.end();
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();
  preferences.begin("admin", false);
  preferences.clear();
  preferences.end();
  preferences.begin("mqtt", false);
  preferences.clear();
  preferences.end();
}

void wifi_config_reset()
{
  esid = "";
  epass = "";
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();
}