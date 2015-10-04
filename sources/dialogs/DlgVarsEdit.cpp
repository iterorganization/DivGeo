#include "DlgVarsEdit.h"

VarDefPtr DlgVarsEdit::RetrieveVarDef( int row ) const
{
  QTableWidgetItem* pItem = pVars->item( row, 0 );
  QVariant qv = pItem->data( Qt::UserRole );
  if( qv.isNull() || qv.userType() != QMetaType::type( "VarDefPtr" ) )
    return null;
  return qv.value< VarDefPtr >();
}

DlgVarsEdit::DlgVarsEdit(VarSetPtr _pVS, ModelPtr _pModel, CViewWndPtr _pView,
                         Console* _pConsole, StringsManager* _pSM,
                         QWidget *parent):
  QDialog(parent),
  pSM( _pSM ),
  pModel( _pModel ),
  pView( _pView ),
  pConsole( _pConsole ),
  pVS( _pVS ),
  changed_num( 0 )
{
  QString title = QString::fromStdString( pVS->Description() );
  this->setWindowTitle( title );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  QPushButton* pBtnCollapse = new QPushButton( "+" );
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

  if( multiple_vars_num != 0 ) {
    pChHold = new QCheckBox( SM_DLG( DLG::VARSEDIT::LBL::HOLD ) );
    pChHold->setChecked( false );
    pLoButtons->addWidget( pChHold );
  }

  pLoButtons->addSpacerItem( new QSpacerItem( 1, 1, QSizePolicy::Expanding ) );

  /* Close button */
  QPushButton* pBtnClose = new QPushButton( SM_DLG( DLG::BTN::CLOSE ) );
  pLoButtons->addWidget( pBtnClose );
  connect( pBtnClose, SIGNAL(clicked()), this, SLOT(accept()) );

  QPushButton* pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );
  connect( pBtnHelp, SIGNAL(clicked()), this, SLOT(slotHelp()) );

  /* Prepare vars table */
  pVars = new QTableWidget( pVS->VSD()->VarDefsNum(), 4 );
  pVars->horizontalHeader()->hide();
  pVars->verticalHeader()->hide();
  pVars->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  pVars->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  pLoMain->addWidget( pVars );

  pSmHelp = new QSignalMapper( this );
  connect( pSmHelp, SIGNAL(mapped(int)), this, SLOT(slotVarHelp(int)) );
  pSmValue = new QSignalMapper( this );
  connect( pSmValue, SIGNAL(mapped(int)), this, SLOT(slotEdited(int)) );
  pSmReset = new QSignalMapper( this );
  connect( pSmReset, SIGNAL(mapped(int)), this, SLOT(slotResetVar(int)) );
  pSmAccept = new QSignalMapper( this );
  connect( pSmAccept, SIGNAL(mapped(int)), this, SLOT(slotAcceptVar(int)) );

  const QString& str_help = SM_DLG( DLG::VARSEDIT::BTN::VAR_HELP );
  const QString& str_mark = SM_DLG( DLG::VARSEDIT::BTN::MARK );
  const QString& str_set = SM_DLG( DLG::VARSEDIT::BTN::SET );

  /* Fill vars table with items and widgets */
  ulong row = 0;
  FOREACHPTRCONST( VarDefPtr, pVD, pVS->VSD()->VarDefs() ) {
    QString name = QString::fromStdString( pVD->Descr() );
    QTableWidgetItem* pItem = new QTableWidgetItem( name );
    pItem->setFlags( Qt::ItemIsEnabled );
    pItem->setData( Qt::UserRole, QVariant::fromValue( pVD ) );
    pVars->setItem( row, 0, pItem );

    QTableWidgetItem* pItem1 = new QTableWidgetItem();
    pItem1->setFlags( Qt::ItemIsEnabled );
    pVars->setItem( row, 1, pItem1 );
    if( !pVD->Help().empty() ) {
      QPushButton* pBtnVarHelp = new QPushButton( str_help );
      pBtnVarHelp->adjustSize();
      connect( pBtnVarHelp, SIGNAL(clicked()), pSmHelp, SLOT(map()) );
      pSmHelp->setMapping( pBtnVarHelp, row );
      pVars->setCellWidget( row, 1, pBtnVarHelp );
    }

    QTableWidgetItem* pItem2 = new QTableWidgetItem();
    pItem2->setFlags( Qt::ItemIsEnabled );
    pVars->setItem( row, 2, pItem2 );
    if( !pVD->HasGroup() ) {
      connect( pVars, SIGNAL(itemClicked(QTableWidgetItem*)),
               this, SLOT(slotOnClick(QTableWidgetItem*)) );
      if( !HasAnyFlag( pVD->VarDefType(), VTF::FILENAME ) ) {
        QLineEdit* pLeValue = new QLineEdit();
        pSmValue->setMapping( pLeValue, row );
        connect( pLeValue, SIGNAL( textEdited(QString)), pSmValue, SLOT(map()) );
        pVars->setCellWidget( row, 2, pLeValue );
      }
    }
    else {
      QPushButton* pBtnVarReset = new QPushButton( str_mark );
      pBtnVarReset->adjustSize();
      pVars->setCellWidget( row, 2, pBtnVarReset );
      pSmReset->setMapping( pBtnVarReset, row );
      connect( pBtnVarReset, SIGNAL(clicked()), pSmReset, SLOT(map()) );

    }

    QTableWidgetItem* pItem3 = new QTableWidgetItem();
    pItem3->setFlags( Qt::ItemIsEnabled );
    pVars->setItem( row, 3, pItem3 );
    QPushButton* pBtnVarSet = new QPushButton( str_set );
    pBtnVarSet->adjustSize();
    pVars->setCellWidget( row, 3, pBtnVarSet );
    pSmAccept->setMapping( pBtnVarSet, row );
    connect( pBtnVarSet, SIGNAL(clicked()), pSmAccept, SLOT(map()) );
    row++;
  }

  pVars->resizeColumnsToContents();
  pVars->resizeRowsToContents();

  /* hack: resize to table contents */
  int contents_width = 0;
  for( int c = 0; c < 4; c++ )
    contents_width += pVars->columnWidth(c);
  int contents_height = pVars->rowCount() * pVars->rowHeight(0);
  setFixedSize( contents_width + 24, contents_height + 52 );

  slotResetAll();
}

