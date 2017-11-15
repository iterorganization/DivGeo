#include "DlgVarsEdit.h"

VarDefPtr DlgVarsEdit::RetrieveVarDef( NPoint _index ) const
{
  QTableWidgetItem* pItem = vars[ _index.y ]->item( _index.x, 0 );
  if( pItem == null )
    return null;
  QVariant qv = pItem->data( Qt::UserRole );
  if( qv.isNull() || qv.userType() != QMetaType::type( "VarDefPtr" ) )
    return null;
  return qv.value< VarDefPtr >();
}

DlgVarsEdit::DlgVarsEdit(VarSetPtr _pVS, ModelPtr _pModel, CViewWndPtr _pView,
                         Console* _pConsole, StringsManager* _pSM,
                         QWidget *parent):
  QDialog( parent,  Qt::WindowSystemMenuHint |
                    Qt::WindowCloseButtonHint |
                    Qt::WindowMinimizeButtonHint ), //1411
  pSM( _pSM ),
  pModel( _pModel ),
  pView( _pView ),
  pConsole( _pConsole ),
  pVS( _pVS ),
  changed_num( 0 ),
  varsSize( 0 ),
  resetEnabled( true )
{
  QString title = QString::fromStdString( pVS->Description() );
  this->setWindowTitle( title );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  pBtnCollapse = new QPushButton( "▲" ); //1411
  pBtnCollapse->setCheckable( true );
  pBtnCollapse->setChecked( false );
  pBtnCollapse->setFixedWidth( 20 );
  pLoButtons->addWidget( pBtnCollapse );
  connect( pBtnCollapse, SIGNAL(toggled(bool)), this, SLOT(slotCollapse(bool)) );

  /* Detect VarSet content type */
  ulong group_vars_num = 0;
  ulong nongroup_vars_num = 0;
  ulong multiple_vars_num = 0;

  FOREACHPTRCONST( VarDefPtr, pVD1, pVS->VSD()->VarDefs() ) {
    pVD1->HasGroup() ? group_vars_num++ : nongroup_vars_num++;
    if( pVD1->IsMultiple() )
      multiple_vars_num++;
  }

  /* Create type-specifiec buttons */
  if( nongroup_vars_num != 0 && group_vars_num == 0 ) {
    pBtnSetAll = new QPushButton( SM_DLG( DLG::VARSEDIT::BTN::SET_ALL ) );
    pLoButtons->addWidget( pBtnSetAll );
    connect( pBtnSetAll, SIGNAL(clicked()), this, SLOT(slotAcceptAll()) );

    pBtnResetAll = new QPushButton( SM_DLG( DLG::VARSEDIT::BTN::RESET_ALL ) );
    pLoButtons->addWidget( pBtnResetAll );
    connect( pBtnResetAll, SIGNAL(clicked()), this, SLOT(slotResetAll()) );
  }
  else
    pBtnSetAll = pBtnResetAll = null;

  pChHold = new QCheckBox( SM_DLG( DLG::VARSEDIT::LBL::HOLD ) );
  pChHold->setChecked( false );
  if( multiple_vars_num != 0 )
    pLoButtons->addWidget( pChHold );

  pLoButtons->addSpacerItem( new QSpacerItem( 1, 1, QSizePolicy::Expanding ) );

  /* Close button */
  QPushButton* pBtnClose = new QPushButton( SM_DLG( DLG::BTN::CLOSE ) );
  pLoButtons->addWidget( pBtnClose );
  connect( pBtnClose, SIGNAL(clicked()), this, SLOT(accept()) );

  QPushButton* pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );
  connect( pBtnHelp, SIGNAL(clicked()), this, SLOT(slotHelp()) );

  // Find max index 1411
  FOREACHPTRCONST( VarDefPtr, pVDa, pVS->VSD()->VarDefs() ) {
    varsSize.x = dg_max( varsSize.x, pVDa->Row()+1 );
    varsSize.y = dg_max( varsSize.y, pVDa->Column()+1 );
  }

  /* Prepare vars tables */
  QHBoxLayout* pLoVars = new QHBoxLayout();
  pLoMain->addLayout( pLoVars );

  vars.resize( varsSize.y );
  for( int c = 0; c < varsSize.y; c++ ) {
    vars[ c ] = new QTableWidget( varsSize.x, 4 );
    vars[ c ]->horizontalHeader()->hide();
    vars[ c ]->verticalHeader()->hide();
    vars[ c ]->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    vars[ c ]->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    vars[ c ]->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch ); //1411
    vars[ c ]->horizontalHeader()->setSectionResizeMode( 2, QHeaderView::Stretch ); //1411
    vars[ c ]->setSelectionBehavior( QAbstractItemView::SelectRows ); //1411
    vars[ c ]->setSelectionMode( QAbstractItemView::NoSelection ); //1411
    pLoVars->addWidget( vars[ c ] );
  }

  pSmHelp = new QSignalMapper( this );
  connect( pSmHelp, SIGNAL(mapped(int)), this, SLOT(slotVarHelp(int)) );
  pSmValue = new QSignalMapper( this );
  connect( pSmValue, SIGNAL(mapped(int)), this, SLOT(slotEdited(int)) );
  pSmReset = new QSignalMapper( this );
  connect( pSmReset, SIGNAL(mapped(int)), this, SLOT(slotMarkVar(int)) );
  pSmAccept = new QSignalMapper( this );
  connect( pSmAccept, SIGNAL(mapped(int)), this, SLOT(slotAcceptVar(int)) );
  pSmClick = new QSignalMapper( this );
  connect( pSmClick, SIGNAL(mapped(int)), this, SLOT(slotOnClick(int)) );

  const QString& str_help = SM_DLG( DLG::VARSEDIT::BTN::VAR_HELP );
  const QString& str_mark = SM_DLG( DLG::VARSEDIT::BTN::MARK );
  const QString& str_set = SM_DLG( DLG::VARSEDIT::BTN::SET );

  /* Fill vars table with items and widgets */
  FOREACHPTRCONST( VarDefPtr, pVD, pVS->VSD()->VarDefs() ) {
    NPoint index = pVD->Index(); //1411 all row indecies switched to VD index
    std::swap( index.x, index.y );
    // Name
    QString name = QString::fromStdString( pVD->Descr() );
    QTableWidgetItem* pItem = new QTableWidgetItem( name );
    pItem->setFlags( Qt::ItemIsEnabled );
    pItem->setData( Qt::UserRole, QVariant::fromValue( pVD ) );
    vars[ index.y ]->setItem( index.x, 0, pItem );

    // Help
    QTableWidgetItem* pItem1 = new QTableWidgetItem();
    pItem1->setFlags( Qt::ItemIsEnabled );
    vars[ index.y ]->setItem( index.x, 1, pItem1 );
    if( !pVD->Help().empty() ) {
      QPushButton* pBtnVarHelp = new QPushButton( str_help );
      pBtnVarHelp->adjustSize();
      connect( pBtnVarHelp, SIGNAL(clicked()), pSmHelp, SLOT(map()) );
      pSmHelp->setMapping( pBtnVarHelp, index.x + (index.y << 16) );
      vars[ index.y ]->setCellWidget( index.x, 1, pBtnVarHelp );
    }

    // Value / Reset
    QTableWidgetItem* pItem2 = new QTableWidgetItem();
    pItem2->setFlags( Qt::ItemIsEnabled );
    vars[ index.y ]->setItem( index.x, 2, pItem2 );
    if( !pVD->HasGroup() ) {
      //connect( vars[ index.y ], SIGNAL(itemClicked(QTableWidgetItem*)),
      //         this, SLOT(slotOnClick(QTableWidgetItem*)) );
      if( !HasAnyFlag( pVD->VarDefType(), VTF::FILENAME ) ) {
        QLineEdit* pLeValue = new QLineEdit();
        pSmValue->setMapping( pLeValue, index.x + (index.y << 16) );
        connect( pLeValue, SIGNAL( textEdited(QString)), pSmValue, SLOT(map()) );
        vars[ index.y ]->setCellWidget( index.x, 2, pLeValue );
      }
    }
    else {
      QPushButton* pBtnVarReset = new QPushButton( str_mark );
      pBtnVarReset->adjustSize();
      vars[ index.y ]->setCellWidget( index.x, 2, pBtnVarReset );
      pSmReset->setMapping( pBtnVarReset, index.x + (index.y << 16) );
      connect( pBtnVarReset, SIGNAL(clicked()), pSmReset, SLOT(map()) );
    }

    // Set
    QTableWidgetItem* pItem3 = new QTableWidgetItem();
    pItem3->setFlags( Qt::ItemIsEnabled );
    vars[ index.y ]->setItem( index.x, 3, pItem3 );
    QPushButton* pBtnVarSet = new QPushButton( str_set );
    pBtnVarSet->adjustSize();
    vars[ index.y ]->setCellWidget( index.x, 3, pBtnVarSet );
    pSmAccept->setMapping( pBtnVarSet, index.x + (index.y << 16) );
    connect( pBtnVarSet, SIGNAL(clicked()), pSmAccept, SLOT(map()) );
  }

  vars[ 0 ]->resizeRowsToContents();
  int h = vars[ 0 ]->rowHeight( 0 );

  for( int c = 0; c < varsSize.y; c++ ) {
    vars[ c ]->resizeColumnsToContents();
    vars[ c ]->setColumnWidth( 1, 50 ); //1411
    vars[ c ]->setColumnWidth( 3, 70 ); //1411

    for( int r = 0; r < varsSize.x; r++ )
      vars[ c ]->setRowHeight( r, h );
  }

  pLlStatus = new QLabel();
  pLoMain->addWidget( pLlStatus );

  /* hack: resize to table contents */
  /*int contents_width = 0;
  for( int c = 0; c < 4; c++ )
    contents_width += pVars->columnWidth(c);
  setMinimumSize( contents_width + 24, contents_height + 52 );*/
  int contents_height = vars[ 0 ]->rowCount() * vars[ 0 ]->rowHeight(0);
  setFixedHeight( contents_height + 73 );

  vars[ 0 ]->selectRow( 0 );

  slotResetAll();
}

