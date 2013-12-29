#ifndef __SETUP_MENU_H
#define __SETUP_MENU_H

#include <vdr/plugin.h>
#include "ambiservice.h"
#include "vdrboblight.h"

//***************************************************************************
// Setup
//***************************************************************************
class cPluginBoblight;
class cAmbiSetup : public cMenuSetupPage, public cAmbiService
{
   public:
      cAmbiSetup(cPluginBoblight* aPlugin);

   protected:

      virtual void Setup();
      virtual eOSState ProcessKey(eKeys Key);
      virtual void Store();

      const char* cineBars[cbCount];
      cPluginBoblight* plugin;
};

#endif
