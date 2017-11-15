#ifndef DLGMOVEROTATE_H
#define DLGMOVEROTATE_H

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

class DlgMoveRotate: public QDialog
{
  Q_OBJECT

  QString sender_name;

  StringsManager* pSM;
  Console* pConsole;

  DoubleSpinBoxExt /*1409*/* pSbX;
  DoubleSpinBoxExt /*1409*/* pSbY;
  DoubleSpinBoxExt /*1409*/* pSbScale;

  QLabel* pLbX;
  QLabel* pLbY;
  QLabel* pLbScale;

  QPushButton* pBtnMove;
  QPushButton* pBtnRotate;
  QPushButton* pBtnCancel;
  QPushButton* pBtnHelp;

  QCheckBox* pChElem;
  QCheckBox* pChTempl;

public:
  enum RetCode { r_move = 2, r_rotate = 3 };

  explicit DlgMoveRotate( StringsManager* _pSM, Console* _pConsole,
                          QWidget *parent = 0 );
  
  bool ChangeElements() { return pChElem->checkState() == Qt::Checked; }
  bool ChangeTemplate() { return pChTempl->checkState() == Qt::Checked; }

  Point Offset() { return Point( pSbX->value(), pSbY->value() ); }
  double Scale() { return pSbScale->value(); }
  void Done( RetCode r );
  
public slots:

  void slotMove() { Done( r_move ); }
  void slotRotate() { Done( r_rotate ); }

signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::ROTMOVE::DIALOG ); }
  
};

#endif // DLGMOVEROTATE_H
