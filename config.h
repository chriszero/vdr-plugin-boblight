/*
 * config.h
 *
 * Copyright (C) 2013 - Christian Völlinger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BOB_CONFIG_H
#define __BOB_CONFIG_H

#include "common.h"
#include "ambiservice.h"

//***************************************************************************
// Configuration
//***************************************************************************

class cBobConfig : public cAmbiService
{
   public:

      cBobConfig();

      // adjust 

      int frequence;
      int threshold;
      int gamma;
      int value;
      int saturation;
      int speed;
      int autospeed;
      
      int interpolation; //bool
      int priority;

      // technical

      int viewMode;
      int startupViewMode;
      int fixedR;
      int fixedG;
      int fixedB;

      int showMainmenu; //bool
      int detectCineBars;
      int cineBarsThreshold;

      int loglevel;

      int dirty;

};

//***************************************************************************
// Global Configuration
//***************************************************************************

extern cBobConfig cfg;

#endif
