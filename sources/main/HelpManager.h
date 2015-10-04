#ifndef HELPMANAGER_H
#define HELPMANAGER_H

#include "ResourcesManager.h"


#include "../common/Resources.Dialogs.h"
#include "../common/Resources.Menu.h"
#include "../common/Resources.Help.h"

class HelpManager : public ResourcesManager
{
public:
  static const QString scsNoHelp;

  HelpManager( Console* pConsole ):
    ResourcesManager( pConsole, "help.dgh" ) {
    ConstructKeys();
    unknown.clear();
    unknown.append( scsNoHelp );
    LoadStrings( true );
  }

private:
  virtual void ConstructKeys();
};

#endif // HELPMANAGER_H
