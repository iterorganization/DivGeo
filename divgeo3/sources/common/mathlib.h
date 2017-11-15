#ifndef _MATHLIB_H
#define _MATHLIB_H

#include "../common/Includes.h"
#include "../common/Point.h"

// Positioning

double Point2PointDist( Point _p1, Point _p2 );
double Point2VectorDist( Point _vp1, Point _vp2, Point _pnt, int* _pPosFlag, double* _pPos );

// DG3: z-coord of cross (vector) product. Used to detect p0 position relative to { p1, p2 } by angle sign.
double CrossProductZ( const Point& _crP1, const Point& _crP2, const Point& _crP0 );

int VIntersect( Point _p1, Point _p2, Point _p3, Point _p4, double* _pAr, double* _pBr );
double CalcLineLength( const PointArray& _crXY_array );

bool PointInRectangle( const Point& _crPnt, const Point& _crRect_p1, const Point& _crRect_p2 );
bool VectorInRectangle( const Point& _crVr_p1, const Point& _crVr_p2,
                        const Point& _crRect_p1, const Point& _crRect_p2 );
double AngleBetween( const Point& _crP1, const Point& _crP2 );

// PolyLines

int CompPolyLinesEx(PointArray& _rvLine1, PointArray& _rvLine2, double _maxError );
int CompPolyLines( PointArray& _rvLine1, PointArray& _rvLine2 );
int PolyLinesIntersect(const PointArray& _crvLine1, const PointArray& _crvLine2,
                       double* _pPos1, double* _pPos2 );
void CutPolyLine( PointArray* _pLine, double _pos, bool _tail );
double ProjectPointToPolyLine( const PointArray& _crvLine, const Point& _crPnt );
int GetPolyLinePoint( const PointArray& _crvLine, double _length, double* _pX, double* _pY );
double PointToPolyLineDist( const PointArray& _crvLine, const Point& _crPnt );

#endif
