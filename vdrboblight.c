/*
 * vdrboblight.c
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
#include "vdrboblight.h"

#include "setup_menu.h"
#include "main_menu.h"

//***************************************************************************
// Plugin 
//***************************************************************************

cPluginBoblight::cPluginBoblight(void)
{
   update = 0;
}

cPluginBoblight::~cPluginBoblight()
{
   stopAtmo();
}

bool cPluginBoblight::ProcessArgs(int argc, char* argv[])
{
   return true;
}

bool cPluginBoblight::Initialize(void)
{
   return true;
}

bool cPluginBoblight::Start(void)
{
   startAtmo();

   return true;
}

int cPluginBoblight::startAtmo()
{
   if (update)
   {
      update->Stop();
      delete update;
   }

   update = new cAmbiThread();
   update->Start();

   return done;
}

int cPluginBoblight::stopAtmo()
{
   if (update)
      update->Stop();

   delete update;
   update = 0;

   return done;
}

void cPluginBoblight::Save() {
   cfg.dirty = 1;
   SetupStore("LogLevel", cfg.loglevel);
   SetupStore("ShowMainmenu", cfg.showMainmenu);
   SetupStore("StartupViewMode", cfg.startupViewMode);

   SetupStore("DetectCineBars", cfg.detectCineBars);
   SetupStore("CineBarsThreshold", cfg.cineBarsThreshold);

   SetupStore("Updaterate", cfg.frequence);
   SetupStore("Threshold", cfg.threshold);
   SetupStore("Gamma", cfg.gamma);
   SetupStore("Value", cfg.value);
   SetupStore("Saturation", cfg.saturation);
   SetupStore("Speed", cfg.speed);
   SetupStore("Autospeed", cfg.autospeed);
   SetupStore("Interpolation", cfg.interpolation);
   SetupStore("Priority", cfg.priority);

   SetupStore("FixedColorRed", cfg.fixedR);
   SetupStore("FixedColorGreen", cfg.fixedG);
   SetupStore("FixedColorBlue", cfg.fixedB);
}

void cPluginBoblight::Stop(void)
{
   stopAtmo();
}

cString cPluginBoblight::Active(void)
{
  return 0;
}

time_t cPluginBoblight::WakeupTime(void)
{
  return 0;
}

cMenuSetupPage* cPluginBoblight::SetupMenu(void) {
   return new cAmbiSetup(this);
}

cOsdObject* cPluginBoblight::MainMenuAction(void)
{
   return new cBoblightPluginMenu(MAINMENUENTRY, this);
}

bool cPluginBoblight::SetupParse(const char* Name, const char* Value)
{
   if      (!strcasecmp(Name, "LogLevel"))         cfg.loglevel = atoi(Value);
   else if (!strcasecmp(Name, "ShowMainmenu"))     cfg.showMainmenu = atoi(Value);
   else if (!strcasecmp(Name, "StartupViewMode")) {
      cfg.startupViewMode = atoi(Value);
      cfg.viewMode = cfg.startupViewMode;
   }

   else if (!strcasecmp(Name, "DetectCineBars"))   cfg.detectCineBars = (cAmbiService::Cinebars)atoi(Value);
   else if (!strcasecmp(Name, "CineBarsThreshold"))        cfg.cineBarsThreshold = atoi(Value);

   else if (!strcasecmp(Name, "Frequence"))        cfg.frequence = atoi(Value);
   else if (!strcasecmp(Name, "Threshold"))        cfg.threshold = atoi(Value);
   else if (!strcasecmp(Name, "Value"))            cfg.value = atoi(Value);
   else if (!strcasecmp(Name, "Saturation"))       cfg.saturation = atoi(Value);
   else if (!strcasecmp(Name, "Speed"))            cfg.speed = atoi(Value);
   else if (!strcasecmp(Name, "Autospeed"))        cfg.autospeed = atoi(Value);
   else if (!strcasecmp(Name, "Interpolation"))    cfg.interpolation = atoi(Value);
   else if (!strcasecmp(Name, "Priority"))         cfg.priority = atoi(Value);


   else if (!strcasecmp(Name, "FixedColorRed"))    cfg.fixedR = atoi(Value);
   else if (!strcasecmp(Name, "FixedColorGreen"))  cfg.fixedG = atoi(Value);
   else if (!strcasecmp(Name, "FixedColorBlue"))   cfg.fixedB = atoi(Value);

   else
      return false;

   return true;
}

bool cPluginBoblight::Service(const char* Id, void* Data)
{
   return false;
}

cString cPluginBoblight::SVDRPCommand(const char* Command, const char* Option, int &ReplyCode)
{
   if (!strcasecmp(Command, "MODE")) 
   {
      if (Option && strcasecmp(Option, "atmo") == 0) 
      {
         cfg.viewMode = cAmbiService::vmAtmo;
         startAtmo();
         ReplyCode = 550;
         return "atmo mode activated";
      }
      else if (Option && strcasecmp(Option, "fixed") == 0) 
      {
         cfg.viewMode = cAmbiService::vmFixedCol;
         startAtmo();
         ReplyCode = 550;
         return "fixed color activated";
      }
      else if (Option && strcasecmp(Option, "black") == 0) 
      {
         cfg.viewMode = cAmbiService::vmBlack;
         startAtmo();
         
         ReplyCode = 550;
         return "stripes black";
      }
      else if (Option && strcasecmp(Option, "detach") == 0) 
      {
         cfg.viewMode = cAmbiService::vmDetached;
         stopAtmo();
         
         ReplyCode = 550;
         return "stripes detached";
      }
      else
      {
         ReplyCode = 901;
         return "Error: Unexpected option";
      }
   }

   return 0;
}

const char** cPluginBoblight::SVDRPHelpPages(void) 
{ 
   static const char* HelpPages[] = 
   {
      "MODE <mode>\n"
      "    Set mode {atmo|fixed|black|detach}\n",
      0
   };

   return HelpPages;
}


//***************************************************************************
// VDR Internal
//***************************************************************************

VDRPLUGINCREATOR(cPluginBoblight);
