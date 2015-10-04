#ifndef CREATECHORD_H
#define CREATECHORD_H

#include <QDialog>
#include <QDoubleSpinBox>
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

  QDoubleSpinBox* pSbX1;
  QDoubleSpinBox* pSbY1;
  QDoubleSpinBox* pSbZ1;
  QDoubleSpinBox* pSbX2;
  QDoubleSpinBox* pSbY2;
  QDoubleSpinBox* pSbZ2;

  QLabel* pLbX1;
  QLabel* pLbY1;
  QLabel* pLbZ1;
  QLabel* pLbX2;
  QLabel* pLbY2;
  QLabel* pLbZ2;

  QDoubleSpinBox* pSbR1;
  QDoubleSpinBox* pSbZ1r;
  QDoubleSpinBox* pSbP1;
  QDoubleSpinBox* pSbR2;
  QDoubleSpinBox* pSbZ2r;
  QDoubleSpinBox* pSbP2;

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
