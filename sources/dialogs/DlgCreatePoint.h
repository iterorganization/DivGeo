#ifndef CREATEPOINT_H
#define CREATEPOINT_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>

#include "../main/StringsManager.h"
#include "../common/Point.h"

class DlgCreatePoint: public QDialog
{
  Q_OBJECT

  StringsManager* pSM;

  QDoubleSpinBox* pSbX;
  QDoubleSpinBox* pSbY;

  QLabel* pLbX;
  QLabel* pLbY;

  QPushButton* pBtnCreate;
  QPushButton* pBtnCancel;
  QPushButton* pBtnHelp;

public:
  explicit DlgCreatePoint( StringsManager* _pSM,
                          const QString& _title, QWidget *parent = 0);
  
  Point Position() const { return Point( pSbX->value(), pSbY->value() ); }

signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::CREATE_NODE::DIALOG ); }
};

#endif // CREATEPOINT_H
