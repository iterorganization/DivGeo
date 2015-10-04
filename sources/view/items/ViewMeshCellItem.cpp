#include "ViewMeshCellItem.h"

QRectF ViewMeshCellItem::boundingRect() const
{
  return markBoundingRect;
}

void ViewMeshCellItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  const QMatrix& mx = painter->worldMatrix();
  QPointF centerT = mx.map( center );

  Point radius( centerLength * M_SQRT2 );
  markBoundingRect = QRectF( centerT - radius, centerT + radius );

  painter->setWorldMatrixEnabled( false );
  if( isMarked ) {
    painter->setPen( penMark );
    painter->drawEllipse( markBoundingRect );
  }

  radius = Point( centerLength );
  painter->setPen( penLine );
  painter->drawLine( QLineF( centerT - radius, centerT + radius ) );
  radius *= Point( -1., 1. );
  painter->drawLine( QLineF( centerT - radius, centerT + radius ) );

  painter->setWorldMatrixEnabled( true );

  // For bounding rect
  QMatrix mxInv = mx.inverted();
  markBoundingRect = mxInv.mapRect( markBoundingRect );
}

void ViewMeshCellItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewMeshCellItem::UpdateGeometry()
{
  MeshCellPtr pMC = dgtype_cast< MeshCellPtr >( pObject );
  center = _C( pMC->Center() );
  // Temporary rect for first paint
  markBoundingRect = QRectF( center - 10, center + 10 );

  UpdateComments();
}

void ViewMeshCellItem::UpdateDependentGeometry()
{
  centerLength = pPrefs->visual.meshCellCenterLength;
}

void ViewMeshCellItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  isMarked = pObject->IsMarked() && !isHighlighted;

  penMark = *pPrefs->visual.pens[ STYLE::MARK ];

  StyleIndex style = isError ? STYLE::ERROR :
                               ( isHighlighted ? STYLE::HIGHLIGHT :
                                                 STYLE::MESHCELLCENTER );
  penLine = *pPrefs->visual.pens[ style ];
}

double ViewMeshCellItem::DistToPoint( QPointF _point ) const
{
  return hypot( _point.x() - center.x(), _point.y() - center.y() );
}

bool ViewMeshCellItem::InRectangle( const QRect& _rect, const QTransform& _transform ) const
{
  QPointF center_t = _transform.map( center );
  return PointInRectangle( _C( center_t ), _C( _rect.topLeft() ), _C( _rect.bottomRight() ) );
}
