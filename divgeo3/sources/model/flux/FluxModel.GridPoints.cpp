#include "FluxModel.h"

GridPointExPtr FluxModel::AddGridPointEx( int _zone, double _value )
{
  SENDER_NAME( "AddGridPointEx" );
  if( _value < 0 || _value > 1 )
    pModel->SendMessage( FATAL_ERROR, SENDER,
                         DG3::VALUE_OUT_OF_RANGE, ARG( 0. )_ARG( 1. ) ); /* relcheck_ignore_line */

  GridPointExPtr pGPX = new GridPointEx( pModel, _zone, _value );
  ActAddGridPointEx( pModel, pGPX, DO_AT_ONCE );
  return pGPX;
}



int FluxModel::CountGridPoints( int _area ) const
{
  int i = 0;

  FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx )
    if( pGPX->CoordsOk() && pGPX->Zone() == _area )
      i++;

  return i;
}


int FluxModel::RebuildCarreGridPoints( int _area )
{
  SENDER_NAME( "RebuildCarreGridPoints" );
  if( !pModel->CarreOutputMode() )
    return pModel->SendMessage( WND_ERROR, SENDER, ERR::CARRE_MODE_NEEDED );

  if( _area < 0 ) {
    FOREACHPTRCONST( GridPointSegPtr, pGPS, pTopology->GridPointSegs() ) {
      if( pGPS->HasFlag( GPSF::USED ) )
        RebuildCarreGridPoints( pGPS->Zone() );
    }
    return 0;
  }


  if( CountGridPoints( _area ) == 0 )
    return 0;  /* $$ - maybe some errorcode? */

  CreatorId crid;
  FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx ) {
    if( pGPX->Zone() != _area )
      continue;
    crid = pGPX->GetCreatorId();
    if( !crid.IsGood() )
      continue;
    if( crid.CarreFlag() != 0)
      break;
  }

  if( !crid.IsGood() )
    crid = CreatorId( _area );

  /* $$ - maybe some errorcode if no gridpoints? */
  //if( pGPX == null ) { /* MB: crid here is empty */ }

  return DistributeGridPoints( crid );
}


int FluxModel::DistributeGridPoints( const CreatorId& _crCrId )
{
  IComponentList vGPX_copy = gridPointsEx;
  FOREACHPTRCONST( GridPointExPtr, pGPX, vGPX_copy ) {
    if( pGPX->CheckZone( _crCrId.Area() ) )
      pGPX->Delete();
  }
  vGPX_copy.clear();

  if( !_crCrId.IsGood() )
    return 0;

  double v;
  for( int i = 0; i < _crCrId.Count(); i++ ) {
    v = _crCrId.CalcValue( (i + 1) / (double)(_crCrId.Count() + 1) );
    v = dg_max( v, 0 );
    v = dg_min( v, 1 );

    /* if (v<0 || v>1) { --- Works bad with Carre mode
      Cancel(dlg->w->app);
      ErrorBox(dlg->wDlg,
          GetResourceString(dlg->dg.wLaw,"errBadLaw",NULL,NULL));
      return;
    } --- */

    GridPointExPtr pGPX_new = AddGridPointEx( _crCrId.Area(), v );
    pGPX_new->ChangeCreatorId( _crCrId );
  }

  return 0;
}


bool FluxModel::UpdateSegment( GridPointSegPtr _pGPS, double _newLength )
{
  if( pTopology->UpdateSegment( _pGPS, _newLength ) ) {
      FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx ) {
      if( pGPX->Zone() == _pGPS->Zone() ) {
        pGPX->RecalcPos();
        pModel->Agent()->AddToUpdate( pGPX, UT::UPDATE );
      }
    }
    return true;
  }
  return false;
}


std::string FluxModel::GetGridPointStatsStr() const
{
  std::string str;
  char buf[2048];
  FOREACHPTRCONST( GridPointSegPtr, pGPS, pTopology->GridPointSegs() ) {
    if( !pGPS->IsUsed() )
      continue;
    ulong num = 0;
    FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx ) {
      if( pGPX->CoordsOk() && pGPX->Zone() == pGPS->Zone() )
        num++;
    }

    num++; /* # of cells = # of grid points + 1 */
    sprintf( buf, "%d%s ", (int)num, pGPS->ShortName().c_str() );
    str += buf;
  }

  return str;
}


GridPointExPtr FluxModel::FindGridPoint( int _area, double _value ) const
{
  FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx ) {
    if( pGPX->Zone() == _area && pGPX->Value() == _value )
      return pGPX;
  }
  return null;
}


int FluxModel::GetGridPointExNumber( GridPointExPtr _pGPX ) const
{
  int n = 1;
  FOREACHPTRCONST( GridPointExPtr, pGPX1, gridPointsEx ) {
    if( pGPX1->Zone() == _pGPX->Zone() &&
        pGPX1->Value() < _pGPX->Value() )
      n++;
  }

  return n;
}


void FluxModel::RemoveGridPointsByArea( int _area )
{
  IComponentList gridPointsEx_copy = gridPointsEx;
  FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx_copy ) {
    if( pGPX->Zone() == _area )
      pGPX->Delete();
  }
}


const IComponentList& FluxModel::GridPointsEx( int _zone ) const
{
  gridPointsEx_selected.clear();
  FOREACHPTRCONST( GridPointExPtr, pGPX, gridPointsEx )
    if( pGPX->Zone() == _zone )
      gridPointsEx_selected.push_back( pGPX );
  return gridPointsEx_selected;
}
