#ifndef EQUIL_H
#define EQUIL_H

#include "../core/IComponent.h"
#include "../core/ListContainer.h"
#include "../core/Actions.h"
#include "../core/CoreConstants.h"

#include "../../common/Includes.h"
#include "../../common/strlib.h"
#include "../../common/Spline.h"

#include "CreatorId.h"
#include "EquilDecl.h"

using namespace Actions;

class Model;

#define CSF_YP    0x1
#define CSF_YM    0x2
#define CSF_XM    0x4
#define CSF_XP    0x8

#define CS_YP    0
#define CS_XP    1
#define CS_YM    2
#define CS_XM    3

typedef struct EquilBorderSort
{
  Point p;
  double b;
  EquilBorderSort(): b( 0. ) { }
  EquilBorderSort( Point _p, double _b ): p( _p ), b( _b ) { }
}* EquilBorderSortPtr;

bool EquilBorderCompare( const EquilBorderSort b1, const EquilBorderSort b2 );

typedef struct SurfCell
{
  double x[4],y[4];
  int n,f;
  int d[4];
}* SurfCellPtr;

class Equil: public IComponent
{
  DG_OBJECT(Equil)

  std::string fileName;
  std::string sender_name;

  NPoint size;
  int signInside;

  double* x,*y,*val;
  double minVal,maxVal;

  SimpleSplineArray hSplines;
  SimpleSplineArray vSplines;
  SurfaceSplinePtr pSurface;

  mutable NPoint cacheSize;
  mutable double* cache;


  /*****************************************************************************
   * Constructors and common interface
   *****************************************************************************/
public:
  Equil( ModelPtr _pModel, const std::string& _fName );
  ~Equil();

  void Release();
  void FreeArrays();

  void AllocCache( NPoint _size ); // Unused
  void FreeCache();

  /*****************************************************************************
   * IComponent implementation
   *****************************************************************************/
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;
  virtual void Delete();

  /*****************************************************************************
   * IObject implementation
   *****************************************************************************/
  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /*****************************************************************************
   * Read and write
   *****************************************************************************/

public: /* Read stack */
  int Load();
private:
  int LoadDgFile();
  int Load1File();

private:
  int WriteDg( const std::string& _fName, NPoint _size ) const; // Unused
  int WriteSpline( const std::string& _fName, NPoint _size ) const; // Unused. Except strange AAmain in 2.1

  /*****************************************************************************
   * Common tools and spline wrappers
   *****************************************************************************/
public: /* Surface line calculation stack */
  int             CalcSurfaceLine( NPoint _index, double level, std::vector< Point >* pXY_array, NPoint _size ) const;
private:
  void            CalcSurfData( NPoint _index, double level, SurfCellPtr pSC, NPoint _size ) const;

public:
  PointArray*     CalcGradientLine( NPoint _p0, int steps, int bMinMax ) const; // Used in CalcXPointGradients() only
  void            CalcValues(); // Used in ActAddEquil::Do() only
  int             CalcGradient( Point _pnt, double* pGrad ) const;
  int             CalcSplines();
  int             FreeSplines();
  int             GetLevel( Point _p, double* pLevel, Point* pGrad ) const;
  double          XYToBorder( Point pnt ) const; // Used in RecalcTopologyCache() only


 /******************************************************************************
   * Macroes
   *****************************************************************************/
public:
  bool In( const Point& xy0, const NPoint& _index1, const NPoint& _index2 ) const;
  bool InS( const Point& xy0, const NPoint& _index1, const NPoint& _index2 ) const;
  
  double AverageCellWidth() const { return fabs( x[size.x - 1] - x[0] ) / size.x; }
  double AverageCellHeight() const { return fabs( y[size.y - 1] - y[0] ) / size.y; }
  bool SplinesAreOk() const { return ( !hSplines.empty() && !vSplines.empty() && pSurface != NULL ); }

  Point CenterOffset( NPoint _p0, NPoint _p1, double k ) const; // TODO:VERSION function name

  /*****************************************************************************
   * Getters and Setters
   *****************************************************************************/
public: /* Getters */
  const std::string& FileName() const { return fileName; }
  NPoint Size() const { return size; }
  double MinValue() const { return minVal; }
  double MaxValue() const { return maxVal; }

  Point GetPoint( const NPoint& _index ) const { return Point( x[_index.x], y[_index.y] ); }
  Point PointFirst() const { return Point( x[0], y[0] ); }
  Point PointLast() const { return Point( x[size.x - 1], y[size.y - 1] ); }

  double Cell( NPoint _index ) const;
  double CellEx( NPoint _index, NPoint _size ) const;
  double CorrCell( NPoint _index, double level ) const;
private:
  double CorrCellEx( NPoint _index, double level, NPoint _size ) const;

public:
  int Cell( Point _pnt, NPoint* _pIndex ) const;
  int CellEx( Point _pnt, NPoint* _pIndex, NPoint _size ) const;

public: /* Setters */
  void SetFileName( const std::string& _fName ) { fileName = _fName; }

};

#endif
