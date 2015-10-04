#ifndef IVIEWSCENE_H
#define IVIEWSCENE_H

#include <QGraphicsScene>
#include <QList>

#include "../model/core/Model.h"
#include "items/IViewItem.h"
#include "../main/UserPreferences.h"
#include "ViewState.h"
#include "ViewStateRecord.h"

#include "items/ViewAxesItem.h"
#include "items/ViewGridItem.h"
#include "items/ViewRadialGridItem.h"
#include "items/ViewHighlightedRectItem.h"
#include "items/ViewRotateToolItem.h"
#include "items/ViewShapeItem.h"
#include "items/ViewLabelItem.h"
#include "items/ViewLegendItem.h"
#include "items/ViewCommentItem.h"

struct SceneTypes {
  enum ESceneTypes {
    SECTION = 0,
    TOP = 1
  };
};
typedef SceneTypes ST;
typedef SceneTypes::ESceneTypes SceneType;

class IViewScene: public QGraphicsScene
{
  Q_OBJECT

protected:

  ViewStateList stackUndo, stackRedo;

  const ModelPtr pModel;
  const SViewState* pState;
  const UserPreferences* pPrefs;

  SceneType sceneType;

  QString sender_name;
  QString title_name;

  ulong showFlags;

  ViewItemsList viewItems;

  ViewAxesItemPtr pAxesItemA, pAxesItemO;
  ViewGridItemPtr pGridItem;
  ViewRadialGridItemPtr pRadialGridItem;

  ViewHighlightedRectItemPtr pHiRect;
  ViewRotateToolItemPtr pRotTool;

  ViewLegendItemPtr pLegendItem;

  ViewShapesList shapeItems;
  ViewLabelsList labelItems;

  ViewItemsList commentItems;

public:
  IViewScene( const ModelPtr _pModel, const QString& _name, SceneType _sceneType,
              const UserPreferences* _pPrefs, QObject* parent ):
    QGraphicsScene( parent ),
    pModel( _pModel ), pState( null ), pPrefs( _pPrefs ),
    sceneType( _sceneType ),
    showFlags( 0x00 ),
    pAxesItemA( null ), pAxesItemO( null ),
    pGridItem( null ), pRadialGridItem( null ),
    pHiRect( null ), pRotTool( null ),
    pLegendItem( null )
  {    
    title_name = _name;
    UpdateSenderName();
    this->setItemIndexMethod( QGraphicsScene::NoIndex );
  }
  virtual ~IViewScene() { /* Call Clear() from child's dtor */ }
  /* Update name shown in log. Call when model's name changed */
  void UpdateSenderName() { sender_name = QString( "Scene-%1 (Model-%2)" ).arg( title_name ).arg( QString::fromStdString( pModel->Name() ) ); }

  /* Delete all items and clear collections.
   * Call IViewScene::Clear() at the beginning of overrided method */
  virtual void Clear();
  /* Create items from model data and fill collections.
   * Call IViewScene::Build() from overrided method */
  virtual void Build( ulong sfs = SHW::ALL );

  /* Add new view item for given model object */
  virtual IViewItemPtr CreateItem( IComponentPtr _pObject ) = 0;
  /* Remove item from all lists. It is caller's responsibility to delete it */
  virtual void RemoveItem( IViewItemPtr _pItem ) = 0;
  /* Find item nearest to position. Radius != 0 sets zone of search */
  virtual IViewItemPtr HitViewItem( QPointF position, ulong hitFlags, double radius = 0. ) = 0;
  /* Cast IViewItem to QGraphicsItem */
  virtual QGraphicsItem* GetGraphicsItem( IViewItemPtr _pItem ) const = 0;
  /* Update visibility of item or items' parts of given type */
  virtual void UpdateVisibility( ShowFlag sf, bool enable ) = 0;
  /* Select view items in rectangle */
  virtual ViewItemsList ItemsInRectangle( ulong _flags, const QRect& _rect , const QTransform &_transform ) const;
  /* Check if there are multiple items for one object and removes them all */
  virtual bool RemoveMultiItemObject( IComponentPtr _pObject ) = 0;

