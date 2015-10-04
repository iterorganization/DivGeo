#ifndef XPOINTSEG_H
#define XPOINTSEG_H

#include "../core/IComponent.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/mathlib.h"

#include "EquilDecl.h"

class XPointSeg:
  public IComponent
{
  DG_OBJECT(XPointSeg)

  XPointTestPtr pXPT;
  int number;
  double startPos,endPos;
  double lineLength;
  PointArray line;

  /* Equilibrium mesh parameters */
  double value_y_delta;
  double average_cell_size;

public:
  XPointSeg( ModelPtr _pModel, XPointTestPtr _pXPT, int _number, double _sp, double _ep );
  ~XPointSeg();

  /*****************************************************************************
   * IComponent implementation
   *****************************************************************************/

  virtual void Delete();
  virtual double DistToPoint( const Point& _crPnt, int* _pPosFlag ) const;
  virtual Point CentralPoint() const { return line.at( line.size()/2 ); }

  /*****************************************************************************
   * IObject implementation
   *****************************************************************************/

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /*****************************************************************************
   * Calculation
   *****************************************************************************/

  void Register();
  int CalcLine();
  void FreeLine() { line.clear(); lineLength = 0; }
  double CalcAngle( XPointTestPtr _pXPT = null ) const;

  void ExcludeFromXPT();

  void ChangeLineLength( double _length ) { lineLength = _length; }
  /* This method cause dependence xpt from equil. Call it after equil or topology changed */
  void SetEquilParams( double _value_y_delta, double _average_cell_size ) {
    value_y_delta = _value_y_delta; average_cell_size = _average_cell_size; }

  /*****************************************************************************
   * Getters
   *****************************************************************************/

  int Number()        const { return number; }
  double LineLength() const { return lineLength; }
  double StartPos()   const { return startPos; }
  double EndPos()     const { return endPos; }

  double GetXptLevel()  const;
  XPointTestPtr XPT()   const { return pXPT; }
  PointArray* Line()          { return &line; }
  
};

#endif
