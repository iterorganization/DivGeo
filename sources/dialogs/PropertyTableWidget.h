#ifndef PROPERTYTABLEWIDGET_H
#define PROPERTYTABLEWIDGET_H

#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>

#include <QString>
#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QPen>
#include <QIcon>

#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>

#include <QColorDialog>

#include <QSignalMapper>

#include "../common/StringConverters.h"
#include "../common/PointConverters.h"
#include "../main/StringsManager.h"
#include "../main/SettingsManager.h"
#include "../main/UserPreferences.h"

class PropertyTableWidget : public QTableWidget
{
  Q_OBJECT

public:

  struct SPreviewTypes {
    enum EPreviewTypes {
      COMMON = 0,
      MARK_HL, TOOLS, GRID_AXES, LABELS, TEXT_NORMALS,
      ELEMS_NODES, SEPARATORS, SOURCES, CHORDS, TEMPLATES,
      MESH, EQUIL, XG_POINTS, XPT, SURFACES, COMMENTS,
      UNKNOWN = 0xFF
    };
    static const int size = 16;
  };
  typedef SPreviewTypes PT;
  typedef SPreviewTypes::EPreviewTypes PreviewType;


private:
  static const int preview_size = 100;
  static const int row_height = 20;

  StringsManager* pSM;
  SettingsManager* pSettings;
  bool updateAll;
  PreviewType previewType;

  QSignalMapper smChangeColor;
  QSignalMapper smChangeValue;
  QSignalMapper smChangeDValue;
  QSignalMapper smChangeCheck;
  QSignalMapper smChangeSelection;

public:
  explicit PropertyTableWidget( StringsManager* _pSM, SettingsManager* _pSettings,
                                bool _updateAll = false, QWidget *parent = 0);

  void AddPenProperty(const QString& _crsName, QPen* _pValue, bool _bColorOnly = false );
  void AddFontProperty(const QString& _crsName, QPen* _pValue );
  void AddColorProperty( const QString& _crsName, QColor* _pValue );
  void AddRangedProperty( const QString& _crsName, int* _pValue, int _minValue, int _maxValue );
  void AddRangedProperty( const QString& _crsName, float* _pValue, float _minValue, float _maxValue );
  void AddVectorProperty( const QString& _crsName, NPoint* _pValue, NPoint _minValue, NPoint _maxValue );
  void AddCheckProperty( const QString& _crsName, bool* _pValue );
  void AddListProperty( const QString& _crsName, int* _pValue, const QStringList& _crItems );
  void AddFlagsProperty( const QString& _crsName, int* _pValue, const QStringList& _crItems );
  void SetupPreview( PreviewType _pt = PT::COMMON );

  bool UpdateAll() const { return updateAll; }

signals:
  void sgnlPropertyChanged();
  
public slots:
  void slotChangeColor( int _row );
  void slotChangeValue( int _row );
  void slotChangeDValue( int _row );
  void slotChangeCheck( int _row );
  void slotChangeSelection( int _row );

public slots:
  void UpdatePreview();
  
};

typedef PropertyTableWidget PTW;
typedef PTW::PT PREVIEW;

#endif // PROPERTYTABLEWIDGET_H
