#ifndef DLGMESHHEADER_H
#define DLGMESHHEADER_H

#include <QDialog>
#include <QBoxLayout>
#include <QTextEdit>
#include <QPushButton>

#include "../model/core/Model.h"
#include "../model/mesh/Mesh.h"
#include "../main/Console.h"
#include "../main/StringsManager.h"

class DlgMeshHeader : public QDialog
{
  Q_OBJECT

  MeshPtr pMesh;
  ModelPtr pModel;
  StringsManager* pSM;
  Console* pConsole;

  QTextEdit* pTeHeader;
  QPushButton* pBtnChange;

public:
  explicit DlgMeshHeader(MeshPtr _pMesh, ModelPtr _pModel,
                         StringsManager* _pSM, Console* _pConsole,
                         QWidget *parent = 0);
    
signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::EDIT_MESH_HEADER::DIALOG ); }
  void slotAccept();
  void slotChanged();
  
};

#endif // DLGMESHHEADER_H
