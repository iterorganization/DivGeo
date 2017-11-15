#ifndef DLGVSDEDIT_H
#define DLGVSDEDIT_H

#include <QDialog>
#include <QBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QSignalMapper>

#include "../model/core/Model.h"
#include "../main/Console.h"
#include "../main/StringsManager.h"
#include "DlgCreateName.h"
#include "DlgVdEdit.h"

class DlgVsdEdit : public QDialog
{
  Q_OBJECT

  VarSetDefPtr pVSD;
  ModelPtr pModel;
  StringsManager* pSM;
  Console* pConsole;

  QLineEdit* pLeDescr;
  QLineEdit* pLeMinVS;
  QLineEdit* pLeMaxVS;

  QTableWidget* pTwVarDefs;
  QSignalMapper smVarDefs;

public:
  explicit DlgVsdEdit( VarSetDefPtr _pVSD, ModelPtr _pModel,
                       StringsManager* _pSM, Console* _pConsole,
                       QWidget *parent = 0);
  
private:
  void RebuildGrid();

signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::VARSETDEFEDIT::DIALOG ); }
  void slotAcceptChanges();
  void slotEdit( int _position );
  
};

#endif // DLGVSDEDIT_H
