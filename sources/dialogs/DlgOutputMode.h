#ifndef DLGOUTPUTMODE_H
#define DLGOUTPUTMODE_H

#include <QDialog>
#include <QBoxLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>

#include "../model/core/Model.h"
#include "../main/StringsManager.h"

class DlgOutputMode : public QDialog
{
  Q_OBJECT

  ModelPtr pModel;
  StringsManager* pSM;

  QRadioButton* pRbSonnet;
  QRadioButton* pRbCarre;

  QCheckBox* pChVariables;
  QCheckBox* pChTargets;
  QCheckBox* pChStructure;
  QCheckBox* pChSurfaces;
  QCheckBox* pChGridPoints;

public:
  explicit DlgOutputMode( ModelPtr _pModel, StringsManager* _pSM,
                          QWidget *parent = 0);

signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::OUTPUTMODE::DIALOG ); }
  void slotAccept();

private:
  void ResetValues();
  
};

#endif // DLGOUTPUTMODE_H