void DlgVarsEdit::slotCollapse( bool b )
{
  for( int c = 0; c < varsSize.y; c++ )
    vars[ c ]->setVisible( !b );
  int new_height;
  if( !b ) {
    new_height = 73 + vars[ 0 ]->rowCount() * vars[ 0 ]->rowHeight(0);
    pBtnCollapse->setText( "▲" ); //1411
    pLlStatus->setVisible( true );
  }
  else {
    new_height = 44;
    pBtnCollapse->setText( "▼" ); //1411
    pLlStatus->setVisible( false );
  }
  this->setFixedHeight( new_height );
}

void DlgVarsEdit::slotAcceptAll()
{
  for( int x = 0; x < varsSize.x; x++ )
    for( int y = 0; y < varsSize.y; y++ )
      slotAcceptVar( x + (y << 16) );
}

void DlgVarsEdit::slotResetAll( bool _viewUpdate )
{
  if( resetEnabled ) {
    for( int x = 0; x < varsSize.x; x++ )
      for( int y = 0; y < varsSize.y; y++ )
        slotResetVar( x + (y << 16), _viewUpdate );
  }
  else
    resetEnabled = true;

}

void DlgVarsEdit::slotVarHelp( int _index )
{
  VarDefPtr pVD = RetrieveVarDef( NPoint( _index & 0xFFFF, _index >> 16 ) );
  QString sName = QString::fromStdString( pVD->Descr() );
  QString sHelp = QString::fromStdString( pVD->Help() );
  emit sgnlVarHelp( sName, sHelp );
}

