#ifndef DLGINVALIDVARS_H
#define DLGINVALIDVARS_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QBoxLayout>

#include "../common/Metatypes.h"
#include "../model/core/Model.h"
#include "../view/ViewWnd.h"
#include "../main/StringsManager.h"
#include "../main/Console.h"
#include "DlgVarsEdit.h"


class DlgInvalidVars : public QDialog
{
  Q_OBJECT

  ModelPtr pModel;
  CViewWndPtr pView;
  StringsManager* pSM;
  Console* pConsole;

  QListWidget* pList;
  QPushButton* pBtnShow;
  QPushButton* pBtnClose;
  QPushButton* pBtnHelp;

public:
  explicit DlgInvalidVars(ModelPtr _pModel, CViewWndPtr _pView,
                          StringsManager* _pSM, Console* _pConsole,
                          QWidget *parent = 0);

signals:
  void CreateVarsEditDlg( VarSetPtr _pVS );
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::INVALIDVARS::DIALOG ); }
  void slotShowVar();

private:
  void RebuildList();
  
};

#endif // DLGINVALIDVARS_H
