#ifndef DLGVSDLIST_H
#define DLGVSDLIST_H

#include <QDialog>
#include <QBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QMap>
#include <QSignalMapper>

#include "../common/Metatypes.h"
#include "../model/core/Model.h"
#include "../main/StringsManager.h"
#include "../main/Console.h"
#include "DlgCreateName.h"
#include "DlgVsdEdit.h"

class DlgVsdList : public QDialog
{
  Q_OBJECT

  ModelPtr pModel;
  StringsManager* pSM;
  Console* pConsole;

  QListWidget* pLwVars;

  QPushButton* pBtnAdd;
  QPushButton* pBtnModify;
  QPushButton* pBtnRemove;
  QPushButton* pBtnClose;
  QPushButton* pBtnHelp;

  typedef QMap< QString, DlgVsdEdit* > MapVsdEditDialogs;
  MapVsdEditDialogs mapEditDialogs;
  QSignalMapper smEditDialogs;


public:
  explicit DlgVsdList( ModelPtr _pModel, StringsManager* _pSM,
                       Console* _pConsole, QWidget *parent = 0);

private:
  void RebuildList();
  VarSetDefPtr RetrieveVSD();

signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::VARSETDEFLIST::DIALOG ); }
  void slotAdd();
  void slotModify();
  void slotRemove();

  void slotCloseEditDialog( const QString& _crsName );
  
};

#endif // DLGVSDLIST_H
