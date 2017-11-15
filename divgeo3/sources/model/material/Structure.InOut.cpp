#include "Structure.h"

#include "../core/Model.h"

int Structure::ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int )
{
  char sToken[ 32 ];
  char sLine[ DG_LINE_LEN ];
  Point pos, pos2;
  int id;
  uint count;
  int errorFlags = 0;
  switch( _token ) {
  case TKN::Nodes100:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, SCANFLT""SCANFLT, &pos.x, &pos.y ) == 2 )
          AddNode( pos );
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::Elems100:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      int id;
      uint node1_id, node2_id;
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%u %u %d", &node1_id, &node2_id, &id ) == 3 )
        {
          ElementPtr pElem = AddElem( dgtype_cast< NodePtr >( At( nodes, node1_id ) ),
                                      dgtype_cast< NodePtr >( At( nodes, node2_id ) ) );
          pElem->ChangeId( id );
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::MarkedElems100:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
       if( sscanf( sLine, "%u", &id ) == 1 )
         At( elements, id )->Mark();
       else
         errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::Separators101:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      int idNode;
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, SCANFLT""SCANFLT"%u%d",
                    &pos.x, &pos.y, &idNode, &id ) == 4 ) {
          SeparatorPtr pSep = AddSeparator( pos,
             dgtype_cast< NodePtr >( At( nodes, idNode ) ) );
          pSep->ChangeId( id );
        }
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  case TKN::MarkedSeparators104:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%u", &id ) == 1 )
          At( separators, id )->Mark();
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;

  case TKN::Chords115:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, SCANFLT""SCANFLT""SCANFLT""SCANFLT""SCANFLT""SCANFLT,
                    &pos.x, &pos.y, &pos.z,  &pos2.x, &pos2.y, &pos2.z ) == 6 )
          AddChord( pos, pos2 );
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    pos = 0.; pos2 = 0.;
    break;
  case TKN::Chords106:
     if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
       REPEAT( count ) {
         fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
         if( sscanf( sLine, SCANFLT""SCANFLT""SCANFLT""SCANFLT,
                     &pos.x, &pos.y, &pos2.x, &pos2.y ) == 4 )
           AddChord( pos, pos2 );
         else
           errorFlags |= DGFE::SYNTAX;
       }
     }
     else return DGFE::SYNTAX;
     break;
  case TKN::MarkedChords110:
    if( sscanf( _cpStr, "%s %u", sToken, &count ) == 2 ) {
      REPEAT( count ) {
        fgets( sLine, DG_LINE_LEN - 1, _pFile ); _rLine++;
        if( sscanf( sLine, "%u", &id ) == 1 )
          At( chords, id )->Mark();
        else
          errorFlags |= DGFE::SYNTAX;
      }
    }
    else return DGFE::SYNTAX;
    break;
  default:
    return DGFE::UNKNOWN_TOKEN;
    break;
  }
  return errorFlags;
}

int Structure::WriteObjects( Token _token, FILE* _pFile, int ) const
{
  const char* sToken = TKN::names[ _token ].s;
  unsigned markedNumber = 0;
  IComponentPtr pObj = null;
  switch( _token ) {
  case TKN::Nodes100: {
    fprintf( _pFile, "%s %u\n", sToken, (unsigned)nodes.size() );
    FOREACHPTRCONST( NodePtr, pNode, nodes )
      fprintf( _pFile, "%e %e\n", pNode->X(), pNode->Y() );
    break;
  }
  case TKN::Elems100: {
    fprintf( _pFile, "%s %d\n", sToken, (unsigned)elements.size() );
    FOREACHPTRCONST( ElementPtr, pElem, elements )
      fprintf( _pFile, "%u %u %d\n", (uint)IndexOf( nodes, pElem->Node(1) ),
               (uint)IndexOf( nodes, pElem->Node(2) ), pElem->Id() );
    break;
  }
  case TKN::MarkedElems100: {
    markedNumber = 0;
    IComponentPtr FOREACHOBJCONST( pObj, pModel->MarkedObjects() )
      if( pObj->Type() == OT::ELEMENT )
        markedNumber++;
    fprintf( _pFile, "%s %d\n", sToken, markedNumber );
    FOREACHOBJCONST( pObj, pModel->MarkedObjects() )
      if( pObj->Type() == OT::ELEMENT )
        fprintf( _pFile, "%u\n", (uint)IndexOf( elements, pObj ) );
    break;
  }
  case TKN::Separators101: {
    fprintf( _pFile, "%s %u\n", sToken, (unsigned)separators.size() );
    FOREACHPTRCONST( SeparatorPtr, pSep, separators )
      fprintf( _pFile, "%e %e %u %d\n", pSep->X(), pSep->Y(),
               (uint)IndexOf( nodes, pSep->Node() ), pSep->Id() );
    break;
  }
  case TKN::MarkedSeparators104:
    markedNumber = 0;
    FOREACHOBJCONST( pObj, pModel->MarkedObjects() )
      if( pObj->Type() == OT::SEPARATOR )
        markedNumber++;
    fprintf( _pFile, "%s %d\n", sToken, markedNumber );
    FOREACHOBJCONST( pObj, pModel->MarkedObjects() )
      if( pObj->Type() == OT::SEPARATOR )
        fprintf( _pFile, "%u\n", (uint)IndexOf( separators, pObj ) );
    break;

  case TKN::Chords106: break; /* obsolete */
  case TKN::Chords115: {
    fprintf( _pFile, "%s %u\n", sToken, (unsigned)chords.size() );
    FOREACHPTRCONST( ChordPtr, pCh, chords ) {
      Point p1 = pCh->Point_1();
      Point p2 = pCh->Point_2();
      fprintf( _pFile, "%e %e %e %e %e %e\n", p1.x, p1.y, p1.z,  p2.x, p2.y, p2.z );
    }
    break;
  }
  case TKN::MarkedChords110:
    markedNumber = 0;
    FOREACHOBJCONST( pObj, pModel->MarkedObjects() )
      if( pObj->Type() == OT::CHORD )
        markedNumber++;
    fprintf( _pFile, "%s %d\n", sToken, markedNumber );
    FOREACHOBJCONST( pObj, pModel->MarkedObjects() )
      if( pObj->Type() == OT::CHORD )
        fprintf( _pFile, "%u\n", (uint)IndexOf( chords, pObj ) );
    break;
  default:
    return DGFE::UNKNOWN_TOKEN;
    break;
  }
  return 0;
}
