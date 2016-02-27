#include "DgDataModel.Token.h"
#include "DgDataModel.TokenNames.h"
#include "DgDataModel.Tree.h"

namespace dm {

template< Token TOKEN >
TokenBase< TOKEN >::TokenBase( TreeItem* _pParent ):
  pSection( _pParent ),
  steps_cnt( 0 ),
  step( 0 ) {}

template< Token TOKEN >
bool TokenBase< TOKEN >::Load( const QStringList& ) {
  return true;
}

template< Token TOKEN >
void TokenBase< TOKEN >::Save( QFile& /*_rFile*/ ) {
  //TODO:
}

template<> bool TokenBase< TKN::Equil100 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
    pSection->AppendChild( "filename", _crStr.value( 1 ) );
  }
  return true;
}


template<> bool TokenBase< TKN::EquilHints114 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
    pSection->AppendChild( "pos1",
      QString( "%1, %2" ).arg( _crStr.value( 1 ), _crStr.value( 2 ) ) );
    pSection->AppendChild( "pos2",
      QString( "%1, %2" ).arg( _crStr.value( 3 ), _crStr.value( 4 ) ) );
  }
  return true;
}


template<> bool TokenBase< TKN::Template100 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
    pSection->AppendChild( "incr",
      QString( "%1, %2" ).arg( _crStr.value( 1 ), _crStr.value( 2 ) ) );
    pSection->AppendChild( "angle", _crStr.value( 3 ) );
    pSection->AppendChild( "filename", _crStr.value( 4 ) );
  }
  return true;
}


template<> bool TokenBase< TKN::Template110 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
    pSection->AppendChild( "incr",
      QString( "%1, %2" ).arg( _crStr.value( 1 ), _crStr.value( 2 ) ) );
    pSection->AppendChild( "angle", _crStr.value( 3 ) );
    pSection->AppendChild( "scale", _crStr.value( 4 ) );
    pSection->AppendChild( "filename", _crStr.value( 5 ) );
  }
  return true;
}


template<> bool TokenBase< TKN::SonnetData100 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
    pSection->AppendChild( "filename", _crStr.value( 1 ) );
  }
  return true;
}


template<> bool TokenBase< TKN::Mesh112 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
    pSection->AppendChild( "filename", _crStr.value( 1 ) );
  }
  return true;
}


template<> bool TokenBase< TKN::MeshFingerprint112 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first(), _crStr.value( 1 ) );
  }
  return true;
}


template<> bool TokenBase< TKN::MeshPoint112 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
    pSection->AppendChild( "cell", _crStr.value( 1 ) );
    pSection->AppendChild( "id", _crStr.value( 2 ) );
    pSection->AppendChild( "pos",
      QString( "%1, %2" ).arg( _crStr.value( 3 ), _crStr.value( 4 ) ) );
  }
  return true;
}


template<> bool TokenBase< TKN::Nodes100 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    step++;
    return false;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( QString( "%1" ).arg( step - 1 ),
      QString( "%1, %2" ).arg( _crStr.value( 0 ), _crStr.value( 1 ) ) );
    step++;
    return step == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Elems100 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    step++;
    return false;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _crStr.value( 2 ),
      QString( "%1 -> %2" ).arg(_crStr.value( 0 ), _crStr.value( 1 ) ) );
    step++;
    return step == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::MarkedElems100 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    step++;
    return false;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _crStr.value( 0 ) );
    step++;
    return step == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::Separators101 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    step++;
    return false;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _crStr.value( 3 ),
      QString( "( %1, %2 ) -> %3" ).arg(_crStr.value( 0 ), _crStr.value( 1 ), _crStr.value( 2 ) ) );
    step++;
    return step == steps_cnt;
  }
  else
    return true;
}


template<> bool TokenBase< TKN::MarkedSeparators104 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    QString scnt = _crStr.value( 1 );
    pSection = pSection->AppendChild( _crStr.first(), scnt );
    steps_cnt = scnt.toUInt();
    step++;
    return false;
  }
  else if( step <= steps_cnt ) {
    pSection->AppendChild( _crStr.value( 0 ) );
    step++;
    return step == steps_cnt;
  }
  else
    return true;
}

/*...*/

template<> bool TokenBase< TKN::ViewAttr103 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
  }
  return true;
}


template<> bool TokenBase< TKN::ViewAttr105 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
    pSection->AppendChild( "pos1",
      QString( "%1, %2" ).arg( _crStr.value( 1 ), _crStr.value( 2 ) ) );
    pSection->AppendChild( "pos2",
      QString( "%1, %2" ).arg( _crStr.value( 3 ), _crStr.value( 4 ) ) );
    pSection->AppendChild( "flags", _crStr.value( 5 ) );
  }
  return true;
}


template<> bool TokenBase< TKN::ViewAttr115 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
    pSection->AppendChild( "pos1",
      QString( "%1, %2" ).arg( _crStr.value( 1 ), _crStr.value( 2 ) ) );
    pSection->AppendChild( "pos2",
      QString( "%1, %2" ).arg( _crStr.value( 3 ), _crStr.value( 4 ) ) );
    pSection->AppendChild( "angle", _crStr.value( 5 ) );
    pSection->AppendChild( "flags", _crStr.value( 6 ) );
  }
  return true;
}


template<> bool TokenBase< TKN::MeshSlidingOptions112 >::Load( const QStringList& _crStr ) {
  if( step == 0 ) {
    step = 1;
    pSection = pSection->AppendChild( _crStr.first() );
    pSection->AppendChild( "flags", _crStr.value( 1 ) );
    pSection->AppendChild( "treshold", _crStr.value( 2 ) );
    pSection->AppendChild( "border", _crStr.value( 3 ) );
  }
  return true;
}


} // namespace dm
