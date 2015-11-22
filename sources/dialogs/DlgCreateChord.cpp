#include "DlgCreateChord.h"

#define DLGCHORD_GRID_WIDGET( name, lo, row, rm ) \
  pSb##name = new DoubleSpinBoxExt /*1409*/(); \
  pSb##name->setDecimals( 5 ); \
  pSb##name->setRange( -100000, 100000 ); \
  pSb##name->setSingleStep( 1. ); \
  (lo)->addWidget( pSb##name, (row), 1, 1, 1 ); \
  pLb##name = new QLabel( SM_DLG( rm ) ); \
  pLb##name->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ); \
  (lo)->addWidget( pLb##name, (row), 0, 1, 1 );


DlgCreateChord::DlgCreateChord( StringsManager* _pSM, QWidget *parent):
  QDialog(parent),
  pSM( _pSM )
{
  this->setWindowTitle( SM_DLG( DLG::CREATE_CHORD::TITLE ) );
  this->setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );
  pTabs = new QTabWidget( this );
  pLoMain->addWidget( pTabs );

  pTabCart = new QWidget();
  QGridLayout* pLoCart = new QGridLayout( pTabCart );
  pTabs->addTab( pTabCart, SM_DLG( DLG::CREATE_CHORD::CARTESIAN::TITLE ) );

  DLGCHORD_GRID_WIDGET( X1, pLoCart, 0, DLG::CREATE_CHORD::CARTESIAN::X1 );
  DLGCHORD_GRID_WIDGET( Y1, pLoCart, 1, DLG::CREATE_CHORD::CARTESIAN::Y1 );
  DLGCHORD_GRID_WIDGET( Z1, pLoCart, 2, DLG::CREATE_CHORD::CARTESIAN::Z1 );
  DLGCHORD_GRID_WIDGET( X2, pLoCart, 3, DLG::CREATE_CHORD::CARTESIAN::X2 );
  DLGCHORD_GRID_WIDGET( Y2, pLoCart, 4, DLG::CREATE_CHORD::CARTESIAN::Y2 );
  DLGCHORD_GRID_WIDGET( Z2, pLoCart, 5, DLG::CREATE_CHORD::CARTESIAN::Z2 );

  pTabCyl = new QWidget();
  QGridLayout* pLoCyl = new QGridLayout( pTabCyl );
  pTabs->addTab( pTabCyl, SM_DLG( DLG::CREATE_CHORD::CYLINDRICAL::TITLE ) );

  DLGCHORD_GRID_WIDGET( R1,  pLoCyl, 0, DLG::CREATE_CHORD::CYLINDRICAL::R1 );
  DLGCHORD_GRID_WIDGET( Z1r, pLoCyl, 1, DLG::CREATE_CHORD::CYLINDRICAL::Z1 );
  DLGCHORD_GRID_WIDGET( P1,  pLoCyl, 2, DLG::CREATE_CHORD::CYLINDRICAL::PHI1 );
  DLGCHORD_GRID_WIDGET( R2,  pLoCyl, 3, DLG::CREATE_CHORD::CYLINDRICAL::R2 );
  DLGCHORD_GRID_WIDGET( Z2r, pLoCyl, 4, DLG::CREATE_CHORD::CYLINDRICAL::Z2 );
  DLGCHORD_GRID_WIDGET( P2,  pLoCyl, 5, DLG::CREATE_CHORD::CYLINDRICAL::PHI2 );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  pBtnCreate = new QPushButton( SM_DLG( DLG::BTN::CREATE ) );
  pLoButtons->addWidget( pBtnCreate );
  pBtnCancel = new QPushButton( SM_DLG( DLG::BTN::CANCEL ) );
  pLoButtons->addWidget( pBtnCancel );
  pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );

  /* Connections */
  connect( pBtnCreate,  SIGNAL(clicked()), this, SLOT(accept()) );
  connect( pBtnCancel,  SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp,    SIGNAL(clicked()), this, SLOT(slotHelp()) );
}

Point DlgCreateChord::Position1() const
{
  if( pTabs->currentWidget() == pTabCyl ) {
    return Point( pSbR1->value() * cos( DEG2RAD*pSbP1->value() ),
                  pSbZ1r->value(),
                  pSbR1->value() * sin( DEG2RAD*pSbP1->value() ) );
  }
  else
    return Point( pSbX1->value(), pSbY1->value(), pSbZ1->value() );
}

Point DlgCreateChord::Position2() const
{
  if( pTabs->currentWidget() == pTabCyl ) {
    return Point( pSbR2->value() * cos( DEG2RAD*pSbP2->value() ),
                  pSbZ2r->value(),
                  pSbR2->value() * sin( DEG2RAD*pSbP2->value() ) );
  }
  else
    return Point( pSbX2->value(), pSbY2->value(), pSbZ2->value() );
}

