#include "CoreConstants.h"

const _FlagsRec GridPointSegFlags::flags[] = {
  _FlagsRec( USED,      'u' ),
  _FlagsRec( TARGET_CW, 't' ),
  _FlagsRec( 0, 0 )
};

const _NameRec MeshSlidingModes::names[] = {
  _NameRec( SURFACE,  "Surfaces" ),
  _NameRec( SPLINE,   "Spline" ),
  _NameRec( 0, 0 )
};

const _NameRec OutputModes::names[] = {
  _NameRec( SONNET, "Sonnet" ),
  _NameRec( CARRE,  "Carre" ),
  _NameRec( 0, 0 )
};

const _NameRec SurfaceZonesRestriction::names[] = {
  _NameRec( B2CARRE,  "B2Carre" ),
  _NameRec( B2EIRENE, "B2Eirene" ),
  _NameRec( TARGETS, "Targets" ),
  _NameRec( TCHAINS, "Chains" ),
  _NameRec( TPOINTS, "Points" ),
  _NameRec( 0, 0 )
};

const _NameRec Visibility::names[] = {
  _NameRec( HIDE, "Hide" ),
  _NameRec( SHOW, "Show" ),
  _NameRec( 0, 0 )
};

const _FlagsRec OutputFlags::flags[] = {
  _FlagsRec( VARS,       'V' ),
  _FlagsRec( STRUCTURE,  'S' ),
  _FlagsRec( TARGETS,    'T' ),
  _FlagsRec( SURFACES,   's' ),
  _FlagsRec( GRIDPOINTS, 'g' ),
  _FlagsRec( CELLS,      'c' ),
  _FlagsRec( 0, 0 )
};

const _FlagsRec SurfaceZoneFlags::flags[] = {
  _FlagsRec( LIMITBYSURFACE,       'i' ),
  _FlagsRec( 0, 0 )
};
