#ifndef VIEWSCENETOP_H
#define VIEWSCENETOP_H

#include "IViewScene.h"
#include "ViewTopItemsDecl.h"

class ViewTopScene: public IViewScene
{
  Q_OBJECT

private:
  // Drawable objects
  IVarItemList* pRadialObjects; // Reference to var containing top objects list
  ViewItemsList vpElementRadialItems; // ViewElementRadialTopItemPtr
  ViewItemsList vpSourceRadialItems;  // ViewSourceRadialTopItemPtr
  ViewItemsList vpChordRadialItems;   // ViewChordRadialTopItemPtr
  ViewMeshRadialTopItemPtr pMeshRadialItem;

  ViewItemsList vpChordItems; // ViewChordTopItemPtr

public:
  ViewTopScene( ModelPtr _pModel, const QString &_name, SceneType _sceneType,
                const UserPreferences* _pPrefs, QObject *parent );
  ~ViewTopScene() { Clear(); }

  virtual void Clear();
  virtual void Build( ulong sfs = SHW::ALL );
  virtual IViewItemPtr CreateItem( IComponentPtr _pObject );
  virtual void RemoveItem( IViewItemPtr _pItem );
  virtual QGraphicsItem* GetGraphicsItem( IViewItemPtr _pItem ) const;
  virtual bool RemoveMultiItemObject( IComponentPtr _pObject );

  virtual IViewItemPtr HitViewItem( QPointF position, ulong hitFlags, double radius = 0. );
  virtual void UpdateVisibility( ShowFlag sf, bool enable );
  virtual ViewItemsList ItemsInRectangle( ulong flags, const QRect& _rect , const QTransform &_transform ) const;

private:
  virtual ViewItemsList* SelectList( ulong sf );
  virtual ViewItemsList *SelectList( ObjectType ot );
  virtual IViewItemPtr SelectSingleItem( ObjectType ot );

public:
  // Getters

};

typedef ViewTopScene* ViewTopScenePtr;

#endif // VIEWSCENETOP_H
