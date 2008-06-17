/* XDivGeo main header file */

#ifndef _x_dg_h
#define _x_dg_h

#define XM_VERSION
#define NO_DUMMIES

#define NeedFunctionPrototypes 1

#include <unistd.h>

#include <Xm/Xm.h>
#include <X11/Shell.h>
#include <Xm/Protocols.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>
#include <Xm/Text.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/DrawingA.h>
#include <Xm/DialogS.h>
#include <Xm/MessageB.h>
#include <Xm/ArrowB.h>
#include <Xm/Label.h>
#include <Xm/FileSB.h>
#include <Xm/List.h>
#include <Xm/TextF.h>
#include <Xm/Frame.h>
#include <Xm/ScrolledW.h>

#include "dg.h"     /* Include DivGeo core */

#include "x_xapp.h"
#include "x_view.h"
#include "x_dlg.h"
#include "vacreate.h"
#include "x_fileio.h"
#include "x_help.h"
#include "x_info.h"
#include "x_menu.h"
#include "x_notify.h"
#include "x_rotmov.h"
#include "x_setup.h"
#include "x_toolb.h"
#include "x_vars.h"
#include "x_creobj.h"
#include "x_verdlg.h"
#include "x_mesh.h"
#include "x_topo.h"

#include "ps_view.h"

extern char* dgResources[];


#endif
