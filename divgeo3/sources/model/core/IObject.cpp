#include "IObject.h"

void ReleaseObject( IObject* _pObject )
{
  if( _pObject != NULL && _pObject->ReleasePtr() )
    delete _pObject;
}

const _NameRec ObjectTypes::names[] = {
  _NameRec( UNKNOWN,      "Unknown" ),
  _NameRec( ELEMENT,      "Element" ),
  _NameRec( NODE,         "Node" ),
  _NameRec( EQUIL,        "Equil" ),
  _NameRec( TEMPLATE,     "Template" ),
  _NameRec( VARSETDEF,    "VarSetDef" ),
  _NameRec( VARDEF,       "VarDef" ),
  _NameRec( VARSET,       "VarSet" ),
  _NameRec( VAR,          "Var" ),
  _NameRec( APP,          "App" ),
  _NameRec( XAPP,         "XApp" ),
  _NameRec( VIEW,         "View" ),
  _NameRec( VIEWLABEL,    "ViewLabel" ),
  _NameRec( SONNET,       "Sonnet" ),
  _NameRec( SEPARATOR,    "Separator" ),
  _NameRec( VARSDLG,      "VarsDlg" ),
  _NameRec( SOURCE,       "Source" ),
  _NameRec( CHORD,        "Chord" ),
  _NameRec( XPOINTTEST,   "XPointTest" ),
  _NameRec( XPOINTSEG,    "XPointSeg" ),
  _NameRec( VIEWSHAPE,    "ViewShape" ),
  _NameRec( MESH,         "Mesh" ),
  _NameRec( MESHCELL,     "MeshCell" ),
  _NameRec( MESHPOINT,    "MeshPoint" ),
  _NameRec( MESHELEMENT,  "MeshElement" ),
  _NameRec( STRINGSOURCE, "StringSource" ),
  _NameRec( GRIDPOINTSEG, "GridPointSeg" ),
  _NameRec( GRIDPOINTEX,  "GridPointEx" ),
  _NameRec( SURFACEZONE,  "SurfaceZone" ),
  _NameRec( SURFACEEX,    "SurfaceEx" ),
  _NameRec( COMMENT,      "Comment" ),
  _NameRec( CONTAINER,    "Container" ),
  _NameRec( 0, 0 )
};
