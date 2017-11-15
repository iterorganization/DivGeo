#ifndef DLGVARHELP_H
#define DLGVARHELP_H

#include <QDialog>
#include <QLabel>
#include <QLayout>

class DlgVarHelp : public QDialog
{
  Q_OBJECT

  QLabel* pLabel;

public:
  explicit DlgVarHelp( const QString& _crsTitle,
                       const QString& _crsText, QWidget *parent = 0);
  
public slots:
  void slotShowWith( const QString& _crsTitle,
                     const QString& _crsText );
  
};

#endif // DLGVARHELP_H
