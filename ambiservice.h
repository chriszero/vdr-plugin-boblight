/*
 * ambiservice.c
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

#ifndef __AMBI_SERVICE_H
#define __AMBI_SERVICE_H

class cAmbiService
{
   public:

      enum Cinebars
      {  
         cbNone,
         cbHorizontal,
         cbVertical,
         cbBoth,
         cbCount
      };

      enum ViewMode
      {
         vmAtmo,
         vmFixedCol,
         vmBlack,
         vmDetached,
         vmCount
      };

      // static
      static const char* viewModes[];
      static const char* cineBars[];
};

#endif // __AMBI_SERVICE_H
