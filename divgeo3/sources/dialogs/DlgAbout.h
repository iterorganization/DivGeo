#ifndef DLGABOUT_H
#define DLGABOUT_H

#include <QDialog>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>

#include "../main/StringsManager.h"

#if defined(__clang__)
  #define COMPILER_NAME "Clang/LLVM"
  #define COMPILER_VERSION __clang_version__
#elif defined(__ICC) || defined(__INTEL_COMPILER)
  #define COMPILER_NAME "Intel ICC/ICPC"
  #define COMPILER_VERSION __VERSION__
#elif defined(__GNUC__) || defined(__GNUG__)
  #define COMPILER_NAME "GNU GCC/G++"
  #define COMPILER_VERSION __VERSION__
#elif defined(__HP_cc) || defined(__HP_aCC)
  #define COMPILER_NAME "Hewlett-Packard C/aC++"
  #define COMPILER_VERSION __HP_aCC
#elif defined(__IBMC__) || defined(__IBMCPP__)
  #define COMPILER_NAME "IBM XL C/C++"
  #define COMPILER_VERSION __xlc__
#elif defined(_MSC_VER)
  #define COMPILER_NAME "Microsoft Visual C++"
  #define COMPILER_VERSION _MSC_FULL_VER
#elif defined(__PGI)
  #define COMPILER_NAME "Portland Group PGCC/PGCPP"
  #define COMPILER_NAME __PGIC__ "." __PGIC_MINOR "." __PGIC_PATCHLEVEL__
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
  #define COMPILER_NAME "Oracle Solaris C++"
  #define COMPILER_NAME __SUNPRO_CC
#endif

class DlgAbout : public QDialog
{
  Q_OBJECT

  StringsManager* pSM;

public:
  explicit DlgAbout( StringsManager* _pSM, const QString& _crsVersion, QWidget *parent = 0);
  
signals:
  void sgnlHelp( int _code );
private slots:
  void slotHelp() { emit sgnlHelp( DLG::ABOUT::DIALOG ); }
  
};

#endif // DLGABOUT_H
