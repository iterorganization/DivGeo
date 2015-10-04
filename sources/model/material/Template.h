#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "MaterialDecl.h"

typedef struct OgrNode
{
  int id;
  double x,y;
  OgrNode( int _id, double _x, double _y ):
  id( _id ), x( _x ), y( _y ) { }
}* OgrNodePtr;
typedef std::list< OgrNodePtr > OgrNodeList;
typedef OgrNodeList::iterator OgrNodeIter;

struct TemplateParams
{
  Point incr;
  double angle, scale;

  TemplateParams( Point _incr, double _angle, double _scale ):
    incr( _incr ),
    angle( _angle ),
    scale( _scale ) { }

  void operator = ( const TemplateParams& _tp ) 
  { incr = _tp.incr; angle = _tp.angle; scale = _tp.scale; }

  bool operator == ( const TemplateParams& _tp )
  { return incr == _tp.incr && angle == _tp.angle && scale == _tp.scale; }
};

class Template:
  public IComponent,
  public IValueHolder
{
  DG_OBJECT(Template)
  DG_PROPERTIES
  { 
    p_drag_status = 0,
    p_drag_value  = 1
  };

  DG_PROPERTIES_TABLE
  {
    AddProperty( "drag_status", &dragStatus );
    AddProperty( "drag_value", &drag );
  }

private:
  std::string sender_name;
  std::string fileName;
  Point incr;
  double angle, scale;

  int dragStatus;
  Point drag;

  PointArray points;
  PointIter itPoint_current;
  
public:
  Template( ModelPtr _pModel, const std::string& _fName, const TemplateParams& _tp );
  void Change( const TemplateParams& _tp );
  virtual void Delete();
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  int LoadDgTemplateFile();
  int LoadOgrTemplateFile();
  int LoadHpglTemplateFile();

  int Load2ColumnsTemplateFile();

  Point TransformPoint( Point _pnt ) const; // rotate and shift
  void Points_Reset() { itPoint_current = points.begin(); }
  bool Points_End() { return itPoint_current == points.end(); }
  Point Points_Next( bool bTransformed = false );

  int GetNearestPoint( const Point& pos, Point* pt, Point* ps ) const;
  int PlaceByHandles( const Point& pa, const Point& p1, const Point& pb, const Point& p2 );

  /* Setters */
  void SetDragStatus( int _dragStatus, Point _drag );
  void SetParameters( TemplateParams& _tp ) { incr = _tp.incr; angle = _tp.angle; scale = _tp.scale; }
  void SetFileName( const std::string& _fName ) { fileName = _fName; }
  void FreePoints() { points.clear(); }

  /* Getters */
  const std::string& FileName()   const { return fileName; }
  TemplateParams GetParameters()  const { return TemplateParams( incr, angle, scale ); }
  const   Point& Incr()           const { return incr; }
  double  Angle()                 const { return angle; }
  double  Scale()                 const { return scale; }
  int     PointsNum()             const { return points.size(); }
  int     DragStatus()            const { return dragStatus; }
  Point   DragPoint()             const { return drag; }
};


#endif
