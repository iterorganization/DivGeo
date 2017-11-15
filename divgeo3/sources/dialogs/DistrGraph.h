#ifndef DISTRGRAPH_H
#define DISTRGRAPH_H

//#include <QWidget>
#include <QPen>
#include <QPainter>
#include <QVector>
#include <QMouseEvent>
#include <QStyleOption>

#include "../model/flux/CreatorId.h"

class DistrGraph : public QWidget
{
  Q_OBJECT

  /* Parameters */
  QPen penRect;
  QPen penCurve;
  QPen penGraph;

  ulong margin;
  ulong extraLeftMargin;
  ulong step; /*2.1*/
  double dstep; /*3.0*/

  /* Variables */
  DistrLaw law;
  double* alpha; // array of [2]
  ulong count;
  QPointF g[2]; // for delta law

  /* Widget variables */
  QSize grSize;

  bool toUpdate;

  /* Ready data */
  QVector< QPointF >  curve_data;
  QVector< QLine >    curve;
  QVector< QPointF >  drag_curve_data;
  QVector< QLine >    drag_curve;
  QVector< QPointF >  graph_data;
  QVector< QLine >    graph;

  /* Drag support */
  bool inDrag;        // false
  long drag_index;    // -1
  double drag_alpha;  // 0.

public:
  explicit DistrGraph(QWidget *parent = 0);
  ~DistrGraph();
  virtual void paintEvent( QPaintEvent* pe );
  virtual void mousePressEvent( QMouseEvent* pe );
  virtual void mouseReleaseEvent( QMouseEvent* pe );
  virtual void mouseMoveEvent( QMouseEvent* pe );
  virtual void resizeEvent( QResizeEvent* pe );

  void RebuildData();
  void RebuildScene();

  /* Calculate new delta-coeffs (delta is array of 2 doubles).
     Method changes g[2] and temporary changes count. */
  void RecalcDeltas( ulong newCount, double delta[2] );

private:
  /* Try to find alpha-coeff for current distribution law and point.
     alpha_index - index of adjusted alpha,
     alpha_value - value of constant alpha (another alpha).
     Returns MAXDOUBLE if impossible. */
  double CalcCoeff( const QPointF& pnt, long adjusted_alpha_index ) const;
  /* Call distribution function.
     alpha_index - index of selected alpha,
     alpha_value - value of selected alpha (same alpha).
     if alpha_index == -1 then member alphas will be used */
  double DistrFunc( double x, long alpha_index = -1, double alpha_value = 0. ) const;

public:

  DistrLaw Law() { return law; }
  /* Enable data and scene rebuild and repaint for changer slots */
  void EnableUpdate( bool en ) { toUpdate = en; }
  
signals:

  void signalChangedAlpha0( double _alpha );
  void signalChangedAlpha1( double _alpha );
  
public slots:

  void slotChangeLaw( int _law );
  void slotChangeAlpha0( double _alpha );
  void slotChangeAlpha1( double _alpha );
  void slotChangeCount( ulong _count ); // Use with RecalcDeltas for delta law
  
};

#endif // DISTRGRAPH_H
