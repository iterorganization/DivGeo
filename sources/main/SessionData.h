#ifndef SESSIONDATA_H
#define SESSIONDATA_H

#include <QPoint>
#include <QSize>
#include <QString>
#include <QVector>
#include <QList>

class SessionViewRecord
{
  static const quint16 scMagic = 0x3323;
  quint8 version;
  bool isOk;
  int size;

public:
  QPoint window_position;
  QSize window_size;
  QPoint scene_center;
  QPointF scene_scale;
  float scene_angle;
  quint8 scene_type;
  quint16 tools[3]; // (left, mid, right) since v2

  bool IsOk() const { return isOk; }
  int Size() const { return size; }

  SessionViewRecord( const QPoint& _crWindow_position,
                     const QSize&  _crWindow_size,
                     const QPoint& _crScene_center,
                     const QPointF& _crScene_scale,
                     double _scene_angle, quint8 _scene_type,
                     quint16* _pTools ):
    version( 0x02 ),
    isOk( true ),
    size( 54 ),
    window_position( _crWindow_position ),
    window_size( _crWindow_size ),
    scene_center( _crScene_center ),
    scene_scale( _crScene_scale ),
    scene_angle( _scene_angle ),
    scene_type( _scene_type ) {
    memcpy( tools, _pTools, sizeof( quint16 ) * 3 );
  }

  SessionViewRecord( const char* _crData ):
    size( 0 ) {
    quint16 magic = *(quint16*)(_crData + size);
    if( magic != scMagic ) {
      isOk = false;
      return;
    }
    size += sizeof( quint16 );

    version = *(quint8*)(_crData + size);
    size += sizeof( quint8 );

    if( version < 0x1 && version > 0x2 ) {
      isOk = false;
      return;
    }

    qint32 px = *(qint32*)(_crData + size);   size += sizeof( qint32 );
    qint32 py = *(qint32*)(_crData + size);   size += sizeof( qint32 );
    window_position = QPoint( px, py );

    px = *(qint32*)(_crData + size);          size += sizeof( qint32 );
    py = *(qint32*)(_crData + size);          size += sizeof( qint32 );
    window_size = QSize( px, py );

    qint64 px64 = *(qint64*)(_crData + size); size += sizeof( qint64 );
    qint64 py64 = *(qint64*)(_crData + size); size += sizeof( qint64 );
    scene_center = QPoint( px64, py64 );

    float pxf = *(float*)(_crData + size);    size += sizeof( float );
    float pyf = *(float*)(_crData + size);    size += sizeof( float );
    scene_scale = QPointF( pxf, pyf );

    scene_angle = *(float*)(_crData + size);  size += sizeof( float );
    scene_type = *(quint8*)(_crData + size);  size += sizeof( quint8 );

    if( version == 0x2 ) {
      tools[0] = *(quint16*)(_crData + size ); size += sizeof( quint16 );
      tools[1] = *(quint16*)(_crData + size ); size += sizeof( quint16 );
      tools[2] = *(quint16*)(_crData + size ); size += sizeof( quint16 );
    }

    isOk = true;
    return;
  }

  QByteArray Save() {
    QByteArray ba( size, 0 );
    char* pData = ba.data();
    int offset = 0;

    quint16 magic = scMagic;
    memcpy( pData, (char*)(&magic), sizeof( quint16 ) );
    offset += sizeof( quint16 );

    memcpy( pData + offset, (char*)(&version), sizeof( quint8 ) );
    offset += sizeof( quint8 );

    size_t s32 = sizeof( qint32 );
    qint32 px = window_position.x(), py = window_position.y();
    memcpy( pData + offset, (char*)(&px), s32 ); offset += s32;
    memcpy( pData + offset, (char*)(&py), s32 ); offset += s32;

    px = window_size.width(), py = window_size.height();
    memcpy( pData + offset, (char*)(&px), s32 ); offset += s32;
    memcpy( pData + offset, (char*)(&py), s32 ); offset += s32;

    size_t s64 = sizeof( qint64 );
    qint64 px64 = scene_center.x(), py64 = scene_center.y();
    memcpy( pData + offset, (char*)(&px64), s64 ); offset += s64;
    memcpy( pData + offset, (char*)(&py64), s64 ); offset += s64;

    size_t sf = sizeof( float );
    float pxf = scene_scale.x(), pyf = scene_scale.y();
    memcpy( pData + offset, (char*)(&pxf), sf ); offset += sf;
    memcpy( pData + offset, (char*)(&pyf), sf ); offset += sf;

    memcpy( pData + offset, (char*)(&scene_angle), sizeof( float ) );
    offset += sizeof( float );
    memcpy( pData + offset, (char*)(&scene_type), sizeof( quint8 ) );
    offset += sizeof( quint8 );

    size_t s16 = sizeof( quint16 );
    memcpy( pData + offset, (char*)(&tools[0]), s16 ); offset += s16;
    memcpy( pData + offset, (char*)(&tools[1]), s16 ); offset += s16;
    memcpy( pData + offset, (char*)(&tools[2]), s16 ); offset += s16;

    return ba;
  }
};

class SessionModelRecord
{
  static const quint16 scMagic = 0x3222;
  quint8 version;
  QList< SessionViewRecord > vViews;

  bool isOk;
  int size;

public:
  QString sName;

  bool IsOk() const { return isOk; }
  int Size() const { return size; }
  const QList< SessionViewRecord >& Views() const { return vViews; }

