/*
 * ambithread.h
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
#pragma once
#include <queue>

#include <vdr/thread.h>
#include <vdr/shutdown.h>

#include "common.h"
#include "config.h"
#include "Pixel.h"
#include "boblightservice.h"


//***************************************************************************
// SEDU Thread
//***************************************************************************

class cAmbiThread : public cThread, public cAmbiService
{
   public:

      cAmbiThread();
      ~cAmbiThread();

      int isRunning() { return Running(); }

      // interface

      void Stop();
     
   private:

      void Action(void);

      int grabImage();
      int detectCineBars();
      int getOsd3DMode();
      int putData();
      int softhddeviceNotDetached();

      // data
	cBoblight bob;

      cPlugin* softHdPlugin;

      cMutex mutex;
      cCondVar waitCondition;
      int loopActive;

      Pixel* image;
      int xBarHeight;
      int yBarWidth;

      int lastxBarHeight;
      int lastyBarWidth;
      
      int osd3DMode;

      bool barsChanged;
      bool osd3dChanged;

      int imageSize;
      int imageWidth;
      int imageHeight;
};