void DlgVarsEdit::slotCollapse( bool b )
{
  pVars->setVisible( !b );
  int new_height = 52;
  if( !b )
    new_height += pVars->rowCount() * pVars->rowHeight(0);
  this->setFixedHeight( new_height );
}

void DlgVarsEdit::slotAcceptAll()
{
  for( int r = 0; r < pVars->rowCount(); r++ )
    slotAcceptVar( r );
}

void DlgVarsEdit::slotResetAll()
{
  for( int r = 0; r < pVars->rowCount(); r++ )
    slotResetVar( r );
}

void DlgVarsEdit::slotVarHelp( int row )
{
  VarDefPtr pVD = RetrieveVarDef( row );
  QString sName = QString::fromStdString( pVD->Descr() );
  QString sHelp = QString::fromStdString( pVD->Help() );
  emit sgnlVarHelp( sName, sHelp );
}

void DlgVarsEdit::slotEdited( int row )
{
  /* Enable reset and set buttons */
  QPushButton* pBtnSet = qobject_cast< QPushButton* >( pVars->cellWidget( row, 3 ) );
  pBtnSet->setEnabled( true );

  changed_num++;
  if( pBtnResetAll != null )
    pBtnResetAll->setEnabled( true );
  if( pBtnSetAll != null )
    pBtnSetAll->setEnabled( true );
}

void DlgVarsEdit::slotOnClick( QTableWidgetItem* pItem )
{
  if( pItem->column() == 2 ) {
    /* using FileDialog */
    VarDefPtr pVD = RetrieveVarDef( pItem->row() );
    if( HasAnyFlag( pVD->VarDefType(), VTF::FILENAME ) ) {
      QString title = SM_DLG( DLG::VARSFILESET::TITLE );
      QString filter = SM_MSG( ENV::VARSFILESETMASK );
      QString fileName = QFileDialog::getOpenFileName(
                           this, title, QDir::currentPath(), filter );
      pItem->setText( fileName );
    }
  }
}

