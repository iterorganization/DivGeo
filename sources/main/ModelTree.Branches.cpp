#include "ModelTree.h"

QString ModelTree::AddBranchForModel( QTreeWidgetItem* _pItem, ModelPtr _pModel,
                                      int _expand_depth, bool _rebuild )
{
  if( _rebuild || _pItem->childCount() != 11 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::NAME ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::FILENAME ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::CREATION_TIME ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::OUTPUT_MODE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::UNUSED_NUMS ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::NODE ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::CHORD ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::ELEMENT ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::SEPARATOR ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::SOURCE ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::VARSETDEF ) );
    //AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::VARDEF ) );
    //AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::VARSET ) );
  }
  int i = 0;
  SetChildData( _pItem, i++, ToQString( _pModel->Name() ) );
  SetChildData( _pItem, i++, ToQString( _pModel->FileName() ) );
  SetChildData( _pItem, i++, ToQString( _pModel->CreationTime() ) );
  SetChildData( _pItem, i++, _pModel->CarreOutputMode() ?
                  SM_GUI( GUI::MODEL_INFO::MODEL::CARRE ) :
                  SM_GUI( GUI::MODEL_INFO::MODEL::SONNET ) );
  SetChildData( _pItem, i++, ToQString( _pModel->GetUnusedNumbersCnt() ) );//1408
  SetChildData( _pItem, i++, _pModel->Struct()->Nodes(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pModel->Struct()->Chords(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pModel->Struct()->Elements(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pModel->Struct()->Separators(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pModel->Sources(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pModel->Vars()->VarSetDefs(), _expand_depth ); /*branch*/
  //SetChildData( _pItem, i++, _pModel->VarDefs(), _expand_depth ); /*branch*/
  //SetChildData( _pItem, i++, _pModel->VarSets(), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::MODEL );
}

QString ModelTree::AddBranchForMesh( QTreeWidgetItem* _pItem, MeshPtr _pMesh,
                                     int _expand_depth, bool _rebuild )
{
  if( _rebuild || _pItem->childCount() != 10 ) {
    Clear( _pItem );
    AddChild( _pItem,  SM_GUI( GUI::MODEL_INFO::MESH::FILENAME ) );
    AddChild( _pItem,  SM_GUI( GUI::MODEL_INFO::MESH::HEADER ) );
    AddChild( _pItem,  SM_GUI( GUI::MODEL_INFO::MESH::FINGERPRINT ) );
    AddChild( _pItem,  SM_GUI( GUI::MODEL_INFO::MESH::SIZE ) );
    AddChild( _pItem,  SM_GUI( GUI::MODEL_INFO::MESH::SLIDING_MODE ) );
    AddChild( _pItem,  SM_GUI( GUI::MODEL_INFO::MESH::SLIDING_THRSHLD ) );
    AddChild( _pItem,  SM_GUI( GUI::MODEL_INFO::MESH::DOUBLE_BORDER ) );
    AddChild( _pItem,  TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::MESHCELL ) );
    AddChild( _pItem,  TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::MESHPOINT ) );
    AddChild( _pItem,  TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::MESHELEMENT ) );
  }

  int i = 0;
  SetChildData( _pItem, i++, ToQString( _pMesh->FileName() ) );
  SetChildData( _pItem, i++, ToQString( _pMesh->HeaderString() ) );
  SetChildData( _pItem, i++, ToQString( (int)_pMesh->Fingerprint() ) );
  SetChildData( _pItem, i++, ToQString( _pMesh->Size() ) );
  SetChildData( _pItem, i++, pModel->CheckMeshSlidingMode( MSM::SPLINE ) ?
                  SM_GUI( GUI::MODEL_INFO::MESH::BY_SPLINES ) :
                  SM_GUI( GUI::MODEL_INFO::MESH::BY_SURFACES ) );
  SetChildData( _pItem, i++, ToQString( pModel->MeshSlidingThreshold() ) );
  SetChildData( _pItem, i++, ToQString( pModel->HasDoubleMeshBorder(), pSM ) );
  SetChildData( _pItem, i++, _pMesh->Cells(), _expand_depth, 100 );
  SetChildData( _pItem, i++, _pMesh->Points(), _expand_depth, 100 );
  SetChildData( _pItem, i++, _pMesh->Elements(), _expand_depth, 100 );

  return SM_GUI( GUI::MODEL_INFO::TYPE::MESH );
}

QString ModelTree::AddBranchForTopology( QTreeWidgetItem* _pItem, TopologyPtr _pTopology,
                                         int _expand_depth, bool _rebuild )
{
  if( _rebuild || _pItem->childCount() != 7 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::TOPOLOGY_NAME ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::FILENAME ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::POLOIDAL ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::TOROIDAL ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::XPOINTTEST ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::GRIDPOINTSEG ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::SURFACEZONE ) );
  }
  FluxModelPtr pFlux = pModel->GetFluxModel();
  int i = 0;
  SetChildData( _pItem, i++, ToQString( pModel->TopologyName() ) );
  SetChildData( _pItem, i++, ToQString( _pTopology->FileName() ) );
  SetChildData( _pItem, i++, ToQString( pFlux->GetGridPointStatsStr() ) );
  SetChildData( _pItem, i++, ToQString( pFlux->GetSurfacesStatsStr() ) );

  SetChildData( _pItem, i++, _pTopology->XPointTests(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pTopology->GridPointSegs(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pTopology->SurfaceZones(), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::TOPOLOGY );
}

QString ModelTree::AddBranchForEquil( QTreeWidgetItem* _pItem, EquilPtr _pEquil,
                                      int /*_expand_depth*/, bool _rebuild )
{
  if( _rebuild || _pItem->childCount() != 3 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::EQUIL::FILENAME ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::EQUIL::SIZE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::EQUIL::VALUE ) );
  }
  int i = 0;
  SetChildData( _pItem, i++, ToQString( _pEquil->FileName() ) );
  SetChildData( _pItem, i++, ToQString( _pEquil->Size() ) );
  SetChildData( _pItem, i++, ToQString( _pEquil->MinValue(), _pEquil->MaxValue() ) );

  return SM_GUI( GUI::MODEL_INFO::TYPE::EQUIL );
}