void DlgVarsEdit::slotEdited( int _index )
{
  /* Enable reset and set buttons */
  NPoint index( _index & 0xFFFF, _index >> 16 );
  QWidget* pWgt = vars[ index.y ]->cellWidget( index.x, 3 );
  QPushButton* pBtnSet = qobject_cast< QPushButton* >( pWgt );
  pBtnSet->setEnabled( true );

  changed_num++;
  if( pBtnResetAll != null )
    pBtnResetAll->setEnabled( true );
  if( pBtnSetAll != null )
    pBtnSetAll->setEnabled( true );

  VarDefPtr pVD = RetrieveVarDef( index );
  if( pVD == null )
    return;
  UPtr value = pModel->Vars()->GetVarEx( pVS, pVD );
  QString str_value = ToQString( pModel->Vars()->GetVarValueDescr( pVD->VarDefType(), value ) );
  pLlStatus->setText( pSM->GetString( DG3::OLD_VALUE, "msg" ).arg( str_value ) );
}

void DlgVarsEdit::slotOnClick( int _index )
{
  NPoint index( _index & 0xFFFF, _index >> 16 );
  if( index.y == 2 ) {
    /* using FileDialog */
    VarDefPtr pVD = RetrieveVarDef( index );
    if( HasAnyFlag( pVD->VarDefType(), VTF::FILENAME ) ) {
      QString title = SM_DLG( DLG::VARSFILESET::TITLE );
      QString filter = SM_MSG( ENV::VARSFILESETMASK );
      QString fileName = QFileDialog::getOpenFileName(
                           this, title, QDir::currentPath(), filter );
      QTableWidgetItem* pItem = vars[ index.y ]->item( index.x, 2 );
      pItem->setText( fileName );
    }
  }
}

