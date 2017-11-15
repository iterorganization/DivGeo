#include "Actions.h"
#include "Model.h"
#include <iostream>

namespace Actions
{
  int ActAddEquil::Do()
  {
    SENDER_NAME( "ActDelEquil::Do" );
    if( pModel->IsLocked() )
      return 0;

    assert( !pModel->GetFluxModel()->HasEquil() );

    EquilPtr pEq = dgtype_cast< EquilPtr >( pObject );

    //eq->x=eq->y=eq->val=NULL;
    
    /* Do not create the UndoAction until sure that no error occured */
    int retCode = pEq->Load();
    if( retCode != 0 )
      return retCode;

    ActDelEquil* pADE = new ActDelEquil( pModel, pEq );
    pEq->CalcValues();
    pModel->GetFluxModel()->SetEquil( pEq );
    pModel->GetFluxModel()->GetTopology()->SetEquil( pEq );
    pModel->ActionStack().AddUndoRec( pADE );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelEquil::Do()
  {
    SENDER_NAME( "ActDelEquil::Do" );
    if( pModel->IsLocked() )
      return 0;

    if( pObject->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    EquilPtr pEq = dgtype_cast< EquilPtr >( pObject );

    IComponentList vXPT_copy = pModel->GetFluxModel()->GetTopology()->XPointTests();
    FOREACHPTRCONST( XPointTestPtr, pXPT, vXPT_copy )
      pXPT->Delete();
    vXPT_copy.clear();

    pEq->FreeSplines();
    pEq->FreeCache();
    ActAddEquil* pAAE = new ActAddEquil( pModel, pEq );
    pModel->GetFluxModel()->SetEquil( null );
    pModel->GetFluxModel()->GetTopology()->SetEquil( null );
    pEq->FreeArrays();
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pAAE );
    return 0;
  }


  int ActAddTopology::Do()
  {
    SENDER_NAME( "ActAddTopology::Do" );
    if( pModel->IsLocked() )
      return 0;

    TopologyPtr pTopo = dgtype_cast< TopologyPtr >( pObject );

    pTopo->SetEquil( pModel->GetFluxModel()->GetEquil() );
    pModel->GetFluxModel()->SetTopology( pTopo );

    ActDelTopology* pADT = new ActDelTopology( pModel, pTopo );
    pModel->ActionStack().AddUndoRec( pADT );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelTopology::Do()
  {
    SENDER_NAME( "ActDelTopology::Do" );
    if( pModel->IsLocked() )
      return 0;

    if( pObject->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    TopologyPtr pTopo = dgtype_cast< TopologyPtr >( pObject );

    pTopo->SetEquil( null );
    pModel->GetFluxModel()->SetTopology( null );

    ActAddTopology* pAAT = new ActAddTopology( pModel, pTopo );
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pAAT );
    return 0;
  }


  int ActAddGridPointEx::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    assert( pObject->Type() == OT::GRIDPOINTEX );

    GridPointExPtr pGPX = dgtype_cast< GridPointExPtr >( pObject );
    ActDelGridPointEx* pADGPE = new ActDelGridPointEx( pModel, pGPX );
  
/*  for (gp1=AppGridPoint1st(a,&ix);gp1!=NULL;gp1=Next(&ix))
    if (gp1->area>gp->area ||(gp1->area==gp->area && gp1->value>gp->value))
      break;
  if (!IsEmptyGroup(a->gridPoints)) gp1=Prev(&ix); -- old
  GroupAddAt(a->gridPoints,gp1,gp); */

    pModel->IncludeObject( pObject );
    if( pGPX->IsDrawn() )
      pGPX->RecalcPos();
    pModel->ActionStack().AddUndoRec( pADGPE );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelGridPointEx::Do()
  {
    SENDER_NAME( "ActDelGridPointEx::Do" );
    if( pModel->IsLocked() )
      return 0;

    assert( pObject->Type() == OT::GRIDPOINTEX );

    if( pObject->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    GridPointExPtr pGPX = dgtype_cast< GridPointExPtr >( pObject );
    ActAddGridPointEx* pAAGPX = new ActAddGridPointEx( pModel, pGPX );
    pModel->ExcludeObject( pObject );
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pAAGPX );
    return 0;
  }

  int ActChangeGridPointEx::Do()
  {
    SENDER_NAME( "ActChangeGridPointEx::Do" );
    if( pModel->IsLocked() )
      return 0;

    if( pObject->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );
    assert( pObject->Type() == OT::GRIDPOINTEX );

    GridPointExPtr pGPX = dgtype_cast< GridPointExPtr >( pObject );
    int oldZone = pGPX->Zone();
    double oldValue = pGPX->Value();
    ActChangeGridPointEx* pActChGPX = new ActChangeGridPointEx( pModel, pGPX, oldZone, oldValue );

/*   GroupDel(a->gridPointsEx,ar->gpx); -- obsolete */
    pGPX->SetZone( zone );
    pGPX->SetValue( value );

/*  for (gp1=AppGridPoint1st(a,&ix);gp1!=NULL;gp1=Next(&ix))
    if (gp1->area>ar->gp->area ||
        (gp1->area==ar->gp->area && gp1->value>ar->gp->value))
      break;
  if (!IsEmptyGroup(a->gridPoints)) gp1=Prev(&ix);
  GroupAddAt(a->gridPoints,gp1,ar->gp); */

    if( pGPX->IsDrawn() )
      pGPX->RecalcPos();
    pModel->ActionStack().AddUndoRec( pActChGPX );

    std::StringList args;
    if( oldZone != zone ) {
      std::stringstream ss;
      ss << "zone" << ": " << oldZone << " -> " << zone;
      args.push_back( ss.str() );
    }
    if( oldValue != value ) {
      std::stringstream ss;
      ss << "value" << ": " << oldValue << " -> " << value;
      args.push_back( ss.str() );
    }
    LogMessage( DG3::ACT_CHANGED, args );
    return 0;
  }

  
  int ActAddSurfaceEx::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    assert( pModel->GetFluxModel()->HasEquil() );
    assert( pObject->Type() == OT::SURFACEEX );
    SurfaceExPtr pSX = dgtype_cast< SurfaceExPtr >( pObject );
    assert( !pSX->HasLine() );
    ActDelSurfaceEx* pActDelSX = new ActDelSurfaceEx( pModel, pSX );

    if( pSX->IsDrawn() )
      pSX->RecalcPos();

      /*LockObject(a->equil,1);*/

    pModel->IncludeObject( pObject );

      /*if (!a->equil->signInside && s->closed)
        a->equil->signInside=dg_sign(s->level);*/

    pModel->ActionStack().AddUndoRec( pActDelSX );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelSurfaceEx::Do()
  {
    SENDER_NAME( "ActDelSurfaceEx::Do" );
    if( pModel->IsLocked() )
      return 0;

    assert( pObject->Type() == OT::SURFACEEX );

    if( pObject->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    SurfaceExPtr pSX = dgtype_cast< SurfaceExPtr >( pObject );
    ActAddSurfaceEx* pActAddSX = new ActAddSurfaceEx( pModel, pSX );
    pSX->InvalidatePos();
    pModel->ExcludeObject( pObject );
    /*LockObject(a->equil,-1);*/
    assert( !pSX->HasLine() );
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pActAddSX );
    return 0;
  }

  int ActChangeSurfaceEx::Do()
  {
    SENDER_NAME( "ActChangeSurfaceEx::Do" );
   if( pModel->IsLocked() )
      return 0;

    assert( pObject->Type() == OT::SURFACEEX );
    SurfaceExPtr pSX = dgtype_cast< SurfaceExPtr >( pObject );

    if( pSX->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    SurfaceExParams sxParams = pSX->GetParameters();

    ActChangeSurfaceEx* pActChSX = ( zone == SZN_XY ) ? 
      new ActChangeSurfaceEx( pModel, pSX, sxParams.origin ) :
      new ActChangeSurfaceEx( pModel, pSX, sxParams.zone, sxParams.level );

    pSX->SetParameters( SurfaceExParams( xy, level, zone ) );
    pSX->RecalcPos();
    pModel->ActionStack().AddUndoRec( pActChSX );

    std::StringList args;
    if( sxParams.origin != xy ) {
      std::stringstream ss;
      ss << "origin" << ": " << sxParams.origin.ToString() << " -> " << xy.ToString();
      args.push_back( ss.str() );
    }
    if( sxParams.zone != zone ) {
      std::stringstream ss;
      ss << "zone" << ": " << sxParams.zone << " -> " << zone;
      args.push_back( ss.str() );
    }
    if( sxParams.level != level ) {
      std::stringstream ss;
      ss << "level" << ": " << sxParams.level << " -> " << level;
      args.push_back( ss.str() );
    }
    LogMessage( DG3::ACT_CHANGED, args );
    return 0;
  }



  int ActAddSurfaceZone::Do()
  {
    if( pModel->IsLocked() )
      return 0;

    assert( pObject->Type() == OT::SURFACEZONE );
    SurfaceZonePtr pSZ = dgtype_cast< SurfaceZonePtr >( pObject );
    ActDelSurfaceZone* pActDelSZ = new ActDelSurfaceZone( pModel, pSZ );
    pModel->IncludeObject( pObject );
    pModel->GetFluxModel()->GetTopology()->InvalidateCache();
    pModel->SetChangeFlag( CHF::TOPOLOGY );
    pModel->ActionStack().AddUndoRec( pActDelSZ );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelSurfaceZone::Do()
  {
    SENDER_NAME( "ActDelSurfaceZone::Do" );
    if( pModel->IsLocked() )
      return 0;

    assert( pObject->Type() == OT::SURFACEZONE );

    SurfaceZonePtr pSZ = dgtype_cast< SurfaceZonePtr >( pObject );
    pSZ->FreeBounds();

    if( pObject->IsLocked() )
      pModel->SendMessage( FATAL_ERROR, SENDER, DG3::OBJECT_IS_LOCKED );

    ActAddSurfaceZone* pActAddSZ = new ActAddSurfaceZone( pModel, pSZ );
    pModel->ExcludeObject( pObject );
    pModel->GetFluxModel()->GetTopology()->InvalidateCache();
    pModel->SetChangeFlag( CHF::TOPOLOGY );
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pActAddSZ );
    return 0;
  }



  int ActAddGridPointSeg::Do()
  {
    if( pModel->IsLocked() )
        return 0;

    GridPointSegPtr pGPS = dgtype_cast< GridPointSegPtr >( pObject );
    assert( pObject->Type() == OT::GRIDPOINTSEG );
    assert( !pGPS->HasLine() );
    ActDelGridPointSeg* pActDelGPS = new ActDelGridPointSeg( pModel, pGPS );
    pGPS->SetLine();
    pModel->IncludeObject( pObject );
    pModel->GetFluxModel()->GetTopology()->InvalidateCache();
    pModel->SetChangeFlag( CHF::TOPOLOGY );
    pModel->ActionStack().AddUndoRec( pActDelGPS );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelGridPointSeg::Do()
  {
    if( pModel->IsLocked() )
        return 0;

    assert( pObject->Type() == OT::GRIDPOINTSEG );
    assert( !pObject->IsLocked() );
    GridPointSegPtr pGPS = dgtype_cast< GridPointSegPtr >( pObject );
    assert( pGPS->HasLine() );
    ActAddGridPointSeg* pActAddGPS = new ActAddGridPointSeg( pModel, pGPS );
    pModel->ExcludeObject( pObject );
    pModel->GetFluxModel()->GetTopology()->InvalidateCache();
    pModel->SetChangeFlag( CHF::TOPOLOGY );
    pGPS->SetLine( NULL );
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pActAddGPS );
    return 0;
  }


  int ActAddXPointSeg::Do()
  {
    if( pModel->IsLocked() )
        return 0;

    XPointSegPtr pXPS = dgtype_cast< XPointSegPtr >( pObject );

    EquilPtr pEquil = pModel->GetFluxModel()->GetEquil();
    pXPS->SetEquilParams( (pEquil->PointFirst().y - pEquil->PointLast().y),
                          (pEquil->AverageCellWidth() + pEquil->AverageCellHeight()) / 2. );
    int i = pXPS->CalcLine();
    if( i != 0 )
      return i;

    ActDelXPointSeg* pActDelXPS = new ActDelXPointSeg( pModel, pXPS );
    pXPS->Register();
    pModel->IncludeObject( pObject );
    pModel->GetFluxModel()->GetTopology()->InvalidateCache();
    pModel->SetChangeFlag( CHF::TOPOLOGY );
/*  LockObject(xps->xpt,1); */
/*  LockObject(a->equil,1); */
    pModel->ActionStack().AddUndoRec( pActDelXPS );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelXPointSeg::Do()
  {
    if( pModel->IsLocked() )
        return 0;

    XPointSegPtr pXPS = dgtype_cast< XPointSegPtr >( pObject );
    assert( !pXPS->IsLocked() );
    assert( pModel->GetFluxModel()->HasEquil() );
    /* LockObject(a->equil,-1); */
    /* LockObject(xps->xpt,-1); */
    ActAddXPointSeg* pActAddXPS = new ActAddXPointSeg( pModel, pXPS );
    pModel->ExcludeObject( pObject );
    pModel->GetFluxModel()->GetTopology()->InvalidateCache();
    pModel->SetChangeFlag( CHF::TOPOLOGY );
    pXPS->ExcludeFromXPT();
    pXPS->FreeLine();
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pActAddXPS );
    return 0;
  }


  int ActAddXPointTest::Do()
  {
    if( pModel->IsLocked() )
        return 0;

    assert( pModel->GetFluxModel()->HasEquil() );
    XPointTestPtr pXPT = dgtype_cast< XPointTestPtr >( pObject );
    ActDelXPointTest* pActDelXPT = new ActDelXPointTest( pModel, pXPT );
    FluxModelPtr pFlux = pModel->GetFluxModel();
    if( !pFlux->GetEquil()->SplinesAreOk() )
      pFlux->GetEquil()->CalcSplines(); // MB: hack? to prepare splines if equil was restored by Undo
    pFlux->GetTopology()->CalcXPointGradients( pXPT );
    pFlux->IncludeObject( pXPT );
    pFlux->GetTopology()->InvalidateCache();
    pModel->SetChangeFlag( CHF::TOPOLOGY );
    /* LockObject(a->equil,1); */
    pModel->ActionStack().AddUndoRec( pActDelXPT );
    LogMessage( DG3::ACT_ADDED );
    return 0;
  }

  int ActDelXPointTest::Do()
  {
    if( pModel->IsLocked() )
        return 0;

    assert( !pObject->IsLocked() );
    /* LockObject(a->equil,-1); */
    XPointTestPtr pXPT = dgtype_cast< XPointTestPtr >( pObject );
    ActAddXPointTest* pActAddXPT = new ActAddXPointTest( pModel, pXPT );
    pModel->ExcludeObject( pXPT );
    pModel->GetFluxModel()->GetTopology()->InvalidateCache();
    pModel->SetChangeFlag( CHF::TOPOLOGY );
    pXPT->FreeArrays();
    LogMessage( DG3::ACT_DELETED );
    pModel->ActionStack().AddUndoRec( pActAddXPT );
    return 0;
  }

}
