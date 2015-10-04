#include "Point.h"

const std::string NPoint::INDEX = "[%x, %y]";
const std::string NPoint::SIZE = "[%x x %y]";
const std::string NPoint::POSITION = "(%x, %y)";

std::string NPoint::ToString( const std::string& _crsFormat ) const
{
  size_t x_pos = 0, y_pos = 0;
  bool isOk = true;
  if( _crsFormat.empty() || _crsFormat.length() < 5 )
    isOk = false;
  else {
    x_pos = _crsFormat.find( "%x" );
    if( x_pos == std::string::npos )
      isOk = false;
    else {
      y_pos = _crsFormat.find( "%y" );
      if( y_pos == std::string::npos || y_pos < x_pos )
        isOk = false;
    }
  }

  std::stringstream ss;
  if( isOk ) {
    ss << _crsFormat.substr( 0, x_pos ) << x
       << _crsFormat.substr( x_pos+2, y_pos-x_pos-2 ) << y
       << _crsFormat.substr( y_pos+2 );
  }
  else
    ss << "[" << x << ", " << y << "]";
  return ss.str();
}

const std::string Point::POS2D = "(%x, %y)";
const std::string Point::POS3D = "(%x, %y, %z)";

std::string Point::ToString( const std::string& _crsFormat ) const
{
  size_t x_pos = 0, y_pos = 0, z_pos = 0;
  bool isOk = true;
  if( _crsFormat.empty() || _crsFormat.length() < 5 )
    isOk = false;
  else {
    x_pos = _crsFormat.find( "%x" );
    if( x_pos == std::string::npos )
      isOk = false;
    else {
      y_pos = _crsFormat.find( "%y" );
      if( y_pos == std::string::npos || y_pos < x_pos )
        isOk = false;
      else {
        z_pos = _crsFormat.find( "%z" );
        if( z_pos < y_pos )
          isOk = false;
      }
    }
  }

  std::stringstream ss;
  if( isOk ) {
    ss << _crsFormat.substr( 0, x_pos ) << x
       << _crsFormat.substr( x_pos+2, y_pos-x_pos-2 ) << y;
    if( z_pos != std::string::npos )
      ss << _crsFormat.substr( y_pos+2, z_pos-y_pos-2 ) << z
         << _crsFormat.substr( z_pos+2 );
    else
      ss << _crsFormat.substr( y_pos+2 );
  }
  else
    ss << "(" << x << ", " << y << ")";
  return ss.str();
}

