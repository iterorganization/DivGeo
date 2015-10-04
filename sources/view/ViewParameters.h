#ifndef VIEWPARAMETERS_H
#define VIEWPARAMETERS_H

/******************************************************************************
 *
 *  This file is obsolete and stay just for information.
 *  Use class SettingsManager instead.
 *
 *****************************************************************************/

#include <QRgb>
#include <QPen>

#include "../common/Point.h"
#include "../common/mathlib.h"
#include "Modes.h"

#define frgb(fr,fg,fb) qRgb((int)(255*fr),(int)(255*fg),(int)(255*fb));

struct SViewColors
{
  QRgb background, highlight, errorHighlight, node, elem, surface,
  highlightRect, template_, normal, mark, xPoint, equil1, equil2,
  axes, grid, gridText, elemNumber, viewLabel, sonnetData, gridPoint,
  separator, source, chord, xPointTest1, xPointTest2, xPointSeg, meshElement1,
  meshElement2, meshElement_x1, meshElement_x2, meshCellCenter1, meshCellCenter2,
  lTool, mTool, rTool, iTool;

  ulong noAlfaMask;
};

struct SViewWidthes
{
  ulong highlight, node, elem, surface, highlightRect, template_, normal,
  mark, xPoint, equil, axes, grid, gridText, elemNumber, viewLabel,
  sonnetData, gridPoint, separator, source, chord, xPointTest1, xPointTest2,
  xPointSeg, meshElement1, meshElement2, meshCellCenter1, meshCellCenter2;
};

struct SViewSettings
{
  ulong nodeRadius, equilLen, normalLen, gridPointLen, sourceR, gridCellMax,
  showPictureIncr, meshCellCenterLength, meshPointRadius, separatrixSegEndLen;

  Point minZoom, labelOffset, arrowSize;

  bool useSquares;
};

struct SViewDelays
{
  ulong update, updateVars, updateInfo, updateMouse, exposure;
};

class ViewParameters
{
  friend class CViewWnd;

  SViewColors colors;
  SViewWidthes widthes;
  SViewDelays delays;
  SViewSettings settings;

  QVector< QPen > pens;
  QPen pen_default;

public:
  ViewParameters();

  const SViewColors& Colors() const { return colors; }
  const SViewWidthes& Widthes() const { return widthes; }
  const SViewDelays& Delays() const { return delays; }
  const SViewSettings& Settings() const { return settings; }

  const QPen &Pen( ViewMode mode ) const;

private:
  void InitDefaultColors();
  void InitDefaultWidthes();
  void InitDefaultDelays();
  void InitDefaultSettings();
  void InitPens();
};

typedef ViewParameters* ViewParametersPtr;

#endif // VIEWPARAMETERS_H
