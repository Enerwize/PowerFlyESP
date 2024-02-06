### PowerflyESP

This is a custom version of EmonESP modified by CircuitSetup to include ATM90E3X library and modified once again for our own needs.
Not necessarily intented to be used by anyone else but released publicly as some parts can be useful. You're welcome to fork and/or contribute.

Mostly used to prepare for others versions of the hardware :
- More CT per board (12, 18 ?)
- Easier stacking
- Revamped powering with three-phase

Some notes: 
- You must use an ESP32
- To compile, you must have the [ATM90E32 Arduino library](https://github.com/CircuitSetup/Split-Single-Phase-Energy-Meter/tree/master/Software/libraries/ATM90E32) in your Arduino libraries folder.
- If you need to calibrate a specific voltage channel on an add-on board, by default the values from the main board calibration are used, so they would need to have a seperate variable defined, and set manually in energy_meter.cpp

For details on setup, please see the [readme under the Split Single Phase Energy Meter.](https://github.com/CircuitSetup/Split-Single-Phase-Energy-Meter/tree/master/Software/EmonESP)

### Changes from CircuitSetup's EmonESP
- Some branding changes
- Added Home Assistant MQTT auto-discovery (https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery) (Thanks @plapointe6 !)
- Moved config storage from (deprecated) EEPROM library to Preferences
- Slightly changes in networking to make unique and consistent mDNS, DHCP client, MQTT and wifi client name
- Add some defaults to MQTT
- Allow reset of Wifi client config only

Original can be found here : https://github.com/CircuitSetup/Expandable-6-Channel-ESP32-Energy-Meter/tree/master/Software/EmonESP

### To do
- [ ] Get voltages from add-on board(s)
- [ ] Set voltage calibration for add-on boards
- [ ] Allow calibration for add-on boards voltages
- [X] Move calibration storage from EEPROM to Preferences library
- [X] Display complete MQTT client name with MAC on web page
- [X] Change DHCP hostname to match MQTT client name
- [X] Default MQTT base-topic to enerwize/<hostname>
- [X] Default MQTT server to 'homeassistant.local'
- [ ] Correct Firmware web page content to point to this repository
- [ ] Add web server button to switch quickly from 230V 50 Hz to 120V 60Hz and vice-versa
- [ ] Reduce HAMqttDevice refresh rate (only needed once at startup)
- [X] Make OTA works (partition scheme ?)
- [ ] Debug 'save failed' message
- [ ] Show IP address as MQTT topic
- [ ] Reduce log verbosity on serial

### Calibration well known values
- SCT-006 20A/25mA : 10895
- SCT-010 80A/40mA : 27596
- SCT-013 100A/50mA : 28197
