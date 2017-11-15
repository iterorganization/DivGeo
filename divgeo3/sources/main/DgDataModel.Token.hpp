#include "DgDataModel.Token.h"
#include "DgDataModel.TokenNames.h"
#include "DgDataModel.Tree.h"
#include "../model/flux/CreatorId.h"

namespace dm {

template< Token TOKEN >
TokenBase< TOKEN >::TokenBase( TreeItem* _pParent ):
  pChild( null ),
  pSection( _pParent ),
  steps_cnt( 0 ),
  step( 0 ) {}

template< Token TOKEN >
bool TokenBase< TOKEN >::Load( const QString&, const QStringList&, unsigned& _rLine ) {
  _rLine++;
  return true;
}

template< Token TOKEN >
void TokenBase< TOKEN >::Save( QFile& /*_rFile*/ ) {
  //TODO:
}

template< Token TOKEN >
void TokenBase< TOKEN >::LoadPoint2d( TreeItem* _pItem, const QString& _crLabel, const QString& _crX, const QString& _crY, unsigned _line ) {
  TreeItem* pItem = _pItem->AppendChild( _line, _crLabel,
                         QString( "( %1, %2 )" ).arg( _crX, _crY ) );
  pItem->AppendChild( _line, "x", _crX );
  pItem->AppendChild( _line, "y", _crY );
}


template< Token TOKEN >
void TokenBase< TOKEN >::LoadCreator( TreeItem* _pItem, const QString& _crWords, unsigned _line ) {
  TreeItem* pItem = _pItem->AppendChild( _line, "creator", _crWords );
  CreatorId cr( _crWords.toStdString() );
  QString type;
  switch( cr.Type() ) {
  case CreatorId::POLOIDAL: type = "poloidal"; break;
  case CreatorId::TOROIDAL: type = "toroidal"; break;
  case CreatorId::UNKNOWN:  type = "unknown"; break;
  }
  QString law;
  switch( cr.Law() ) {
  case DGLAW::NORMAL:    law = "normal"; break;
  case DGLAW::FLIPPED:   law = "flipped"; break;
  case DGLAW::TWOALPHA:  law = "2alpha"; break;
  case DGLAW::SYMMETRIC: law = "symmettric"; break;
  case DGLAW::DELTA:     law = "delta"; break;
  }
  pItem->AppendChild( _line, "type", type );
  pItem->AppendChild( _line, "area", QString( "%1" ).arg( cr.Area() ) );
  pItem->AppendChild( _line, "count", QString( "%1" ).arg( cr.Count() ) );
  pItem->AppendChild( _line, "level1", QString( "%1" ).arg( cr.level1 ) );
  pItem->AppendChild( _line, "level2", QString( "%1" ).arg( cr.level2 ) );
  pItem->AppendChild( _line, "alpha1", QString( "%1" ).arg( cr.Alpha1() ) );
  pItem->AppendChild( _line, "alpha2", QString( "%1" ).arg( cr.Alpha2() ) );
  pItem->AppendChild( _line, "law", law );
  pItem->AppendChild( _line, "carre", QString( "%1" ).arg( cr.CarreFlag() ) );
}


template< Token TOKEN >
void TokenBase< TOKEN >::LoadVariable( TreeItem* _pItem, const QString& _crStr, unsigned _line ) {
  _pItem->AppendChild( _line, "value", _crStr );
}


template<> bool TokenBase< TKN::DgFile >::Load( const QString& _crStr, const QStringList& _crWords, unsigned& _rLine ) {
  switch( step ) {
  case 0:
    pSection = pSection->AppendChild( _rLine, _crWords.first(), _crWords.value( 1 ) );
    pSection->AppendChild( _rLine++, QString( "%1" ).arg( step ), _crWords.value( 2 ) );
    return step++, false;
  case 1:
    pSection->AppendChild( _rLine++, QString( "%1" ).arg( step ), _crStr );
    return step++, false;
  case 2:
    pSection->AppendChild( _rLine++, QString( "%1" ).arg( step ), _crStr );
    return step++, true;
  default:
    return true;
  }
}


template<> bool TokenBase< TKN::Equil100 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first() );
    pSection->AppendChild( _rLine, "filename", _crWords.value( 1 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::EquilHints114 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first() );
    pSection->AppendChild( _rLine, "pos1",
      QString( "%1, %2" ).arg( _crWords.value( 1 ), _crWords.value( 2 ) ) );
    pSection->AppendChild( _rLine, "pos2",
      QString( "%1, %2" ).arg( _crWords.value( 3 ), _crWords.value( 4 ) ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::Template100 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first() );
    pSection->AppendChild( _rLine, "incr",
      QString( "%1, %2" ).arg( _crWords.value( 1 ), _crWords.value( 2 ) ) );
    pSection->AppendChild( _rLine, "angle", _crWords.value( 3 ) );
    pSection->AppendChild( _rLine, "filename", _crWords.value( 4 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::Template110 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first() );
    pSection->AppendChild( _rLine, "incr",
      QString( "%1, %2" ).arg( _crWords.value( 1 ), _crWords.value( 2 ) ) );
    pSection->AppendChild( _rLine, "angle", _crWords.value( 3 ) );
    pSection->AppendChild( _rLine, "scale", _crWords.value( 4 ) );
    pSection->AppendChild( _rLine, "filename", _crWords.value( 5 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::SonnetData100 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first() );
    pSection->AppendChild( _rLine, "filename", _crWords.value( 1 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::Mesh112 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first() );
    pSection->AppendChild( _rLine, "filename", _crWords.value( 1 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::MeshFingerprint112 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first(), _crWords.value( 1 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::MeshPoint112 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first() );
    pSection->AppendChild( _rLine, "cell", _crWords.value( 1 ) );
    pSection->AppendChild( _rLine, "id", _crWords.value( 2 ) );
    pSection->AppendChild( _rLine, "pos",
      QString( "%1, %2" ).arg( _crWords.value( 3 ), _crWords.value( 4 ) ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::Nodes100 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, QString( "%1" ).arg( step - 1 ),
      QString( "%1, %2" ).arg( _crWords.value( 0 ), _crWords.value( 1 ) ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Elems100 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, _crWords.value( 2 ),
      QString( "%1 -> %2" ).arg(_crWords.value( 0 ), _crWords.value( 1 ) ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::MarkedElems100 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, _crWords.value( 0 ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Separators101 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, _crWords.value( 3 ),
      QString( "( %1, %2 ) -> %3" ).arg(_crWords.value( 0 ), _crWords.value( 1 ), _crWords.value( 2 ) ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::MarkedSeparators104 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, _crWords.value( 0 ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Chords115 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++,QString( "%1" ).arg( step ),
      QString( "( %1, %2, %3 ) -> ( %4, %5, %6 )" )
        .arg( _crWords.value( 0 ), _crWords.value( 1 ), _crWords.value( 2 ),
              _crWords.value( 3 ), _crWords.value( 4 ), _crWords.value( 5 ) ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Chords106 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++,QString( "%1" ).arg( step ),
      QString( "( %1, %2 ) -> ( %3, %4 )" )
        .arg( _crWords.value( 0 ), _crWords.value( 1 ),
              _crWords.value( 2 ), _crWords.value( 3 ) ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::MarkedChords110 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();

    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, _crWords.value( 0 ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Surfaces100 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString posx = _crWords.value( 0 );
    QString posy = _crWords.value( 1 );
    TreeItem* pItem = pSection->AppendChild( _rLine, QString( "%1" ).arg( step ),
                           QString( "( %1, %2 )" ).arg( posx, posy ) );
    LoadPoint2d( pItem, "pos", posx, posy, _rLine );
    _rLine++;
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Surfaces108 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString posx = _crWords.value( 0 );
    QString posy = _crWords.value( 1 );
    QString creator = _crWords.value( 2 );
    TreeItem* pItem = pSection->AppendChild( _rLine, QString( "%1" ).arg( step ),
                 QString( "( %1, %2 ), %3" ).arg( posx, posy, creator ) );
    LoadPoint2d( pItem, "pos", posx, posy, _rLine );
    LoadCreator( pItem, creator, _rLine );
    _rLine++;
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::GridPoints100 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString zone = _crWords.value( 0 );
    QString value = _crWords.value( 1 );
    TreeItem* pItem = pSection->AppendChild( _rLine, QString( "%1" ).arg( step ),
                           QString( "%1, %2" ).arg( zone, value ) );
    pItem->AppendChild( _rLine, "zone", zone );
    pItem->AppendChild( _rLine, "value", value );
    _rLine++;
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::GridPoints108 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString zone = _crWords.value( 0 );
    QString value = _crWords.value( 1 );
    QString creator = _crWords.value( 2 );
    TreeItem* pItem = pSection->AppendChild( _rLine, QString( "%1" ).arg( step ),
                           QString( "%1, %2, %3" ).arg( zone, value, creator ) );
    pItem->AppendChild( _rLine, "zone", zone );
    pItem->AppendChild( _rLine, "value", value );
    LoadCreator( pItem, creator, _rLine );
    _rLine++;
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::SurfacesEx114 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString zone    = _crWords.value( 0 );
    QString value   = _crWords.value( 1 );
    QString posx    = _crWords.value( 2 );
    QString posy    = _crWords.value( 3 );
    QString creator = _crWords.value( 4 );
    TreeItem* pItem = pSection->AppendChild( _rLine, QString( "%1" ).arg( step ),
      QString( "%1, %2, ( %3, %4 ), %5" ).arg( zone, value, posx, posy, creator ) );
    pItem->AppendChild( _rLine, "zone", zone );
    pItem->AppendChild( _rLine, "value", value );
    LoadPoint2d( pItem, "pos", posx, posy, _rLine );
    LoadCreator( pItem, creator, _rLine );
    _rLine++;
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::GridPointsEx114 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString zone = _crWords.value( 0 );
    QString value = _crWords.value( 1 );
    QString creator = _crWords.value( 2 );
    TreeItem* pItem = pSection->AppendChild( _rLine, QString( "%1" ).arg( step ),
                           QString( "%1, %2, %3" ).arg( zone, value, creator ) );
    pItem->AppendChild( _rLine, "zone", zone );
    pItem->AppendChild( _rLine, "value", value );
    LoadCreator( pItem, creator, _rLine );
    _rLine++;
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::VarSetDefs101 >::Load( const QString& _crStr, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    if( pChild == null )
      pChild = new TokenBase< TKN::VarSetDef101 >( pSection );
    if( not pChild->Load( _crStr, _crWords, _rLine ) )
      return false;
    else {
      delete pChild;
      pChild = null;
    }
    return step++ == steps_cnt;
  }
  else
    return true;
}

template<> bool TokenBase< TKN::VarSetDef101 >::Load( const QString& _crStr, const QStringList& _crWords, unsigned& _rLine ) {
  switch( step ) {
  case 0:
    pSection = pSection->AppendChild( _rLine, "VarSetDef102" );
    pSection->AppendChild( _rLine++, "name", _crWords.value( 0 ) );
    return step++, false;
  case 1:
    pSection->AppendChild( _rLine++, "descr", _crStr );
    return step++, false;
  case 2: {
    if( pChild == null ) {
      if( _crWords.first() != "text101" )
        return true; //error
      pChild = new TokenBase< TKN::text101 >( pSection );
    }
    if( pChild->Load( _crStr, _crWords, _rLine ) ) {
      delete pChild;
      pChild = null;
      step++;
    }
    return false;
  }
  case 3:
    pSection->AppendChild( _rLine  , "min sets", _crWords.value( 0 ) );
    pSection->AppendChild( _rLine++, "max sets", _crWords.value( 1 ) );
    return step++, false;
  case 4:
    if( pChild == null )
      pChild = new TokenBase< TKN::VarDefs102 >( pSection );
    if( pChild->Load( _crStr, _crWords, _rLine ) ) {
      delete pChild;
      pChild = null;
      return step++, false;
    }
    return false;
  case 5:
    if( pChild == null )
      pChild = new TokenBase< TKN::Vars110 >( pSection );
    if( not pChild->Load( _crStr, _crWords, _rLine ) )
      return false;
    else {
      delete pChild;
      pChild = null;
      return true;
    }
  default:
    return true;
  }
}


template<> bool TokenBase< TKN::Vars110 >::Load( const QString& _crStr, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    LoadVariable( pSection, _crStr, _rLine++ );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::VarDefs102 >::Load( const QString& _crStr, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    if( pChild == null )
      pChild = new TokenBase< TKN::VarDef102 >( pSection );
    if( not pChild->Load( _crStr, _crWords, _rLine ) )
      return false;
    else {
      delete pChild;
      pChild = null;
    }
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::VarDef102 >::Load( const QString& _crStr, const QStringList& _crWords, unsigned& _rLine ) {
  switch( step ) {
  case 0:
    pSection = pSection->AppendChild( _rLine, "VarDef102" );
    pSection->AppendChild( _rLine++, "name", _crWords.value( 0 ) );
    return step++, false;
  case 1:
    pSection->AppendChild( _rLine++, "descr", _crWords.value( 0 ) );
    return step++, false;
  case 2: {
    if( pChild == null ) {
      if( _crWords.first() != "text101" )
        return true; //error
      pChild = new TokenBase< TKN::text101 >( pSection );
    }
    if( pChild->Load( _crStr, _crWords, _rLine ) ) {
      delete pChild;
      pChild = null;
      step++;
    }
    return false;
  }
  case 3: {
    TreeItem* pItem = pSection->AppendChild( _rLine++, "types", _crWords.value( 0 ) );
    pItem = pSection->AppendChild( _rLine, "flags", _crWords.value( 1 ) );
    LoadPoint2d( pSection, "index", _crWords.value( 2 ), _crWords.value( 3 ), _rLine++ );
    return step++, false;
  }
  case 4: // EnumText102
    return step++, false;
  case 5:
    if( pChild == null ) {
      if( _crWords.first() != "text101" )
        return true; //error
      pChild = new TokenBase< TKN::text101 >( pSection );
    }
    if( not pChild->Load( _crStr, _crWords, _rLine ) )
      return false;
    else {
      delete pChild;
      pChild = null;
      step++;
      return true;
    }
  default:
    return true;
  }
}


template<> bool TokenBase< TKN::text101 >::Load( const QString& _crStr, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crWords.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crWords.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, QString( "%1" ).arg( step ), _crStr );
    return step++ == steps_cnt;
  }
  else
    return true;
}

/*...*/

template<> bool TokenBase< TKN::ViewAttr103 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine++, _crWords.first() );
  }
  return true;
}


template<> bool TokenBase< TKN::ViewAttr105 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first() );
    pSection->AppendChild( _rLine, "pos1",
      QString( "%1, %2" ).arg( _crWords.value( 1 ), _crWords.value( 2 ) ) );
    pSection->AppendChild( _rLine, "pos2",
      QString( "%1, %2" ).arg( _crWords.value( 3 ), _crWords.value( 4 ) ) );
    pSection->AppendChild( _rLine, "flags", _crWords.value( 5 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::ViewAttr115 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first() );
    pSection->AppendChild( _rLine, "pos1",
      QString( "%1, %2" ).arg( _crWords.value( 1 ), _crWords.value( 2 ) ) );
    pSection->AppendChild( _rLine, "pos2",
      QString( "%1, %2" ).arg( _crWords.value( 3 ), _crWords.value( 4 ) ) );
    pSection->AppendChild( _rLine, "angle", _crWords.value( 5 ) );
    pSection->AppendChild( _rLine, "flags", _crWords.value( 6 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::MeshSlidingOptions112 >::Load( const QString&, const QStringList& _crWords, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crWords.first() );
    pSection->AppendChild( _rLine, "flags", _crWords.value( 1 ) );
    pSection->AppendChild( _rLine, "treshold", _crWords.value( 2 ) );
    pSection->AppendChild( _rLine, "border", _crWords.value( 3 ) );
    _rLine++;
  }
  return true;
}


} // namespace dm
