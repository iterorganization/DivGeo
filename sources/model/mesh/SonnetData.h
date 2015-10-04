#ifndef SONNETDATA_H
#define SONNETDATA_H

#include "../core/IComponent.h"

#include "../../common/Includes.h"
#include "../../common/Types.h"

#include "../../common/strlib.h"

//class Model;
//typedef Model* ModelPtr;

class SonnetData:
  public IComponent
{
  DG_OBJECT(SonnetData)

  std::string fName;
  NPoint size;
  PointArray points;

public:
  SonnetData( const std::string& _fName ): IComponent( OT::SONNET, null ) { fName = _fName; }

  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  int LoadSonnetFile();

  /* Getters */
  const std::string& FileName() const { return fName; }
  Point GetPoint( unsigned i )  const { assert( i < points.size() ); return points[i]; }
  NPoint Size()                 const { return size; }

  /* Setters */
  void SetFileName( const std::string& _fName ) { fName = _fName; }
  void FreePoints() { points.clear(); }
};

#endif
