#ifndef DLGSETANGLE_H
#define DLGSETANGLE_H

#include <QDialog>
#include <QLabel>
#include "DoubleSpinBoxExt.h" //1409
#include <QCheckBox>
#include <QPushButton>

#include "../common/Constants.h"
#include "../main/StringsManager.h"
#include "../main/Console.h"

class DlgSetAngle : public QDialog
{
  Q_OBJECT

  QString sender_name;

  StringsManager* pSM;
  Console* pConsole;

  QLabel* pLbAngle;
  DoubleSpinBoxExt /*1409*/* pSbAngle;

  QPushButton* pBtnOk;
  QPushButton* pBtnCancel;
  QPushButton* pBtnHelp;

public:
  explicit DlgSetAngle(double currentRad, StringsManager* _pSM, Console* _pConsole,
                        QWidget *parent = 0);
  
  double AngleRad() const { return pSbAngle->value()*DEG2RAD; }

public slots:
  void slotDone();

signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::SETANGLE::DIALOG ); }
  
};

#endif // DLGSETANGLE_H
