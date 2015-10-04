#include "ViewCommentItem.h"

QRectF ViewCommentItem::boundingRect() const
{
  return fullBoundingRect;
}

void ViewCommentItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget )
{
  UNUSED(item);
  UNUSED(widget);

  QLineF line = QLineFromPoints( pos_end, position );

  painter->setPen( pen );
  painter->drawLine( line );

  QFontMetricsF fm = QFontMetricsF( font );
  textRect = fm.boundingRect( QRectF( 0., 0., 1000., 1000. ), Qt::TextWordWrap, sText );

  QPointF pos = _C( position );

  const QMatrix& mx = painter->worldMatrix();
  QPointF posT = mx.map( pos );
  posT -= textRect.center();

  QMatrix mxR;
  mxR.rotate( pState->xyAngle*RAD2DEG );
  QPointF posT1 = mxR.map( pos );
  Point offset = Point( -5., 5. ) + Point(-1., 1.) * _C( textRect.center() );
  offset /= pState->scale;
  posT1 += _C( offset );

  QMatrix mxR2;
  mxR2.rotate( -pState->xyAngle*RAD2DEG );
  mxR2.translate( posT1.x(), posT1.y() );
  mxR2.scale( 1./pState->scale.x, -1./pState->scale.y );
  poly = mxR2.map( textRect );

  painter->setBrush( QBrush( pPrefs->visual.background_color ) );

  QRectF rectExt( QPointF( 0., 0. ), textRect.size() + QSize( 10., 10. ) );
  poly = mxR2.map( rectExt );

  painter->drawPolygon( poly );

  painter->setFont( font );
  painter->setWorldMatrixEnabled( false );
  painter->drawText( posT.x(), posT.y(), textRect.width(), textRect.height(),
                     Qt::TextWordWrap, sText );
  //painter->drawText( posT, sText );
  painter->setWorldMatrixEnabled( true );

  BOUND_DECLARE;
  BOUND_INIT( pos_end );
  foreach( QPointF pnt, poly )
    BOUND_QPOINT( pnt );
  BOUND_RECT( fullBoundingRect );

  //painter->drawRect( boundingRect() );
}

void ViewCommentItem::WritePostScriptData( QFile* _pFile )
{
  UNUSED( _pFile ); //TODO: PostScript
}

void ViewCommentItem::UpdateGeometry()
{
  CommentPtr pComment = dgtype_cast< CommentPtr >( pObject );
  position = pComment->Position();
  pos_end = pComment->PosEnd();
  sText = QString::fromStdWString( pComment->Text() );
  if( pLinkedObject != null ) {
    pos_end = pLinkedObject->CentralPoint();
    pComment->ChangePosEnd( pos_end );
    /*sText = QString::fromStdString( pLinkedObject->DetailedInfo() ) +
            "\n" + sText;*/
  }

  UpdateComments();
}

void ViewCommentItem::UpdateDependentGeometry()
{
  QFontMetricsF fm = QFontMetricsF( font );
  textRect = fm.boundingRect( sText );
  textRect.moveCenter( _C( position ) );

  BOUND_DECLARE;
  BOUND_INIT( pos_end );
  BOUND_QPOINT( textRect.topLeft() );
  BOUND_QPOINT( textRect.bottomRight() );
  BOUND_RECT( fullBoundingRect );
}

void ViewCommentItem::UpdateStyle()
{
  setZValue( 2. );
  pen = *pPrefs->visual.pens[ STYLE::COMMENT ];
  font = QFont( "Arial", pPrefs->visual.commentTextSize );
}

double ViewCommentItem::DistToPoint( QPointF _point ) const
{
  if( poly.containsPoint( _point, Qt::OddEvenFill ) )
    return 0.;
  else
    return MAXDOUBLE;
}

void ViewCommentItem::SetLinkedObject( IComponentPtr _pLinkedObject )
{
  ReleaseObject( &pLinkedObject );
  pLinkedObject = DG_SHARE( _pLinkedObject );

  UpdateGeometry();
  UpdateDependentGeometry();
}
