#include "DoubleSpinBoxExt.h"

DoubleSpinBoxExt::DoubleSpinBoxExt():
  QDoubleSpinBox() { }

QString DoubleSpinBoxExt::textFromValue( double _value ) const
{
  return QDoubleSpinBox /*1409*/::textFromValue( _value );
}

double DoubleSpinBoxExt::valueFromText( const QString& _crtext ) const
{
  QLocale c( QLocale::C );
  bool ok;
  double current = value();
  QString fixed = QString( _crtext ).replace( ',', '.' );
  double newvalue = c.toDouble( fixed, &ok );
  if( not ok )
    newvalue = current;
  return newvalue;
}

QValidator::State DoubleSpinBoxExt::validate( QString& _rtext, int& _rpos ) const
{
  Q_UNUSED( _rtext );
  Q_UNUSED( _rpos );
  return QValidator::Acceptable;
}