QString ModelTree::AddBranchForElement(QTreeWidgetItem* _pItem, ElementPtr _pElem,
                                       int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 8 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::ID ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MARKED ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::INTARGET ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::NODE_1 ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::NODE_2 ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::LENGTH ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARSCONT ) );
  }
  SetChildData( _pItem, i++, ToQString( _pElem->Id() ) );
  SetChildData( _pItem, i++, ToQString( _pElem->IsMarked(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pElem->IsInTarget(), pSM ) );
  SetChildData( _pItem, i++, _pElem->Node(1), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pElem->Node(2), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, ToQString( _pElem->Length() ) );
  SetChildData( _pItem, i++, _pElem->Vars(), _expand_depth );  /*branch*/
  SetChildData( _pItem, i++, _pElem->Containers(), _expand_depth );  /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::ELEMENT );
}

QString ModelTree::AddBranchForSeparator(QTreeWidgetItem* _pItem, SeparatorPtr _pSep,
                                         int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 5 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::ID ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MARKED ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::POSITION ) );
    AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::NODE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS ) );
  }
  SetChildData( _pItem, i++, ToQString( _pSep->Id() ) );
  SetChildData( _pItem, i++, ToQString( _pSep->IsMarked(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pSep->Position() ) );
  SetChildData( _pItem, i++, ToQString( _pSep->Node()->Position() ) );
  SetChildData( _pItem, i++, _pSep->Vars(), _expand_depth );  /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::SEPARATOR );
}