void DlgVarsEdit::slotMarkVar( int _index )
{
  slotResetVar( _index, true );
  NPoint index( _index & 0xFFFF, _index >> 16 );
  VarDefPtr pVD = RetrieveVarDef( index );
  if( pVD == null )
    return;
  UPtr value = pModel->Vars()->GetVarEx( pVS, pVD );
  QString str_value = ToQString( pModel->Vars()->GetVarValueDescr( pVD->VarDefType(), value ) );
  pLlStatus->setText( pSM->GetString( DG3::OLD_VALUE, "msg" ).arg( str_value ) );
}

void DlgVarsEdit::slotResetVar( int _index, bool _viewUpdate )
{
  static const QString& scrsNosel = SM_MSG( STR::NOSEL );

  NPoint index( _index & 0xFFFF, _index >> 16 );
  VarDefPtr pVD = RetrieveVarDef( index );
  if( pVD == null )
    return;

  /* Reset var string */
  UPtr value = pModel->Vars()->GetVarEx( pVS, pVD );

  bool reset = false;
  bool set = false; // enabling SET button while holding value

  if( pVD->HasGroup() ) {
    if( _viewUpdate ) {
      assert( value.Type() == UPtr::LIST );
      pModel->UnmarkAll();
      pModel->MarkGroup( value.ListRef() );
      resetEnabled = false;
      pModel->ActionStack().Complete( "Vars:Reset" );
    }
  }
  else {
    QString str_value = ToQString( pModel->Vars()->GetVarValueDescr( pVD->VarDefType(), value ) );
    if( !HasAnyFlag( pVD->VarDefType(), VTF::FILENAME ) ) {
      QWidget* pWgt = vars[ index.y ]->cellWidget( index.x, 2 );
      QLineEdit* pLeValue = qobject_cast< QLineEdit* >( pWgt );
      QString str_current = pLeValue->text();
      if( pLeValue != null && str_value != str_current ) {
        if( pChHold->isChecked() and not value.IsNOSEL() and str_current != scrsNosel )
          set = true;
        else {
          pLeValue->setText( str_value );
          reset = true;
        }
      }
    }
    else {
      QTableWidgetItem* pItem = vars[ index.y ]->item( index.x, 2 );
      QString str_current = pItem->text();
      if( str_value != str_current ) {
        if( pChHold->isChecked() and not value.IsNOSEL() and str_current != scrsNosel )
          set = true;
        else {
          pItem->setText( str_value );
          reset = true;
        }
      }
    }
  }

  if( reset && not pVD->HasGroup()) { //1411
    /* Disable reset and set buttons */
    QWidget* pWgt = vars[ index.y ]->cellWidget( index.x, 3 );
    QPushButton* pBtnSet = qobject_cast< QPushButton* >( pWgt );
    pBtnSet->setDisabled( true );

    changed_num--;
    if( changed_num == 0 && pBtnResetAll != null && pBtnSet != null ) {
      pBtnResetAll->setDisabled( true );
      pBtnSetAll->setDisabled( true );
    }
  }

  if( set ) {
    /* Enable set button */
    QWidget* pWgt = vars[ index.y ]->cellWidget( index.x, 3 );
    QPushButton* pBtnSet = qobject_cast< QPushButton* >( pWgt );
    pBtnSet->setEnabled( true );
  }

  if( _viewUpdate ) {
    resetEnabled = false;
    pModel->ActionStack().Complete( "Vars:Reset" );
  }
}

