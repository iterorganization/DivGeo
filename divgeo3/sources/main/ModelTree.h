#ifndef MODELTREE_H
#define MODELTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>

#include "../common/Metatypes.h"
#include "../common/Point.h"
#include "../common/StringConverters.h"
#include "../model/core/IComponent.h"
#include "../model/core/Model.h"
#include "../model/vars/VarsDecl.h"

#include "StringsManager.h"

#define TYPE_LABEL_MULT( code ) QString( "%1s:" ).arg( SM_GUI( code ) )
#define TYPE_LABEL( code ) QString( "%1:" ).arg( SM_GUI( code ) )

class ModelTree
{
protected:
  QString str_yes, str_no;
  StringsManager *pSM;
  QTreeWidget* pTree;
  ModelPtr pModel;

public:
  ModelTree( StringsManager* _pSM );

protected:
  /* 'expand_depth' parameter means depth of built child-branch:
   * at 0 (the lowest) branch is ready to expand but
   * it will be built on expandEvent */

  /* For child item of 'pItem' with index='index'
   * set value-string and try to add branch if possible. ... */
  /* ...set plain string as value-string (no branch) */
  void SetChildData( QTreeWidgetItem* pItem, int index, const QString& str );
  /* ...set model object description as branch */
  void SetChildData( QTreeWidgetItem* pItem, int index, IComponentPtr pObject,
                     int expand_depth, int _mode = 0 );
  void SetChildData( QTreeWidgetItem* pItem, int index, IVarItemPtr pObject,
                     int expand_depth );
  void SetChildData( QTreeWidgetItem* pItem, int index, IVarOriginPtr pObject,
                     int expand_depth );
  /* ...set list of objects as branch if 'group_size' == 0 or cut it to groups */
  void SetChildData( QTreeWidgetItem* pItem, int index, const IComponentList& objects,
                     int expand_depth, ulong group_size = 0 );
  /* ...set list of variables as branch */
  void SetChildData( QTreeWidgetItem* pItem, int index, const VarList& vars,
                     int expand_depth );

  /* ...set property name string */
  void SetChildLabel( QTreeWidgetItem* pItem, int index, const QString& str );
  /* Add new child item with property name */
  void AddChild( QTreeWidgetItem* pItem, const QString& str );

  /* Add branch if expand depth > 0 or set value string for 'pItem' ...*/
  /* ...add list of variables to 'pItem' as branch */
  void AddVarList(QTreeWidgetItem* pItem, const VarList& vars, int expand_depth );
  /* ...add list of objects to 'pItem' as branch or part of them if 'count' > 0 */
  template< typename T >
  void AddObjectList( QTreeWidgetItem* _pItem, const std::list< T* >& _crObjects,
                      int _expand_depth, ulong _from = 0, ulong _count = 0 );

  /* Create branch and/or set item value.
   * If 'expand >= 1' and item has no children than branch will be rebuild
   *  also if 'rebuild = false' */
  QString AddObjectDescr(QTreeWidgetItem* _pItem, IComponentPtr _pObject,
    // Create branch and fill it with data (>=1) or just set item value (0)
      int _expand_depth,
    // Rebuild branch (true) or just update data (false)
      bool _rebuild = false, int _mode = 0 );
  QString AddObjectDescr( QTreeWidgetItem* _pItem, IVarItemPtr _pObject,
      int _expand_depth, bool _rebuild = false );
  QString AddObjectDescr( QTreeWidgetItem* _pItem, IVarOriginPtr _pObject,
      int _expand_depth, bool _rebuild = false );

  QString AddActionDescr( QTreeWidgetItem* _pItem, IActionPtr _pAction,
                          int _expand_depth, bool _rebuild = false );

  QString AddBranchForModel         ( QTreeWidgetItem* _pItem, ModelPtr     _pModel,    int _expand_depth, bool _rebuild = false );
  QString AddBranchForMesh          ( QTreeWidgetItem* _pItem, MeshPtr      _pMesh,     int _expand_depth, bool _rebuild = false );
  QString AddBranchForTopology      ( QTreeWidgetItem* _pItem, TopologyPtr  _pTopology, int _expand_depth, bool _rebuild = false );
  QString AddBranchForEquil         ( QTreeWidgetItem* _pItem, EquilPtr     _pEquil,    int _expand_depth, bool _rebuild = false );

