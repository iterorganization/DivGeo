#include "ToolsPanel.h"

ToolsPanel::ToolsPanel( StringsManager* _pSM, QWidget* _pParent ):
  QDockWidget( _pParent ),
  pSM( _pSM ),
  //pngTools32( ":/main/tools.png" ),
  pngTools24( ":/main/tools24.png" ),
  pBtn_left( null ),
  pBtn_middle( null ),
  pBtn_right( null )
{
  setFixedWidth( 80 );
  setFeatures( 0 );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setTitleBarWidget( new QLabel( "Tools" ) );

  QWidget* pDockWidgetContents = new QWidget();
  pDockWidgetContents->setGeometry( QRect( 0, 0, width(), 100 ) );
  QHBoxLayout* pLoMain = new QHBoxLayout( pDockWidgetContents );
  pLoMain->setSpacing( 1 );
  pLoMain->setMargin( 1 );

  QVBoxLayout* pLoLeft = new QVBoxLayout();
  pLoLeft->setSpacing( 1 );
  pLoLeft->setMargin( 0 );
  pLoMain->addLayout( pLoLeft );
  QVBoxLayout* pLoRight = new QVBoxLayout();
  pLoRight->setSpacing( 1 );
  pLoRight->setMargin( 0 );
  pLoMain->addLayout( pLoRight );

  const int S = ico_size;

  AddButton( pLoLeft, TOOLS::ZOOMPAN,         0,   0   );
  AddButton( pLoLeft, TOOLS::ROTATE,          0,   S   );
  AddButton( pLoLeft, TOOLS::STRETCH,         0,   2*S );
  pLoLeft->addSpacing( 10 );
  AddButton( pLoLeft, TOOLS::DELETE,          S,   4*S );
  AddButton( pLoLeft, TOOLS::ADDELEMENT,      2*S, 0   );
  AddButton( pLoLeft, TOOLS::ADDSOURCES,      2*S, S   );
  AddButton( pLoLeft, TOOLS::ADDCHORDS,       2*S, 2*S );
  AddButton( pLoLeft, TOOLS::ADDSURFACE,      2*S, 3*S );
  AddButton( pLoLeft, TOOLS::ADDGRIDPOINT,    2*S, 4*S );
  pLoLeft->addSpacing( 10 );
  AddButton( pLoLeft, TOOLS::MOVEMESHPOINT,   3*S, 0   );
  pLoLeft->addSpacing( 10 );
  AddButton( pLoLeft, TOOLS::COMMENT,         0,   3*S );

  AddButton( pLoRight, TOOLS::MARK,           S,   0   );
  AddButton( pLoRight, TOOLS::EXAMINE,        S,   S   );
  AddButton( pLoRight, TOOLS::MOVE,           S,   2*S );
  pLoRight->addSpacing( 10 );
  AddButton( pLoRight, TOOLS::SPLITELEMENT,   3*S, S   );
  AddButton( pLoRight, TOOLS::JOINELEMENTS,   3*S, 2*S );
  AddButton( pLoRight, TOOLS::CONNECTPOINTS,  3*S, 3*S );
  AddButton( pLoRight, TOOLS::REPOSITION,     3*S, 4*S );
  AddButton( pLoRight, TOOLS::REVERSENORMALS, 4*S, 0   );
  pLoRight->addSpacing( 10 );
  AddButton( pLoRight, TOOLS::EXTENDCHORD,    4*S, S   );
  AddButton( pLoRight, TOOLS::ADJUSTCHORD,    4*S, 2*S );

  pLoLeft->addSpacerItem( new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding ) );
  pLoRight->addSpacerItem( new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding ) );

  setWidget( pDockWidgetContents );

  connect( &smLeftClick, SIGNAL(mapped(int)), this, SIGNAL(toolSelected(int)) );
  connect( &smMiddleClick, SIGNAL(mapped(int)), this, SIGNAL(toolSelected(int)) );
  connect( &smRightClick, SIGNAL(mapped(int)), this, SIGNAL(toolSelected(int)) );
}

MouseToolPushButton* ToolsPanel::AddButton( QVBoxLayout* _pLo, ToolId _id,
                                    int _img_x, int _img_y )
{
  QIcon ico( pngTools24.copy( _img_x, _img_y, ico_size, ico_size ) );
  MouseToolPushButton* pBtn = new MouseToolPushButton( ico );
  QString sText( SM_TOOL( _id ).first() );
  pBtn->setToolTip( sText.replace( "&", "" ) );
  _pLo->addWidget( pBtn );
  mapButtons.insert( _id, pBtn );

  connect( pBtn, SIGNAL(leftClicked()), &smLeftClick, SLOT(map()) );
  smLeftClick.setMapping( pBtn, _id | Qt::LeftButton );
  connect( pBtn, SIGNAL(middleClicked()), &smMiddleClick, SLOT(map()) );
  smMiddleClick.setMapping( pBtn, _id | Qt::MiddleButton );
  connect( pBtn, SIGNAL(rightClicked()), &smRightClick, SLOT(map()) );
  smRightClick.setMapping( pBtn, _id | Qt::RightButton );

  return pBtn;
}

void ToolsPanel::EnableButton( ToolId _toolId, bool _enable )
{
  MouseToolPushButton* pBtn = mapButtons[ _toolId ];
  if( pBtn == null )
    return;
  pBtn->setEnabled( _enable );
}

void ToolsPanel::slotToolSelected( int _id )
{
  Qt::MouseButton button = (Qt::MouseButton)(_id & TOOLS::MASK_BUTTON);
  ToolId toolId = (ToolId)(_id & TOOLS::MASK_TOOL);

  MouseToolPushButton* pBtn = mapButtons[ toolId ];
  if( pBtn == null )
    return;

  MouseToolPushButton* pBtn_old;
  QChar letter;
  bool oldBtnIsChecked = true;

  switch( button ) {
  case Qt::LeftButton:
    pBtn_old = pBtn_left;
    pBtn_left = pBtn;
    oldBtnIsChecked = (pBtn_old == pBtn_middle) || (pBtn_old == pBtn_right);
    letter = 'L';
    break;
  case Qt::MidButton:
    pBtn_old = pBtn_middle;
    pBtn_middle = pBtn;
    oldBtnIsChecked = (pBtn_old == pBtn_left) || (pBtn_old == pBtn_right);
    letter = 'M';
    break;
  case Qt::RightButton:
    pBtn_old = pBtn_right;
    pBtn_right = pBtn;
    oldBtnIsChecked = (pBtn_old == pBtn_left) || (pBtn_old == pBtn_middle);
    letter = 'R';
    break;
  default:
    return;
  }

  if( pBtn == pBtn_old )
    return;
  if( pBtn_old != null ) {
    pBtn_old->setChecked( oldBtnIsChecked );
    pBtn_old->ChangeLetter( letter, false );
  }
  pBtn->ChangeLetter( letter, true );
  pBtn->setChecked( true );
}
