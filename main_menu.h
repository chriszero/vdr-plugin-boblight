#ifndef __MAIN_MENU_H
#define __MAIN_MENU_H

#include <vdr/plugin.h>

#include "vdrboblight.h"

//***************************************************************************
// Plugins Main Menu
//***************************************************************************

class cPluginBoblight;

class cBoblightPluginMenu : public cMenuSetupPage
{
   public:

      cBoblightPluginMenu(const char* title, cPluginBoblight* aPlugin);
      virtual ~cBoblightPluginMenu() { };
      virtual void Store();
      virtual eOSState ProcessKey(eKeys key);

   protected:
   	  void Create(void);
      cPluginBoblight* plugin;
};

#endif
