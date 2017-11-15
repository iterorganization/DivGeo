#include "DlgVarHelp.h"

DlgVarHelp::DlgVarHelp(const QString& _crsTitle,
                       const QString& _crsText, QWidget *parent) :
  QDialog( parent, Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint ) //1411
{
  setWindowTitle( _crsTitle );
  QVBoxLayout* pLoMain = new QVBoxLayout( this );
  pLabel = new QLabel( _crsText );
  pLoMain->addWidget( pLabel );
}

void DlgVarHelp::slotShowWith( const QString& _crsTitle,
                               const QString& _crsText )
{
  setWindowTitle( _crsTitle );
  pLabel->setText( _crsText );
  show();
  raise();
}
