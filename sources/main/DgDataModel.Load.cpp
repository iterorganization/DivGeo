#include "DgDataModel.h"
#include "DgDataModel.Tree.h"
#include "DgDataModel.Token.h"

#include "../common/strlib.h"
#include "../common/Point.h"

namespace dm {

void DgDataModel::Load() {
  if( status == NOTEXISTS )
    return;
  unsigned line = 0;

  TreeItem* pHeader = pRoot->AppendChild( line, "header" );
  while( true ) {
    QString str( file.readLine() );
    int colon = str.indexOf( ':' );
    if( colon != -1 )
      pHeader->AppendChild( line, str.left( colon ), str.mid( colon + 1 ).trimmed() );
    else
      pHeader->AppendChild( line, "", str.trimmed() );
    line++;
    if( str.left( 5 ) == "-----" )
      break; // end of header
  }
  // content
  IToken* pToken = null;
  while( not file.atEnd() ) {
    QString str( file.readLine() );
    if( str.trimmed().isEmpty() ) {
      pRoot->AppendChild( line, "" );
      line++;
      continue;
    }

    QStringList words = str.split( ' ' );
    words.last() = words.last().trimmed();

    if( words.first() == "DgFile" ) {
      pRoot->AppendChild( line, words.first(), words.value( 1 ) )
          ->AppendChild( line, "", "{\n; For compatibility\n}" );
      line += 3;
      continue;
    }

    if( pToken == null ) {

      int token = Name2Int( words.first().toUtf8().data(), TKN::names );
      if( token == -1 ) {
        pRoot->AppendChild( line++, "syntax error" );
        continue;
      }

      switch( token ) {
      case TKN::Equil100:               pToken = new TokenBase< TKN::Equil100              >( pRoot ); break;
      case TKN::EquilHints114:          pToken = new TokenBase< TKN::EquilHints114         >( pRoot ); break;
      case TKN::Template100:            pToken = new TokenBase< TKN::Template100           >( pRoot ); break;
      case TKN::Template110:            pToken = new TokenBase< TKN::Template110           >( pRoot ); break;
      case TKN::SonnetData100:          pToken = new TokenBase< TKN::SonnetData100         >( pRoot ); break;
      case TKN::Mesh112:                pToken = new TokenBase< TKN::Mesh112               >( pRoot ); break;
      case TKN::MeshFingerprint112:     pToken = new TokenBase< TKN::MeshFingerprint112    >( pRoot ); break;
      case TKN::MeshPoint112:           pToken = new TokenBase< TKN::MeshPoint112          >( pRoot ); break;
      case TKN::Nodes100:               pToken = new TokenBase< TKN::Nodes100              >( pRoot ); break;
      case TKN::Elems100:               pToken = new TokenBase< TKN::Elems100              >( pRoot ); break;
      case TKN::Surfaces100:            pToken = new TokenBase< TKN::Surfaces100           >( pRoot ); break;
      case TKN::Surfaces108:            pToken = new TokenBase< TKN::Surfaces108           >( pRoot ); break;
      case TKN::GridPoints100:          pToken = new TokenBase< TKN::GridPoints100         >( pRoot ); break;
      case TKN::GridPoints108:          pToken = new TokenBase< TKN::GridPoints108         >( pRoot ); break;
      case TKN::XPointsEx114:           pToken = new TokenBase< TKN::XPointsEx114          >( pRoot ); break;
      case TKN::GridPointSegs114:       pToken = new TokenBase< TKN::GridPointSegs114      >( pRoot ); break;
      case TKN::SurfaceZones114:        pToken = new TokenBase< TKN::SurfaceZones114       >( pRoot ); break;
      case TKN::SurfacesEx114:          pToken = new TokenBase< TKN::SurfacesEx114         >( pRoot ); break;
      case TKN::GridPointsEx114:        pToken = new TokenBase< TKN::GridPointsEx114       >( pRoot ); break;
      case TKN::Separators101:          pToken = new TokenBase< TKN::Separators101         >( pRoot ); break;
      case TKN::Sources104:             pToken = new TokenBase< TKN::Sources104            >( pRoot ); break;
      case TKN::Chords115:              pToken = new TokenBase< TKN::Chords115             >( pRoot ); break;
      case TKN::Chords106:              pToken = new TokenBase< TKN::Chords106             >( pRoot ); break;
      case TKN::MarkedElems100:         pToken = new TokenBase< TKN::MarkedElems100        >( pRoot ); break;
      case TKN::MarkedSeparators104:    pToken = new TokenBase< TKN::MarkedSeparators104   >( pRoot ); break;
      case TKN::MarkedSources104:       pToken = new TokenBase< TKN::MarkedSources104      >( pRoot ); break;
      case TKN::MarkedChords110:        pToken = new TokenBase< TKN::MarkedChords110       >( pRoot ); break;
      case TKN::MarkedMeshElements112:  pToken = new TokenBase< TKN::MarkedMeshElements112 >( pRoot ); break;
      case TKN::MarkedMeshCells112:     pToken = new TokenBase< TKN::MarkedMeshCells112    >( pRoot ); break;
      case TKN::VarSetDefs100:          pToken = new TokenBase< TKN::VarSetDefs100         >( pRoot ); break;
      case TKN::VarDefs100:             pToken = new TokenBase< TKN::VarDefs100            >( pRoot ); break;
      case TKN::VarSetDefs101:          pToken = new TokenBase< TKN::VarSetDefs101         >( pRoot ); break;
      case TKN::VarDefs101:             pToken = new TokenBase< TKN::VarDefs101            >( pRoot ); break;
      case TKN::VarDefs102:             pToken = new TokenBase< TKN::VarDefs102            >( pRoot ); break;
      case TKN::VarSets100:             pToken = new TokenBase< TKN::VarSets100            >( pRoot ); break;
      case TKN::XPoint100:              pToken = new TokenBase< TKN::XPoint100             >( pRoot ); break;
      case TKN::MaxElemId101:           pToken = new TokenBase< TKN::MaxElemId101          >( pRoot ); break;
      case TKN::CreaTime101:            pToken = new TokenBase< TKN::CreaTime101           >( pRoot ); break;
      case TKN::OutputMode109:          pToken = new TokenBase< TKN::OutputMode109         >( pRoot ); break;
      case TKN::TopoName114:            pToken = new TokenBase< TKN::TopoName114           >( pRoot ); break;
      case TKN::ViewAttr103:            pToken = new TokenBase< TKN::ViewAttr103           >( pRoot ); break;
      case TKN::ViewAttr105:            pToken = new TokenBase< TKN::ViewAttr105           >( pRoot ); break;
      case TKN::ViewAttr115:            pToken = new TokenBase< TKN::ViewAttr115           >( pRoot ); break;
      case TKN::OutputFlags111:         pToken = new TokenBase< TKN::OutputFlags111        >( pRoot ); break;
      case TKN::MeshSlidingOptions112:  pToken = new TokenBase< TKN::MeshSlidingOptions112 >( pRoot ); break;
      case TKN::MeshFileHeader113:      pToken = new TokenBase< TKN::MeshFileHeader113     >( pRoot ); break;
      case TKN::Comments316:            pToken = new TokenBase< TKN::Comments316           >( pRoot ); break;
      case TKN::Properties316:          pToken = new TokenBase< TKN::Properties316         >( pRoot ); break;
      default: break;
      }
    }
    if( pToken != null and pToken->Load( words, line ) ) {
      delete pToken;
      pToken = null;
    }
  }
}

} // namespace dm
