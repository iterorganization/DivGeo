#ifndef UPDATEINFO_H
#define UPDATEINFO_H

#include <list>
#include <algorithm>
#include <assert.h>

#include "Macroes.h"

#include "IModelAgent.h"

DG_DEFINE_QUICK(IComponent)

struct UpdateInfo
{
  bool isEmpty;
  IComponentList create;
  IComponentList remove;
  IComponentList update;
  IComponentList ustyle;

  UpdateInfo(): isEmpty( true ) {}

  UpdateInfo( IComponentPtr _pObject, UpdateType _ut );
  UpdateInfo( const IComponentList& _crList, UpdateType _ut );
  ~UpdateInfo() { Clear(); }

  void AddToCreate( IComponentPtr _pObject );
  void AddToRemove( IComponentPtr _pObject );
  void AddToUpdate( IComponentPtr _pObject, bool _fullUpdate = true );

  void Clear();
  void Append( const UpdateInfo& _crUI );
  void Append( IComponentPtr _pObject, UpdateType _ut );
  void Append( const IComponentList& _crList, UpdateType _ut );

  bool HasCreatedOrDeleted() const { return !create.empty() || !remove.empty(); }
};

typedef UpdateInfo UPI;


#endif // UPDATEINFO_H
