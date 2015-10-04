#include "SettingsManager.h"

const char* SettingsManager::scpIniExt = ".ini";
const char* SettingsManager::scpDefaultExt = ".default";

SettingsManager::SettingsManager( const QString& _crsProfileDir, const QString& _crsName )
{
  QFileInfo fiName( _crsName );
  QFileInfo fiDir( _crsProfileDir );

  sName = fiName.completeBaseName();
  sProfileDir = fiDir.absolutePath();
  sIniPath = sProfileDir + "/" + sName + scpIniExt;
  sDefaultIniPath = sProfileDir + "/" + sName + scpDefaultExt + scpIniExt;
}

void SettingsManager::Save( const QString& _crsIniPath ) const
{
  if( _crsIniPath.isEmpty() )
    return;

  QSettings settings( _crsIniPath, QSettings::IniFormat );

  foreach( const PropertyRecord& rProperty, vTable ) {
    switch( rProperty.type ) {
    case PR::PT::BEGIN: settings.beginGroup( rProperty.sKey ); break;
    case PR::PT::END:   settings.endGroup(); break;
    case PR::PT::PEN:
    case PR::PT::FONT: {
      QPen* pPen = rProperty.dataPtr.value< QPenPtr >();
      if( pPen == null ) break;
      settings.setValue( rProperty.sKey + "Color", pPen->color().name() );
      QString sType = rProperty.type == PR::PT::PEN ? "Width" : "FontSize";
      settings.setValue( rProperty.sKey + sType, pPen->width() );
      break;
    }
    case PR::PT::COLOR: {
      QColor* pColor = rProperty.dataPtr.value< QColorPtr >();
      if( pColor == null ) break;
      settings.setValue( rProperty.sKey, pColor->name() );
      break;
    }
    case PR::PT::INT:
    case PR::PT::LIST: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      settings.setValue( rProperty.sKey, *pInt );
      break;
    }
    case PR::PT::FLOAT: {
      float* pFloat = rProperty.dataPtr.value< FloatPtr >();
      if( pFloat == null ) break;
      settings.setValue( rProperty.sKey, *pFloat );
      break;
    }
    case PR::PT::BOOL: {
      bool* pBool = rProperty.dataPtr.value< BoolPtr >();
      if( pBool == null ) break;
      settings.setValue( rProperty.sKey, *pBool );
      break;
    }
    case PR::PT::NPOINT: {
      NPoint* pPoint = rProperty.dataPtr.value< NPointPtr >();
      if( pPoint == null ) break;
      settings.setValue( rProperty.sKey + "X", pPoint->x );
      settings.setValue( rProperty.sKey + "Y", pPoint->y );
      break;
    }
    case PR::PT::FLAGS: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      QString sReg;
      int flag = 1;
      int dataSize = rProperty.vUserData.size();
      for( int c = 0; c < dataSize; c++ ) {
        sReg += QString( "%1" ).arg( HasAnyFlag( *pInt, flag ) ? 1 : 0 );
        flag <<= 1;
      }

      settings.setValue( rProperty.sKey, sReg );
    }
    case PR::PT::UNKNOWN:
      break;
  }
  }

  settings.sync();
}

