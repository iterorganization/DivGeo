#ifndef DLGTOPOLOGY_H
#define DLGTOPOLOGY_H

#include <QDialog>
#include <QBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSpacerItem>

#include "../common/Metatypes.h"
#include "../model/core/Model.h"
#include "../main/StringsManager.h"
#include "../main/Console.h"

class DlgTopology : public QDialog
{
  Q_OBJECT

  ModelPtr pModel;
  StringsManager* pSM;
  Console* pConsole;

  IComponentPtr pObject;

  QTabWidget* pTabs;
  QWidget* pTabXPoints;
  QWidget* pTabSections;
  QWidget* pTabZones;
  QWidget* pTabName;

  QLineEdit* pLeXPLevel;
  QLineEdit* pLeSNumber;
  QLineEdit* pLeSName;
  QLineEdit* pLeSDescr;

  QPushButton* pBtnChangeXPoint;

  QCheckBox* pChRevTarget;
  QCheckBox* pChHasGP;

  QPushButton* pBtnChangeSection;
  QPushButton* pBtnChangeDirection;

  QComboBox* pCbZones;

  QLineEdit* pLeZNumber;
  QLineEdit* pLeZName;
  QLineEdit* pLeZDescr;
  QLineEdit* pLeZBaseSect;
  QLineEdit* pLeZLimSect;

  QComboBox* pCbZOrient;
  QCheckBox* pChClosed;

  QPushButton* pBtnDelete;

  QLineEdit* pLeName;
  QPushButton* pBtnSetName;

public:
  explicit DlgTopology( ModelPtr _pModel, StringsManager* _pSM, Console* _pConsole,
                        QWidget *parent = 0);

public slots:
  void LoadSelectedObject( IComponentPtr _pObject );
  
signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::TOPOLOGY::DIALOG ); }
  void slotDetectAllXPoints();
  void slotChangeXPoint();

  void slotDetectAllSections();
  void slotChangeSection();
  void slotRevertSection();

  void slotZoneSelectionChanged( int _index );
  void slotZoneSet();
  void slotZoneDelete();

  void slotNameChanged();
  void slotSetTopoName();

private:
  void RebuildZoneList();
};

#endif // DLGTOPOLOGY_H
