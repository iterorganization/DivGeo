#ifndef VIEWOBJECTS_H
#define VIEWOBJECTS_H

#include <QPen>
#include <QFont>
#include <QStaticText>
#include <QGraphicsItem>
#include <QPainter>

#include "common/PointConverters.h"
#include "../ViewState.h"
#include "../../main/UserPreferences.h"
#include "../Modes.h"

#include "../../model/sonnet/SonnetDecl.h"
#include "../../model/flux/EquilDecl.h"
#include "../../model/diagnostics/Chords.h"

#define BOUND_DECLARE Point dg__topLeft__, dg__bottomRight__;
#define BOUND_INIT( point ) dg__topLeft__ = dg__bottomRight__ = (point);
#define BOUND_QINIT( point ) dg__topLeft__ = dg__bottomRight__ = _C(point);
#define BOUND_INIT_RECT( rect ) dg__topLeft__ = (rect).topLeft(); dg__bottomRight__ = (rect).bottomRight();
#define BOUND_POINT( point ) { if( (point).x < dg__topLeft__.x ) dg__topLeft__.x = (point).x; else \
                               if( (point).x > dg__bottomRight__.x ) dg__bottomRight__.x = (point).x; \
                               if( (point).y < dg__topLeft__.y ) dg__topLeft__.y = (point).y; else \
                               if( (point).y > dg__bottomRight__.y ) dg__bottomRight__.y = (point).y; }
#define BOUND_QPOINT( point ) { if( (point).x() < dg__topLeft__.x ) dg__topLeft__.x = (point).x(); else \
                                if( (point).x() > dg__bottomRight__.x ) dg__bottomRight__.x = (point).x(); \
                                if( (point).y() < dg__topLeft__.y ) dg__topLeft__.y = (point).y(); else \
                                if( (point).y() > dg__bottomRight__.y ) dg__bottomRight__.y = (point).y(); }
#define BOUND_RECT( rect ) (rect) = QRectF( _C( dg__topLeft__ ), _C( dg__bottomRight__ ) );

const double PHI_STEP = M_PI / 24.; /* the toroidal angle subtended by a wall segment */

const double boundOffset = 2.;
const double boundOffset2 = boundOffset * 2.;

class ViewCommentItem;
typedef ViewCommentItem* ViewCommentItemPtr;
typedef QList< ViewCommentItemPtr > ViewCommentsList;

typedef class IViewItem
{
public:
  enum EParameters {
    p_showNormals     = 0x01,
    p_showNumbers     = 0x02,
    p_showXProjection = 0x03,
    p_showZProjection = 0x04,
    p_showIrregular   = 0x05,
    p_showAll         = 0x06,
    p_showError       = 0x07
  };
  typedef EParameters ParamID;

protected:
  IComponentPtr pObject;
  const SViewState* pState;
  const UserPreferences* pPrefs;

  QRectF fullBoundingRect;

  bool isRadial;

  ViewCommentsList comments;

public:
  IViewItem( IComponentPtr _pObject ):
    pObject( DG_SHARE( _pObject ) ),
    pPrefs( null ),
    isRadial( false ) { }

  virtual ~IViewItem();

  /* Update item geometry using model's data */
  virtual void UpdateGeometry() = 0;
  /* Update item geometry using view parameters */
  virtual void UpdateDependentGeometry() = 0;
  /* Update item colors and widthes using current settings argument */
  virtual void UpdateStyle() = 0;
  /* Access to QGraphicsItem::setVisible(bool) */
  virtual void SetVisible( bool visible ) = 0;
  /* Write object geometry data to PostScript file */
  virtual void WritePostScriptData( QFile* _pFile ) = 0;

  /* Evaluate distance from item to given position */
  virtual double DistToPoint( QPointF _point ) const { UNUSED( _point ); return MAXDOUBLE; }
  /* Check if object is in view rectangle */
  virtual bool InRectangle( const QRect& _rect, const QTransform& _transform ) const { UNUSED( _rect ); UNUSED( _transform ); return false; }

  /* You also need to override the next methods of QGraphicsItem:
   * Just returns a bounding rect that was built in UpdateGeometry() or paint() methods
  virtual QRectF boundingRect() const;
   * Paint geometry and sometimes build the bounding rect
  virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );
   */

  /* Common initialization */
  void Init( ) { UpdateGeometry(); /*UpdateStyle();*/ }
  /* IValueHolder::SetValue()-like method to set boolean flags different to each item type */
  virtual void SetFlag( ParamID /*id*/, bool /*enable*/ ) {}
  /* Set container for colors, size and other parameters */
  void SetSettingsManager( const UserPreferences* _pPrefs ) { pPrefs = _pPrefs; }
  /* Set view parameters and update all view-dependent data */
  void SetViewState(const SViewState* _pState );
  /* Returns binded model object. Mostly used in tools */
  IComponentPtr ModelObject() { return pObject; }
  /* Returns isRadial flag for objects with plain and radial top-items */
  bool IsRadial() const { return isRadial; }

  /* Register linked comment */
  void IncludeComment( ViewCommentItemPtr _pCommentItem )
  { comments.push_back( _pCommentItem ); }
  /* Unregister linked comment */
  void ExcludeComment( ViewCommentItemPtr _pCommentItem )
  { comments.removeOne( _pCommentItem ); }
  /* Get registered comments */
  const ViewCommentsList& Comments() const { return comments; }
  /* Unregister all comments */
  void ReleaseComments();
  /* pdate all comments */
  void UpdateComments();

protected:
  /* Build cyclic geometry of line in top view */
  void BuildTopProjection( double x1, double x2, QList< QLineF >& _lines, QRectF &_boundingRect ) const;

}* IViewItemPtr;

typedef QList< IViewItemPtr > ViewItemsList;
typedef QSet< IViewItemPtr > ViewItemsSet;

#endif