void SettingsManager::Load()
{
  if( !QFile( sIniPath ).exists() ) {
    Save( sDefaultIniPath );
    Save( sIniPath );
    return;
  }

  QSettings settings( sIniPath, QSettings::IniFormat );
  settings.sync();

  foreach( const PropertyRecord& rProperty, vTable ) {
    switch( rProperty.type ) {
    case PR::PT::BEGIN: settings.beginGroup( rProperty.sKey ); break;
    case PR::PT::END:   settings.endGroup(); break;
    case PR::PT::PEN:
    case PR::PT::FONT: {
      QPen* pPen = rProperty.dataPtr.value< QPenPtr >();
      if( pPen == null ) break;
      QString sColor = settings.value( rProperty.sKey + "Color", pPen->color().name() ).toString();
      pPen->color().setNamedColor( sColor );
      QString sType = rProperty.type == PR::PT::PEN ? "Width" : "FontSize";
      int width = settings.value( rProperty.sKey + sType, pPen->width() ).toInt();
      pPen->setWidth( width );
      break;
    }
    case PR::PT::COLOR: {
      QColor* pColor = rProperty.dataPtr.value< QColorPtr >();
      if( pColor == null ) break;
      QString sColor = settings.value( rProperty.sKey + "Color", pColor->name() ).toString();
      pColor->setNamedColor( sColor );
      break;
    }
    case PR::PT::INT:
    case PR::PT::LIST: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      *pInt = settings.value( rProperty.sKey, *pInt ).toInt();
      break;
    }
    case PR::PT::FLOAT: {
      float* pFloat = rProperty.dataPtr.value< FloatPtr >();
      if( pFloat == null ) break;
      *pFloat = settings.value( rProperty.sKey, *pFloat ).toFloat();
      break;
    }
    case PR::PT::BOOL: {
      bool* pBool = rProperty.dataPtr.value< BoolPtr >();
      if( pBool == null ) break;
      *pBool = settings.value( rProperty.sKey, *pBool ).toBool();
      break;
    }
    case PR::PT::NPOINT: {
      NPoint* pPoint = rProperty.dataPtr.value< NPointPtr >();
      if( pPoint == null ) break;
      int x = settings.value( rProperty.sKey + "X", pPoint->x ).toInt();
      int y = settings.value( rProperty.sKey + "Y", pPoint->y ).toInt();
      *pPoint = NPoint( x, y );
      break;
    }
    case PR::PT::FLAGS: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      QString sReg = settings.value( rProperty.sKey, sReg ).toString();

      *pInt = 0;
      int flag = 1;
      int regSize = sReg.length();
      for( int c = regSize - 1; c >= 0; c++ ) {
        if( sReg[c] == '1' )
          *pInt |= flag;
        flag <<= 1;
      }
    }
    case PR::PT::UNKNOWN:
      break;
  }
  }
}

void SettingsManager::Apply( ModelPtr _pModel )
{
  foreach( const PropertyRecord& rProperty, vTable ) {
    if( rProperty.pHolder == null )
      continue;

    UPtr value;

    switch( rProperty.type ) {
    case PR::PT::INT: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      value.SetValue( *pInt, true );
      break;
    }
    case PR::PT::LIST: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      value.SetValue( (ulong)*pInt, true );
      break;
    }
    case PR::PT::FLOAT: {
      float* pFloat = rProperty.dataPtr.value< FloatPtr >();
      if( pFloat == null ) break;
      value.SetValue( (double)*pFloat, true );
      break;
    }
    case PR::PT::FLAGS: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      value.SetValue( (ulong)*pInt, true );
      break;
    }
    case PR::PT::BOOL: {
      bool* pBool = rProperty.dataPtr.value< BoolPtr >();
      if( pBool == null ) break;
      value.SetValue( *pBool, true );
      break;
    }
    case PR::PT::NPOINT: {
      NPoint* pPoint = rProperty.dataPtr.value< NPointPtr >();
      if( pPoint == null ) break;
      value.SetValue( *pPoint, true );
      break;
    }
    default:
      break;
    }

    if( !value.IsNull() )
      ActChangeValue( _pModel, null, rProperty.pHolder, rProperty.parameterId, value,
                    NO_REDRAW, DO_AT_ONCE );
  }
}

void SettingsManager::Retrieve()
{
  foreach( const PropertyRecord& rProperty, vTable ) {
    if( rProperty.pHolder == null )
      continue;

    UPtr value = rProperty.pHolder->GetValue( rProperty.parameterId );

    switch( rProperty.type ) {
    case PR::PT::INT: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      *pInt = value.Int();
      break;
    }
    case PR::PT::LIST:
    case PR::PT::FLAGS: {
      int* pInt = rProperty.dataPtr.value< IntPtr >();
      if( pInt == null ) break;
      *pInt = (int)value.ULong();
      break;
    }
    case PR::PT::FLOAT: {
      float* pFloat = rProperty.dataPtr.value< FloatPtr >();
      if( pFloat == null ) break;
      *pFloat = (float)value.Double();
      break;
    }
    case PR::PT::BOOL: {
      bool* pBool = rProperty.dataPtr.value< BoolPtr >();
      if( pBool == null ) break;
      *pBool = value.Bool();
      break;
    }
    case PR::PT::NPOINT: {
      NPoint* pPoint = rProperty.dataPtr.value< NPointPtr >();
      if( pPoint == null ) break;
      *pPoint = value.NPointRef();
      break;
    }
    default:
      break;
    }
  }
}
