#ifndef DLGSTATICTICS_H
#define DLGSTATICTICS_H

#include <QDialog>
#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

#include "../common/StringConverters.h"
#include "../main/StringsManager.h"
#include "../model/core/Model.h"

class DlgStatictics : public QDialog
{
  Q_OBJECT

  ModelPtr pModel;
  StringsManager* pSM;
  QLabel* pLblNodes;
  QLabel* pLblElems;
  QLabel* pLblSources;
  QLabel* pLblUnusedNumbers;
  QLabel* pLblSurfaces;
  QLabel* pLblGridPoints;
  QLabel* pLblSeparators;
  QLabel* pLblChords;

public:
  explicit DlgStatictics( ModelPtr _pModel, StringsManager* _pSM,
                          QWidget *parent = 0);
public slots:
  void UpdateInfo();

signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::INFO::DIALOG ); }
};

#endif // DLGSTATICTICS_H