QString ModelTree::AddBranchForSource(QTreeWidgetItem* _pItem, SourcePtr _pSrc,
                                      int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 2 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::POSITION ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARSCONT ) );
  }
  SetChildData( _pItem, i++, ToQString( _pSrc->Position() ) );
  SetChildData( _pItem, i++, _pSrc->Vars(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pSrc->Containers(), _expand_depth );  /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::SOURCE );
}

QString ModelTree::AddBranchForChord(QTreeWidgetItem* _pItem, ChordPtr _pCh,
                                     int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 7 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MARKED ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::POINT_1 ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::POINT_2 ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::IS3D ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::LENGTH ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARSCONT ) );
  }
  SetChildData( _pItem, i++, ToQString( _pCh->IsMarked(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pCh->Point_1() ) );
  SetChildData( _pItem, i++, ToQString( _pCh->Point_2() ) );
  SetChildData( _pItem, i++, ToQString( _pCh->Is3D(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pCh->Length() ) );
  SetChildData( _pItem, i++, _pCh->Vars(), _expand_depth );  /*branch*/
  SetChildData( _pItem, i++, _pCh->Containers(), _expand_depth );  /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::CHORD );
}

QString ModelTree::AddBranchForNode(QTreeWidgetItem* _pItem, NodePtr _pNode,
                                    int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 6 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::POSITION ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::REGULAR ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::ISEND ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MODEL::INTARGET ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::ELEMENT ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::SEPARATOR ) );
  }
  SetChildData( _pItem, i++, ToQString( _pNode->Position() ) );
  SetChildData( _pItem, i++, ToQString( _pNode->IsRegular( null ), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pNode->IsEndNode( null ), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pNode->IsInTarget(), pSM ) );
  SetChildData( _pItem, i++, _pNode->ConnectedElements(), _expand_depth );   /*branch*/
  SetChildData( _pItem, i++, _pNode->ConnectedSeparators(), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::NODE );
}

