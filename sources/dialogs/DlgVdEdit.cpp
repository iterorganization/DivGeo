#include "DlgVdEdit.h"

DlgVdEdit::DlgVdEdit( VarDefPtr _pVD, ModelPtr _pModel,
                      StringsManager* _pSM, Console* _pConsole,
                      QWidget *parent) :
  QDialog(parent),
  pVD( _pVD ),
  pModel( _pModel ),
  pSM( _pSM ),
  pConsole( _pConsole )
{
  QString title = SM_DLG( DLG::VARDEFEDIT::TITLE_EX )
                  .arg( QString::fromStdString( pVD->Name() ) );
  this->setWindowTitle( title );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );

  QFormLayout* pLoProperties = new QFormLayout();
  pLoMain->addLayout( pLoProperties );

  pLeDescr = new QLineEdit( QString::fromStdString( pVD->Descr() ) );
  pLoProperties->addRow( SM_DLG( DLG::VARDEFEDIT::LBL::DESCR ), pLeDescr );

  /* setup type combobox */
  pCbType = new QComboBox();
  pLoProperties->addRow( SM_DLG( DLG::VARDEFEDIT::TYPE::TITLE ), pCbType );

  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::INT ),         VT::INT );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::FLOAT ),       VT::FLOAT );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::TEXT ),        VT::TEXT );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::FILENAME ),    VT::FILENAME );
  pCbType->insertSeparator( 4 );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::ELEMENT ),     VT::ELEM );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::ELEMSET ),     VT::ELEMS );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::TARGET1 ),     VT::TARGET1 );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::TARGET2 ),     VT::TARGET2 );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::STRUCTURE ),   VT::STRUCTURE );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::STRUCTPART ),  VT::STRUCTPART );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::CHORDS ),      VT::CHORDS );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::TOPVIEW ),     VT::TOPVIEW );
  pCbType->insertSeparator( 13 );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::MESH_CELLS ),  VT::MESH_CELLS );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::MESH_ELEMS ),  VT::MESH_ELEMENTS );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::MESH_HELEMS ), VT::MESH_H_ELEMENTS );
  pCbType->addItem( SM_DLG( DLG::VARDEFEDIT::TYPE::MESH_VELEMS ), VT::MESH_V_ELEMENTS );

  /* select current index */
  for( int index = 0; index < pCbType->count(); index++ ) {
    QVariant qv = pCbType->itemData( index );
    if( qv.isNull() )
      continue;
    VarType vt = (VarType)qv.toInt();
    if( pVD->VarDefType() == vt ) {
      pCbType->setCurrentIndex( index );
      break;
    }
  }

  /* setup scope combobox */
  pCbScope = new QComboBox();
  pLoProperties->addRow( SM_DLG( DLG::VARDEFEDIT::SCOPE::TITLE ), pCbScope );

  pCbScope->addItem( SM_DLG( DLG::VARDEFEDIT::SCOPE::WITH_VARSET ) );
  pCbScope->addItem( SM_DLG( DLG::VARDEFEDIT::SCOPE::WITH_ELEMS ), VF::FORELEMS );
  pCbScope->addItem( SM_DLG( DLG::VARDEFEDIT::SCOPE::WITH_SOURCES ), VF::FORSOURCES );
  pCbScope->addItem( SM_DLG( DLG::VARDEFEDIT::SCOPE::WITH_CHORDS ), VF::FORCHORDS );

  /* select current index */
  switch( (VarDefFlag)(pVD->Flags() & VF::MULTIPLE) ) {
  case VF::FORELEMS: pCbScope->setCurrentIndex( 1 ); break;
  case VF::FORSOURCES: pCbScope->setCurrentIndex( 2 ); break;
  case VF::FORCHORDS: pCbScope->setCurrentIndex( 3 ); break;
  default: pCbScope->setCurrentIndex( 0 ); break;
  }

  /* setup default value */
  QString sValue;
  if( !pVD->HasGroup() ) {
    UPtr value = pModel->Vars()->GetVar( pVD->VSD(), pVD, null );
    if( value.Type() == UPtr::STR )
      sValue = QString::fromStdString( value.StringRef() );
  }
  pLeDefault = new QLineEdit( sValue );
  pLoProperties->addRow( SM_DLG( DLG::VARDEFEDIT::LBL::DEFAULT ), pLeDefault );

  pChExport = new QCheckBox( SM_DLG( DLG::VARDEFEDIT::LBL::NO_EXPORT ) );
  pLoMain->addWidget( pChExport );
  pChExport->setChecked( HasAnyFlag( pVD->Flags(), VF::NOEXPORT ) );

  pChLayer = new QCheckBox( SM_DLG( DLG::VARDEFEDIT::LBL::LAYER ) );
  pLoMain->addWidget( pChLayer );
  pChExport->setChecked( HasAnyFlag( pVD->Flags(), VF::LAYERINDEX ) );

  /* Bottom buttons */
  QHBoxLayout* pLoButtons = new QHBoxLayout();
  pLoMain->addLayout( pLoButtons );

  QPushButton* pBtnOk = new QPushButton( SM_DLG( DLG::BTN::OK ) );
  pLoButtons->addWidget( pBtnOk );
  QPushButton* pBtnClose = new QPushButton( SM_DLG( DLG::BTN::CLOSE ) );
  pLoButtons->addWidget( pBtnClose );
  QPushButton* pBtnHelp = new QPushButton( SM_DLG( DLG::BTN::HELP ) );
  pLoButtons->addWidget( pBtnHelp );
  QPushButton* pBtnEditHelp = new QPushButton( SM_DLG( DLG::VARDEFEDIT::BTN::EDIT_HELP ) );
  pLoButtons->addWidget( pBtnEditHelp );
  QPushButton* pBtnRemove = new QPushButton( SM_DLG( DLG::VARDEFEDIT::BTN::REMOVE ) );
  pLoButtons->addWidget( pBtnRemove );

  connect( pBtnOk, SIGNAL(clicked()), this, SLOT(slotAccept()) );
  connect( pBtnClose, SIGNAL(clicked()), this, SLOT(reject()) );
  connect( pBtnHelp, SIGNAL(clicked()), this, SLOT(slotHelp()) );
  connect( pBtnEditHelp, SIGNAL(clicked()), this, SLOT(slotEditHelp()) );
  connect( pBtnRemove, SIGNAL(clicked()), this, SLOT(slotRemove()) );

  connect( pCbType, SIGNAL(currentIndexChanged(int)),
           this,    SLOT(slotTypeChanged(int)) );
}

