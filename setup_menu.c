
#include "setup_menu.h"
#include "config.h"
#include "common.h"

//***************************************************************************
// Object
//***************************************************************************

cAmbiSetup::cAmbiSetup(cPluginBoblight* aPlugin)
{
   plugin = aPlugin;
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

   Add(new cMenuEditStraItem(tr("Detect cinema bars"), &cfg.detectCineBars, 3, cineBars));

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
   plugin->Save();
}