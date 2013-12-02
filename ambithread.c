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
   cineBarsHor = 0;
   cineBarsVer = 0;
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
   cMutexLock lock(&mutex);

   tell(0, "boblight Thread started (pid=%d)", getpid());

   loopActive = true;
   bob.open();

   while (loopActive && Running())
   {
      MsTime start = msNow();

      // work ...
      if(bob.ping() == success) {

         if(cfg.dirty > 0) {
            cfg.dirty = 0;
            bob.sendOptions();
         }

         if (cfg.viewMode == vmAtmo)
         {
            if (grabImage() == success)
            {
               detectCineBars();
               putData();

               MsTime elapsed = msNow() - start;
               wait = 1000 / cfg.frequence - elapsed;
               tell(3, "sleeping %ldms (%d Hz)", wait, cfg.frequence);
            }
            else
            {
               wait = 10000;   // retry softhd grab every 10 seconds
            }
         }
         else
         {
            putData();
            wait = 500; // less load on fixed color or black 
         }
      }
      else { // Connection lost, reconnect
         bob.close();
         bob.open();
         wait = 10000;
      }

      waitCondition.TimedWait(mutex, wait);  // wait time in ms
   } 

   bob.close();
   loopActive = false;

   tell(0, "boblight thread ended (pid=%d)", getpid());
}

//***************************************************************************
// Grab Image
//***************************************************************************

int cAmbiThread::grabImage()
{
   SoftHDDevice_AtmoGrabService_v1_1_t req;

   free(image);
   image = 0;

   cPlugin* softHdPlugin = cPluginManager::GetPlugin("softhddevice");
   int softHdGrabService = (softHdPlugin && softHdPlugin->Service(ATMO1_GRAB_SERVICE, 0));

   if (!softHdGrabService)
      return error("Can't find softhddevice %s, aborting grab, retrying in 10 seconds!", 
                   softHdPlugin ? "service" : "plugin");
 
   // grab image at sofhddevice
   req.width = -64; //warum? steht hier => http://projects.vdr-developer.org/projects/plg-softhddevice/repository/revisions/master/entry/video.c#L7372
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
   const int threshold = 3;    // threshold for black level of cine bars
   Pixel* p;
   int off;

   // check horizontal bars

   if (cfg.detectCineBars == cbHorizontal || cfg.detectCineBars == cbBoth)
   {
      for (off = 0; off < imageHeight/5; off++)  // cinebar height max 1/5 of the screen height
      {
         int above = 0;
         
         for (int x = 0; x < imageWidth; x++)
         {
            p = &image[off*imageWidth + x];
            
            if (p->r > threshold || p->g > threshold || p->b > threshold)
               above++;
            
            p = &image[((imageHeight-1)-off)*imageWidth + x];
            
            if (p->r > threshold || p->g > threshold || p->b > threshold)
               above++;
         }
         
         if (above > imageWidth/8)              // max 1/8 failed pixel
            break;
      }
      
      if (cineBarsHor != off)
      {
         static int last = 0;
         static int count = 0;

         if (off != last)
         {
            last = off;
            count = 0;
         }

         if (count++ >= cfg.frequence)
         {
            count = 0;
            cineBarsHor = off;
            tell(0, "Switch horizontal cine bars to %d", cineBarsHor);
         }
      }
   }

   // check vertical bars
   
   if (cfg.detectCineBars == cbVertical || cfg.detectCineBars == cbBoth)
   {
      for (off = 0; off < imageWidth/5; off++)    // cinebar height max 1/5 of the screen width
      {
         int above = 0;
         
         for (int y = 0; y < imageHeight; y++)
         {
            p = &image[y*imageWidth + off];
            
            if (p->r > threshold || p->g > threshold || p->b > threshold)
               above++;
            
            p = &image[y*imageWidth + ((imageWidth-1)-off)];
            
            if (p->r > threshold || p->g > threshold || p->b > threshold)
            above++;
         }
         
         if (above > imageHeight/6)         // max 1/6 failed pixel
            break;
      }
      
      if (cineBarsVer != off)
      {
         static int last = 0;
         static int count = 0;

         if (off != last)
         {
            last = off;
            count = 0;
         }

         if (count++ >= cfg.frequence)
         {
            count = 0;

            cineBarsVer = off;
            tell(0, "Switch vertical cine bars to %d", cineBarsVer);
         }
      }
   }

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
      Pixel pixel = {0,0,0,0};
      Pixel* p = &pixel;

   	for (int y = 0; y < imageHeight; y++) {
   		// skip horizontal cinebars
         if(y < cineBarsHor) continue;
         if(y > imageHeight - cineBarsHor) continue;

         int rgb[3];
   		row = imageWidth * y;
   		for (int x = 0; x < imageWidth; x++) {
            // skip vertical cinebars
            if(x < cineBarsVer) continue;
            if(x > imageWidth - cineBarsVer) continue;

   			p = &image[row + x];
   			rgb[0] = p->r;
   			rgb[1] = p->g;
   			rgb[2] = p->b;
   			bob.writeColor(rgb, x - cineBarsVer, y - cineBarsHor); 
   		}
   	}
      bob.setScanRange(imageWidth - (2*cineBarsVer), imageHeight - (2*cineBarsHor));
   }

   bob.send();
   
   return success;
}
