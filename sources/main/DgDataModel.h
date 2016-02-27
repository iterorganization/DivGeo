#ifndef DGDATAMODEL_H
#define DGDATAMODEL_H

#include <QAbstractItemModel>
#include <QFile>

namespace dm {

class TreeItem;

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

  TreeItem* pRoot;

public:
  DgDataModel( const QString& _crFilename );
  ~DgDataModel();

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

  bool IsReadonly() const { return status != READWRITE; }
};

} // namespace dm

#endif // DGDATAMODEL_H