void DlgVarsEdit::slotAcceptVar( int _index )
{
  SENDER_NAME_Q( "DlgVarsEdit::slotAcceptVar" );

  NPoint index( _index & 0xFFFF, _index >> 16 );
  VarDefPtr pVD = RetrieveVarDef( index );
  if( pVD == null )
    return;

  int r = 0;
  /* Check locks */
  std::string reason;
  if( pModel->Vars()->GetVarExLocks( pVS, pVD, reason ) )
    r = pConsole->Send( WND_ERROR, SENDER, ERR::USEDBY,
                    QStringList( QString::fromStdString( reason ) ) );
  else {
    /* Create UPtr and try to set it */
    UPtr val;
    if( pVD->HasGroup() ) {
      IVarItemList objects;
      ConvertVarItemList( pModel->MarkedObjects(), objects );
      val.SetValue( objects, true );
    }
    else {
      QWidget* pWgt = vars[ index.y ]->cellWidget( index.x, 2 );
      QLineEdit* pLeValue = qobject_cast< QLineEdit* >( pWgt );
      QString sText = (pLeValue != null) ? pLeValue->text() : "";
      val.SetValue( sText.toStdString(), true );
    }

    r = pModel->Vars()->SetVarEx( pVS, pVD, val );
    /* Fail */
    if( r != 0 ) {
      IVarItemPtr pErrObj = null;
      pModel->Vars()->CheckValue( val, pVD->VarDefType(), &pErrObj );
      if( pErrObj != null )
        pView->CurrentScene()->CreateLabelItem( CastVarItem< IComponent >( pErrObj ), SM_MSG( STR::ERRLABEL ) );
      pModel->ActionStack().Cancel();
      //pModel->UndoStack().Complete();
      pConsole->Send( WND_ERROR, SENDER, r );
    }
  }

  /* Success */
  if( r == 0 ) {
    /* disable local set button */
    QWidget* pWgt = vars[ index.y ]->cellWidget( index.x, 3 );
    if( not pVD->HasGroup() ) { //1411
      QPushButton* pBtnSet = qobject_cast< QPushButton* >( pWgt );
      pBtnSet->setDisabled( true );
    }
    else {
      UPtr value = pModel->Vars()->GetVarEx( pVS, pVD );
      QString str_value = ToQString( pModel->Vars()->GetVarValueDescr( pVD->VarDefType(), value ) );
      pLlStatus->setText( pSM->GetString( DG3::NEW_VALUE, "msg" ).arg( str_value ) );
    }
    /* Disable common reset and set buttons */
    slotResetVar( _index );
    /* Update all */
    resetEnabled = false;
    pModel->ActionStack().Complete( "Vars:Accept" );
  }
}

