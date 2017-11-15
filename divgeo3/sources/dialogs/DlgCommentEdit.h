#ifndef DLGCOMMENTEDIT_H
#define DLGCOMMENTEDIT_H

#include <QDialog>
#include <QBoxLayout>
#include <QTextEdit>
#include <QPushButton>

#include "../common/Metatypes.h"
#include "../main/StringsManager.h"

class DlgCommentEdit : public QDialog
{
  Q_OBJECT

  CommentPtr pComment;
  IComponentPtr pLinkedObject;
  StringsManager* pSM;
  QTextEdit* pTeHelp;

public:
  explicit DlgCommentEdit( CommentPtr _pComment, IComponentPtr _pLinkedObject,
                           StringsManager* _pSM, QWidget *parent = 0);

signals:
  void sgnlHelp( int _code );

private slots:
  void slotHelp() { emit sgnlHelp( DLG::COMMENTEDIT::DIALOG ); }
  void slotAccept();
  void slotInsert();
  
};

#endif // DLGCOMMENTEDIT_H
