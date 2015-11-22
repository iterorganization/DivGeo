#ifndef CREATECHORD_H
#define CREATECHORD_H

#include <QDialog>
#include "DoubleSpinBoxExt.h" //1409
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTabWidget>

#include "../main/StringsManager.h"
#include "../main/Console.h"
#include "../common/Constants.h"
#include "../common/Point.h"

class DlgCreateChord: public QDialog
{
  Q_OBJECT

  StringsManager* pSM;

  QTabWidget* pTabs;
  QWidget* pTabCart;
  QWidget* pTabCyl;

  DoubleSpinBoxExt /*1409*/* pSbX1;
  DoubleSpinBoxExt /*1409*/* pSbY1;
  DoubleSpinBoxExt /*1409*/* pSbZ1;
  DoubleSpinBoxExt /*1409*/* pSbX2;
  DoubleSpinBoxExt /*1409*/* pSbY2;
  DoubleSpinBoxExt /*1409*/* pSbZ2;

  QLabel* pLbX1;
  QLabel* pLbY1;
  QLabel* pLbZ1;
  QLabel* pLbX2;
  QLabel* pLbY2;
  QLabel* pLbZ2;

  DoubleSpinBoxExt /*1409*/* pSbR1;
  DoubleSpinBoxExt /*1409*/* pSbZ1r;
  DoubleSpinBoxExt /*1409*/* pSbP1;
  DoubleSpinBoxExt /*1409*/* pSbR2;
  DoubleSpinBoxExt /*1409*/* pSbZ2r;
  DoubleSpinBoxExt /*1409*/* pSbP2;

  QLabel* pLbR1;
  QLabel* pLbZ1r;
  QLabel* pLbP1;
  QLabel* pLbR2;
  QLabel* pLbZ2r;
  QLabel* pLbP2;

  QPushButton* pBtnCreate;
  QPushButton* pBtnCancel;
  QPushButton* pBtnHelp;

public:
  explicit DlgCreateChord( StringsManager* _pSM, QWidget* parent = 0);

  Point Position1() const;
  Point Position2() const;

signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::CREATE_CHORD::DIALOG ); }
  
};

#endif // CREATECHORD_H
