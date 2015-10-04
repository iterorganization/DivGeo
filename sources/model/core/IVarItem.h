#ifndef IVARITEM_H
#define IVARITEM_H

#include "../../common/Macroes.h"
#include "IObject.h"

DG_DEFINE_QUICK(Var)

class IVarItem:
  virtual public IObject
{
  DG_OBJECT(IVarItem)

protected:
  VarList containers;

public:
  void AddContainer( VarPtr _pV );
  void RemoveContainer( VarPtr _pV );

  bool HasContainers()        const { return !containers.empty(); }
  ulong ContainersCount()     const { return containers.size(); }
  const VarList& Containers() const { return containers; }

  void FreeContainers();
  void DeleteContainers();

  // IObject implementation
  virtual std::string Description() const { return ""; }
  virtual std::string ShortInfo() const { return ""; }
  virtual std::string DetailedInfo() const { return ""; }

  virtual ~IVarItem() {}
};
DG_DEFINE_PTR(IVarItem)
DG_DEFINE_LIST(IVarItem)

#ifndef CURRENTITER
  #define CURRENTITER dg__it__
  #define ENDITER dg__it_end__
#endif

#ifndef FOREACHVARITEM
  #define FOREACHVARITEM( type, ptr, grp ) \
    type ptr = dgtype_cast< type >( (grp).empty() ? null : (grp).front() ); \
    for( IVarItemIter CURRENTITER = (grp).begin(); \
          CURRENTITER != (grp).end(); \
          ++CURRENTITER, ptr = (CURRENTITER != (grp).end() ) ? dgtype_cast< type >( *CURRENTITER ) : null )

  #define FOREACHVARITEMCONST( type, ptr, grp ) \
    type ptr = dgtype_cast< type >( (grp).empty() ? null : (grp).front() ); \
    for( IVarItemIterConst CURRENTITER = (grp).begin(), ENDITER = (grp).end(); \
          CURRENTITER != ENDITER; \
          ++CURRENTITER, ptr = (CURRENTITER != ENDITER ) ? dgtype_cast< type >( *CURRENTITER ) : null )
#endif

/* Check if gPart is the part of gStructure */
static bool CheckStructurePart( const IVarItemList& _crStructure, const IVarItemList& _crPart, IVarItemPtr* _ppErrObj ) {
  FOREACH_CONST( IVarItemIterConst, it, _crPart ) {
    if( !Contains( _crStructure, *it ) ) {
      if( _ppErrObj != null )
        *_ppErrObj = *it;
      return false;
    }
  }
  return true;
}

#endif // IVARITEM_H
