/*
 * vdrboblight.h
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
#ifndef __VDRBOBLIGHT_H
#define __VDRBOBLIGHT_H

#include <vdr/plugin.h>

#include "config.h"
#include "ambiservice.h"
#include "ambithread.h"
#include "setup_menu.h"
#include "main_menu.h"

//***************************************************************************
// 
//***************************************************************************

static const char *VERSION        = GITVERSION;
static const char *DESCRIPTION    = "Boblight with data from softhddevice";
static const char *MAINMENUENTRY  = "Boblight";


//***************************************************************************
// Plugin
//***************************************************************************

class cPluginBoblight : public cPlugin 
{
   public:
      
      cPluginBoblight(void);
      virtual ~cPluginBoblight();
      virtual const char* Version(void)          { return VERSION; }
      virtual const char* Description(void)      { return DESCRIPTION; }
      virtual const char* CommandLineHelp(void)  { return 0; }
      virtual bool ProcessArgs(int argc, char* argv[]);
      virtual bool Initialize(void);
      virtual bool Start(void);
      virtual void Stop(void);
      virtual void Housekeeping(void)   { };
      virtual void MainThreadHook(void) { };
      virtual cString Active(void);
      virtual time_t WakeupTime(void);
      virtual const char* MainMenuEntry(void) { return cfg.showMainmenu ?  MAINMENUENTRY : 0; }
      virtual cOsdObject* MainMenuAction(void);
      virtual cMenuSetupPage* SetupMenu(void);
      virtual bool SetupParse(const char* Name, const char* Value);
      virtual bool Service(const char* Id, void* Data = NULL);
      virtual const char** SVDRPHelpPages(void);
      virtual cString SVDRPCommand(const char* Command, const char* Option, int &ReplyCode);

      int startAtmo();
      int stopAtmo();
      void Save();
      cAmbiThread* update;

      int isRunning()
      {
         if (!update)
            return no;

         return update->isRunning();
      }
};

#endif