void DlgVarsEdit::slotResetVar( int row )
{
  VarDefPtr pVD = RetrieveVarDef( row );

  /* Reset var string */
  UPtr value = pModel->Vars()->GetVarEx( pVS, pVD );

  bool reset = false;

  if( pVD->HasGroup() ) {
    assert( value.Type() == UPtr::LIST );
    pModel->UnmarkAll();
    pModel->MarkGroup( value.ListRef() );
    pModel->ActionStack().Complete( "Vars:Reset" );
  }
  else {
    QString str_value = QString::fromStdString( pModel->Vars()->GetVarValueDescr( pVD->VarDefType(), value ) );
    if( !HasAnyFlag( pVD->VarDefType(), VTF::FILENAME ) ) {
      QLineEdit* pLeValue = qobject_cast< QLineEdit* >( pVars->cellWidget( row, 2 ) );
      if( pLeValue != null && str_value != pLeValue->text() ) {
        pLeValue->setText( str_value );
        reset = true;
      }
    }
    else {
      if( str_value != pVars->item( row, 2 )->text() ) {
        pVars->item( row, 2 )->setText( str_value );
        reset = true;
      }
    }
  }

  if( reset ) {
    /* Disable reset and set buttons */
    QPushButton* pBtnSet = qobject_cast< QPushButton* >( pVars->cellWidget( row, 3 ) );
    pBtnSet->setDisabled( true );

    changed_num--;
    if( changed_num == 0 && pBtnResetAll != null && pBtnSet != null ) {
      pBtnResetAll->setDisabled( true );
      pBtnSetAll->setDisabled( true );
    }
  }

  pModel->ActionStack().Complete( "Vars:Reset" );
}

void DlgVarsEdit::slotAcceptVar( int row )
{
  SENDER_NAME_Q( "DlgVarsEdit::slotAcceptVar" );

  VarDefPtr pVD = RetrieveVarDef( row );

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
      QLineEdit* pLeValue = qobject_cast< QLineEdit* >( pVars->cellWidget( row, 2 ) );
      QString sText = (pLeValue != null) ? pLeValue->text() : "";
      val.SetValue( sText.toStdString(), true );
    }

    r = pModel->Vars()->SetVarEx( pVS, pVD, val );
    /* Fail */
    if( r != 0 ) {
      IVarItemPtr pErrObj = null;
      pModel->Vars()->CheckValue( val, pVD->VarDefType(), &pErrObj );
      if( pErrObj != null )
        pView->CurrentScene()->CreateLabelItem( CastVarItem( pErrObj ), SM_MSG( STR::ERRLABEL ) );
      pModel->ActionStack().Cancel();
      //pModel->UndoStack().Complete();
      pConsole->Send( WND_ERROR, SENDER, r );
    }
  }

  /* Success */
  if( r == 0 ) {
    pModel->ActionStack().Complete( "Vars:Accept" );
    slotResetVar( row );
  }
}

void DlgVarsEdit::contextMenuEvent( QContextMenuEvent* _pCME )
{
  QTableWidgetItem* pItem = pVars->itemAt( _pCME->pos() - pVars->pos() );
  if( pItem == null )
    return _pCME->accept();

  current_row = pItem->row();

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


void DlgVarsEdit::slotPopupReset() { slotResetVar( current_row ); }

void DlgVarsEdit::slotPopupCompareEQ() { Compare( EQ ); }
void DlgVarsEdit::slotPopupCompareNE() { Compare( NE ); }
void DlgVarsEdit::slotPopupCompareGT() { Compare( GT ); }
void DlgVarsEdit::slotPopupCompareGE() { Compare( GE ); }
void DlgVarsEdit::slotPopupCompareLT() { Compare( LT ); }
void DlgVarsEdit::slotPopupCompareLE() { Compare( LE ); }
void DlgVarsEdit::Compare( CompareOperator _co )
{
  SENDER_NAME_Q( "DlgVarsEdit::Compare" );
  VarDefPtr pVD = RetrieveVarDef( current_row );
  QTableWidgetItem* pItem = pVars->item( current_row, 2 );

  if( !pVD->IsMultiple() ) {
    pConsole->Send( WND_ERROR, SENDER, DLG::VARSEDIT::ERR::BAD_SCOPE );
    return;
  }

  QLineEdit* pLeValue = qobject_cast< QLineEdit* >( pVars->cellWidget( current_row, 2 ) );
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
  VarDefPtr pVD = RetrieveVarDef( current_row );

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
    QString sDescr = QString::fromStdString( pModel->Vars()->GetVarValueDescr( pVD->VarDefType(), value ) );
    pView->CurrentScene()->CreateLabelItem( CastVarOrigin( *it ), sDescr );
  }

  pModel->ActionStack().Complete( "Vars:Popup" );
}
