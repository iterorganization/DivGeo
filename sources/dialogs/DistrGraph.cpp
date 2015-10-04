#include "DistrGraph.h"

DistrGraph::DistrGraph( QWidget* parent ) :
  QWidget( parent ),
  margin( 10 ),
  extraLeftMargin( 10 ),
  step( 3 ),
  dstep( 0.025 ),
  law( DGLAW::DELTA ),
  count( 0 ),
  toUpdate( false ),
  inDrag( false ),
  drag_index( -1 ),
  drag_alpha( 0. )
{
  penRect =   QPen( QColor::fromRgb( 128, 128, 128 ) );
  penCurve =  QPen( QColor::fromRgb( 0, 0, 255 ) );
  penGraph =  QPen( QColor::fromRgb( 255, 0, 0 ) );

  alpha = new double[2];
  alpha[0] = alpha[1] = 0.;

  g[0] = g[1] = QPoint( -1., -1. );

  //setMouseTracking( true );

  setStyleSheet( "background-color:white;" );

  RebuildData();
  RebuildScene();
  update();
}

DistrGraph::~DistrGraph()
{
  delete[] alpha;
}

void DistrGraph::paintEvent( QPaintEvent* pe )
{
  UNUSED( pe );
  QPainter p( this );

  QStyleOption opt;
  opt.init( this );
  style()->drawPrimitive( QStyle::PE_Widget, &opt, &p, this );

  /* Rect */
  p.setPen( penRect );
  p.drawRect( margin, margin,
              width() - 2. * margin, height() - 2. * margin );
  p.drawLine( margin + extraLeftMargin, margin,
              margin + extraLeftMargin, height() - margin );

  /* Curve */
  p.setPen( penCurve );
  p.drawLines( inDrag ? drag_curve : curve );

  /* Graph */
  if( !inDrag ) {
    p.setPen( penGraph );
    p.drawLines( graph );
  }
}

void DistrGraph::resizeEvent( QResizeEvent* pe )
{
  UNUSED( pe );
  grSize = QSize( width() - 2. * margin - extraLeftMargin,
                  height() - 2. * margin );

  RebuildScene();
  update();
}

void DistrGraph::RebuildData()
{
  double& ref_alpha0 = (drag_index == 0) ? drag_alpha : alpha[0];

  /* Curve */
  QVector< QPointF >& ref_curve_data = inDrag ? drag_curve_data : curve_data;
  if( ref_alpha0 <= 0 )
    return;

  ref_curve_data.clear();
  for( double x = 0.; x < 1. + dstep; x += dstep ) {
    if( x > 1. )
      x = 1.;
    double f = DistrFunc( x, drag_index, drag_alpha );
    ref_curve_data.append( QPointF( x, f ) );
  }

  /* Graph */
  if( !inDrag )
    graph_data.clear();
  for( ulong t = 0; t < count; t++ ) {
    double x = 1. / (count + 1) * (t + 1);
    double f = 0.;
    if( ref_alpha0 > 0 )
      f = DistrFunc( x, drag_index, drag_alpha );
    if( !inDrag )
      graph_data.append( QPointF( x, f ) );
    QVector< QPointF >::iterator it = ref_curve_data.begin(),
                              itEnd = ref_curve_data.end();
    while( it != itEnd && it->x() < x )
      ++it;
    ref_curve_data.insert( it, QPointF( x, f ) );
  }
}

#define FROMDATAPOINT( dataPoint ) QPoint( \
  margin + extraLeftMargin + (long)(grSize.width() * dataPoint.x()), \
  margin + (long)(grSize.height() * (1 - dataPoint.y())) )

void DistrGraph::RebuildScene()
{
  /* Curve */
  QVector< QLine >& ref_curve = inDrag ? drag_curve : curve;
  QVector< QPointF >& ref_curve_data = inDrag ? drag_curve_data : curve_data;
  ref_curve.clear();
  if( !ref_curve_data.isEmpty() ) {
    QPoint point_prev = FROMDATAPOINT( ref_curve_data[0] );
    for( ulong c = 1; c < (ulong)ref_curve_data.size(); c++ ) {
      QPoint point = FROMDATAPOINT( ref_curve_data[c] );
      ref_curve.append( QLine( point_prev, point ) );
      point_prev = point;
    }
  }

  if( inDrag )
    return;

  /* Graph */
  graph.clear();
  for( ulong c = 0; c < (ulong)graph_data.size(); c++ ) {
    QPoint point = FROMDATAPOINT( graph_data[c] );
    if( alpha[0] <= 0 )
      graph.append( QLine( QPoint( point.x(), margin ),
                           QPoint( point.x(), height() - margin ) ) );
    else {
      graph.append( QLine( point, QPoint( point.x(), height() - margin ) ) );
      if( law == DGLAW::FLIPPED ) {
        graph.append( QLine( QPoint( margin, point.y() ),
                             QPoint( margin + extraLeftMargin, point.y() ) ) );
        graph.append( QLine( point, QPoint( width() - margin, point.y() ) ) );
      }
      else
        graph.append( QLine( QPoint( margin, point.y() ), point ) );
    }
  }
}

#undef FROMDATAPOINT

void DistrGraph::slotChangeLaw( int _law )
{
  law = (DistrLaw)_law;
  if( toUpdate ) {
    g[0].setX( -1 );
    g[1].setX( -1 );
    RebuildData();
    RebuildScene();
    update();
  }
}

void DistrGraph::slotChangeAlpha0( double _alpha )
{
  alpha[0] = _alpha;
  if( toUpdate ) {
    g[0].setX( -1 );
    g[1].setX( -1 );
    RebuildData();
    RebuildScene();
    update();
  }
}

