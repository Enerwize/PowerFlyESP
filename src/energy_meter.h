/*
   -------------------------------------------------------------------
   EmonESP Serial to Emoncms gateway
   -------------------------------------------------------------------
   Created for use with the CircuitSetup.us Split Phase Energy Meter by jdeglavina

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

#ifndef _ENERGY_METER
#define _ENERGY_METER

#define NUM_BOARDS 7
#define NUM_INPUTS 6

#define NUM_CHANNELS (NUM_BOARDS*NUM_INPUTS)

//#define ENABLE_OLED_DISPLAY

/*
   The following calibration values can be set here or in the EmonESP interface
   EmonESP values take priority if they are set
*/
/*
   4231 for 60 hz 6 channel meter 
   135 for 50 hz 6 channel meter
*/

#define LINE_FREQ_DEFAULT 135

/*
   Sets the current gain to 1x, 2x, or 4x. If your CT has a low current output use 2x or 4x.
   0 (1x)
   21 (2x)
   42 (4x)
*/
#define PGA_GAIN_DEFAULT 0

/*
   For meter <= v1.2:
      42080 - 9v AC Transformer - Jameco 112336
      32428 - 12v AC Transformer - Jameco 167151
   For meter > v1.3:
      7305 - 9v AC Transformer - Jameco 157041
*/
#define VOLTAGE_GAIN_DEFAULT 13839

/*
 When PGA Gain is set to 0
  20A/25mA SCT-006: 11131
  30A/1V SCT-013-030: 8650
  50A/1V SCT-013-050: 15420
  80A/26.6mA SCT-010: 41996
  100A/50mA SCT-013-000: 27961
  120A/40mA: SCT-016: 41880
*/
#define CURRENT_GAIN_DEFAULT 27961

extern void energy_meter_setup();
extern void energy_meter_loop();

#endif
