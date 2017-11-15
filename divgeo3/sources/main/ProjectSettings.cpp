#include "ProjectSettings.h"

ProjectSettings::ProjectSettings( ModelPtr _pModel ):
  SettingsManager(),
  pModel( _pModel )
{
  vTable.append( PR( "Output",    DLG::OUTPUTMODE::TITLE, PR::PT::BEGIN ) );
  vTable.append( PR( "Mode",      DLG::OUTPUTMODE::TITLE, PR::PT::LIST,
    QVariant::fromValue( (IntPtr)&outputMode ), pModel, Model::p_output_mode )
                 << 0
                 << DLG::OUTPUTMODE::LBL::SONNET /*1*/
                 << DLG::OUTPUTMODE::LBL::CARRE  /*2*/ );
  vTable.append( PR( "OutputFlags", DLG::OUTPUTMODE::LBL::FLAGS, PR::PT::FLAGS,
    QVariant::fromValue( (IntPtr)&outputFlags ), pModel, Model::p_output_flags )
                 << DLG::OUTPUTMODE::LBL::VARS
                 << DLG::OUTPUTMODE::LBL::STRUCTURE
                 << DLG::OUTPUTMODE::LBL::TARGETS
                 << DLG::OUTPUTMODE::LBL::SURFACES
                 << DLG::OUTPUTMODE::LBL::GRIDPOINTS
                 << DLG::OUTPUTMODE::LBL::CELLS );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "MeshEditing",   DLG::MESHOPTIONS::TITLE, PR::PT::BEGIN ) );
  vTable.append( PR( "SlidingMode",   DLG::MESHOPTIONS::MODE::TITLE, PR::PT::LIST,
    QVariant::fromValue( (IntPtr)&slidingMode ), pModel, Model::p_mesh_sliding_mode )
                 << 0 << 0 << DLG::MESHOPTIONS::MODE::SURFACES /*2*/
                 << 0 << 0 << DLG::MESHOPTIONS::MODE::SPLINES  /*5*/ );
  vTable.append( PR( "Margin",        DLG::MESHOPTIONS::LBL::SLIDE, PR::PT::FLOAT,
    QVariant::fromValue( (FloatPtr)&margin ), pModel, Model::p_mesh_sliding_threshold )
                 << 0 << 10 );
  vTable.append( PR( "DoubleBorder",  DLG::MESHOPTIONS::LBL::BORDER, PR::PT::BOOL,
    QVariant::fromValue( (BoolPtr)&doubleBorder ), pModel, Model::p_double_mesh_border ) );
  vTable.append( PR( PR::PT::END ) );

  vTable.append( PR( "Topology",      DLG::PRJSETTINGS::TOPOLOGY::TITLE, PR::PT::BEGIN ) );
  vTable.append( PR( "SzRestriction", DLG::PRJSETTINGS::TOPOLOGY::RESTRICT::TITLE, PR::PT::LIST,
    QVariant::fromValue( (IntPtr)&szRestriction ), pModel, Model::p_sz_restriction )
                 << DLG::PRJSETTINGS::TOPOLOGY::RESTRICT::TARGETS  /*0 (carre) */
                 << DLG::PRJSETTINGS::TOPOLOGY::RESTRICT::TCHAINS  /*1 (eirene) */
                 << DLG::PRJSETTINGS::TOPOLOGY::RESTRICT::TPOINTS  /*2*/ );
  vTable.append( PR( PR::PT::END ) );
}

