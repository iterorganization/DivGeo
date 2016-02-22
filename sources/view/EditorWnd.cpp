#include "EditorWnd.h"
#include "../main/DgDataModel.h"

#include <QTreeView>
#include <QLayout>
#include <QCloseEvent>

EditorWnd::EditorWnd( const QString& _crFilename, QWidget* _pParent ):
    QWidget( _pParent ),
    ptw( new QTreeView() ),
    filename( _crFilename ) {
  QHBoxLayout* pLo = new QHBoxLayout( this );
  pLo->addWidget( ptw );
  ptw->setModel( new DgDataModel( _crFilename ) );
  setWindowTitle( QString( "Editor - %1" ).arg( filename ) );
}

void EditorWnd::closeEvent( QCloseEvent* pe )
{
  emit EditorClosed();
  //TODO: save file
  pe->accept();
}