QString ModelTree::AddBranchForMeshPoint(QTreeWidgetItem* _pItem, MeshPointPtr _pMP,
                                         int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 9 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::INDEX ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::POSITION ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::CHANGED ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::CUTFLAG ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::BORDER ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::MESHELEMENT ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::MESHCELL ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::NEIGHBOURS ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::OUTERMOST ) );
  }
  SetChildData( _pItem, i++, ToQString( _pMP->Index(), false ) );
  SetChildData( _pItem, i++, ToQString( _pMP->Position() )
                + SM_GUI( GUI::MODEL_INFO::MESH::BACKUP )
                + ToQString( _pMP->Backup() ) );
  SetChildData( _pItem, i++, ToQString( _pMP->IsChanged(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pMP->CutFlag() ) );
  SetChildData( _pItem, i++, ToQString( _pMP->IsOnBorder(), pSM ) );
  SetChildData( _pItem, i++, _pMP->ConnectedElements(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pMP->ConnectedCells(), _expand_depth );    /*branch*/
  SetChildData( _pItem, i++, _pMP->NeighbourPoints(), _expand_depth );   /*branch*/
  SetChildData( _pItem, i++, (IComponentPtr)_pMP->OutermostPoint(), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::MESHPOINT );
}

QString ModelTree::AddBranchForMeshCell(QTreeWidgetItem* _pItem, MeshCellPtr _pMC,
                                        int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 12 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::NUMBER ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::INDEX ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::CENTER ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::RATIO ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MARKED ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::CHANGED ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::REGULAR ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::CUTFLAG ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::MESHPOINT ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::MESHELEMENT ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::NEIGHBOURS ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARSCONT ) );
  }
  SetChildData( _pItem, i++, ToQString( _pMC->eNumber() ) );
  SetChildData( _pItem, i++, ToQString( _pMC->Index(), false ) );
  SetChildData( _pItem, i++, ToQString( _pMC->Center() )
                + SM_GUI( GUI::MODEL_INFO::MESH::BACKUP )
                + ToQString( _pMC->BackupCenter() ) );
  SetChildData( _pItem, i++, ToQString( _pMC->Ratio() )
                + SM_GUI( GUI::MODEL_INFO::MESH::BACKUP )
                + ToQString( _pMC->BackupRatio() ) );
  SetChildData( _pItem, i++, ToQString( _pMC->IsMarked(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pMC->IsChanged(), pSM ) );

  QString irregular_str;
  switch( _pMC->IsIrregular() ) {
  case 0: irregular_str = SM_MSG( STR::REGULAR ); break;
  case STR::MESH_CELL_TWISTED:    irregular_str = SM_MSG( STR::MESH_CELL_TWISTED ); break;
  case STR::MESH_CELL_BAD_CENTER: irregular_str = SM_MSG( STR::MESH_CELL_BAD_CENTER ); break;
  case STR::MESH_CELL_CONCAVE:    irregular_str = SM_MSG( STR::MESH_CELL_CONCAVE ); break;
  default: irregular_str = "?"; break;
  }
  SetChildData( _pItem, i++, irregular_str );

  QString cutflag_str = QString( "%1, %2, %3, %4" )
                        .arg( _pMC->CutFlag(0) ).arg( _pMC->CutFlag(1) )
                        .arg( _pMC->CutFlag(2) ).arg( _pMC->CutFlag(3) );
  SetChildData( _pItem, i++, cutflag_str );
  SetChildData( _pItem, i++, _pMC->ConnectedPoints(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pMC->ConnectedElements(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pMC->NeighbourCells(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pMC->Containers(), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::MESHCELL );
}

QString ModelTree::AddBranchForMeshElement(QTreeWidgetItem* _pItem, MeshElementPtr _pME,
                                           int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 6 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MARKED ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::CUTFLAG ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::VERTICAL ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::MESHPOINT ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::MESHCELL ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARSCONT ) );
  }
  SetChildData( _pItem, i++, ToQString( _pME->IsMarked(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pME->CutFlag() ) );
  SetChildData( _pItem, i++, ToQString( _pME->IsVertical(), pSM ) );
  SetChildData( _pItem, i++, _pME->ConnectedPoints(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pME->ConnectedCells(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pME->Containers(), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::MESHELEMENT );
}

QString ModelTree::AddBranchForXPointTest(QTreeWidgetItem* _pItem, XPointTestPtr _pXPT,
                                          int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 5 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::ID ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::MESH::CENTER ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::LEVEL ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::VERTICIES ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::XPOINTSEG ) );
  }
  SetChildData( _pItem, i++, ToQString( _pXPT->ID() ) );
  SetChildData( _pItem, i++, ToQString( _pXPT->Center() ) );
  SetChildData( _pItem, i++, ToQString( _pXPT->Level() ) );
  SetChildData( _pItem, i++, ToQString( (int)4 ) ); /*branch*/
  QTreeWidgetItem* pChild = _pItem->child( i-1 );
  for( ulong c = 0; c < 4; c++ ) {
    pChild->addChild( new QTreeWidgetItem( QStringList()
                                           << ToQString( _pXPT->MinMax(c), false )
                                           << QString( "%1 (%2)" ).arg( _pXPT->MinMaxLevel(c) )
                                           .arg( _pXPT->MinMaxT(c) ) ) );
  }
  SetChildData( _pItem, i++, _pXPT->Segments(), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::XPOINTTEST );
}

QString ModelTree::AddBranchForXPointSeg(QTreeWidgetItem* _pItem, XPointSegPtr _pXPS,
                                         int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 5 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ZONE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::START ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::END ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::LENGTH ) );
    AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::XPOINTTEST ) );
  }
  SetChildData( _pItem, i++, ToQString( _pXPS->Number() ) );
  SetChildData( _pItem, i++, ToQString( _pXPS->StartPos() ) );
  SetChildData( _pItem, i++, ToQString( _pXPS->EndPos() ) );
  SetChildData( _pItem, i++, ToQString( _pXPS->LineLength() ) );
  SetChildData( _pItem, i++, _pXPS->XPT(), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::XPOINTSEG );
}

