/*
 * ambithread.c
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

#include <vdr/plugin.h>

#include "softhdservice.h"
#include "ambithread.h"
#include "config.h"

//***************************************************************************
// Class cAmbiThread
//***************************************************************************
//***************************************************************************
// Object
//***************************************************************************

cAmbiThread::cAmbiThread()
{
   loopActive = false;
   image = 0;
   imageSize = 0;
   imageWidth = 0;
   imageHeight = 0;
   xBarHeight = 0;
   yBarWidth = 0;
   lastxBarHeight = 0;
   lastyBarWidth = 0;
   barsChanged = true;

   softHdPlugin = cPluginManager::GetPlugin("softhddevice");
   int softHdGrabService = (softHdPlugin && softHdPlugin->Service(ATMO1_GRAB_SERVICE, 0));

   if (!softHdGrabService)
      error("Can't find softhddevice %s!", softHdPlugin ? "service" : "plugin");
}

cAmbiThread::~cAmbiThread()
{
	bob.close();
}

//***************************************************************************
// Stop Thread
//***************************************************************************

void cAmbiThread::Stop()
{   
   loopActive = false;
   waitCondition.Broadcast();    // wakeup the thread

   Cancel(3);                    // wait up to 3 seconds for thread was stopping
}

//***************************************************************************
// Action
//***************************************************************************

void cAmbiThread::Action()
{
   MsTime wait = 0;
   MsTime lastPing = 0;
   MsTime lastBoarderDetect = 0;
   int lastPingResult = na;
   cMutexLock lock(&mutex);

   tell(0, "boblight Thread started (pid=%d)", getpid());

   loopActive = true;
   bob.open();

   while (loopActive && Running())
   {
      MsTime start = msNow();

      if(ShutdownHandler.IsUserInactive() || softhddeviceNotDetached() == fail)
      {
         bob.close();
         waitCondition.TimedWait(mutex, 1000);
         continue;
      }

      // Reduce load, just ping every second
      if(start - lastPing > 1000) {
         lastPing = start;
         lastPingResult = bob.ping();
      }

      // Softhddevice is not detached, work...
      if(lastPingResult == success) {

         if(cfg.dirty > 0) {
            cfg.dirty = 0;
            bob.sendOptions();
         }

         switch(cfg.viewMode) 
         {
            case vmAtmo:
               if (grabImage() == success)
               {
                  if(start - lastBoarderDetect > 5000) {
                     lastBoarderDetect = start;
                     detectCineBars();
                  }
                                    
                  putData();

                  MsTime elapsed = msNow() - start;
                  wait = 1000 / cfg.frequence - elapsed;
                  tell(3, "sleeping %ldms (%d Hz)", wait, cfg.frequence);
               }
               else
               {
                  wait = 10000;   // retry softhd grab every 10 seconds
               }
               break;
            
            case vmBlack:
            case vmFixedCol:
               putData();
               wait = 500; // less load on fixed color or black 
               break;
            
            case vmDetached:
               bob.close();
               wait = 1000;
               break;

            default:
               break;
         }
      }
      else { // Connection lost, reconnect
         wait = 10000;
         switch(cfg.viewMode) 
         {            
            case vmDetached:
               bob.close();
               break;

            default:
               bob.close();
               bob.open();
               wait = 5000;
         }
      }

      waitCondition.TimedWait(mutex, wait);  // wait time in ms
   } 

   bob.close();
   loopActive = false;

   tell(0, "boblight thread ended (pid=%d)", getpid());
}

int cAmbiThread::softhddeviceNotDetached()
{
   int reply_code = 0;
   cString reply_msg;
   reply_msg = softHdPlugin->SVDRPCommand("STAT", "", reply_code);

   if(910 == reply_code)
   {
      tell(1, "Softhddevice NOT detached: %d", reply_code);
      return success;
   }
   else 
   {
      tell(1, "Softhddevice detached: %d", reply_code);
      return fail;
   }
}

//***************************************************************************
// Grab Image
//***************************************************************************

int cAmbiThread::grabImage()
{
   SoftHDDevice_AtmoGrabService_v1_1_t req;

   free(image);
   image = 0;

   // grab image at sofhddevice
   req.width = 64;
   req.height = 64;
   req.img = 0;

   if (!softHdPlugin->Service(ATMO1_GRAB_SERVICE, &req) || !req.img)
      return fail;
   
   tell(3, "Got image with %dx%d pixel; %d bytes", req.width, req.height, req.size);

   image = (Pixel*)req.img;
   imageSize = req.size;
   imageWidth = req.width;
   imageHeight = req.height;
	
   return success;
}

//***************************************************************************
// Detect Cine Bars
//***************************************************************************

int cAmbiThread::detectCineBars()
{
   /*
      Annahme: Wenn im mittleren oberen und unterem Drittel des Bildes alle aufeinander folgenden Pixel schwarz sind
      haben wir einen Horizontalen Balken.

      |0|x x|0|
      |y|0 0|y|
      |y|0 0|y|
      |0|x x|0|

   */
   if (cfg.detectCineBars == cbNone) {
      return done;
   }

   Pixel* p;
   const int xOffset = imageWidth / 4;
   const int yOffset = imageHeight / 4;

   int tempxBarHeight = 0;
   int tempyBarWidth = 0;
   

   if (cfg.detectCineBars == cbHorizontal || cfg.detectCineBars == cbBoth) {
      // check for xBar
      for (int y = 0; y < yOffset; ++y) {
         int row = imageWidth * y;
         int xBarCount = 0;
         for (int x = xOffset; x < imageWidth - xOffset; ++x) {

            p = &image[row + x];
            if(p->isBlack(cfg.cineBarsThreshold)) {
               ++xBarCount;
            }
         }
         if(xBarCount == imageWidth - (2*xOffset)){
            ++tempxBarHeight;
         }
         else {
            break;
         }
      }
   }

   if (cfg.detectCineBars == cbVertical || cfg.detectCineBars == cbBoth) {
      // check for yBar
      for (int x = 0; x < xOffset; ++x) {
         int yBarCount = 0;
         for (int y = yOffset; y < imageHeight - yOffset; ++y) {

            int row = imageWidth * y;
            p = &image[row + x];
            if(p->isBlack(cfg.cineBarsThreshold)) {
               ++yBarCount;
            }
         }
         if(yBarCount == imageHeight - (2*yOffset) ){
            ++tempyBarWidth;
         }
         else {
            break;
         }
      }
   }

   if (tempxBarHeight != lastxBarHeight || tempyBarWidth != lastyBarWidth) {
      barsChanged = true;
      xBarHeight = tempxBarHeight;
      yBarWidth = tempyBarWidth;
   }

   lastxBarHeight = tempxBarHeight;
   lastyBarWidth = tempyBarWidth;

   if(barsChanged) tell(1, "V2 Black border detection horBar: %d verBar: %d", xBarHeight, yBarWidth);
   return done;
}


