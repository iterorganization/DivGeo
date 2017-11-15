#ifndef DLGSTRETCH_H
#define DLGSTRETCH_H

#include <QDialog>
#include "DoubleSpinBoxExt.h" //1409
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QCheckBox>

#include "../main/StringsManager.h"
#include "../main/Console.h"
#include "../common/Constants.h"
#include "../common/Point.h"

class DlgStretch : public QDialog
{
  Q_OBJECT

  QString sender_name;

  StringsManager* pSM;
  Console* pConsole;

  DoubleSpinBoxExt /*1409*/* pSbX;
  DoubleSpinBoxExt /*1409*/* pSbY;

  QLabel* pLbX;
  QLabel* pLbY;

  QPushButton* pBtnOk;
  QPushButton* pBtnReset;
  QPushButton* pBtnCancel;
  QPushButton* pBtnHelp;

  const Point& current;
  double minScale;
  double maxScale;

public:
  explicit DlgStretch(const Point &_current, double _min, double _max,
                      StringsManager *_pSM, Console *_pConsole,
                      QWidget *parent = 0);

  Point Scale() { return Point( pSbX->value(), pSbY->value() ); }

signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::STRETCH::DIALOG ); }
  void slotReset() { pSbX->setValue( 1. ); pSbY->setValue( 1. ); }
};

#endif // DLGSTRETCH_H
