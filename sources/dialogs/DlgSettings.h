#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include <QPushButton>
#include <QBoxLayout>
#include <QTreeWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>

#include "PropertyTableWidget.h"
#include "../common/StringConverters.h"
#include "../main/StringsManager.h"
#include "../main/SettingsManager.h"
#include "../model/core/Model.h"

class DlgSettings : public QDialog
{
  Q_OBJECT

  ModelPtr pModel;
  StringsManager* pSM;
  SettingsManager* pSettings;
  SettingsManager settings_default;

  QTreeWidget* pTree;
  QTreeWidgetItem* pCommonBranch;
  QTreeWidgetItem* pMeshBranch;
  QTreeWidgetItem* pVisualBranch;

  QList< PropertyTableWidget* > tables;

public:
  explicit DlgSettings( ModelPtr _pModel, StringsManager* _pSM,
                        SettingsManager* _pSettings,
                        const QString& _crstitle, QWidget *_pParent = 0);

  void RebuildTree(); /* 2-level depth is possible */

signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::SETTINGS::DIALOG ); }
  void slotApply();
  void slotCancel();
  void slotUpdateAllPreviews();

private:
  PropertyTableWidget* MakePropertyTable( QTreeWidgetItem* _pBranch,
                                          const QString& _crsName );
  
};

#endif // DLGSETTINGS_H
