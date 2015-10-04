#ifndef MESH_H
#define MESH_H

#include "../core/IComponent.h"
#include "../core/IValueHolder.h"
#include "../core/IManager.h"
#include "../core/Actions.h"

#include "../../common/Includes.h"
#include "../../common/strlib.h"

#include "MeshConstants.h"
#include "MeshDecl.h"

class Model;

typedef struct MeshCellData
{
  int eN;
  NPoint index; // (nx, ny)
  Point p1, p2, p3, p4, center; // (cX, cY)
  double ratio;
  MeshCellPtr pMC;
  MeshCellData* neighbours[4];
  int cutFlags[4];

  MeshCellData( int _eN, NPoint _index, Point _p1, Point _p2, Point _p3, Point _p4, Point _center, double _ratio ):
  eN( _eN ), index( _index ), p1( _p1 ), p2( _p2 ), p3( _p3 ), p4( _p4 ), center( _center ), ratio( _ratio ), pMC( null ) { }
}* MeshCellDataPtr;

class Mesh:
  public IComponent,
  public IValueHolder,
  public IManager
{
  DG_OBJECT(Mesh)
  DG_PROPERTIES
  {
    p_file_name      = 0,
    p_header_string  = 1,
    p_alt            = 2,
    p_fingerprint    = 3
  };

  DG_PROPERTIES_TABLE
  {
    AddProperty( "filename", &fileName );
    AddProperty( "header_string", &headerString );
    AddProperty( "alt", &alt );
    AddProperty( "fingerprint", &fingerprint );
  }

  DG_HIERARCHY_TABLE {
    //RegisterType( OT::MESH,         pMesh );
    RegisterType( OT::MESHCELL,     &cells );
    RegisterType( OT::MESHELEMENT,  &elements );
    RegisterType( OT::MESHPOINT,    &points );
  }

  int alt;
  NPoint size;

  bool bComplete;

  int cellCount, pointCount, elementCount;

  int fileFormat;

  long fingerprint;
  bool bMeshFingerprintOk;

  std::string fileName;
  std::string headerString;
  std::string sender_name;

  IComponentList cells;
  IComponentList points;
  IComponentList elements;

public:
  /* Constructors */

  Mesh( ModelPtr _pModel );
  virtual ~Mesh() { Release(); }

  /* IManager implementation */
  virtual int ReadObjects( Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int _mode = 0 );
  virtual int WriteObjects( Token _token, FILE* _pFile, int /*_mode*/ = 0 ) const;

  /* IComponent implementation */
  virtual void Delete();
  virtual void CalcExtens( Point* _pMin, Point* _pMax ) const;

  virtual std::string Description() const;
  virtual std::string DetailedInfo() const;
  virtual std::string ShortInfo() const;

  /* Actions */

  void SetFileName( const std::string& fName );
  void SetAlt( int alt );
  void SetFingerprint( long fingerprint );
  void SetHeaderString( const std::string& hs );
  int CheckHeaderString( const std::string& hs );

  /* High-level actions */
  int RecalculateCenters();

  /* Utilities */
  int CalcFingerprint(); //not defined, not used
  MeshCellPtr FindMeshCellByNumber( int eN );
  MeshElementPtr FindMeshElementById( int id1, int id2);
  void GetMinMaxPointX( double* pMin, double* pMax );

  /* File I/O */
  //TODO: to App
  int SaveMesh( std::string fileName );

  std::string GetIrregularCellsList() const;

  void FreeVarContainings();

  /* Getters */
  const std::string& FileName() { return fileName; }
  const std::string& HeaderString() { return headerString; }

  bool IsComplete() { return bComplete; }
  int Alt() { return alt; }
  long Fingerprint() { return fingerprint; }
  NPoint Size() { return size; }

  const IComponentList& Cells() { return cells; }
  const IComponentList& Elements() { return elements; }
  const IComponentList& Points() { return points; }

  /* Setters */
  void SetFileFormat( int ff ) { fileFormat = ff; }

  /* Forwarded values */ //TODO: move to mesh
  double SlidingThreshold() const;
  void SetSlidingThreshold( double _val );

  bool HasDoubleBorder() const;
  void SetDoubleBorderFlag( bool _flag );

  bool CheckSlidingMode( ulong _mode ) const;
  void SetSlidingMode( ulong _mode );

public:

  int CreateTopology( MeshCellDataArray* pMCDs );

private:

  MeshElementPtr AddMeshElement( MeshPointPtr pt1, MeshPointPtr pt2, bool bV );
  MeshPointPtr AddMeshPoint( Point _position );
  MeshCellPtr AddMeshCell( int eN, NPoint _index, MeshPointPtr pt1, MeshPointPtr pt2, MeshPointPtr pt3, MeshPointPtr pt4,
                            Point _center, double ratio);

};

#endif
