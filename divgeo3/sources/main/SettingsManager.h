#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QRgb>
#include <QColor>
#include <QPen>

#include <QVariant>
#include <QSettings>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QVector>

#include "../model/core/CoreConstants.h"
#include "../model/core/IValueHolder.h"
#include "../model/core/Actions.h"
#include "../common/Resources.Dialogs.h"
#include "../common/Point.h"

#include <QMetaType>
typedef QString* QStringPtr;
typedef QColor* QColorPtr;
typedef QPen* QPenPtr;
typedef NPoint* NPointPtr;
typedef int* IntPtr;
typedef bool* BoolPtr;
typedef float* FloatPtr;
Q_DECLARE_METATYPE( QStringPtr )
Q_DECLARE_METATYPE( QColorPtr )
Q_DECLARE_METATYPE( QPenPtr )
Q_DECLARE_METATYPE( NPointPtr )
Q_DECLARE_METATYPE( IntPtr )
Q_DECLARE_METATYPE( BoolPtr )
Q_DECLARE_METATYPE( FloatPtr )

typedef unsigned long ulong;
#define frgb(fr,fg,fb) QColor::fromRgb( qRgb((int)(255*fr),(int)(255*fg),(int)(255*fb)) )

#ifndef null
  #define null NULL
#endif

class SettingsManager
{
public:
  /* Properties description table */
  struct PropertyRecord
  {
    struct SPropertyTypes {
      enum EPropertyTypes {
        UNKNOWN,
        BEGIN,  // Begin of tree branch. Additional data: preview type
        END,    // End of tree branch
        PEN,    // Color and width properties
        COLOR,  // Color property
        FONT,   // Font size
        INT,    // SpinBox, ranged value. Additional data: min, max
        FLOAT,  // SpinBox, ranged value. Additional data: min, max
        BOOL,   // CheckBox
        NPOINT, // X and Y INT properties
        LIST,   // ComboBox. Additional data: codes of item strings
        FLAGS,  // Group of CheckBoxes. Additional data: strings
        STRING  // LineEdit
      };
    };
    typedef SPropertyTypes PT;
    typedef SPropertyTypes::EPropertyTypes PropertyType;

    QString sKey;       // QSettings key
    ulong descrCode;    // GUI string code
    PropertyType type;  // Table item type

    QVariant dataPtr;   // Pointer to variable

    IValueHolderPtr pHolder; // Model object
    int parameterId;         // Variable id

    QVector< int > vUserData; // Additional data
    bool updateAll;

    PropertyRecord(): type( PT::UNKNOWN ), pHolder( null ), updateAll( false ) {}
    PropertyRecord( PropertyType _pt ): type( _pt ), pHolder( null ), updateAll( false ) {}

    PropertyRecord( const QString& _crsKey, ulong _descrCode,
                    PropertyType _pt, QVariant _dataPtr = QVariant() ):
      sKey( _crsKey ),  descrCode( _descrCode ),
      type( _pt ),      dataPtr( _dataPtr ),
      pHolder( null ),  updateAll( false ) {}

    PropertyRecord( const QString& _crsKey, ulong _descrCode,
                    PropertyType _pt, QVariant _dataPtr,
                    IValueHolderPtr _pHolder, int _parameterId ):
      sKey( _crsKey ),    descrCode( _descrCode ),
      type( _pt ),        dataPtr( _dataPtr ),
      pHolder( _pHolder), parameterId( _parameterId ),
      updateAll( false ) {}

    PropertyRecord& operator << ( int _value ) {
      vUserData.append( _value ); return *this; }
  };
  typedef PropertyRecord PR;

protected:
  QVector< PropertyRecord > vTable;

  /* Paths */
  static const char* scpIniExt;
  static const char* scpDefaultExt;

  QString sIniPath;
  QString sDefaultIniPath;
  QString sProfileDir;
  QString sName;

public:
  SettingsManager( const QString& _crsProfileDir = "", const QString& _crsName = "" );

  void Save( const QString& _crsIniPath ) const;
  void Load();

  void Retrieve();
  void Apply( ModelPtr _pModel );

  void Append( const PropertyRecord& _crPR ) { vTable.append( _crPR ); }

  const QString& IniPath() const { return sIniPath; }
  const QString& DefaultIniPath() const { return sDefaultIniPath; }
  const QString& ProfileDir() const { return sProfileDir; }

  const QVector< PropertyRecord >& Table() { return vTable; }
};

#endif // SETTINGSMANAGER_H