void DistrGraph::slotChangeAlpha1( double _alpha )
{
  alpha[1] = _alpha;
  if( toUpdate ) {
    g[0].setX( -1 );
    g[1].setX( -1 );
    RebuildData();
    RebuildScene();
    update();
  }
}

void DistrGraph::RecalcDeltas(ulong newCount, double delta[2] )
{
  const double reset_delta = 1e-5;
  const ulong curve_fit_iters = 10;

  /* Recalculate deltas if law is Delta and # of points changed */
  if( count != newCount-1 && law == DGLAW::DELTA ) {
    /* Get 2 points of the "old" graph */
    if( g[0].x() < 0. ) {
      g[0].setX( 1./3. );
      g[0].setY( DistrFunc( g[0].x() ) );
      g[1].setX( 2./3. );
      g[1].setY( DistrFunc( g[1].x() ) );
    }

    ulong count_old = count;
    count = newCount-1;

    double* alpha_old = alpha;
    alpha = delta;

    /* Fit the "new" graph as closely as possible to match the old points */
    alpha[0] = alpha[1] = reset_delta;
    int b = 3;
    for( ulong i = 0; i < curve_fit_iters; i++ ) {
      double t = CalcCoeff( g[0], 0 );
      if( t != MAXDOUBLE ) {
        alpha[0] = t;
        b &= ~1;
      }
      else
        b |= 1;

      t = CalcCoeff( g[1], 1 );
      if( t != MAXDOUBLE ) {
        alpha[1] = t;
        b &= ~2;
      }
      else
        b |= 2;
    }
    if( b == 0 && count > 1 ) {
      g[0].setX( -1 );
      g[1].setX( -1 );
    }

    alpha = alpha_old;
    count = count_old;
  }
}

void DistrGraph::slotChangeCount( ulong _count )
{
  if( law != DGLAW::DELTA ||
      count == _count - 1 ) {
    g[0].setX( -1 );
    g[1].setX( -1 );
  }

  count = _count - 1;
  if( toUpdate ) {
    RebuildData();
    RebuildScene();
    update();
  }
}

#define FROMWIDGETPOINT( wgtPoint ) QPointF( \
  (double)(wgtPoint.x() - margin - extraLeftMargin) / (double)grSize.width(), \
  1 - (double)(wgtPoint.y() - margin ) / (double)grSize.height() )


void DistrGraph::mousePressEvent( QMouseEvent* pe )
{
  if( pe->button() != Qt::LeftButton )
    return;

  QPointF pressPos = FROMWIDGETPOINT( pe->pos() );

  drag_index = ( (law == DGLAW::TWOALPHA || law == DGLAW::DELTA) &&
                 pressPos.x() > 0.5 ) ? 1 : 0;
  drag_alpha = alpha[ drag_index ];
  inDrag = true;
}

void DistrGraph::mouseMoveEvent( QMouseEvent* pe )
{
  if( !inDrag )
    return;

  QPointF pressPos = FROMWIDGETPOINT( pe->pos() );
  if( pressPos.x() >= 0 && pressPos.x() <= 1 &&
      pressPos.y() >= 0 && pressPos.y() <= 1 ) {
    drag_alpha = CalcCoeff( pressPos, drag_index );
    if( drag_alpha != MAXDOUBLE ) {
      if( drag_index == 0 )
        emit signalChangedAlpha0( drag_alpha );
      else
        emit signalChangedAlpha1( drag_alpha );
      RebuildData();
      RebuildScene();
    }
  }

  update();
}

void DistrGraph::mouseReleaseEvent( QMouseEvent* pe )
{
  UNUSED( pe );
  if( !inDrag )
    return;

  if( drag_alpha != MAXDOUBLE )
    alpha[ drag_index ] = drag_alpha;

  inDrag = false;
  drag_index = -1;

  RebuildData();
  RebuildScene();
  update();
}

#undef FROMWIDGETPOINT

double DistrGraph::DistrFunc(double x, long alpha_index, double alpha_value ) const
{
  if( alpha_index == 0 )
    return CreatorId( count, alpha_value, alpha[1], law ).CalcValue( x );
  else if( alpha_index == 1 )
    return CreatorId( count, alpha[0], alpha_value, law ).CalcValue( x );
  else
    return CreatorId( count, alpha[0], alpha[1], law ).CalcValue( x );
}


double DistrGraph::CalcCoeff(const QPointF& pnt, long adjusted_alpha_index ) const
{
  const double lower_common = 1e-10;
  const double upper_alpha = 1e10;
  const double upper_delta = 0.5;
  const ulong max_iter = 1000; /* Emergency break */

  double lower = lower_common;
  double upper = (law == DGLAW::DELTA ) ? upper_delta : upper_alpha;

  double v_lw = DistrFunc( pnt.x(), adjusted_alpha_index, lower );
  double v_up = DistrFunc( pnt.x(), adjusted_alpha_index, upper );

  if( (v_lw - pnt.y()) * (v_up - pnt.y()) >= 0 )
    return MAXDOUBLE;

  double middle = 0.;
  for( ulong i = 0; i < max_iter; i++ ) {
    middle = (upper + lower) / 2;
    if( middle == upper || middle == lower )
      break;
    double v_md = DistrFunc( pnt.x(), adjusted_alpha_index, middle );

    if( (v_md - pnt.y()) * (v_lw - pnt.y()) == 0 )
      break;
    double v_test = (v_md - pnt.y()) * (v_up - pnt.y());
    if( v_test == 0 )
      break;
    else if( v_test < 0 ) {
      lower = middle;
      v_lw = v_md;
    }
    else {
      upper = middle;
      v_up = v_md;
    }
  }
  return middle;
}
