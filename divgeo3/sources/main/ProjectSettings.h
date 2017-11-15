#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include "SettingsManager.h"
#include "../model/core/Model.h"

class ProjectSettings:
    public SettingsManager
{
  ModelPtr pModel;

  int outputMode;
  int outputFlags;

  int slidingMode;
  float margin;
  bool doubleBorder;

  int szRestriction;

public:
  ProjectSettings( ModelPtr _pModel );
};

#endif // PROJECTSETTINGS_H
