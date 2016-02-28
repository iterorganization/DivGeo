#include "DgDataModel.Token.h"
#include "DgDataModel.TokenNames.h"
#include "DgDataModel.Tree.h"
#include "../model/flux/CreatorId.h"

namespace dm {

template< Token TOKEN >
TokenBase< TOKEN >::TokenBase( TreeItem* _pParent ):
  pSection( _pParent ),
  steps_cnt( 0 ),
  step( 0 ) {}

template< Token TOKEN >
bool TokenBase< TOKEN >::Load( const QStringList&, unsigned& _rLine ) {
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
void TokenBase< TOKEN >::LoadCreator( TreeItem* _pItem, const QString& _crStr, unsigned _line ) {
  TreeItem* pItem = _pItem->AppendChild( _line, "creator", _crStr );
  CreatorId cr( _crStr.toStdString() );
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


template<> bool TokenBase< TKN::Equil100 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first() );
    pSection->AppendChild( _rLine, "filename", _crStr.value( 1 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::EquilHints114 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first() );
    pSection->AppendChild( _rLine, "pos1",
      QString( "%1, %2" ).arg( _crStr.value( 1 ), _crStr.value( 2 ) ) );
    pSection->AppendChild( _rLine, "pos2",
      QString( "%1, %2" ).arg( _crStr.value( 3 ), _crStr.value( 4 ) ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::Template100 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first() );
    pSection->AppendChild( _rLine, "incr",
      QString( "%1, %2" ).arg( _crStr.value( 1 ), _crStr.value( 2 ) ) );
    pSection->AppendChild( _rLine, "angle", _crStr.value( 3 ) );
    pSection->AppendChild( _rLine, "filename", _crStr.value( 4 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::Template110 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first() );
    pSection->AppendChild( _rLine, "incr",
      QString( "%1, %2" ).arg( _crStr.value( 1 ), _crStr.value( 2 ) ) );
    pSection->AppendChild( _rLine, "angle", _crStr.value( 3 ) );
    pSection->AppendChild( _rLine, "scale", _crStr.value( 4 ) );
    pSection->AppendChild( _rLine, "filename", _crStr.value( 5 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::SonnetData100 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first() );
    pSection->AppendChild( _rLine, "filename", _crStr.value( 1 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::Mesh112 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first() );
    pSection->AppendChild( _rLine, "filename", _crStr.value( 1 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::MeshFingerprint112 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first(), _crStr.value( 1 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::MeshPoint112 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first() );
    pSection->AppendChild( _rLine, "cell", _crStr.value( 1 ) );
    pSection->AppendChild( _rLine, "id", _crStr.value( 2 ) );
    pSection->AppendChild( _rLine, "pos",
      QString( "%1, %2" ).arg( _crStr.value( 3 ), _crStr.value( 4 ) ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::Nodes100 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, QString( "%1" ).arg( step - 1 ),
      QString( "%1, %2" ).arg( _crStr.value( 0 ), _crStr.value( 1 ) ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Elems100 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, _crStr.value( 2 ),
      QString( "%1 -> %2" ).arg(_crStr.value( 0 ), _crStr.value( 1 ) ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::MarkedElems100 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, _crStr.value( 0 ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Separators101 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, _crStr.value( 3 ),
      QString( "( %1, %2 ) -> %3" ).arg(_crStr.value( 0 ), _crStr.value( 1 ), _crStr.value( 2 ) ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::MarkedSeparators104 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, _crStr.value( 0 ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Chords115 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++,QString( "%1" ).arg( step ),
      QString( "( %1, %2, %3 ) -> ( %4, %5, %6 )" )
        .arg( _crStr.value( 0 ), _crStr.value( 1 ), _crStr.value( 2 ),
              _crStr.value( 3 ), _crStr.value( 4 ), _crStr.value( 5 ) ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Chords106 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++,QString( "%1" ).arg( step ),
      QString( "( %1, %2 ) -> ( %3, %4 )" )
        .arg( _crStr.value( 0 ), _crStr.value( 1 ),
              _crStr.value( 2 ), _crStr.value( 3 ) ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::MarkedChords110 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();

    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _rLine++, _crStr.value( 0 ) );
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Surfaces100 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString posx = _crStr.value( 0 );
    QString posy = _crStr.value( 1 );
    TreeItem* pItem = pSection->AppendChild( _rLine, QString( "%1" ).arg( step ),
                           QString( "( %1, %2 )" ).arg( posx, posy ) );
    LoadPoint2d( pItem, "pos", posx, posy, _rLine );
    _rLine++;
    return step++ == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Surfaces108 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString posx = _crStr.value( 0 );
    QString posy = _crStr.value( 1 );
    QString creator = _crStr.value( 2 );
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


template<> bool TokenBase< TKN::GridPoints100 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString zone = _crStr.value( 0 );
    QString value = _crStr.value( 1 );
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


template<> bool TokenBase< TKN::GridPoints108 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString zone = _crStr.value( 0 );
    QString value = _crStr.value( 1 );
    QString creator = _crStr.value( 2 );
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


template<> bool TokenBase< TKN::SurfacesEx114 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString zone    = _crStr.value( 0 );
    QString value   = _crStr.value( 1 );
    QString posx    = _crStr.value( 2 );
    QString posy    = _crStr.value( 3 );
    QString creator = _crStr.value( 4 );
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


template<> bool TokenBase< TKN::GridPointsEx114 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  else if( step <= steps_cnt ) {
    QString zone = _crStr.value( 0 );
    QString value = _crStr.value( 1 );
    QString creator = _crStr.value( 2 );
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


template<> bool TokenBase< TKN::VarSetDefs101 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  switch( step ) {
  case 0: {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    return step++, steps_cnt == 0;
  }
  case 1:
    pSection->AppendChild( _rLine++, "name", _crStr.value( 0 ) );
    return step++, false;
  case 2:
    pSection->AppendChild( _rLine++, "descr", _crStr.value( 0 ) );
    return step++, false;
  case 3:
    if( _crStr.first() != "text101" )
      return true;
  default:
    return true;
  }

/*  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _rLine++, _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    step++;
    return false;
  }
  else if( step <= steps_cnt ) {
    QString zone = _crStr.value( 0 );
    QString value = _crStr.value( 1 );
    QString creator = _crStr.value( 2 );
    TreeItem* pItem = pSection->AppendChild( _rLine, QString( "%1" ).arg( step ),
                           QString( "%1, %2, %3" ).arg( zone, value, creator ) );
    pItem->AppendChild( _rLine, "zone", zone );
    pItem->AppendChild( _rLine, "value", value );
    LoadCreator( pItem, creator, _rLine );
    _rLine++;
    return step++ == steps_cnt;
  }
  else
    return true;*/
}

/*...*/

template<> bool TokenBase< TKN::ViewAttr103 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine++, _crStr.first() );
  }
  return true;
}


template<> bool TokenBase< TKN::ViewAttr105 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first() );
    pSection->AppendChild( _rLine, "pos1",
      QString( "%1, %2" ).arg( _crStr.value( 1 ), _crStr.value( 2 ) ) );
    pSection->AppendChild( _rLine, "pos2",
      QString( "%1, %2" ).arg( _crStr.value( 3 ), _crStr.value( 4 ) ) );
    pSection->AppendChild( _rLine, "flags", _crStr.value( 5 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::ViewAttr115 >::Load( const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first() );
    pSection->AppendChild( _rLine, "pos1",
      QString( "%1, %2" ).arg( _crStr.value( 1 ), _crStr.value( 2 ) ) );
    pSection->AppendChild( _rLine, "pos2",
      QString( "%1, %2" ).arg( _crStr.value( 3 ), _crStr.value( 4 ) ) );
    pSection->AppendChild( _rLine, "angle", _crStr.value( 5 ) );
    pSection->AppendChild( _rLine, "flags", _crStr.value( 6 ) );
    _rLine++;
  }
  return true;
}


template<> bool TokenBase< TKN::MeshSlidingOptions112 >::Load(const QStringList& _crStr, unsigned& _rLine ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _rLine, _crStr.first() );
    pSection->AppendChild( _rLine, "flags", _crStr.value( 1 ) );
    pSection->AppendChild( _rLine, "treshold", _crStr.value( 2 ) );
    pSection->AppendChild( _rLine, "border", _crStr.value( 3 ) );
    _rLine++;
  }
  return true;
}


} // namespace dm
