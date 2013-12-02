/*
 * config.c
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

#include <string.h>

#include "config.h"
#include "common.h"


//***************************************************************************
// Global Configuration
//***************************************************************************

cBobConfig cfg;

//***************************************************************************
// cConfigData
//***************************************************************************

cBobConfig::cBobConfig()
{
   // to be configured
   frequence = 25;
   threshold = 20;
   gamma = 10;
   value = 80;
   saturation = 30;
   speed = 60;
   autospeed = 0;
   interpolation = 1; //bool
   priority = 128;

   detectCineBars = cbBoth;
   
   loglevel = 0;

   showMainmenu = yes;
   viewMode = vmAtmo;
   fixedR = 111;
   fixedG = 101;
   fixedB = 0;

   dirty = 0;
}
