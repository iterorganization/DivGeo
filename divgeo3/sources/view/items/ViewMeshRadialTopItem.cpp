#include "ViewMeshRadialTopItem.h"

QRectF ViewMeshRadialTopItem::boundingRect() const
{
  return fullBoundingRect;
}


void ViewMeshRadialTopItem::paint( QPainter *painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  painter->setPen( penLine );
  foreach( const QLineF& line, lines )
    painter->drawLine( line );
}

void ViewMeshRadialTopItem::WritePostScriptData( QFile* )
{
  //TODO: PostScript
}

void ViewMeshRadialTopItem::UpdateGeometry()
{
  MeshPtr pMesh = dgtype_cast< MeshPtr >( pObject );

  double minX = MAXDOUBLE;
  double maxX = -MAXDOUBLE;
  FOREACHPTRCONST( MeshPointPtr, pMP, pMesh->Points() ) {
    double r = pMP->X();
    if( r < minX )
      minX = r;
    if( r > maxX )
      maxX = r;
  }

  BuildTopProjection( minX, maxX, lines, fullBoundingRect );
  UpdateComments();
}

void ViewMeshRadialTopItem::UpdateDependentGeometry()
{
  // No dependent geometry
}

void ViewMeshRadialTopItem::UpdateStyle()
{
  bool isHighlighted = pObject->IsHighlighted();
  setZValue( isHighlighted ? 1. : 0. );
  penLine = *pPrefs->visual.pens[ isHighlighted ? STYLE::HIGHLIGHT : STYLE::MESHELEMENT ];
}
