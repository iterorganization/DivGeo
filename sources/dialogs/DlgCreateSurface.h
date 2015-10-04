#ifndef CREATESURFACE_H
#define CREATESURFACE_H

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSpacerItem>
#include <QSignalMapper>
#include <QCheckBox>

#include "../main/StringsManager.h"
#include "../main/Console.h"

#include "../model/flux/SurfaceEx.h"

#include "DistrGraph.h"

class DlgCreateSurface:
  public QDialog
{
  Q_OBJECT

  QString sender_name;

  StringsManager* pSM;
  Console* pConsole;
  SurfaceExPtr* ppSX;
  int outputMode;
  CreatorId crid;

  DistrGraph* pDigr;
  QTabWidget* pTabs;
  QWidget* pTabMult;
  QWidget* pTabSingle;
  QTabWidget* pTabsSingle;
  QWidget* pTabByLevel;
  QWidget* pTabByPoint;

  /* single */
  QComboBox*      pCbAreaS;
  QDoubleSpinBox* pSbValue;
  QDoubleSpinBox* pSbX;
  QDoubleSpinBox* pSbY;

  /* multiple */
  QLabel*         pLbAreaM;
  QComboBox*      pCbAreaM;
  QLabel*         pLbLevel1;
  QDoubleSpinBox* pSbLevel1;
  QLabel*         pLbLevel2;
  QDoubleSpinBox* pSbLevel2;
  QLabel*         pLbCount;
  QSpinBox*       pSbCount;
  QLabel*         pLbAlpha1;
  QDoubleSpinBox* pSbAlpha1;
  QLabel*         pLbAlpha2;
  QDoubleSpinBox* pSbAlpha2;
  QLabel*         pLbLaw;
  QComboBox*      pCbLaw;
  QCheckBox*      pChRemoveOld;

  /* buttons */
  QPushButton*    pBtnReset;
  QPushButton*    pBtnCopy;
  QPushButton*    pBtnCopyL1;
  QPushButton*    pBtnCopyL2;
  QPushButton*    pBtnMarkBoundElems;
  QPushButton*    pBtnCreate;
  QPushButton*    pBtnClose;
  QPushButton*    pBtnHelp;

public:
  explicit DlgCreateSurface( StringsManager* _pSM, Console* _pConsole,
          int _outputMode, SurfaceExPtr* _ppSX, const IComponentList& _crZones,
                             QWidget *parent = 0);

  bool IsSingleByLevel() const;
  bool IsSingleByPoint() const;
  bool IsMultiple() const;
  int SingleArea() const;
  double SingleLevel() const;
  Point SinglePoint() const;
  CreatorId MultipleCreator() const;
  bool RemoveOldOption() const;
  const CreatorId& OldCreatorId() const;

  void setOutputMode( int _outputMode );

signals:
  void signalMarkBoundingElem( int _area );
  void sgnlHelp( int _code );
  
private slots:
  void slotHelp() { emit sgnlHelp( DLG::CREATE_SURFACE::DIALOG ); }
  void slotCountChanged( int count );
  void slotLawChanged( int index );
  void slotAreaChanged( int index );
  void slotCreate();
  void slotReset();
  void slotCopy();
  void slotCopyLevel(int index);
  void slotMarkBoundElems();
};

#endif // CREATESURFACE_H
