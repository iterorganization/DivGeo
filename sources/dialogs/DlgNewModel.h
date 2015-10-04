#ifndef DLGNEWMODEL_H
#define DLGNEWMODEL_H

#include <QDialog>
#include <QFileDialog>

#include <QLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>

#include "../main/StringsManager.h"
#include "../main/Console.h"

class DlgNewModel : public QDialog
{
  Q_OBJECT

  StringsManager* pSM;
  Console* pConsole;
  QFileDialog* pBrowseDlg;

  QPushButton* pBtnBrowse;
  QLineEdit* pLePath;

  bool useConfig;

public:
  explicit DlgNewModel(StringsManager* _pSM, Console* _pConsole,
                        const QString& _crsDefaultConfig,
                        QWidget *parent = 0);

  QString Path() const { return useConfig ? pLePath->text() : ""; }
  
private slots:
  void slotCreate();
  void slotBrowse();
  void slotNoCfgToggled( bool _nocfg );
  
};

#endif // DLGNEWMODEL_H