QString ModelTree::AddBranchForGridPointSeg(QTreeWidgetItem* _pItem, GridPointSegPtr _pGPS,
                                            int _expand_depth,  bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 10 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ZONE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::NAME ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::DESCR ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ISUSED ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::DIR ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::DIRTARGET ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::LEVEL) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::LENGTH ) );
    AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::XPOINTSEG ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::GRIDPOINTEX ) );
  }
  SetChildData( _pItem, i++, ToQString( _pGPS->Zone() ) );
  SetChildData( _pItem, i++, ToQString( _pGPS->ShortName() ) );
  SetChildData( _pItem, i++, ToQString( _pGPS->LongName() ) );
  SetChildData( _pItem, i++, ToQString( _pGPS->IsUsed(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pGPS->Dir() ) );
  SetChildData( _pItem, i++, ToQString( _pGPS->TargetDir() ) );
  SetChildData( _pItem, i++, ToQString( _pGPS->Level() ) );
  SetChildData( _pItem, i++, ToQString( _pGPS->LineLength() ) );
  SetChildData( _pItem, i++, _pGPS->XPS(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++,
    pModel->GetFluxModel()->GridPointsEx( _pGPS->Zone() ), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::GRIDPOINTSEG );
}

QString ModelTree::AddBranchForGridPointEx(QTreeWidgetItem* _pItem, GridPointExPtr _pGPX,
                                           int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 8 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ZONE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::EQUIL::VALUE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::POSITION ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::DERIVATIVE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::CRID ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ISDRAWN ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ISOK ) );
    AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::GRIDPOINTSEG ) );
  }
  SetChildData( _pItem, i++, ToQString( _pGPX->Zone() ) );
  SetChildData( _pItem, i++, ToQString( _pGPX->Value() ) );
  SetChildData( _pItem, i++, ToQString( _pGPX->Position() ) );
  SetChildData( _pItem, i++, ToQString( _pGPX->Derivative() ) );
  SetChildData( _pItem, i++, ToQString( _pGPX->GetCreatorId(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pGPX->IsDrawn(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pGPX->CoordsOk(), pSM ) );
  SetChildData( _pItem, i++,
    pModel->GetFluxModel()->GetTopology()->FindGridPointSeg( _pGPX->Zone() ), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::GRIDPOINTEX );
}

QString ModelTree::AddBranchForSurfaceZone(QTreeWidgetItem* _pItem, SurfaceZonePtr _pSZ,
                                           int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 10 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ZONE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::NAME ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::DESCR ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::GPZONES ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ZONESIGN ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ORIENT ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::LEVEL) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::LIMITED ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::INNERMOST ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::SURFACEEX ) );
  }
  SetChildData( _pItem, i++, ToQString( _pSZ->Zone() ) );
  SetChildData( _pItem, i++, ToQString( _pSZ->ShortName() ) );
  SetChildData( _pItem, i++, ToQString( _pSZ->LongName() ) );
  SetChildData( _pItem, i++, QString( "%1, %2" )
                .arg( _pSZ->GPZone(1) ).arg( _pSZ->GPZone(2) ) ); /*branch*/
  QTreeWidgetItem* pChild = _pItem->child( i-1 );
  for( ulong c = 1; c <= 2; c++ ) {
    AddChild( pChild, TYPE_LABEL( GUI::MODEL_INFO::TYPE::GRIDPOINTSEG ) );
    SetChildData( pChild, c-1,
      pModel->GetFluxModel()->GetTopology()->FindGridPointSeg( _pSZ->GPZone(c) ), _expand_depth );
  }

  SetChildData( _pItem, i++, ToQString( _pSZ->Sign() ) );
  SetChildData( _pItem, i++, ToQString( _pSZ->Orient() ) );
  SetChildData( _pItem, i++, QString( "%1, %2" )
                .arg( _pSZ->Level1() ).arg( _pSZ->Level2() ) );
  SetChildData( _pItem, i++, ToQString( _pSZ->IsLimited(), pSM ) );
  SetChildData( _pItem, i++, (IComponentPtr)_pSZ->Innermost(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++,
    pModel->GetFluxModel()->SurfacesEx( _pSZ->Zone() ), _expand_depth );/*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::SURFACEZONE );
}

QString ModelTree::AddBranchForSurfaceEx(QTreeWidgetItem* _pItem, SurfaceExPtr _pSX,
                                         int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 8 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ZONE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::LEVEL) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ORIGIN ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::CRID ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ISCLOSED ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ISDRAWN ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::TOPOLOGY::ISOK ) );
    AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::SURFACEZONE ) );
  }
  SetChildData( _pItem, i++, _pSX->IsVirtual() ?
                  SM_GUI( GUI::MODEL_INFO::TOPOLOGY::VIRTUAL ) :
                  ToQString( _pSX->Zone() ) );
  SetChildData( _pItem, i++, ToQString( _pSX->Level() ) );
  SetChildData( _pItem, i++, ToQString( _pSX->Origin() ) );
  SetChildData( _pItem, i++, ToQString( _pSX->GetCreatorId(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pSX->IsClosed(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pSX->IsDrawn(), pSM ) );
  SetChildData( _pItem, i++, ToQString( _pSX->CoordsOk(), pSM ) );
  SetChildData( _pItem, i++,
    pModel->GetFluxModel()->GetTopology()->FindSurfaceZone( _pSX->Zone() ), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::SURFACEEX );
}

QString ModelTree::AddBranchForVarSetDef(QTreeWidgetItem* _pItem, VarSetDefPtr _pVSD,
                                         int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 8 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::NAME ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::DESCR ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::HELP ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::MINMAXSETS ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::VARDEF ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::VARSET ) );
    //AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::VAR ) );
  }
  SetChildData( _pItem, i++, ToQString( _pVSD->Name() ) );
  SetChildData( _pItem, i++, ToQString( _pVSD->Descr() ) );
  SetChildData( _pItem, i++, ToQString( _pVSD->Help() ) );
  SetChildData( _pItem, i++, ToQString( NPoint( _pVSD->MinVarSets(), _pVSD->MaxVarSets() ), false ) );
  SetChildData( _pItem, i++, _pVSD->VarDefs(), _expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pVSD->VarSets(), _expand_depth ); /*branch*/
  //SetChildData( _pItem, i++, _pVSD->Vars(), expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::VARSETDEF );
}

