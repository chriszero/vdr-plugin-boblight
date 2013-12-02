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

#include "config.h"
#include "ambiservice.h"
#include "ambithread.h"

//***************************************************************************
// 
//***************************************************************************

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Boblight with data from softhddevice";
static const char *MAINMENUENTRY  = "Boblight";

//***************************************************************************
// Setup
//***************************************************************************

class cAmbiSetup : public cMenuSetupPage, public cAmbiService
{
   public:

      cAmbiSetup();

   protected:

      virtual void Setup();
      virtual eOSState ProcessKey(eKeys Key);
      virtual void Store();

      const char* cineBars[cbCount];
      const char* seduRGBOrders[6];
      int rgbOrderIndex;
};

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
      virtual cMenuSetupPage* SetupMenu(void) { return new cAmbiSetup; }
      virtual bool SetupParse(const char* Name, const char* Value);
      virtual bool Service(const char* Id, void* Data = NULL);
      virtual const char** SVDRPHelpPages(void);
      virtual cString SVDRPCommand(const char* Command, const char* Option, int &ReplyCode);

      int startAtmo();
      int stopAtmo();
      cAmbiThread* update;

      int isRunning()
      {
         if (!update)
            return no;

         return update->isRunning();
      }
};

//***************************************************************************
// Plugins Main Menu
//***************************************************************************

class cSeduPluginMenu : public cMenuSetupPage
{
   public:

      cSeduPluginMenu(const char* title, cPluginBoblight* aPlugin);
      virtual ~cSeduPluginMenu() { };
      
      virtual eOSState ProcessKey(eKeys key);

   protected:

      void Store() { }
      cPluginBoblight* plugin;
};

cSeduPluginMenu::cSeduPluginMenu(const char* title, cPluginBoblight* aPlugin)
{
   SetTitle(title ? title : "");
   plugin = aPlugin;

   Clear();

   cOsdMenu::Add(new cMenuEditStraItem(tr("View Mode"), 
                                       (int*)&cfg.viewMode, 
                                       (int)cAmbiService::vmCount,
                                       cAmbiService::viewModes));

   Add(new cMenuEditIntItem(tr("Fixed Color Red"), &cfg.fixedR, 0, 255));
   Add(new cMenuEditIntItem(tr("Fixed Color Green"), &cfg.fixedG, 0, 255));
   Add(new cMenuEditIntItem(tr("Fixed Color Blue"), &cfg.fixedB, 0, 255));

   SetHelp(0, 0, 0, 0);

   Display();
}

//***************************************************************************
// Process Key
//***************************************************************************

eOSState cSeduPluginMenu::ProcessKey(eKeys key)
{
   eOSState state = cOsdMenu::ProcessKey(key);
   
   if (key == kLeft || key == kRight)
   {
      if (cfg.viewMode == cAmbiService::vmDetached && plugin->isRunning())
         plugin->stopAtmo();
      else if (cfg.viewMode != cAmbiService::vmDetached && !plugin->isRunning())
         plugin->startAtmo();
   }

   if (state != osUnknown)
      return state;

   if (key == kOk)
   {
      SetupStore("FixedColorRed", cfg.fixedR);
      SetupStore("FixedColorGreen", cfg.fixedG);
      SetupStore("FixedColorBlue", cfg.fixedB);
      SetupStore("ViewMode", (int)cfg.viewMode);

      return osEnd;
   }
   return state;
}

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

cOsdObject* cPluginBoblight::MainMenuAction(void)
{
   return new cSeduPluginMenu(MAINMENUENTRY, this);
}

bool cPluginBoblight::SetupParse(const char* Name, const char* Value)
{
   if      (!strcasecmp(Name, "LogLevel"))         cfg.loglevel = atoi(Value);
   else if (!strcasecmp(Name, "ShowMainmenu"))     cfg.showMainmenu = atoi(Value);
   else if (!strcasecmp(Name, "ViewMode"))         cfg.viewMode = (cAmbiService::ViewMode)atoi(Value);

   else if (!strcasecmp(Name, "DetectCineBars"))   cfg.detectCineBars = (cAmbiService::Cinebars)atoi(Value);

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
   if (!update)
      return "Error: Plugin not initialized!";

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
// Class Setup Menu
//***************************************************************************
//***************************************************************************
// Object
//***************************************************************************

cAmbiSetup::cAmbiSetup()
{
   cineBars[0] = "Horizontal";
   cineBars[1] = "Vertical";
   cineBars[2] = "Both";

   Setup();
}

//***************************************************************************
// Setup
//***************************************************************************

void cAmbiSetup::Setup() 
{
   Clear();

   Add(new cMenuEditIntItem(tr("Log level"), &cfg.loglevel, 0, 3));
   Add(new cMenuEditBoolItem(tr("Show mainmenu"), &cfg.showMainmenu));

   Add(new cMenuEditIntItem(tr("Updaterate [Hz]"), &cfg.frequence, 1, 100));

   Add(new cMenuEditStraItem(tr("Detect cinema bars"), (int*)&cfg.detectCineBars, 3, cineBars));

   Add(new cMenuEditIntItem(tr("Threshold (0-255)"), &cfg.threshold, 0, 255));
   Add(new cMenuEditIntItem(tr("Gamma (0-10.0)"), &cfg.gamma, 0, 100));
   Add(new cMenuEditIntItem(tr("Value (0-20.0)"), &cfg.value, 0, 200));
   Add(new cMenuEditIntItem(tr("Saturation (0-20.0)"), &cfg.saturation, 0, 200));
   Add(new cMenuEditIntItem(tr("Speed (0-100)"), &cfg.speed, 0, 100));
   Add(new cMenuEditIntItem(tr("Autospeed (0-100)"), &cfg.autospeed, 0, 100));
   Add(new cMenuEditBoolItem(tr("Interpolation"), &cfg.interpolation));
   Add(new cMenuEditIntItem(tr("Priority 0=Highest, 255=Lowest"), &cfg.priority, 0, 255));
}

eOSState cAmbiSetup::ProcessKey(eKeys key) 
{
   eOSState state = cMenuSetupPage::ProcessKey(key);

   return state;
}

void cAmbiSetup::Store()
{
   cfg.dirty = 1;
   SetupStore("LogLevel", cfg.loglevel);
   SetupStore("ShowMainmenu", cfg.showMainmenu);
   SetupStore("ViewMode", (int)cfg.viewMode);

   SetupStore("DetectCineBars", cfg.detectCineBars);

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

//***************************************************************************
// VDR Internal
//***************************************************************************

VDRPLUGINCREATOR(cPluginBoblight);
