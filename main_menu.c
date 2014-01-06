
#include <vdr/plugin.h>

#include "ambiservice.h"
#include "main_menu.h"
#include "config.h"
#include "common.h"

cBoblightPluginMenu::cBoblightPluginMenu(const char* title, cPluginBoblight* aPlugin)
{
   plugin = aPlugin;

   Create();
}

void cBoblightPluginMenu::Create(void) {
   SetMenuCategory(mcPluginSetup);
   Clear();

   Add(new cMenuEditStraItem(tr("View Mode"), &cfg.viewMode, cAmbiService::vmCount, cAmbiService::viewModes));
   Add(new cMenuEditStraItem(tr("Startup View Mode"), &cfg.startupViewMode, cAmbiService::vmCount, cAmbiService::viewModes));

   Add(new cMenuEditIntItem(tr("Fixed Color Red"), &cfg.fixedR, 0, 255));
   Add(new cMenuEditIntItem(tr("Fixed Color Green"), &cfg.fixedG, 0, 255));
   Add(new cMenuEditIntItem(tr("Fixed Color Blue"), &cfg.fixedB, 0, 255));

   SetHelp(0, 0, 0, 0);
   Display();
}

void cBoblightPluginMenu::Store() {
	plugin->Save();
}

//***************************************************************************
// Process Key
//***************************************************************************

eOSState cBoblightPluginMenu::ProcessKey(eKeys key)
{
   eOSState state = cOsdMenu::ProcessKey(key);
   
   if (state != osUnknown)
      return state;

   if (key == kLeft || key == kRight)
   {
      if (cfg.viewMode == cAmbiService::vmDetached && plugin->isRunning())
         plugin->stopAtmo();
      else if (cfg.viewMode != cAmbiService::vmDetached && !plugin->isRunning())
         plugin->startAtmo();
   }

   if (key == kOk)
   {
      plugin->Save();
      return osEnd;
   }

   return state;
}