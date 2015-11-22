#ifndef DLGGLUEELEMS_H
#define DLGGLUEELEMS_H

#include <QDialog>
#include <QLabel>
#include "DoubleSpinBoxExt.h" //1409
#include <QCheckBox>
#include <QPushButton>

#include "../common/Constants.h"
#include "../main/StringsManager.h"
#include "../main/Console.h"

class DlgGlueElems : public QDialog
{
  Q_OBJECT

  QString sender_name;

  StringsManager* pSM;
  Console* pConsole;

  QLabel* pLbMaxDev;
  DoubleSpinBoxExt /*1409*/* pSbMaxDev;
  QLabel* pLbMM1;
  QLabel* pLbLenLimit;
  DoubleSpinBoxExt /*1409*/* pSbLenLimit;
  QLabel* pLbMM2;
  QCheckBox* pChSplitLonger;
  QCheckBox* pChMarked;

  QPushButton* pBtnOk;
  QPushButton* pBtnCancel;
  QPushButton* pBtnHelp;

public:
  explicit DlgGlueElems( StringsManager* _pSM, Console* _pConsole,
                QWidget* parent );

  double MaxDev()     const { return pSbMaxDev->value(); }
  double LenLimit()   const { return pSbLenLimit->value(); }
  bool MarkedOnly()   const { return pChMarked->checkState() == Qt::Checked; }
  bool SplitLonger()  const { return pChSplitLonger->checkState() == Qt::Checked; }

public slots:
  void slotDone();

signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::GLUEELEMS::DIALOG ); }

};

#endif // DLGGLUEELEMS_H
