#ifndef ITOOLDATA_H
#define ITOOLDATA_H

#include "../common/Includes.h"
#include "../model/core/Model.h"
#include <QList>
#include "items/IViewItem.h"
#include "items/ViewShapeItem.h"

typedef class ToolBase
{
public:
  bool isShifted;
  bool isMoved;
  bool isPossible;
  IViewItemPtr pItem;
  Point position;

  ToolBase( bool _isShifted = false ):
    isShifted( _isShifted ),
    isMoved( false ),
    isPossible( false ),
    pItem( null ) { }

  ToolBase( const Point& _position, bool _isShifted = false ):
    isShifted( _isShifted ),
    isMoved( false ),
    isPossible( false ),
    pItem( null ),
    position( _position ) { }

  void SetMoved( bool _isMoved = true ) { isMoved = _isMoved; }
}* IToolDataPtr;


typedef struct MeshPointNeighbour
{
  MeshPointPtr pPoint;
  double oldPos;
  Point oldPnt;
  bool isMoved;
}* MeshPointNeighbourPtr;

typedef std::vector< MeshPointNeighbour > MeshPointNeighbourArray;
typedef MeshPointNeighbourArray::iterator MeshPointNeighbourIter;

typedef struct ToolMoveMeshPoint:
  public ToolBase
{
  ViewItemsSet group;

  MeshPointNeighbourArray pts, ptsA;
  PointArray line;
  ParamSplinePtr pSpline;

  int ptsMin, ptsMax, ptsCount, ptsPtNo/*, bDoubleLeft, bDoubleRight*/;
  double posMin, posMax, leftMinDist, rightMinDist;

public:
  ToolMoveMeshPoint( bool _isShifted = false ):
      ToolBase( _isShifted ),
      pSpline( null ),
      ptsMin( 0 ), ptsMax( 0 ), ptsCount( 0 ), ptsPtNo( 0 ),
      posMin( 0. ), posMax( 0. ), leftMinDist( 0. ), rightMinDist( 0. ) { }
}* ToolMoveMeshPointPtr;



typedef struct ToolTransformView:
  public ToolBase
{
  Point pos, center;
  double angle;

  ToolTransformView( const Point& _position, bool _isShifted = false ):
    ToolBase( _isShifted ),
    pos( _position ) {}
}* ToolTransformViewPtr;



typedef struct ToolMark:
  public ToolBase
{
  Point pos;
  IViewItemPtr pFirstItem;
  ViewItemsList items;
  bool include;
  ulong flags;

  ToolMark( const Point& _position, bool _isShifted = false ):
    ToolBase( _isShifted ),
    pos( _position ),
    pFirstItem( null ),
    include( false ),
    flags( 0 ) {}
}* ToolMarkPtr;


typedef struct ToolAddElement:
  public ToolBase
{
  IViewItemPtr pNodeFirstItem;
  IViewItemPtr pNodeSecondItem;
  IViewItemPtr pElementItem;
  IViewItemPtr pElementTopItem;

  ToolAddElement( bool _isShifted = false ):
    ToolBase( _isShifted ),
    pNodeFirstItem( null ),
    pNodeSecondItem( null ),
    pElementItem( null ),
    pElementTopItem( null ) {}
}* ToolAddElementPtr;


typedef struct ToolAddChord:
  public ToolBase
{
  IViewItemPtr pItem2;

  ToolAddChord( const Point &_position ):
    ToolBase( _position ),
    pItem2( null ) {}
}* ToolAddChordPtr;


typedef struct ToolMove:
  public ToolBase
{
  ViewItemsList group;

  /* Chords special */
  int posFlag;
  double len;

  ToolMove( const Point& _position, bool _isShifted = false ):
    ToolBase( _position, _isShifted ),
    posFlag( 0 ),
    len( 0. ) {}

}* ToolMovePtr;



typedef struct ToolSplit:
  public ToolBase
{
  IViewItemPtr pElementItemO;
  IViewItemPtr pElementItemA;
  IViewItemPtr pElementItemB;
  IViewItemPtr pNodeItem;

  ToolSplit():
    ToolBase(),
    pElementItemO( null ),
    pElementItemA( null ),
    pElementItemB( null ),
    pNodeItem( null ) {}

}* ToolSplitPtr;


typedef struct ToolJoin:
  public ToolBase
{
  IViewItemPtr pNodeItem;
  ViewItemsList items;

  ToolJoin():
    ToolBase( false ),
    pNodeItem( null ) {}

}* ToolJoinPtr;


typedef struct ToolConnect:
  public ToolBase
{
  IViewItemPtr pElementItem;
  IViewItemPtr pNodeItemA;
  IViewItemPtr pNodeItemB;

  ToolConnect():
    ToolBase(),
    pElementItem( null ),
    pNodeItemA( null ),
    pNodeItemB( null ) {}

}* ToolConnectPtr;


typedef struct ToolReverse:
  public ToolBase
{
  ViewItemsList items;
  IComponentList objects;

  ToolReverse( bool _isShifted = false ):
    ToolBase( _isShifted ) {}

}* ToolReversePtr;


typedef struct ToolAdjust:
  public ToolBase
{
  double len;
  int posFlag;

  ToolAdjust( const Point& _position ):
    ToolBase( _position ),
    len( 0. ),
    posFlag( 0 ) {}

}* ToolAdjustPtr;


typedef struct ToolReposition:
  public ToolBase
{
  Point pt;
  Point p0;

  ViewShapeItemPtr pPointItem;
  ViewShapeItemPtr pLineItem;

  IViewItemPtr pElementItem;
  IViewItemPtr pNodeItem_first;
  IViewItemPtr pSeparatorItem;
  IViewItemPtr pNodeItem_second;

  int posFlag;

  ToolReposition( bool _isShifted = false ):
    ToolBase( _isShifted ),
    pPointItem( null ),
    pLineItem( null ),
    pElementItem( null ),
    pNodeItem_first( null ),
    pSeparatorItem( null ),
    pNodeItem_second( null ),
    posFlag( 0 ) {}

}* ToolRepositionPtr;


typedef struct ToolStretch:
  public ToolBase
{
  bool horizontal;
  bool vertical;
  Point oldScale;
  QRect hr;
  ToolStretch( const Point& _scale, const Point& _position,
               bool _isShifted = false ):
    ToolBase( _position, _isShifted ),
    horizontal( false ),
    vertical( false ),
    oldScale( _scale ) {}
}* ToolStretchPtr;


typedef struct ToolComment:
  public ToolBase
{
  IViewItemPtr pLinkedItem;
  bool newComment;
  ToolComment( const Point& _position, bool _isShifted = false ):
    ToolBase( _position, _isShifted ),
    pLinkedItem( null ),
    newComment( true ) {}
}* ToolCommentPtr;

#endif