  void AddViewRecord( const SessionViewRecord& _crView ) {
    if( !_crView.IsOk() )
      return;
    vViews.append( _crView );
    size += _crView.Size();
  }

  SessionModelRecord( const QString& _crsName ):
    version( 0x01 ),
    isOk( true ),
    size( 6 + _crsName.length() ),
    sName( _crsName ) { }

  SessionModelRecord( const char* _crData ):
    size( 0 ) {
    quint16 magic = *(quint16*)(_crData + size);
    if( magic != scMagic ) {
      isOk = false;
      return;
    }
    size += sizeof( quint16 );

    version = *(quint8*)(_crData + size);
    size += sizeof( quint8 );

    switch( version ) {
    case 0x01: {
      quint16 nameSize = *(quint16*)(_crData + size);
      size += sizeof( quint16 );
      if( nameSize > 0 )
        sName = QString::fromUtf8( _crData + size, nameSize );
      size += nameSize;
      quint8 count = *(quint8*)(_crData + size);
      size += sizeof( quint8 );
      if( count > 0 ) {
        for( quint8 c = 0; c < count; c++ ) {
          vViews.append( SessionViewRecord( _crData + size ) );
          size += vViews.last().Size();
          if( !vViews.last().IsOk() ) {
            isOk = false;
            return;
          }
        }
      }

      isOk = true;
      break;
    }
    default:
      isOk = false;
      break;
    }
    return;
  }

  QByteArray Save() {
    QByteArray ba( size, 0 );
    char* pData = ba.data();
    int offset = 0;

    size_t s16 = sizeof( quint16 );
    quint16 magic = scMagic;
    memcpy( pData, (char*)(&magic), s16 );  offset += s16;

    memcpy( pData + offset, (char*)(&version), sizeof( quint8 ) );
    offset += sizeof( quint8 );

    switch( version ) {
    case 0x01: {
      quint16 nameSize = sName.length();
      memcpy( pData + offset, (char*)(&nameSize), s16 ); offset += s16;
      memcpy( pData + offset, sName.toUtf8().data(), nameSize );
      offset += nameSize;

      quint8 count = vViews.count();
      memcpy( pData + offset, (char*)(&count), sizeof( quint8 ) );
      offset += sizeof( quint8 );

      if( count > 0 ) {
        for( quint8 c = 0; c < count; c++ ) {
          memcpy( pData + offset, vViews[c].Save().data(), vViews[c].Size() );
          offset += vViews[c].Size();
        }
      }
      break;
    }
    }


    return ba;
  }
};

class SessionData
{
public:
  struct PF {
    enum PanelFlag {
      OLDTOOLS  = 0x1,
      MODELINFO = 0x2,
      VIEWINFO  = 0x4,
      CONSOLE   = 0x8,
      VIEWBAR   = 0x10,
      EDITBAR   = 0x20,
      TOOLS     = 0x40,

      DEFAULT   = MODELINFO | VIEWINFO | CONSOLE | VIEWBAR | EDITBAR | TOOLS
    };

    static const size_t ulong = 6;
  };

private:

  static const quint16 scMagic = 0x3121;
  quint8 version;
public:
  quint32 panels;
private:
  QList< SessionModelRecord > vModels;
  bool isOk;
  int size;

public:
  bool IsOk() const { return isOk; }
  int Size() const { return size; }

  const QList< SessionModelRecord >& Models() const { return vModels; }

  void AddModelRecord( const SessionModelRecord& _crModel ) {
    if( !_crModel.IsOk() )
      return;
    vModels.append( _crModel );
    size += _crModel.Size();
  }

  SessionData():
    version( 0x02 ),
    panels( PF::DEFAULT ),
    isOk( true ),
    size( 8 ) {}

  SessionData( const char* _crData ):
    size( 0 ) {
    quint16 magic = *(quint16*)(_crData + size);
    if( magic != scMagic ) {
      isOk = false;
      return;
    }
    size += sizeof( quint16 );

    version = *(quint8*)(_crData + size);
    size += sizeof( quint8 );

    if( version >= 0x02 ) {
      panels = *(quint32*)(_crData + size);
      size += sizeof( quint32 );
    }

    quint8 count = *(quint8*)(_crData + size);
    size += sizeof( quint8 );
    if( count > 0 ) {
      for( quint8 c = 0; c < count; c++ ) {
        vModels.append( SessionModelRecord( _crData + size ) );
        size += vModels.last().Size();
        if( !vModels.last().IsOk() ) {
          isOk = false;
          return;
        }
      }
    }
    isOk = true;
    return;
  }

  QByteArray Save() {
    QByteArray ba( size, 0 );
    char* pData = ba.data();
    int offset = 0;

    quint16 magic = scMagic;
    memcpy( pData, (char*)(&magic), sizeof( quint16 ) );
    offset += sizeof( quint16 );

    memcpy( pData + offset, (char*)(&version), sizeof( quint8 ) );
    offset += sizeof( quint8 );

    memcpy( pData + offset, (char*)(&panels), sizeof( quint32 ) );
    offset += sizeof( quint32 );

    quint8 count = vModels.count();
    memcpy( pData + offset, (char*)(&count), sizeof( quint8 ) );
    offset += sizeof( quint8 );

    if( count > 0 ) {
      for( quint8 c = 0; c < count; c++ ) {
        memcpy( pData + offset, vModels[c].Save().data(), vModels[c].Size() );
        offset += vModels[c].Size();
      }
    }

    return ba;
  }
};


#endif // SESSIONDATA_H