  QString AddBranchForElement       ( QTreeWidgetItem* _pItem, ElementPtr       _pElem, int _expand_depth, bool _rebuild = false );
  QString AddBranchForSeparator     ( QTreeWidgetItem* _pItem, SeparatorPtr     _pSep,  int _expand_depth, bool _rebuild = false );
  QString AddBranchForSource        ( QTreeWidgetItem* _pItem, SourcePtr        _pSrc,  int _expand_depth, bool _rebuild = false );
  QString AddBranchForChord         ( QTreeWidgetItem* _pItem, ChordPtr         _pCh,   int _expand_depth, bool _rebuild = false );
  QString AddBranchForNode          ( QTreeWidgetItem* _pItem, NodePtr          _pNode, int _expand_depth, bool _rebuild = false );

  QString AddBranchForMeshPoint     ( QTreeWidgetItem* _pItem, MeshPointPtr     _pMP,   int _expand_depth, bool _rebuild = false );
  QString AddBranchForMeshCell      ( QTreeWidgetItem* _pItem, MeshCellPtr      _pMC,   int _expand_depth, bool _rebuild = false );
  QString AddBranchForMeshElement   ( QTreeWidgetItem* _pItem, MeshElementPtr   _pME,   int _expand_depth, bool _rebuild = false );

  QString AddBranchForXPointTest    ( QTreeWidgetItem* _pItem, XPointTestPtr    _pXPT,  int _expand_depth, bool _rebuild = false );
  QString AddBranchForXPointSeg     ( QTreeWidgetItem* _pItem, XPointSegPtr     _pXPS,  int _expand_depth, bool _rebuild = false );
  QString AddBranchForGridPointSeg  ( QTreeWidgetItem* _pItem, GridPointSegPtr  _pGPS,  int _expand_depth, bool _rebuild = false );
  QString AddBranchForGridPointEx   ( QTreeWidgetItem* _pItem, GridPointExPtr   _pGPX,  int _expand_depth, bool _rebuild = false );
  QString AddBranchForSurfaceZone   ( QTreeWidgetItem* _pItem, SurfaceZonePtr   _pSZ,   int _expand_depth, bool _rebuild = false );
  QString AddBranchForSurfaceEx     ( QTreeWidgetItem* _pItem, SurfaceExPtr     _pSX,   int _expand_depth, bool _rebuild = false );

  QString AddBranchForVarSetDef     ( QTreeWidgetItem* _pItem, VarSetDefPtr     _pVSD,  int _expand_depth, bool _rebuild = false );
  QString AddBranchForVarDef        ( QTreeWidgetItem* _pItem, VarDefPtr        _pVD,   int _expand_depth, bool _rebuild = false );
  QString AddBranchForVarSet        ( QTreeWidgetItem* _pItem, VarSetPtr        _pVS,   int _expand_depth, bool _rebuild = false );
  QString AddBranchForVar           ( QTreeWidgetItem* _pItem, VarPtr           _pVar,  int _expand_depth, bool _rebuild = false, int _mode = 0 );

  template< typename T>
  QString AddBranchForContainer     ( QTreeWidgetItem* _pItem, ListContainer<T>* _pCont, int _expand_depth, bool _rebuild = false );

  QString AddBranchForStack         ( QTreeWidgetItem* _pItem, const ActionList& _crStack, int _expand_depth, bool _rebuild = false );

  void Clear( QTreeWidgetItem* pItem );

  QString FlagToStr( long flags ) const;
  QString TypeFlagToStr( VarType type ) const;
  const QString& TypeToStr( VarType type ) const;
  const QString& TypeToStr( ObjectType type ) const;

  void UpdateTreeItem( QTreeWidgetItem* pItem );
};

#endif // MODELTREE_H