void DlgVarsEdit::contextMenuEvent( QContextMenuEvent* _pCME )
{
  QTableWidgetItem* pItem = null;
  int c = 0;
  for( ; c < varsSize.y; c++ )
    if( (pItem = vars[ c ]->itemAt( _pCME->pos() - vars[ c ]->pos() )) != null )
      break;
  if( pItem == null )
    return _pCME->accept();

  currentIndex = NPoint( pItem->row(), c );

  QMenu menu( this );
  QAction* pAct = menu.addAction( SM_DLG( DLG::VARSEDIT::VARS_POPUP::RESET ),
                                  this, SLOT(slotPopupReset()) );
  pAct->setToolTip( SM_DLG( DLG::VARSEDIT::VARS_POPUP::RESET_HELP ) );

  menu.addSeparator();

  QMenu* pSubMenu = menu.addMenu( SM_DLG( DLG::VARSEDIT::VARS_POPUP::COMPARE ) );
  pSubMenu->setToolTip( SM_DLG( DLG::VARSEDIT::COMPARE::HELP ) );

  pAct = pSubMenu->addAction( SM_DLG( DLG::VARSEDIT::COMPARE::EQUAL ),
                              this, SLOT(slotPopupCompareEQ()) );
  pAct = pSubMenu->addAction( SM_DLG( DLG::VARSEDIT::COMPARE::NOT_EQUAL ),
                              this, SLOT(slotPopupCompareNE()) );
  pAct = pSubMenu->addAction( SM_DLG( DLG::VARSEDIT::COMPARE::GT ),
                              this, SLOT(slotPopupCompareGT()) );
  pAct = pSubMenu->addAction( SM_DLG( DLG::VARSEDIT::COMPARE::GE ),
                              this, SLOT(slotPopupCompareGE()) );
  pAct = pSubMenu->addAction( SM_DLG( DLG::VARSEDIT::COMPARE::LT),
                              this, SLOT(slotPopupCompareLT()) );
  pAct = pSubMenu->addAction( SM_DLG( DLG::VARSEDIT::COMPARE::LE ),
                              this, SLOT(slotPopupCompareLE()) );

  pAct = menu.addAction( SM_DLG( DLG::VARSEDIT::VARS_POPUP::DISPLAY ),
                         this, SLOT(slotPopupDisplay()) );
  pAct->setToolTip( SM_DLG( DLG::VARSEDIT::VARS_POPUP::DISPLAY_HELP ) );
  menu.exec( _pCME->globalPos() );
}


void DlgVarsEdit::slotPopupReset() { slotResetVar( currentIndex.x + (currentIndex.y << 16) ); }

