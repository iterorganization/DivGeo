#ifndef DLGMESHOPTIONS_H
#define DLGMESHOPTIONS_H

#include <QDialog>
#include <QBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

#include "../model/core/Model.h"
#include "../model/mesh/Mesh.h"
#include "../main/Console.h"
#include "../main/StringsManager.h"

class DlgMeshOptions : public QDialog
{
  Q_OBJECT

  MeshPtr pMesh;
  ModelPtr pModel;
  StringsManager* pSM;
  Console* pConsole;

  QComboBox* pCbSlide;
  QLineEdit* pLeMargin;
  QCheckBox* pChDouble;

public:
  explicit DlgMeshOptions( MeshPtr _pMesh, ModelPtr _pModel,
                           StringsManager* _pSM, Console* _pConsole,
                           QWidget *parent = 0);

signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::MESHOPTIONS::DIALOG ); }
  void slotAccept();

private:
  void ResetValues();
  
};

#endif // DLGMESHOPTIONS_H