void DlgVdEdit::slotTypeChanged( int _index )
{
  VarType vt = (VarType)pCbType->itemData( _index ).toInt();
  pLeDefault->setDisabled( HasAnyFlag( vt, VTF::HASGROUP ) );
}

void DlgVdEdit::slotAccept()
{
  SENDER_NAME_Q( "DlgVdEdit::slotAccept" );
  VarType vt = (VarType)pCbType->itemData( pCbType->currentIndex() ).toInt();

  int flags = pCbScope->itemData( pCbScope->currentIndex() ).toInt();

  if( pChExport->isChecked() )
    flags |= VF::NOEXPORT;
  if( pChLayer->isChecked() )
    flags |= VF::LAYERINDEX;

  if( HasAnyFlag( flags, VF::MULTIPLE ) && HasAnyFlag( vt, VTF::HASGROUP ) ) {
    pConsole->Send( WND_ERROR, SENDER, ERR::MULTGROUP );
    return;
  }

  if( HasAnyFlag( vt, VTF::SINGLE ) ) {
    if( HasAnyFlag( flags, VF::MULTIPLE ) ) {
      pConsole->Send( WND_ERROR, SENDER, ERR::SINGLE );
      return;
    }
    FOREACHPTRCONST( VarDefPtr, pVD_s, pModel->Vars()->VarDefs() ) {
      if( pVD_s != pVD && pVD->VarDefType() == vt ) {
        pConsole->Send( WND_ERROR, SENDER, ERR::SINGLE );
        return;
      }
    }
  }

  /* Additional checks if VF_LAYERINDEX set */

  if( HasAnyFlag( flags, VF::LAYERINDEX ) ) {
    if( HasAnyFlag( flags, VF::MULTIPLE ) ) {
      pConsole->Send( WND_ERROR, SENDER, DLG::VARDEFEDIT::ERR::MULTIPLE );
      return;
    }
    if( HasAnyFlag( vt, VTF::HASGROUP ) ) {
      pConsole->Send( WND_ERROR, SENDER, DLG::VARDEFEDIT::ERR::GROUP );
      return;
    }
    FOREACHPTRCONST( VarDefPtr, pVD_s, pVD->VSD()->VarDefs() ) {
      if( pVD_s != pVD && HasAnyFlag( pVD_s->Flags(), VF::LAYERINDEX ) ) {
        pConsole->Send( WND_ERROR, SENDER, DLG::VARDEFEDIT::ERR::SECOND,
          QStringList() << QString::fromStdString( pVD_s->Name() )
                        << QString::fromStdString( pVD_s->Descr() ) );
        return;
      }
    }
  }

  QString sDescr = pLeDescr->text();
  if( sDescr.isEmpty() )
    sDescr = "CLASSIFIED";

  ActChangeValue( pModel, pVD, pVD, VarDef::p_descr, sDescr.toStdString(), REDRAW, DO_AT_ONCE );
  int err = pVD->Change( pVD->VSD(), vt, flags, pVD->Index() );

  if( err != 0 && err != STR::VARSLOST ) {
    pModel->ActionStack().Cancel();
    pConsole->Send( WND_ERROR, SENDER, err );
    return;
  }

  if( !HasAnyFlag( vt, VTF::HASGROUP ) ) {
    std::string sDefault = pLeDefault->text().toStdString();
    err = pModel->Vars()->SetVar( pVD->VSD(), pVD, null, UPtr( sDefault ) );

    if( err != 0 ) {
      pModel->ActionStack().Cancel();
      pConsole->Send( WND_ERROR, SENDER, err );
      return;
    }
  }

  pModel->ActionStack().Complete( "VarDef:Accept" );
  accept();
}

void DlgVdEdit::slotRemove()
{
  pVD->Delete();
  pModel->ActionStack().Complete( "VarDef:Remove" );
  accept();
}

void DlgVdEdit::slotEditHelp()
{
  DlgVdHelpEdit* pDialog = new DlgVdHelpEdit( pVD, pSM, this );
  connect( pDialog, SIGNAL(sgnlHelp(int)), this, SIGNAL(sgnlHelp(int)) );
  if( pDialog->exec() == QDialog::Accepted )
    pModel->ActionStack().Complete( "VarDef:EditHelp" );
}
