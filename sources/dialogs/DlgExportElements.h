#ifndef EXPORTELEMENTS_H
#define EXPORTELEMENTS_H

#include <QFileDialog>
#include <QGridLayout>
#include <QWidget>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>

#include "../main/StringsManager.h"

class DlgExportElements:
    public QFileDialog
{
  Q_OBJECT

  QRadioButton* pMarked;
  QRadioButton* pAll;

  QPushButton* pBtnHelp;

public:
  DlgExportElements( const StringsManager* pSM, QWidget* parent = 0, Qt::WindowFlags flags = 0 ):
    QFileDialog( parent, flags )
  {
    const QString& title = SM_DLG( DLG::EXPORT_ELEMS_TEMPLATE::TITLE );
    const QString& filter = SM_DLG( DLG::EXPORT_ELEMS_TEMPLATE::FILTER );
    const QString& labelMarked = SM_DLG( DLG::EXPORT_ELEMS_TEMPLATE::LBL::MARKED );
    const QString& labelAll = SM_DLG( DLG::EXPORT_ELEMS_TEMPLATE::LBL::ALL );

    setDefaultSuffix( "ogr" );

    setWindowTitle( title );
    setNameFilter( filter );

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

    pMarked = new QRadioButton( labelMarked );
    pMarked->setChecked( true );
    pAll= new QRadioButton( labelAll );
    pAll->setChecked( false );
    pGrid->addWidget( pMarked, 2, 0, 1, 3 );
    pGrid->addWidget( pAll,    3, 0, 1, 3 );

    pGrid->addWidget( fileNameLabel, 4, 0 );
    pGrid->addWidget( fileNameEdit,  4, 1 );
    pGrid->addWidget( btnBox,        4, 2, 2, 1 );
    pGrid->addWidget( fileTypeLabel, 5, 0 );
    pGrid->addWidget( fileTypeCombo, 5, 1 );

    pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
    connect( pBtnHelp, SIGNAL(clicked()), this, SLOT(slotHelp()) );
    pGrid->addWidget( pBtnHelp,      3, 2, 1, 1 );
  }

  bool Marked() const { return pMarked->isChecked(); }

signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::EXPORT_ELEMS_TEMPLATE::DIALOG ); }
};

#endif // EXPORTELEMENTS_H
