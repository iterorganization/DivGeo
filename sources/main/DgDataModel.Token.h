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
  virtual bool Load( const QStringList& _crStr, unsigned& _rLine ) = 0;
  virtual void Save( QFile& _rFile ) = 0;
  virtual ~IToken() {}
};

template< Token TOKEN >
class TokenBase: public IToken {
protected:
  TreeItem* pSection;
  unsigned steps_cnt;
  unsigned step;

public:
  TokenBase( TreeItem* _pParent );
  virtual bool Load( const QStringList&, unsigned& _rLine );
  virtual void Save( QFile& _rFile );
};

} // namespace dm

#include "DgDataModel.Token.hpp"

#endif // DGDATAMODEL_TOKEN_H

