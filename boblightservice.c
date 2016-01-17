/*
 * boblightservice.c
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

#include <stdio.h>

#include <libboblight/boblight.h>
#include "boblightservice.h"

//***************************************************************************
// Open/Close
//***************************************************************************

int cBoblight::open()
{
  //init boblight
  m_boblight = boblight_init();
  tell(1, "Successfully loaded and initalized libboblight");

   if (!boblight_connect(m_boblight, NULL, -1, 1000000) || !boblight_setpriority(m_boblight, cfg.priority))
    {
      tell(0, "Error connecting to boblight %s", boblight_geterror(m_boblight));
      close();
      return fail;
    }
   tell(0, "Connected to boblight");
   sendOptions();
   return success;
}

int cBoblight::close()
{
  if(m_boblight != 0) {
    tell(1, "Destroying boblight");
    boblight_destroy(m_boblight);   // calls delete *void
    m_boblight = 0;     
  }            // set pointer to 0
  return success;
}

int cBoblight::ping() {
  if(m_boblight == 0) {
    tell(1, "boblight not initalized");
    return fail;
  }

  if (!boblight_ping(m_boblight, NULL)) {
    tell(0, "Connecting to boblight lost: %s", boblight_geterror(m_boblight));
    return fail;
  }
  return success;
}

//***************************************************************************
// Write Pixel
//***************************************************************************

int cBoblight::writePix(int *rgb)
{
   tell(2, "writePix r:%d g:%d b:%d", rgb[0], rgb[1], rgb[2]);
   boblight_addpixel(m_boblight, -1, rgb);

   return success;
}

int cBoblight::writeColor(int *rgb, int x, int y)
{ 
	boblight_addpixelxy(m_boblight, x, y, rgb);

   return success;
}

int cBoblight::send() {
	if (!boblight_sendrgb(m_boblight, 0, NULL))
    {
      tell(1, "Error sendrgb boblight %s", boblight_geterror(m_boblight));
      return fail;
    }
	return success;
}

int cBoblight::setScanRange(int width, int height) {
  boblight_setscanrange(m_boblight, width, height);
  return success;
}

int cBoblight::sendOptions() {
  if (m_boblight == 0) return fail;
  char buf[32];

  sprintf(buf, "%s %1f", "value", cfg.value * 0.1f);
  boblight_setoption(m_boblight, -1, buf);

  sprintf(buf, "%s %d", "threshold", cfg.threshold);
  boblight_setoption(m_boblight, -1, buf);

  sprintf(buf, "%s %.1f", "gamma", cfg.gamma * 0.1f);
  boblight_setoption(m_boblight, -1, buf);

  sprintf(buf, "%s %1f", "saturation", cfg.saturation * 0.1f);
  boblight_setoption(m_boblight, -1, buf);

  sprintf(buf, "%s %d", "speed", cfg.speed);
  boblight_setoption(m_boblight, -1, buf);

  sprintf(buf, "%s %d", "autospeed", cfg.autospeed);
  boblight_setoption(m_boblight, -1, buf);

  sprintf(buf, "%s %s", "interpolation", cfg.interpolation ? "true" : "false");
  boblight_setoption(m_boblight, -1, buf);

  boblight_setpriority(m_boblight, cfg.priority);

  return success;
}
