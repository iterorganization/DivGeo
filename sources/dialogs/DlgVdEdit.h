#ifndef DLGVDEDIT_H
#define DLGVDEDIT_H

#include <QDialog>
#include <QBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>

#include "../common/Metatypes.h"
#include "../model/core/Model.h"
#include "../main/Console.h"
#include "../main/StringsManager.h"
#include "DlgVdHelpEdit.h"

class DlgVdEdit : public QDialog
{
  Q_OBJECT

  VarDefPtr pVD;
  ModelPtr pModel;
  StringsManager* pSM;
  Console* pConsole;

  QLineEdit* pLeDescr;
  QComboBox* pCbType;
  QComboBox* pCbScope;
  QLineEdit* pLeDefault;
  QCheckBox* pChExport;
  QCheckBox* pChLayer;

public:
  explicit DlgVdEdit( VarDefPtr _pVD, ModelPtr _pModel,
                      StringsManager* _pSM, Console* _pConsole,
                      QWidget *parent = 0);

signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::VARDEFEDIT::DIALOG ); }
  void slotTypeChanged(int _index );

  void slotAccept();
  void slotRemove();
  void slotEditHelp();
  
};

#endif // DLGVDEDIT_H
