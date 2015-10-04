#include "IViewItem.h"
#include "../ViewWnd.h"

void IViewItem::ReleaseComments()
{
  foreach( ViewCommentItemPtr pCommentItem, comments )
    pCommentItem->SetLinkedObject( null );
  comments.clear();
}

void IViewItem::UpdateComments()
{
  foreach( ViewCommentItemPtr pCommentItem, comments ) {
    pCommentItem->UpdateEndPosition();
    pCommentItem->UpdateGeometry();
    pCommentItem->UpdateDependentGeometry();
  }
}

IViewItem::~IViewItem()
{
  ReleaseObject( &pObject );
  ReleaseComments();
}

void IViewItem::BuildTopProjection( double x1, double x2, QList< QLineF >& _lines, QRectF& _boundingRect ) const
{
  /* if PHI_STEP is too small, will not draw the last segment, so increase the max by a small amount */

  double cos_phi_0 = 1., sin_phi_0 = 0.;
  _lines.clear();

  for( double phi = PHI_STEP; phi <= M_2XPI; phi += PHI_STEP ) {
    double cos_phi = cos( phi );
    double sin_phi = sin( phi );
    QPointF p1( cos_phi * x1, sin_phi * x1 );
    QPointF p2( cos_phi * x2, sin_phi * x2 );
    QPointF p1_0( cos_phi_0 * x1, sin_phi_0 * x1 );
    QPointF p2_0( cos_phi_0 * x2, sin_phi_0 * x2 );
    _lines.push_back( QLineF( p1, p2 ) );
    _lines.push_back( QLineF( p1, p1_0) );
    _lines.push_back( QLineF( p2, p2_0) );

    cos_phi_0 = cos_phi;
    sin_phi_0 = sin_phi;
  }

  double x1_abs = fabs( x1 );
  double x2_abs = fabs( x2 );
  double x_abs_max = (x1_abs > x2_abs) ? x1_abs : x2_abs;
  _boundingRect.setCoords( -x_abs_max, x_abs_max, x_abs_max, -x_abs_max );
}

void IViewItem::SetViewState(const SViewState *_pState )
{
  if( _pState == null )
    return;
  pState = _pState;
  UpdateDependentGeometry();
  UpdateStyle();
}
