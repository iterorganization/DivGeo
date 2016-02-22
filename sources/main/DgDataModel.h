#ifndef DGDATAMODEL_H
#define DGDATAMODEL_H

#include <QAbstractItemModel>
#include <QFile>

class DgDataModel: public QAbstractItemModel
{
  static const int COLUMNS_COUNT = 2;
  QFile file;
  enum {
    NOTEXISTS,
    READONLY,
    READWRITE,
    OPENFAIL,
  } status;

  class TreeItem {
    QString                name;
    QString               value;
    QList< TreeItem* > children;
    TreeItem*       pParentItem;

  public:
    TreeItem( TreeItem* _pParent, const QString& _crName, const QString& _crValue = "" );
    ~TreeItem();

    void Load();
    void Save() const;

    void     AppendChild( TreeItem* _pChild );
    void     AppendChild( const QString& _crName, const QString& _crValue = "" );
    TreeItem*      Child( int _row );
    int       ChildCount() const;
    int      ColumnCount() const;
    QVariant        Data( int _column ) const;
    void         SetData( int _column, const QVariant& _crValue );
    int              Row() const;
    TreeItem* ParentItem();
    void           Clear();
    void         SetName( const QString& _crName );
    void        SetValue( const QString& _crValue );

  };

  TreeItem root;

public:
  DgDataModel( const QString& _crFilename );

  virtual QModelIndex   index( int _row, int _column,
                               const QModelIndex& _crParent = QModelIndex() ) const Q_DECL_OVERRIDE;
  virtual QModelIndex  parent( const QModelIndex& _crIndex)                   const Q_DECL_OVERRIDE;
  virtual int        rowCount( const QModelIndex& _crParent = QModelIndex() ) const Q_DECL_OVERRIDE;
  virtual int     columnCount( const QModelIndex& )                           const Q_DECL_OVERRIDE;
  virtual QVariant       data( const QModelIndex& _crIndex,
                               int _role = Qt::DisplayRole )                  const Q_DECL_OVERRIDE;
  virtual bool        setData( const QModelIndex& _crIndex,
                               const QVariant& _crValue, int _role = Qt::EditRole ) Q_DECL_OVERRIDE;
  virtual Qt::ItemFlags flags( const QModelIndex& _crIndex )                  const Q_DECL_OVERRIDE;

  void Load();
  void Save() const;
};

#endif // DGDATAMODEL_H