  /* Create shape item with specified geometry */
  ViewShapeItemPtr CreateShapeItem( ViewShapeItem::Shape _shape, const Point& _p1, const Point& _p2 = Point() );
  /* Remove selected shape item from scene and delete item */
  void RemoveShapeItem( ViewShapeItemPtr _pItem );
  /* Remove and delete all items */
  void RemoveAllShapeItems();

  /* Create label item */
  ViewLabelItemPtr CreateLabelItem( IComponentPtr _pObject, const QString& _text );
  /* Remove selected label item from scene and delete item */
  void RemoveLabelItem( ViewLabelItemPtr _pItem );
  /* Remove and delete all items */
  void RemoveAllLabelItems();

  /* Find object in list by ShowFlag or ShowFlagEx. Uses SelectList overrided method */
  IViewItemPtr HitItemByType( ulong sf, QPointF position, double *pDist = null, double radius = 0 );
  /* Just clear scene and build it again */
  void Rebuild() { Clear(); Build(); }
  /* BoundingRect without grid and axis */
  QRectF FullBoundingRect();
  /* BoundingRect for listed objects */
  QRectF GroupBoundingRect( const IComponentList& _objects );
  /* Set new highlight rectangle */
  void UpdateHighlightRect( QPolygonF hr = QPolygonF() );
  /* Show or hide rotation tool */
  void UpdateRotTool( bool enable = true );
  /* Update grid geometry. Call it if zoom-factor was changed */
  void UpdateGrid();
  /* Update visibility of each item */
  void SetVisibility(ulong sfs, bool forced = false );
  /* Set view state to each item updating view-dependent data */
  void SetViewState( const SViewState* _pState );
  /* Setup background brash. Don't use it in paintEvent-methods! */
  void SetupBackground() { setBackgroundBrush( QBrush( pPrefs->visual.background_color ) ); }
  /* Update style of all items */
  void UpdateStyle();

  /* Wrappers of some abstract methods with objects lists support */
  void RemoveListItems( const IComponentList& _pObjects );
  void CreateListItems( const IComponentList& _pObjects );
  void FullUpdateListItems( const IComponentList& _pObjects );
  void StyleUpdateListItems( const IComponentList& _pObjects );

  // getters
  const SViewState* ViewState() const { return pState; }
  ulong ShowFlags() const { return showFlags; }
  const QString& Name() const { return title_name; }
  bool HasModel( ModelPtr _pModel ) { return _pModel == pModel; }
  SceneType Type() { return sceneType; }
  bool IsTopView() { return sceneType == ST::TOP; }

protected:
  /* Update showFlags register and check if there are any changes.
   * Call it at the beginning of overrided UpdateVisibility method */
  bool UpdateShowFlag( ShowFlag sf, bool enable );
  /* Select view items list by show flag */
  virtual ViewItemsList* SelectList( ulong sf ) = 0;
  /* Select view items list by object type */
  virtual ViewItemsList* SelectList( ObjectType ot ) = 0;
  /* Select unique view item */
  virtual IViewItemPtr SelectSingleItem( ObjectType ot ) = 0;


public:
  /* Select view element items by model objects list */
  ViewItemsList GetItemsList(const IComponentList& objects, bool multitype = false );
  IViewItemPtr GetItem( IComponentPtr _pObject );

  /* Methods of undo/redo view state managment for this scene */
  void AddViewState( const ViewStateRecord& vsr ); //{ stackUndo.append( vsr ); }
  ViewStateRecord UndoViewChange();
  ViewStateRecord RedoViewChange();
  bool HasUndoStateRecords() { return !stackUndo.isEmpty(); }
  bool HasRedoStateRecords() { return !stackRedo.isEmpty(); }

  static ulong OT2SHW( ObjectType _ot );


signals:
  void message( MessageType _mt, AlarmLevel _al, QString _sender, QString _text );
  void message( MessageType _mt, AlarmLevel _al, QString _sender, ulong msg_code, QStringList args = QStringList() );
};

typedef IViewScene* IViewScenePtr;
typedef QList< IViewScenePtr > SceneList;

#endif // IVIEWSCENE_H
