#ifndef STRINGCONVERTERS_H
#define STRINGCONVERTERS_H

#include <QString>
#include <string>
#include <qglobal.h>
#include <QPoint>

#include "Point.h"
#include "model/flux/CreatorId.h"

QString ToQString( const QString& s );
QString ToQString( const std::string& s );
QString ToQString( int v );
QString ToQString( ulong v );
QString ToQString( const NPoint& p, bool size = true );
QString ToQString( double v1, double v2 );
QString ToQString( double v );
QString ToQString( const Point& p );
QString ToQString( const QPointF& p );

#include "main/StringsManager.h"
#include "Resources.Gui.h"
#include "Resources.Dialogs.h"

QString ToQString( bool b , StringsManager* pSM);
QString ToQString( const CreatorId& crid, StringsManager* pSM  );

#endif // STRINGCONVERTERS_H
