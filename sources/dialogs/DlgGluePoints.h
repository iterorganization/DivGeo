#ifndef DLGGLUEPOINTS_H
#define DLGGLUEPOINTS_H

#include <QDialog>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>

#include "../common/Constants.h"
#include "../main/StringsManager.h"
#include "../main/Console.h"

class DlgGluePoints : public QDialog
{
  Q_OBJECT

  QString sender_name;

  StringsManager* pSM;
  Console* pConsole;

  QLabel* pLbDist;
  QDoubleSpinBox* pSbDist;
  QLabel* pLbMM;
  QCheckBox* pChMarked;

  QPushButton* pBtnOk;
  QPushButton* pBtnCancel;
  QPushButton* pBtnHelp;

public:
  explicit DlgGluePoints( StringsManager* _pSM, Console* _pConsole,
                          QWidget* parent = 0 );
  
  double Dist() const { return pSbDist->value(); }
  bool MarkedOnly() const { return pChMarked->checkState() == Qt::Checked; }
  
public slots:
  void slotDone();

signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::GLUEPOINTS::DIALOG ); }
  
};

#endif // DLGGLUEPOINTS_H
