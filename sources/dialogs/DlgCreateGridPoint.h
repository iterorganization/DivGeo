#ifndef CREATEGRIDPOINT_H
#define CREATEGRIDPOINT_H

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include "DoubleSpinBoxExt.h" //1409
#include <QSpacerItem>

#include "../main/StringsManager.h"
#include "../main/Console.h"

#include "../model/flux/GridPointEx.h"

#include "DistrGraph.h"

class DlgCreateGridPoint:
  public QDialog
{
  Q_OBJECT

  QString sender_name;

  StringsManager* pSM;
  Console* pConsole;
  GridPointExPtr* ppGPX;
  int outputMode;

  DistrGraph* pDigr;
  QTabWidget* pTabs;
  QWidget* pTabSingle;
  QWidget* pTabMult;

  /* single */
  QComboBox*      pCbZoneS;
  DoubleSpinBoxExt /*1409*/* pSbValue;

  /* multiple */
  QLabel*         pLbZoneM;
  QComboBox*      pCbZoneM;
  QLabel*         pLbCount;
  QSpinBox*       pSbCount;
  QLabel*         pLbAlpha1;
  DoubleSpinBoxExt /*1409*/* pSbAlpha1;
  QLabel*         pLbAlpha2;
  DoubleSpinBoxExt /*1409*/* pSbAlpha2;
  QLabel*         pLbLaw;
  QComboBox*      pCbLaw;

  /* buttons */
  QPushButton*    pBtnReset;
  QPushButton*    pBtnCopy;
  QPushButton*    pBtnCreate;
  QPushButton*    pBtnClose;
  QPushButton*    pBtnHelp;

public:
  explicit DlgCreateGridPoint(StringsManager* _pSM, Console* _pConsole,
    int _outputMode, GridPointExPtr* _ppGPX, const IComponentList& _crZones,
                              QWidget *parent = 0 );
  
  bool IsSingle() const;
  bool IsMultiple() const;
  int SingleArea() const;
  double SingleValue() const;
  CreatorId MultipleCreator() const;

  void setOutputMode( int _outputMode ) { outputMode = _outputMode; }

signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::CREATE_GPOINT::DIALOG ); }
  void slotCountChanged( int count );
  void slotLawChanged( int index);
  void slotCreate();
  void slotReset();
  void slotCopy();
  
};

#endif // CREATEGRIDPOINT_H
