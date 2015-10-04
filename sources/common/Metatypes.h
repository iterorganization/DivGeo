#ifndef METATYPES_H
#define METATYPES_H

#include <QMetaType>

#include "../model/core/Model.h"
#include "../view/ViewWnd.h"

Q_DECLARE_METATYPE( VarSetDefPtr )
Q_DECLARE_METATYPE( VarDefPtr )
Q_DECLARE_METATYPE( VarSetPtr )

Q_DECLARE_METATYPE( IComponentListPtr )
Q_DECLARE_METATYPE( IComponentPtr )
Q_DECLARE_METATYPE( IVarItemListPtr )
Q_DECLARE_METATYPE( IVarItemPtr )
Q_DECLARE_METATYPE( IVarOriginPtr )
Q_DECLARE_METATYPE( VarListPtr )

Q_DECLARE_METATYPE( ModelPtr )
Q_DECLARE_METATYPE( CViewWndPtr )

Q_DECLARE_METATYPE( SurfaceZonePtr )

Q_DECLARE_METATYPE( IActionPtr )

#endif // METATYPES_H
