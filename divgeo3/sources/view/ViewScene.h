#ifndef VIEWSCENE_H
#define VIEWSCENE_H

#include "IViewScene.h"
#include "ViewItemsDecl.h"

class ViewScene: public IViewScene
{
  Q_OBJECT

private:

  // Drawable objects
  ViewItemsList viewItemsWithText;

  ViewEquilItemPtr pEquilItem;
  ViewTemplateItemPtr pTemplateItem;
  ViewSonnetDataItemPtr pSonnetDataItem;

  ViewItemsList vpNodeItems;          // ViewNodeItemPtr
  ViewItemsList vpElementItems;       // ViewElementItemPtr
  ViewItemsList vpSeparatorItems;     // ViewSeparatorItemPtr
  ViewItemsList vpSourceItems;        // ViewSourceItemPtr
  ViewItemsList vpChordItems;         // ViewChordItemPtr

  ViewItemsList vpSurfaceExItems;     // ViewSurfaceExItemPtr
  ViewItemsList vpXPointTestItems;    // ViewXPointTestItemPtr
  ViewItemsList vpXPointSegItems;     // ViewXPointSegItemPtr
  ViewItemsList vpGridPointExItems;   // ViewGridPointExItemPtr
  ViewItemsList vpGridPointSegItems;  // ViewGridPointSegItemPtr

  ViewItemsList vpMeshElementItems;   // ViewMeshElementItemPtr
  ViewItemsList vpMeshCellItems;      // ViewMeshCellItemPtr
  ViewItemsList vpMeshPointQuasiItems;// ViewMeshPointQuasiItemPtr

public:
  ViewScene(ModelPtr _pModel, const QString& _name, SceneType _sceneType,
            const UserPreferences* _pPrefs, QObject *parent );
  ~ViewScene() { Clear(); }

  virtual void Clear();
  virtual void Build( ulong sfs = SHW::ALL );
  virtual IViewItemPtr CreateItem( IComponentPtr _pObject );
  virtual void RemoveItem( IViewItemPtr _pItem );
  virtual QGraphicsItem* GetGraphicsItem( IViewItemPtr _pItem ) const;
  virtual bool RemoveMultiItemObject( IComponentPtr _pObject );

  /* If SGWX::MESHPOINT flag is set that returns ViewMeshPointQuasiItem object and it is the caller responsibility to delete it */
  virtual IViewItemPtr HitViewItem( QPointF position, ulong hitFlags, double radius = 0. );
  virtual void UpdateVisibility( ShowFlag sf, bool enable );
  virtual ViewItemsList ItemsInRectangle( ulong flags, const QRect& _rect , const QTransform &_transform ) const;

private:
  virtual ViewItemsList* SelectList( ulong sf );
  virtual ViewItemsList* SelectList( ObjectType ot );
  virtual IViewItemPtr SelectSingleItem( ObjectType ot );

public:
  // Getters

};

typedef ViewScene* ViewScenePtr;

#endif // VIEWSCENE_H
