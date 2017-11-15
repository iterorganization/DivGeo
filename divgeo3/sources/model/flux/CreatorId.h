#ifndef CREATORID_H
#define CREATORID_H

#include <stdio.h>
#include <cstring>
#include <string>

#include "../../common/Constants.h"
#include "../../common/Macroes.h"
#include "../../common/mathlib.h"
#define CID_UNCHANGEDFLAG '+'

struct SDistrLaws {
  enum EDistrLaws {
    NORMAL    = 0,  /* !!!!! Warning !!!!!                    */
    FLIPPED   = 1,  /* As part of Creator ID, the law #       */
    TWOALPHA  = 2,  /* is written to and read from DG files.  */
    SYMMETRIC = 3,  /* These constants may NOT be removed or  */
    DELTA     = 4   /* changed without installing additional  */
  };                /* tests.                                 */
};

typedef SDistrLaws DGLAW;
typedef DGLAW::EDistrLaws DistrLaw;

class CreatorId
{
public:
  enum EType { UNKNOWN, TOROIDAL, POLOIDAL };

private:
  bool isGood;
  EType type;
  bool unchanged;

  int area;
  int count;
  double alpha1;
  double alpha2;
  DistrLaw law;
  int carreFlag;

public: /* There are independent variables, that can be changed anywhere */
  double level1;
  double level2;

public:
  /* Constructor for "empty" creatorId with default values */
  CreatorId( int _area = 0 );
  /* Simplified constructor for DistrGraph only! */
  CreatorId( int _count, double _alpha1, double _alpha2, DistrLaw _law );
  /* Constrictor for poloidal (GridPointsEx) data */
  CreatorId( int _area, int _count, double _alpha1, double _alpha2,
             DistrLaw _law, int _carreFlag );
  /* Constructor for toroidal (SurfacesEx) data */
  CreatorId( int _area, int _count, double _alpha1, double _alpha2,
             double _level1, double _level2, DistrLaw _law, int _carreFlag );
  /* Constructor with string-parser.
   * Check IsGood() result after construction. */
  CreatorId( const std::string& str );

  bool IsEqualTo( const CreatorId& _crid, bool exceptUnchangedFlag = false ) const;
  bool operator == ( const CreatorId& _crid ) const;
  bool operator != ( const CreatorId& _crid ) const;

  /* Copy of this instance with isUnchanged = false */
  CreatorId   ChangedCopy()         const;
  /* Calc value of distribution function described by creatorId */
  double      CalcValue( double x ) const;
  std::string ToStdString()         const;
  //TODO: Carre field type?
  void        ResetCarreFlag() { carreFlag = !!carreFlag; }

  EType   Type()        const { return type; }

  bool    IsGood()      const { return isGood; }
  bool    IsUnchanged() const { return unchanged; }
  int     CarreFlag()   const { return carreFlag; }
  int     Area()        const { return area; }
  int     Law()         const { return (int)law; }
  int     Count()       const { return count; }
  double  Alpha1()      const { return alpha1; }
  double  Alpha2()      const { return alpha2; }

private:
  bool CalcDeltaCoeffs( double* paa, double* pbb, double* pcc ) const;
  bool ParseString( const std::string& crid_str );

};

#endif // CREATORID_H
