#ifndef DLGCREATENAME_H
#define DLGCREATENAME_H

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

#include "../main/StringsManager.h"

class DlgCreateName : public QDialog
{
  Q_OBJECT

  StringsManager* pSM;

  QLineEdit* pLeName;

  QPushButton* pBtnCreate;
  QPushButton* pBtnCancel;
  QPushButton* pBtnHelp;

public:
  explicit DlgCreateName(ulong _titleCode,
                          StringsManager* _pSM, QWidget *parent = 0);
  
  QString GetName() const { return pLeName->text(); }

signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::CREATE_CHORD::DIALOG ); }
  
};

#endif // DLGCREATENAME_H
