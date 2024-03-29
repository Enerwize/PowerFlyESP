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

// See energy meter specific configuration in energy_meter.h
#define ENABLE_ENERGY_METER

// If USE_SERIAL_INPUT is defined, EmonESP will check the serial port for
// input. Only enable this if there is input expected on the serial port;
// otherwise it seems to read garbage data.
//#define USE_SERIAL_INPUT

#include <esp_task_wdt.h>
#include "emonesp.h"
#include "config.h"
#include "esp_wifi.h"
#include "web_server.h"
#include "ota.h"
#include "input.h"
//#include "emoncms.h"
#include "mqtt.h"

#ifdef ENABLE_ENERGY_METER
#include "energy_meter.h"
#endif

static char input[MAX_DATA_LEN];

// -------------------------------------------------------------------
// SETUP
// -------------------------------------------------------------------
void setup() {
#ifdef ENABLE_WDT
  enableLoopWDT();
#endif

  Serial.begin(115200);
#ifdef DEBUG_SERIAL1
  Serial1.begin(115200);
#endif
  delay(200);

  // Read saved settings from the config
  config_load_settings();
  delay(200);

  // Initialise the WiFi
  wifi_setup();
  delay(200);

  // Bring up the web server
  web_server_setup();
  delay(200);

#ifdef ENABLE_WDT
  DBUGS.println("Watchdog timer is enabled.");
  feedLoopWDT();
#endif

#ifdef ENABLE_WEB_OTA
  // Start the OTA update systems
  ota_setup();
#endif

#ifdef ENABLE_ENERGY_METER
  energy_meter_setup();
#endif

  DBUGLN("Server started");

} // end setup

// -------------------------------------------------------------------
// LOOP
// -------------------------------------------------------------------
void loop()
{
#ifdef ENABLE_WDT
  feedLoopWDT();
#endif
  web_server_loop();
  wifi_loop();

#ifdef ENABLE_WEB_OTA
  ota_loop();
#endif

#ifdef ENABLE_ENERGY_METER
  energy_meter_loop();
#endif

  boolean gotInput = input_get(input);
  if (gotInput) {
    DBUGS.println(".");
  }

  if (wifi_client_connected()) {
    if (mqtt_server != 0) {
      mqtt_loop();
      if (gotInput) {
        mqtt_publish(input);
      }
    }
  }

} // end loop
