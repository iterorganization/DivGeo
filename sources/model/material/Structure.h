#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "Element.h"
#include "Node.h"
#include "Separator.h"

class Structure:
  public IComponent,
  public IManager
{
  DG_OBJECT(Structure)
  DG_HIERARCHY_TABLE {
    RegisterType( OT::ELEMENT,    &elements );
    RegisterType( OT::NODE,       &nodes );
    RegisterType( OT::SEPARATOR,  &separators );
    RegisterType( OT::CHORD,      &chords );
  }

  IComponentList nodes;
  IComponentList elements;
  IComponentList separators;
  IComponentList chords;

public:
  Structure( ModelPtr _pModel );
  virtual ~Structure();

  /************************************************************
   * Actions
   ************************************************************/
  NodePtr       AddNode( Point _pnt, bool checkIfExists = false );
  ElementPtr    AddElem( NodePtr _pN1, NodePtr _pN2 );
  SeparatorPtr  AddSeparator( Point _position, NodePtr _pN );
  ChordPtr      AddChord( Point _p1, Point _p2, bool b3d = false );

  void DeleteAllSeparators();

  /************************************************************
   * IManager implementation
   ************************************************************/
  virtual int ReadObjects(Token _token, const char* _cpStr, FILE* _pFile, ulong& _rLine, int = 0 );
  virtual int WriteObjects(Token _token, FILE* _pFile, int /*_mode*/= 0 ) const;

  /************************************************************
   * Modifiers
   ************************************************************/

  /* Create elements between closest nodes pairs */
  void GlueNodes( double _maxDist, bool _markedOnly, int* _prCount );
  /* Merge elements forming an almost straight line */
  void GlueElems( double _maxDist, double _maxLen, bool _cutLonger, bool _markedOnly, int* _prCount );
  /* Force external normals of all adjacend elements into one dir */
  void GlueNormals( bool _markedOnly, int* _prCount );
  /* Renumber all elements and separators. Separators are renumbered first */
  void RenumberElements();

  int ConvertChordsToElems( const IComponentList& _chords, IComponentPtr *ppErrObj );
  int ConvertElemsToChords( const IComponentList &_elems, IComponentPtr *ppErrObj );

  /************************************************************
   * Getters
   ************************************************************/

  IComponentList  UnusedNodes()                 const;
  int             GetNextElemId()               const;
  ElementPtr      FindElementByID( int id )     const;
  IComponentList  GetMarkedElements()           const;
  IVarOriginPtr   FindObject( int _id )         const;
  IComponentPtr   LockedNode()                  const;
  NodePtr         FindNode( const Point& pnt )  const;
  ChordPtr        FindChord( const Point& p1, const Point& p2, bool checkOrder = true ) const;

  const IComponentList& Nodes()       const { return nodes; }
  const IComponentList& Elements()    const { return elements; }
  const IComponentList& Separators()  const { return separators; }
  const IComponentList& Chords()      const { return chords; }

};

#endif // STRUCTURE_H