QString ModelTree::AddBranchForVarDef(QTreeWidgetItem* _pItem, VarDefPtr _pVD,
                                      int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 8 ) {
    Clear( _pItem );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::NAME ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::DESCR ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::HELP ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::ENUMVALUES ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::ROWCOLUMN ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::VARTYPE ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::TYPEFLAGS ) );
    AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::FLAGS ) );
    //AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::VARSETDEF ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::VAR ) );
  }
  SetChildData( _pItem, i++, ToQString( _pVD->Name() ) );
  SetChildData( _pItem, i++, ToQString( _pVD->Descr() ) );
  SetChildData( _pItem, i++, ToQString( _pVD->Help() ) );
  SetChildData( _pItem, i++, ToQString( _pVD->EnumValues() ) );
  SetChildData( _pItem, i++, ToQString( _pVD->Index(), false ) );
  SetChildData( _pItem, i++, TypeToStr( _pVD->VarDefType() ) );
  SetChildData( _pItem, i++, TypeFlagToStr( _pVD->VarDefType() ) );
  SetChildData( _pItem, i++, FlagToStr( _pVD->Flags() ) );
  //SetChildData( _pItem, i++, (IComponentPtr)_pVD->VSD(), expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pVD->Vars(), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::VARDEF );
}

QString ModelTree::AddBranchForVarSet(QTreeWidgetItem* _pItem, VarSetPtr _pVS,
                                      int _expand_depth, bool _rebuild )
{
  int i = 0;
  if( _rebuild || _pItem->childCount() != 8 ) {
    Clear( _pItem );
    //AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::VARSETDEF ) );
    AddChild( _pItem, TYPE_LABEL_MULT( GUI::MODEL_INFO::TYPE::VAR ) );
  }
  //SetChildData( _pItem, i++, (IComponentPtr)pVS->VSD(), expand_depth ); /*branch*/
  SetChildData( _pItem, i++, _pVS->Vars(), _expand_depth ); /*branch*/

  return SM_GUI( GUI::MODEL_INFO::TYPE::VARSET );
}