void DlgVarsEdit::slotPopupCompareEQ() { Compare( EQ ); }
void DlgVarsEdit::slotPopupCompareNE() { Compare( NE ); }
void DlgVarsEdit::slotPopupCompareGT() { Compare( GT ); }
void DlgVarsEdit::slotPopupCompareGE() { Compare( GE ); }
void DlgVarsEdit::slotPopupCompareLT() { Compare( LT ); }
void DlgVarsEdit::slotPopupCompareLE() { Compare( LE ); }
void DlgVarsEdit::Compare( CompareOperator _co )
{
  SENDER_NAME_Q( "DlgVarsEdit::Compare" );
  VarDefPtr pVD = RetrieveVarDef( currentIndex );
  if( pVD == null )
    return;

  QTableWidgetItem* pItem = vars[ currentIndex.y ]->item( currentIndex.x, 2 );

  if( !pVD->IsMultiple() ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::VARSEDIT::ERR::BAD_SCOPE );
    return;
  }

  QWidget* pWgt = vars[ currentIndex.y ]->cellWidget( currentIndex.x, 2 );
  QLineEdit* pLeValue = qobject_cast< QLineEdit* >( pWgt );
  QString sText = (pLeValue != null) ? pLeValue->text() : "";
  const char* caValue = sText.toUtf8().data();

  int i1, i2;
  double f1, f2;

  switch( pVD->VarDefType() ) {
  case VT::INT: {
    if( sscanf( caValue, "%d", &i1 ) != 1 ) {
      pConsole->Send( WND_ERROR, SENDER, DLG::VARSEDIT::ERR::BAD_NUMBER );
      return;
    }
    break;
  }
  case VT::FLOAT: {
    if( sscanf( caValue, SCANFLT, &f1 ) != 1 ) {
      pConsole->Send( WND_ERROR, SENDER, DLG::VARSEDIT::ERR::BAD_NUMBER );
      return;
    }
    break;
  }
  case VT::TEXT:
    break;
  case VT::FILENAME:
    sText = pItem->text();
    break;
  default:
    return;
  }

  IVarOriginList origins;
  pModel->Vars()->GetVarOriginGroup( pVS, pVD, false, origins );

  FOREACH( IVarOriginIter, itOrigin, origins ) {
    UPtr value = pModel->Vars()->GetVar( *itOrigin, pVD, pVS );
    assert( value.Type() == UPtr::STR );
    QString sText2 = QString::fromStdString( value.StringRef() );
    const char* caValue2 = sText2.toUtf8().data();

    int comp = 0;
    switch( pVD->VarDefType() ) {
    case VT::INT: {
      if( sscanf( caValue2, "%d", &i2 ) != 1 )
        comp = -2;
      else if( i1 == i2 )
        comp = 0;
      else
        comp = (i1 < i2 ) ? 1 : -1;
      break;
    }
    case VT::FLOAT: {
      if( sscanf( caValue2, SCANFLT, &f2 ) != 1 )
        comp = -2;
      else if( f1 == f2 )
        comp = 0;
      else
        comp = (f1 < f2) ? 1 : -1;
      break;
    }
    case VT::TEXT:
    case VT::FILENAME:
      comp = sText2.compare( sText );
      break;
    default:
      assert( false );
    }

    bool removeIt = false;
    switch( _co ) {
    case EQ: if( comp != 0 )                removeIt = true; break;
    case NE: if( comp == 0 )                removeIt = true; break;
    case GT: if( comp != 1 )                removeIt = true; break;
    case GE: if( comp != 1 && comp != 0 )   removeIt = true; break;
    case LT: if( comp != -1 )               removeIt = true; break;
    case LE: if( comp != -1 && comp != 0 )  removeIt = true; break;
    }

    if( removeIt )
      itOrigin = --origins.erase( itOrigin );
  }

  pModel->UnmarkAll();
  IComponentList origins_ic;
  ConvertVarOriginList( origins, origins_ic );
  pModel->MarkGroup( origins_ic );
  pModel->ActionStack().Complete( "Vars:Compare" );

  pConsole->Send( MT::WINDOW, AL::INFO, SENDER,
                  DLG::VARSEDIT::LBL::OBJECT_MATCHING,
                  QStringList( QString( "%1" ).arg( origins.size() ) ) );
}

void DlgVarsEdit::slotPopupDisplay()
{
  SENDER_NAME_Q( "DlgVarsEdit::slotPopupDisplay" );
  VarDefPtr pVD = RetrieveVarDef( currentIndex );
  if( pVD == null )
    return;

  if( !pVD->IsMultiple() ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::VARSEDIT::ERR::BAD_SCOPE );
    return;
  }

  IVarOriginList origins;
  pModel->Vars()->GetVarOriginGroup( pVS, pVD, false, origins );

  pView->CurrentScene()->RemoveAllLabelItems();

  FOREACH_CONST( IVarOriginIter, it, origins ) {
    UPtr value = pModel->Vars()->GetVar( *it, pVD, pVS );
    assert( value.Type() == UPtr::STR );
    QString sDescr = ToQString( pModel->Vars()->GetVarValueDescr( pVD->VarDefType(), value ) );
    pView->CurrentScene()->CreateLabelItem( CastVarOrigin< IComponent >( *it ), sDescr );
  }

  pModel->ActionStack().Complete( "Vars:Popup" );
}
