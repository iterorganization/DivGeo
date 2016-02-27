#include "EditorWnd.h"
#include "../main/DgDataModel.h"

#include <QTreeView>
#include <QLayout>
#include <QCloseEvent>
#include <QFocusEvent>
#include <QHeaderView>

EditorWnd::EditorWnd( const QString& _crFilename, QWidget* _pParent ):
    QWidget( _pParent ),
    ptw( new QTreeView() ),
    filename( _crFilename ),
    pmodel( new dm::DgDataModel( _crFilename ) ) {
  QHBoxLayout* pLo = new QHBoxLayout( this );
  pLo->addWidget( ptw );
  ptw->setModel( pmodel );
  ptw->header()->moveSection( 1, 0 );
  pmodel->Load();
  ptw->resizeColumnToContents( 0 );
  ptw->resizeColumnToContents( 1 );
  setWindowTitle( QString( "Editor - %1" ).arg( filename ) );
}

EditorWnd::~EditorWnd() {
  delete pmodel;
}

void EditorWnd::closeEvent( QCloseEvent* _pe )
{
  emit EditorClosed();
  //TODO: save file
  _pe->accept();
}

void EditorWnd::focusInEvent( QFocusEvent* _pe )
{
  emit EditorInFocus( this ); // Recursive call!!!
  _pe->accept();
}
