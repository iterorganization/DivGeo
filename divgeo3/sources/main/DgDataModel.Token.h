#ifndef DGDATAMODEL_TOKEN_H
#define DGDATAMODEL_TOKEN_H

class QFile;
class QString;
class QStringList;

#include "DgDataModel.TokenNames.h"

namespace dm {

class TreeItem;

class IToken {
public:
  virtual bool Load( const QString& _crStr, const QStringList& _crWords, unsigned& _rLine ) = 0;
  virtual void Save( QFile& _rFile ) = 0;
  virtual ~IToken() {}
};

template< Token TOKEN >
class TokenBase: public IToken {
protected:
  IToken* pChild;
  TreeItem* pSection;
  unsigned steps_cnt;
  unsigned step;

public:
  TokenBase( TreeItem* _pParent );
  virtual bool Load( const QString&, const QStringList&, unsigned& _rLine );
  virtual void Save( QFile& _rFile );

  void LoadPoint2d ( TreeItem* _pItem, const QString& _crLabel, const QString& _crX, const QString& _crY, unsigned _line );
  void LoadPoint3d ( TreeItem* _pItem, const QString& _crLabel, const QString& _crX, const QString& _crY, const QString& _crZ, unsigned _line );
  void LoadCreator ( TreeItem* _pItem, const QString& _crStr, unsigned _line );
  void LoadVariable( TreeItem* _pItem, const QString& _crStr, unsigned _line );
};

} // namespace dm

#include "DgDataModel.Token.hpp"

#endif // DGDATAMODEL_TOKEN_H