QString ModelTree::AddBranchForVar(QTreeWidgetItem* _pItem, VarPtr _pVar,
                                   int _expand_depth, bool _rebuild, int _mode )
{
  switch( _mode ) {
  case 1: {
    if( _rebuild || _pItem->childCount() != 4 ) {
      Clear( _pItem );
      AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::VARSETDEF ) );
      AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::VARDEF ) );
      AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::TYPE::VARSET ) );
      AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::VALUE ) );
      AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::ORIGIN ) );
    }

    int i = 0;
    SetChildData( _pItem, i++, (IComponentPtr)_pVar->VD()->VSD(), _expand_depth ); /*branch*/
    SetChildData( _pItem, i++, _pVar->VD(), _expand_depth ); /*branch*/
    SetChildData( _pItem, i++, (IComponentPtr)_pVar->VS(), _expand_depth ); /*branch*/
    SetChildData( _pItem, i++, _pVar, _expand_depth, 2 ); /*branch*/
    SetChildData( _pItem, i++, _pVar->Origin(), _expand_depth ); /*branch*/
    break;
  }
  case 0: {
    if( _rebuild || _pItem->childCount() != 2 ) {
      Clear( _pItem );
      AddChild( _pItem, SM_GUI( GUI::MODEL_INFO::VARS::ORIGIN ) );
      AddChild( _pItem, TYPE_LABEL( GUI::MODEL_INFO::VALUE ) );
    }

    int i = 0;
    SetChildData( _pItem, i++, _pVar->Origin(), _expand_depth ); /*branch*/
    SetChildData( _pItem, i++, _pVar, _expand_depth, 2 ); /*branch*/
    break;
  }
  case 2: {
    Clear( _pItem );
    UPtr val = _pVar->Value().Share();
    if( !val.IsNull() && val.Type() == UPtr::LIST ) {
      AddObjectList( _pItem, val.ListRef(), _expand_depth );
      _pItem->setData( 1, Qt::UserRole, QVariant::fromValue( val.ListPtr() ) );
    }
    _pItem->setText( 1, QString::fromStdString( val.ToString() ) );
    break;
  }
  }
  return SM_GUI( GUI::MODEL_INFO::TYPE::VAR );
}

QString ModelTree::AddBranchForStack( QTreeWidgetItem* _pItem, const ActionList& _crStack,
                                      int _expand_depth, bool /*_rebuild*/ )
{
  Clear( _pItem );
  FOREACH_CONST( ActionList::const_iterator, itHi, _crStack ) {
    ActHighLevel* pAct = static_cast< ActHighLevel* >( *itHi );
    QTreeWidgetItem* pHiItem = new QTreeWidgetItem();
    _pItem->addChild( pHiItem );
    pHiItem->setText( 0, QString( "[%1]" ).arg( pAct->Size() ) );
    pHiItem->setText( 1, QString::fromStdString( pAct->Descr() ) );
    FOREACH_CONST( ActionList::const_iterator, itSub, pAct->Actions() ) {
      IAction* pActSub = *itSub;
      QTreeWidgetItem* pSubItem = new QTreeWidgetItem();
      pHiItem->addChild( pSubItem );
      AddActionDescr( pSubItem, pActSub, _expand_depth, true );
      pSubItem->setData( 1, Qt::UserRole, QVariant::fromValue( pActSub ) );
    }
  }

  return "";
}
