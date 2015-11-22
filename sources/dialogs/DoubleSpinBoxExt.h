#ifndef DOUBLESPINBOXEXT_H
#define DOUBLESPINBOXEXT_H

#include <QDoubleSpinBox>

class QString;

class DoubleSpinBoxExt : public QDoubleSpinBox /*1409*/
{
  Q_OBJECT
public:
    DoubleSpinBoxExt();

    QString textFromValue( double _value ) const;
    double valueFromText( const QString& _crtext ) const;
    QValidator::State validate( QString& _rtext, int& _rpos ) const;
};

#endif // DOUBLESPINBOXEXT_H
