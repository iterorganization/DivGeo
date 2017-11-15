#ifndef DLGVDHELPEDIT_H
#define DLGVDHELPEDIT_H

#include <QDialog>
#include <QBoxLayout>
#include <QTextEdit>
#include <QPushButton>

#include "../common/Metatypes.h"
#include "../main/StringsManager.h"

class DlgVdHelpEdit : public QDialog
{
  Q_OBJECT

  VarDefPtr pVD;
  StringsManager* pSM;

  QTextEdit* pTeHelp;

public:
  explicit DlgVdHelpEdit( VarDefPtr _pVD, StringsManager* _pSM, QWidget *parent = 0);

signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::HELPEDIT::DIALOG ); }
  void slotAccept();
  
};

#endif // DLGVDHELPEDIT_H
