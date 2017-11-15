#include "ViewMeshElementItem.h"

QRectF ViewMeshElementItem::boundingRect() const
{
  return QRectF( dg_min( line.x1(), line.x2() ) - boundOffset, dg_min( line.y1(), line.y2() ) - boundOffset,
    fabs( line.x1() - line.x2() ) + boundOffset2, fabs( line.y1() - line.y2() ) + boundOffset2 );
}


void ViewMeshElementItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  if( isMarked ) {
    painter->setPen( penMark );
    painter->drawLine( line );
  }

  painter->setPen( penLine );
  painter->drawLine( line );
}

void ViewMeshElementItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}


void ViewMeshElementItem::UpdateGeometry()
{
  MeshElementPtr pME = dgtype_cast< MeshElementPtr >( pObject );
  line = QLineFromPoints( pME->MPoint(0)->Position(), pME->MPoint(1)->Position() );
  UpdateComments();
}

void ViewMeshElementItem::UpdateDependentGeometry()
{
  // No dependent geometry
}


void ViewMeshElementItem::UpdateStyle()
{
  MeshElementPtr pME = dgtype_cast< MeshElementPtr >( pObject );
  /* Check for neighboring irregular cells */
  bool isRegular = ( pME->Cell(0) == null || pME->Cell(0)->IsIrregular() == 0 ) &&
                   ( pME->Cell(1) == null || pME->Cell(1)->IsIrregular() == 0 );
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );

  isMarked = pObject->IsMarked() && !pObject->IsHighlighted();
  penMark = *pPrefs->visual.pens[ STYLE::MARK ];

  StyleIndex style = STYLE::EMPTY;

  if( isRegular ) {
    if( isError )             style = STYLE::ERROR;
    else if( isHighlighted )  style = STYLE::HIGHLIGHT;
    else                      style = STYLE::MESHELEMENT;
  }
  else { /* irregular */
    if( isError )             style = STYLE::MESHELEM_IRREG_ERR;
    else if( isHighlighted )  style = STYLE::MESHELEM_IRREG_HL;
    else                      style = STYLE::MESHELEM_IRREG;
  }

  penLine = *pPrefs->visual.pens[ style ];
}


double ViewMeshElementItem::DistToPoint( QPointF _point ) const
{
  /*Point central_dist = _C( (line.p1() + line.p2()) / 2. - _point );
  return hypot( central_dist );*/
  return Point2VectorDist( _C( line.p1() ), _C( line.p2() ), _C( _point ), null, null );
}

bool ViewMeshElementItem::InRectangle( const QRect& _rect, const QTransform& _transform ) const
{
  QLineF line_t = _transform.map( line );
  return VectorInRectangle( _C( line_t.p1() ), _C( line_t.p2() ), _C( _rect.topLeft() ), _C( _rect.bottomRight() ) );
}
