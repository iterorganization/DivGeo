#ifndef STRINGSMANAGER_H
#define STRINGSMANAGER_H

#include "ResourcesManager.h"

#include "../common/Resources.Dialogs.h"
#include "../common/Resources.Gui.h"
#include "../common/Resources.Menu.h"
#include "../common/Resources.Msg.h"
#include "../common/Resources.Log.h"

#include "../model/core/IAction.h"

#define SM_DLG(code) pSM->GetString( code, "dlg" )
#define SM_GUI(code) pSM->GetString( code, "gui" )
#define SM_MSG(code) pSM->GetString( code, "msg" )
#define SM_MENU(code) pSM->GetStrings( code, "menu" )
#define SM_TOOL(code) pSM->GetStrings( code, "tools" )
#define SM_LOG(code) pSM->GetString( code, "log" )

class StringsManager: public ResourcesManager
{
public:
  StringsManager( Console* pConsole ):
    ResourcesManager( pConsole, "strings.dgr" ) {
    ConstructKeys();
    LoadStrings();
  }

  virtual void ConstructKeys() {
    StringsKeys();
    MenusKeys();
    DialogsKeys();
    GuiKeys();
    LogKeys();
  }

private:
  void StringsKeys();
  void MenusKeys();
  void DialogsKeys();
  void GuiKeys();
  void LogKeys();

};

#endif // STRINGSMANAGER_H
