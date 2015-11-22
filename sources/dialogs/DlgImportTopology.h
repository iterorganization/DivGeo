#ifndef IMPORTTOPOLOGY_H
#define IMPORTTOPOLOGY_H

#include <QFileDialog>
#include <QGridLayout>
#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

#include "../main/StringsManager.h"

class DlgImportTopology:
    public QFileDialog
{
  Q_OBJECT

  QCheckBox* pCheck;
  QPushButton* pBtnHelp;

public:
  DlgImportTopology( const StringsManager* pSM, const QString& _crsExtDg, const QString& _crsExtTopo, const QString& _crsPath, QWidget* parent = 0, Qt::WindowFlags flags = 0 ):
    QFileDialog( parent, flags )
  {
    const QString& title = SM_DLG( DLG::IMPORTTOPO::TITLE );
    const QString& filter = SM_DLG( DLG::IMPORTTOPO::FILTER ).arg( _crsExtDg ).arg( _crsExtTopo );
    const QString& label = SM_DLG( DLG::IMPORTTOPO::LBL::DETECT_XPNT );

    setOption( QFileDialog::DontUseNativeDialog, true );

    setWindowTitle( title );
    setNameFilter( filter );
    setDirectory( _crsPath );

    QGridLayout* pGrid = qobject_cast< QGridLayout* >( layout() );
    if( pGrid == NULL )
      return;

    QWidget* fileNameLabel  = pGrid->itemAtPosition( 2, 0 )->widget();
    QWidget* fileNameEdit   = pGrid->itemAtPosition( 2, 1 )->widget();
    QWidget* btnBox         = pGrid->itemAtPosition( 2, 2 )->widget();
    QWidget* fileTypeLabel  = pGrid->itemAtPosition( 3, 0 )->widget();
    QWidget* fileTypeCombo  = pGrid->itemAtPosition( 3, 1 )->widget();

    pGrid->removeWidget( fileNameLabel );
    pGrid->removeWidget( fileNameEdit );
    pGrid->removeWidget( btnBox );
    pGrid->removeWidget( fileTypeLabel );
    pGrid->removeWidget( fileTypeCombo );

    pCheck = new QCheckBox( label );
    pCheck->setChecked( true );
    pGrid->addWidget( pCheck, 2, 0, 1, 3 );

    pGrid->addWidget( fileNameLabel, 3, 0 );
    pGrid->addWidget( fileNameEdit,  3, 1 );
    pGrid->addWidget( btnBox,        3, 2, 2, 1 );
    pGrid->addWidget( fileTypeLabel, 4, 0 );
    pGrid->addWidget( fileTypeCombo, 4, 1 );

    pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
    connect( pBtnHelp, SIGNAL(clicked()), this, SLOT(slotHelp()) );
    pGrid->addWidget( pBtnHelp,      2, 2, 1, 1 );
  }

  bool DetectXPoint() const { return pCheck->isChecked(); }

signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::IMPORTTOPO::DIALOG ); }
};

#endif //IMPORTTOPOLOGY_H
