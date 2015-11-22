#ifndef DLGVARSEDIT_H
#define DLGVARSEDIT_H

#include <QDialog>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QTableWidget>
#include <QSignalMapper>
#include <QHeaderView>
#include <QMenu>

#include "../common/Metatypes.h"
#include "../model/core/Model.h"
#include "../view/ViewWnd.h"
#include "../main/StringsManager.h"
#include "../main/Console.h"

class DlgVarsEdit : public QDialog
{
  Q_OBJECT

  enum CompareOperator { EQ, NE, GT, GE, LT, LE };

  StringsManager* pSM;
  ModelPtr pModel;
  CViewWndPtr pView;
  Console* pConsole;
  VarSetPtr pVS;

  QCheckBox* pChHold;
  QVector< QTableWidget* > vars;

  QPushButton* pBtnSetAll;
  QPushButton* pBtnResetAll;

  QSignalMapper* pSmHelp;
  QSignalMapper* pSmValue;
  QSignalMapper* pSmReset;
  QSignalMapper* pSmAccept;
  QSignalMapper* pSmClick;

  QLabel* pLlStatus;

  ulong changed_num;
  NPoint currentIndex;
  NPoint varsSize;
  bool resetEnabled;

public:
  explicit DlgVarsEdit(VarSetPtr _pVS, ModelPtr _pModel, CViewWndPtr _pView,
                       Console* _pConsole, StringsManager* _pSM,
                       QWidget *parent = 0);

  ModelPtr GetModel() const { return pModel; }
  VarSetPtr GetVS() const { return pVS; }

  void contextMenuEvent( QContextMenuEvent* _pCME );

signals:
  void sgnlHelp( int _code );
  void sgnlVarHelp( const QString& _crsTitle, const QString& _crsText );

public slots:
  void slotResetAll( bool _viewUpdate = true ); //1409

private slots:
  void slotHelp() { emit sgnlHelp( DLG::VARSEDIT::DIALOG ); }
  void slotCollapse( bool b );
  void slotAcceptAll();
  void slotVarHelp( int _index );
  void slotEdited( int _index );
  void slotOnClick( int _index );
  void slotResetVar( int _index, bool _viewUpdate = true ); //1409
  void slotAcceptVar( int _index );

  void slotPopupReset();
  void slotPopupCompareEQ();
  void slotPopupCompareNE();
  void slotPopupCompareGT();
  void slotPopupCompareGE();
  void slotPopupCompareLT();
  void slotPopupCompareLE();
  void slotPopupDisplay();

private:
  VarDefPtr RetrieveVarDef( NPoint _index ) const;
  void Compare( CompareOperator _co );
};

typedef QMap< VarSetPtr, DlgVarsEdit* > MapVarsEditDialogs;

#endif // DLGVARSEDIT_H
