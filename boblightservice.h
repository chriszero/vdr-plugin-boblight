/*
 * boblightservice.h
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

#ifndef __BOBLIGHT_H
#define __BOBLIGHT_H

#include "common.h"
#include "config.h"
#include "ambiservice.h"

class cBoblight : public cAmbiService
{
   public:

   cBoblight();
   ~cBoblight()  { close(); }

   int open();
   int close();

   int writePix(int *rgb);
   int writeColor(int *rgb, int x, int y);
   int setScanRange(int width, int height);
   int ping();
	int send();
   int sendOptions();

   private:
	void* m_boblight;
	
};

#endif