//***************************************************************************
// Put Data
//***************************************************************************

int cAmbiThread::putData()
{

   if (cfg.viewMode == vmFixedCol)
   {
      int pFixedCol[3] = {cfg.fixedR, cfg.fixedG, cfg.fixedB};
      bob.writePix(pFixedCol);
   }
   else if (cfg.viewMode == vmBlack) {
      int pFixedCol[3] = {0,0,0};
      bob.writePix(pFixedCol);
   }
   else if(cfg.viewMode == vmAtmo) {

   	int row = 0;
      Pixel* p;

   	for (int y = 0; y < imageHeight; y++) {
   		// skip horizontal cinebars
         if(y < xBarHeight || y > imageHeight - xBarHeight) continue;

         int rgb[3];
   		row = imageWidth * y;
   		for (int x = 0; x < imageWidth; x++) {
            // skip vertical cinebars
            if(x < yBarWidth || x > imageWidth - yBarWidth) continue;

   			p = &image[row + x];
   			rgb[0] = p->r;
   			rgb[1] = p->g;
   			rgb[2] = p->b;
   			bob.writeColor(rgb, x - yBarWidth, y - xBarHeight); 
   		}
   	}
      if (barsChanged) {
         bob.setScanRange(imageWidth - (2*yBarWidth), imageHeight - (2*xBarHeight));
         barsChanged = false;
      }
   }

   bob.send();
   
   return success;
}
