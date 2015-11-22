#include "PropertyTableWidget.h"
#include "DoubleSpinBoxExt.h"

PropertyTableWidget::PropertyTableWidget(StringsManager* _pSM,
               SettingsManager* _pSettings, bool _updateAll, QWidget *parent ):
  QTableWidget(parent),
  pSM( _pSM ),
  pSettings( _pSettings ),
  updateAll( _updateAll ),
  previewType( PT::COMMON )
{
  horizontalHeader()->hide();
  verticalHeader()->hide();
  setColumnCount( 2 );
  setColumnWidth( 0, 150 );
  setColumnWidth( 1, 150 );
  setFixedWidth( 320 );
  verticalHeader()->setDefaultSectionSize( row_height );
  setShowGrid( false );
  setStyleSheet( "QTableWidget{ border:0px; }" );
  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setSelectionMode( QAbstractItemView::NoSelection );
  setEditTriggers( QAbstractItemView::NoEditTriggers );
  setFocusPolicy( Qt::NoFocus );

  connect( &smChangeColor,  SIGNAL(mapped(int)),
           this,            SLOT(slotChangeColor(int)) );
  connect( &smChangeValue,  SIGNAL(mapped(int)),
           this,            SLOT(slotChangeValue(int)) );
  connect( &smChangeDValue, SIGNAL(mapped(int)),
           this,            SLOT(slotChangeDValue(int)) );
  connect( &smChangeCheck,  SIGNAL(mapped(int)),
           this,            SLOT(slotChangeCheck(int)) );
  connect( &smChangeSelection,  SIGNAL(mapped(int)),
           this,                SLOT(slotChangeSelection(int)) );
}

void PropertyTableWidget::SetupPreview( PreviewType _pt )
{
  if( _pt == PT::COMMON ||
      _pt == PT::UNKNOWN || rowCount() == 0 )
    return;

  previewType = _pt;
  setColumnCount( 3 );
  setColumnWidth( 0, 150 );
  setColumnWidth( 1, 70 );
  setFixedWidth( 240 + preview_size );
  if( rowCount()*row_height < preview_size ) {
    setRowCount( rowCount()+1 );
    setRowHeight( rowCount()-1, preview_size - (rowCount()-1)*row_height );
    setFixedHeight( preview_size + 2 );
  }
  setSpan( 0, 2, rowCount(), 1 );
  setCellWidget( 0, 2, new QLabel() );
  UpdatePreview();
}

void PropertyTableWidget::UpdatePreview()
{
  if( previewType == PT::COMMON ||
      previewType == PT::UNKNOWN )
    return;

  QLabel* pLbl = qobject_cast< QLabel* >( cellWidget( 0, 2 ) );
  if( pLbl == null )
    return;

  UserPreferences::Visual& rvis = static_cast< UserPreferences* >( pSettings )->visual;

  QColor bgColor = rvis.background_color;
  QPixmap *pPixmap = new QPixmap( QSize( preview_size, preview_size ) );
  pPixmap->fill( bgColor );
  QPainter painter( pPixmap );
  painter.setPen( QPen( QBrush( Qt::black ), 1 ) );
  painter.drawRect( 0, 0, pPixmap->width()-2, pPixmap->height()-2 );
  painter.setPen( QPen( QBrush( Qt::white ), 1 ) );
  painter.drawRect( 1, 1, pPixmap->width()-3, pPixmap->height()-3 );
  //QPoint line[3] = { QPoint( 30, 30 ), QPoint( 50, 70 ), QPoint( 70, 30 ) };
  QPoint lineA[3] = { QPoint( 20, 20 ), QPoint( 40, 60 ), QPoint( 80, 80 ) };
  QLine lineB( 20, 20, 80, 80 );
  Point center( 50, 50 );

  switch( previewType ) {
  case PT::TEXT_NORMALS: {
    int normalLength = rvis.normalLen;
    painter.setPen( *rvis.pens[ STYLE::ELEM ] );
    painter.drawLine( 10, 20, 80, 90 );
    painter.setPen( *rvis.pens[ STYLE::NORMAL ] );
    painter.drawLine( 45, 55, 45 + M_1_SQRT2*normalLength,
                              55 - M_1_SQRT2*normalLength );
    painter.setPen( *rvis.pens[ STYLE::TEXT ] );
    QFont font = painter.font();
    font.setPointSize( rvis.pens[ STYLE::TEXT ]->width() );
    painter.setFont( font );
    painter.drawText( 10, 20, "21" );
    painter.drawText( 80, 90, "30" );
    break;
  }
  case PT::TOOLS: {
    painter.setPen( *rvis.pens[ STYLE::TOOL ] );
    painter.drawEllipse( _C( center ), 30, 30 );
    painter.drawLine( _C( center ), QPoint( 50, 20 ) );
    break;
  }
  case PT::MARK_HL: {
    QLine lines_mk[] = { QLine( 10, 20, 20, 60 ), QLine( 20, 60, 50, 80 ) };
    QLine lines_hl[] = { QLine( 50, 20, 60, 60 ), QLine( 60, 60, 90, 80 ) };

    painter.setPen( *rvis.pens[ STYLE::MARK ] );
    painter.drawLines( lines_mk, 2 );
    painter.setPen( *rvis.pens[ STYLE::HIGHLIGHT ] );
    painter.drawLines( lines_hl, 2 );
    break;
  }
  case PT::GRID_AXES: {
    QLine lines_grid[] = { QLine( 20, 10, 20, 90 ), QLine( 60, 10, 60, 90 ),
                           QLine( 80, 10, 80, 90 ), QLine( 10, 20, 90, 20 ),
                           QLine( 10, 40, 90, 40 ), QLine( 10, 80, 90, 80 ) };
    Point arrowSize = rvis.arrowSize;
    QLine lines_axes[] = { QLine( 40, 10, 40, 90 ), QLine( 10, 60, 90, 60 ),
                           QLine( 40, 10, 40 - arrowSize.x, 10 + arrowSize.y ),
                           QLine( 40, 10, 40 + arrowSize.x, 10 + arrowSize.y ),
                           QLine( 90, 60, 90 - arrowSize.y, 60 + arrowSize.x ),
                           QLine( 90, 60, 90 - arrowSize.y, 60 - arrowSize.x ) };
    painter.setPen( *rvis.pens[ STYLE::GRID ] );
    painter.drawLines( lines_grid, 6 );
    painter.setPen( *rvis.pens[ STYLE::AXES ] );
    painter.drawLines( lines_axes, 6 );
    painter.setPen( *rvis.pens[ STYLE::GRIDTEXT ] );
    QFont font = painter.font();
    font.setPointSize( rvis.pens[ STYLE::GRIDTEXT ]->width() );
    painter.setFont( font );
    painter.drawText( QPoint( 42, 40 ), "y'10" );
    painter.drawText( QPoint( 62, 70 ), "x'10" );
    break;
  }
  case PT::LABELS: {
    painter.setPen( *rvis.pens[ STYLE::ELEM ] );
    painter.drawLine( 20, 40, 60, 20 );
    painter.setPen( *rvis.pens[ STYLE::LABEL ] );
    QFont font = painter.font();
    font.setPointSize( rvis.labelTextSize );
    painter.setFont( font );
    Point offset = rvis.labelOffset;
    QLine line( 40, 30, 40 + offset.x, 30 + offset.y );
    painter.drawLine( line );
    painter.drawText( line.p2(), "Label" );
    break;
  }
  case PT::COMMENTS: {
    /*painter.setPen( *rvis.pens[ STYLE::ELEM ] );
    painter.drawLine( 20, 40, 60, 20 );*/
    painter.setPen( *rvis.pens[ STYLE::COMMENT ] );
    QFont font = painter.font();
    font.setPointSize( rvis.commentTextSize );
    painter.setFont( font );
    QFontMetrics fm( font );
    QRect rect = fm.boundingRect( "Text" );
    rect.moveCenter( QPoint( 20, 20 ) );
    QLine line( 80, 80, 20, 20 );
    painter.drawLine( line );
    painter.drawRect( rect );
    rect.setTopLeft( rect.topLeft() + QPoint( 1, 1 ) );
    painter.fillRect( rect, QBrush( rvis.background_color ) );
    painter.drawText( rect, "Text" );
    break;
  }
  case PT::ELEMS_NODES: {
    painter.setPen( *rvis.pens[ STYLE::ELEM ] );
    painter.drawLine( lineA[0], lineA[1] );
    painter.drawLine( lineA[1], lineA[2] );
    ulong nodeRadius = rvis.nodeRadius;
    QPoint nodeRadiusVect( nodeRadius, nodeRadius );
    QRect nodeRects[] =
    { QRect( lineA[0] - nodeRadiusVect, lineA[0] + nodeRadiusVect ),
      QRect( lineA[1] - nodeRadiusVect, lineA[1] + nodeRadiusVect ),
      QRect( lineA[2] - nodeRadiusVect, lineA[2] + nodeRadiusVect ) };
    painter.setPen( *rvis.pens[ STYLE::NODE ] );
    if( rvis.useSquares ) {
      for( ulong c = 0; c < 3; c++ )
        painter.drawRect( nodeRects[c] );
    }
    else {
      for( ulong c = 0; c < 3; c++ )
        painter.drawEllipse( nodeRects[c] );
    }
    break;
  }
  case PT::SEPARATORS:
    painter.setPen( *rvis.pens[ STYLE::SEPARATOR ] );
    painter.drawLine( lineB );
    break;

  case PT::CHORDS:
    painter.setPen( *rvis.pens[ STYLE::CHORD ] );
    painter.drawLine( lineB );
    break;

  case PT::SOURCES: {
    ulong srcRadius = rvis.sourceRadius;
    Point rp( (int)(0.886 * srcRadius), (int)(0.500 * srcRadius) );
    Point r0( 0., srcRadius );

    QLineF lines[3];
    lines[0] = QLineFromPoints( center - r0, center + r0 );
    lines[1] = QLineFromPoints( center - rp, center + rp );
    rp.x = -rp.x;
    lines[2] = QLineFromPoints( center - rp, center + rp );

    painter.setPen( *rvis.pens[ STYLE::SOURCE ] );
    painter.drawLines( lines, 3 );
    break;
  }
  case PT::TEMPLATES: {
    painter.setPen( *rvis.pens[ STYLE::TEMPLATE ] );
    painter.drawLine( lineA[0], lineA[1] );
    painter.drawLine( lineA[1], lineA[2] );
    break;
  }
  case PT::MESH: {
    QRect rect_Elem = QRect( 10, 30, 20, 40 );
    QLine lines_Irr[] = { QLine( 30, 30, 70, 30 ), QLine( 70, 30, 70, 70 ) };
    QLine lines_IrrHL[] = { QLine( 50, 30, 90, 50 ), QLine( 30, 70, 90, 50 ),
                            QLine( 70, 70, 90, 50 ) };
    double radius_l = rvis.meshCellCenterLength * M_SQRT2;
    QPointF radius( radius_l, radius_l );
    QPointF radius_inv( radius_l, -radius_l );
    QPointF centers[3] = { QPointF( 20, 50 ), QPointF( 63, 45 ), QPointF( 69, 45 ) };
    QLineF lines_Center[] =
    { QLineF( centers[0] + radius,      centers[0] - radius ),
      QLineF( centers[0] + radius_inv,  centers[0] - radius_inv ),
      QLineF( centers[1] + radius,      centers[1] - radius ),
      QLineF( centers[1] + radius_inv,  centers[1] - radius_inv ),
      QLineF( centers[2] + radius,      centers[2] - radius ),
      QLineF( centers[2] + radius_inv,  centers[2] - radius_inv ) };

    /*int penWidth = rvis.pens[ STYLE::MESHELEMENT ]->width();
    rvis.pens[ STYLE::MESHELEMENT_IRREG ]->setWidth( penWidth );
    rvis.pens[ STYLE::MESHELEMENT_IRREG_HL ]->setWidth( penWidth );*/

    painter.setPen( *rvis.pens[ STYLE::MESHELEMENT ] );
    painter.drawRect( rect_Elem );
    painter.setPen( *rvis.pens[ STYLE::MESHELEM_IRREG ] );
    painter.drawLines( lines_Irr, 2 );
    painter.setPen( *rvis.pens[ STYLE::MESHELEM_IRREG_HL ] );
    painter.drawLines( lines_IrrHL, 3 );
    painter.setPen( *rvis.pens[ STYLE::MESHCELLCENTER ] );
    painter.drawLines( lines_Center, 6 );
    break;
  }
  case PT::EQUIL: {
    QRect rect_pos( 20, 50, 30, 30 );
    QRect rect_neg( 50, 20, 30, 30 );
    QLine lines_pos[] = { QLine( 20, 20, 20, 50 ), QLine( 50, 80, 80, 80 ) };
    QLine lines_neg[] = { QLine( 20, 20, 50, 20 ), QLine( 80, 50, 80, 80 ) };

    int penWidth = rvis.pens[ STYLE::EQUIL_NEG ]->width();
    rvis.pens[ STYLE::EQUIL_POS ]->setWidth( penWidth );

    painter.setPen( *rvis.pens[ STYLE::EQUIL_NEG ] );
    painter.drawRect( rect_neg );
    painter.drawLines( lines_neg, 2 );
    painter.setPen( *rvis.pens[ STYLE::EQUIL_POS ] );
    painter.drawRect( rect_pos );
    painter.drawLines( lines_pos, 2 );
    break;
  }
  case PT::XG_POINTS: {
    QLine line_xps( 10, 20, 80, 90 );
    QPoint pos_gpx[] = { QPoint( 20, 30 ), QPoint( 35, 45 ),
                         QPoint( 55, 65 ), QPoint( 70, 80 ) };
    int gpxHalfLength = rvis.gridPointLen/2;
    QPointF gpxNormal( M_1_SQRT2 * gpxHalfLength, -M_1_SQRT2 * gpxHalfLength );
    QLineF lines_gpx[4];
    for( ulong c = 0; c < 4; c++ )
      lines_gpx[c] = QLineF( pos_gpx[c] + gpxNormal, pos_gpx[c] - gpxNormal );

    const double gpsAngle_rad = 20. * DEG2RAD;
    const double baseAngle_rad = 45. * DEG2RAD;
    double arrowLength = rvis.arrowSize.y;
    double cos1 = cos( gpsAngle_rad + baseAngle_rad );
    double sin1 = sin( gpsAngle_rad + baseAngle_rad );
    double cos2 = cos( gpsAngle_rad - baseAngle_rad );
    double sin2 = sin( gpsAngle_rad - baseAngle_rad );

    Point p1( 40, 50 );
    Point pLen( arrowLength, 0 );
    Point p12( p1.x + pLen.x*cos1 - pLen.y*sin1,
               p1.y + pLen.x*sin1 + pLen.y*cos1 );
    Point p22( p1.x + pLen.x*cos2 + pLen.y*sin2,
               p1.y - pLen.x*sin2 + pLen.y*cos2 );
    Point pT2( p1.x + arrowLength, p1.y - arrowLength );

    QLineF lines_gps[] =  { QLineFromPoints( p1, p12 ),
                            QLineFromPoints( p1, p22 ),
                            QLineFromPoints( p1, pT2 ) };
    QString text = "GPS";

    painter.setPen( *rvis.pens[ STYLE::XPOINT ] );
    painter.drawLine( line_xps );
    painter.setPen( *rvis.pens[ STYLE::GPOINT ] );
    painter.drawLines( lines_gpx, 4 );
    painter.drawLines( lines_gps, 3 );
    painter.setPen( *rvis.pens[ STYLE::TEXT ] );
    painter.drawText( lines_gps[2].p2(), text );
    break;
  }
  case PT::XPT: {
    QRect rect_xpt( 30, 40, 40, 30 );
    QLine lines_xpt[] = { QLine( 50, 10, 50, 40 ), QLine( 50, 70, 50, 90 ),
                          QLine( 10, 50, 30, 50 ), QLine( 70, 50, 90, 50 ) };

    painter.setPen( *rvis.pens[ STYLE::XPOINTTEST ] );
    painter.drawLines( lines_xpt, 4 );
    painter.setPen( *rvis.pens[ STYLE::XPOINTTEST_BOUND ] );
    painter.drawRect( rect_xpt );
    break;
  }
  case PT::SURFACES: {
    QLine lines_sx[] = { QLine( 20, 10, 20, 90 ), QLine( 30, 10, 25, 90 ),
                         QLine( 50, 10, 35, 90 ), QLine( 80, 10, 50, 90 ) };

    painter.setPen( *rvis.pens[ STYLE::SURFACE] );
    painter.drawLines( lines_sx, 4 );
    break;
  }
  default:
    break;
  }

  painter.end();
  pLbl->setPixmap( *pPixmap );
}

void PropertyTableWidget::AddPenProperty( const QString& _crsName, QPen* _pValue,
                                          bool _bColorOnly )
{
  QColor color = _pValue->color();
  int width = _pValue->width();
  QString sName = _crsName.isEmpty() ? "" : _crsName + " ";
  AddColorProperty( sName + SM_DLG( DLG::SETTINGS::VISUAL::COLOR ), &color );
  item( rowCount() - 1, 0 )->setData( Qt::UserRole, QVariant::fromValue( (QPenPtr)_pValue ) );
  if( !_bColorOnly ) {
    AddRangedProperty( sName + SM_DLG( DLG::SETTINGS::VISUAL::WIDTH ), &width, 0, 10 );
    item( rowCount() - 1, 0 )->setData( Qt::UserRole, QVariant( 0 ) );
  }
}

void PropertyTableWidget::AddFontProperty(const QString& _crsName, QPen* _pValue )
{
  QColor color = _pValue->color();
  int width = _pValue->width();
  QString sName = _crsName.isEmpty() ? "" : _crsName + " ";
  AddColorProperty( sName + SM_DLG( DLG::SETTINGS::VISUAL::COLOR ), &color );
  item( rowCount() - 1, 0 )->setData( Qt::UserRole, QVariant::fromValue( (QPenPtr)_pValue ) );
  AddRangedProperty( sName + SM_DLG( DLG::SETTINGS::VISUAL::FONTSIZE ), &width, 0, 30 );
  item( rowCount() - 1, 0 )->setData( Qt::UserRole, QVariant( 0 ) );
}

void PropertyTableWidget::AddColorProperty(
    const QString& _crsName, QColor* _pValue ) {
  int row = rowCount();
  setRowCount( row + 1 );
  setFixedHeight( (row + 1) * row_height );

  QTableWidgetItem* pLineItem = new QTableWidgetItem();
  pLineItem->setText( _crsName );
  pLineItem->setData( Qt::UserRole, QVariant::fromValue( (QColorPtr)_pValue ) );
  this->setItem( row, 0, pLineItem );

  QPixmap pxmp( QSize( 16, 16 ) );
  pxmp.fill( *_pValue );
  QPushButton* pBtn = new QPushButton( QIcon( pxmp ), "" );
  pBtn->setFixedSize( row_height-2, row_height-2 );
  connect( pBtn, SIGNAL(clicked()), &smChangeColor, SLOT(map()) );
  smChangeColor.setMapping( (QObject*)pBtn, row );
  setCellWidget( row, 1, pBtn );
}

void PropertyTableWidget::AddRangedProperty(
    const QString& _crsName, int* _pValue, int _minValue, int _maxValue ) {
  int row = rowCount();
  setRowCount( row + 1 );
  setFixedHeight( (row + 1) * row_height );

  QTableWidgetItem* pLineItem = new QTableWidgetItem();
  pLineItem->setText( _crsName );
  pLineItem->setData( Qt::UserRole, QVariant::fromValue( _pValue ) );
  this->setItem( row, 0, pLineItem );

  QSpinBox* pSb = new QSpinBox();
  pSb->setFixedSize( 50, row_height-2 );
  pSb->setRange( _minValue, _maxValue );
  pSb->setValue( *_pValue );
  connect( pSb, SIGNAL(valueChanged(int)), &smChangeValue, SLOT(map()) );
  smChangeValue.setMapping( (QObject*)pSb, row );
  this->setCellWidget( row, 1, pSb );
}

void PropertyTableWidget::AddRangedProperty(
    const QString& _crsName, float* _pValue, float _minValue, float _maxValue )
{
  int row = rowCount();
  setRowCount( row + 1 );
  setFixedHeight( (row + 1) * row_height );

  QTableWidgetItem* pLineItem = new QTableWidgetItem();
  pLineItem->setText( _crsName );
  pLineItem->setData( Qt::UserRole, QVariant::fromValue( _pValue ) );
  this->setItem( row, 0, pLineItem );

  DoubleSpinBoxExt /*1409*/* pSb = new DoubleSpinBoxExt /*1409*/();
  pSb->setFixedSize( 50, row_height-2 );
  pSb->setRange( _minValue, _maxValue );
  pSb->setValue( *_pValue );
  connect( pSb, SIGNAL(valueChanged(double)), &smChangeDValue, SLOT(map()) );
  smChangeDValue.setMapping( (QObject*)pSb, row );
  this->setCellWidget( row, 1, pSb );
}

void PropertyTableWidget::AddVectorProperty( const QString& _crsName, NPoint* _pValue, NPoint _minValue, NPoint _maxValue )
{
  NPoint tmp = *_pValue;
  QString sName = _crsName.isEmpty() ? "" : _crsName + " ";
  AddRangedProperty( sName + SM_DLG( DLG::SETTINGS::VISUAL::X ), &tmp.x, _minValue.x, _maxValue.x );
  item( rowCount() - 1, 0 )->setData( Qt::UserRole, QVariant::fromValue( (NPointPtr)_pValue ) );
  AddRangedProperty( sName + SM_DLG( DLG::SETTINGS::VISUAL::Y ), &tmp.y, _minValue.y, _maxValue.y );
  item( rowCount() - 1, 0 )->setData( Qt::UserRole, QVariant( 0 ) );
}

void PropertyTableWidget::AddCheckProperty( const QString& _crsName, bool* _pValue )
{
  int row = rowCount();
  setRowCount( row + 1 );
  setFixedHeight( (row + 1) * row_height );

  QTableWidgetItem* pLineItem = new QTableWidgetItem();
  pLineItem->setText( _crsName );
  pLineItem->setData( Qt::UserRole, QVariant::fromValue( _pValue ) );
  this->setItem( row, 0, pLineItem );

  QCheckBox* pCb = new QCheckBox();
  pCb->setFixedSize( row_height-2, row_height-2 );
  pCb->setChecked( *_pValue );
  connect( pCb, SIGNAL(toggled(bool)), &smChangeCheck, SLOT(map()) );
  smChangeCheck.setMapping( (QObject*)pCb, row );
  this->setCellWidget( row, 1, pCb );
}

void PropertyTableWidget::AddListProperty( const QString& _crsName, int* _pValue, const QStringList& _crItems )
{
  int row = rowCount();
  setRowCount( row + 1 );
  setFixedHeight( (row + 1) * row_height );

  QTableWidgetItem* pLineItem = new QTableWidgetItem();
  pLineItem->setText( _crsName );
  pLineItem->setData( Qt::UserRole, QVariant::fromValue( _pValue ) );
  this->setItem( row, 0, pLineItem );

  QComboBox* pCmb = new QComboBox();
  int c = 0;
  foreach( const QString& crsItem, _crItems ) {
    if( !crsItem.isEmpty() )
      pCmb->addItem( crsItem, QVariant( c ) );
    c++;
  }

  pCmb->setCurrentIndex( pCmb->findData( *_pValue ) );
  connect( pCmb, SIGNAL(currentIndexChanged(int)), &smChangeSelection, SLOT(map()) );
  smChangeSelection.setMapping( (QObject*)pCmb, row );
  this->setCellWidget( row, 1, pCmb );
}

void PropertyTableWidget::AddFlagsProperty( const QString& _crsName, int* _pValue, const QStringList& _crItems )
{
  int row = rowCount();
  setRowCount( row + 1 );
  setFixedHeight( (row + 1) * row_height );

  QTableWidgetItem* pLineItem = new QTableWidgetItem();
  pLineItem->setText( _crsName );
  pLineItem->setData( Qt::UserRole, QVariant::fromValue( _pValue ) );
  this->setItem( row, 0, pLineItem );

  int flag = 1;
  foreach( const QString& crsItem, _crItems ) {
    if( !crsItem.isEmpty() ) {
      row++;
      setRowCount( row + 1 );
      setFixedHeight( (row + 1) * row_height );

      QTableWidgetItem* pCheckItem = new QTableWidgetItem();
      pCheckItem->setText( crsItem );
      pCheckItem->setData( Qt::UserRole, QVariant::fromValue( flag ) );
      this->setItem( row, 0, pCheckItem );

      QCheckBox* pCb = new QCheckBox();
      pCb->setFixedSize( row_height-2, row_height-2 );
      pCb->setChecked( HasAnyFlag( *_pValue, flag ) );
      connect( pCb, SIGNAL(toggled(bool)), &smChangeCheck, SLOT(map()) );
      smChangeCheck.setMapping( (QObject*)pCb, row );
      this->setCellWidget( row, 1, pCb );
    }
    flag <<= 1;
  }
}

void PropertyTableWidget::slotChangeColor( int _row )
{
  QTableWidgetItem* pItem = item( _row, 0 );
  QVariant userData = pItem->data( Qt::UserRole );
  int userType = userData.userType();
  const int colorType = QMetaType::type( "QColorPtr" );
  const int penType = QMetaType::type( "QPenPtr" );
  QColor color;
  QColorPtr pColor = null;
  QPenPtr pPen = null;
  if( userType == colorType ) {
    pColor = userData.value< QColorPtr >();
    color = *pColor;
  }
  else if( userType == penType ) {
    pPen = userData.value< QPenPtr >();
    color = pPen->color();
  }

  QColorDialog dialog( color, this );
  if( dialog.exec() != QDialog::Accepted ||
      color == dialog.selectedColor() )
    return;
  color = dialog.selectedColor();

  if( userType == colorType )
    *pColor = color;
  else if( userType == penType )
    pPen->setColor( color );

  QPushButton* pBtn = qobject_cast< QPushButton* >( cellWidget( _row, 1 ) );
  QPixmap pxmp( QSize( pBtn->width()-4, pBtn->height()-4 ) );
  pxmp.fill( color );
  pBtn->setIcon( QIcon( pxmp ) );
  UpdatePreview();
  emit sgnlPropertyChanged();
}

void PropertyTableWidget::slotChangeDValue( int _row )
{
  QTableWidgetItem* pItem = item( _row, 0 );
  int userType = pItem->data( Qt::UserRole ).userType();
  const int floatType = QMetaType::type( "FloatPtr" );
  DoubleSpinBoxExt /*1409*/* pSb = qobject_cast< DoubleSpinBoxExt /*1409*/* >( cellWidget( _row, 1 ) );
  if( userType != floatType )
    return;
  float* pValue = pItem->data( Qt::UserRole ).value< FloatPtr >();
  *pValue = pSb->value();
  UpdatePreview();
  emit sgnlPropertyChanged();
}

void PropertyTableWidget::slotChangeValue( int _row )
{
  QTableWidgetItem* pItem = item( _row, 0 );
  int userType = pItem->data( Qt::UserRole ).userType();
  const int penType = QMetaType::type( "QPenPtr" );
  const int pointType = QMetaType::type( "NPointPtr" );
  const int intType = QMetaType::type( "IntPtr" );
  QSpinBox* pSb = qobject_cast< QSpinBox* >( cellWidget( _row, 1 ) );
  if( userType == intType ) {
    int* pValue = pItem->data( Qt::UserRole ).value< IntPtr >();
    *pValue = pSb->value();
  }
  else if( userType == pointType ) {
    NPoint* pPoint = pItem->data( Qt::UserRole ).value< NPointPtr >();
    if( pPoint == null )
      return;
    pPoint->x = pSb->value();
  }
  else {
    pItem = item( _row-1, 0 );
    if( pItem == null )
      return;
    userType = pItem->data( Qt::UserRole ).userType();
    if( userType == penType ) {
      QPen* pPen = pItem->data( Qt::UserRole ).value< QPenPtr >();
      if( pPen == null )
        return;
      pPen->setWidth( pSb->value() );
    }
    else /*if( userType == pointType )*/ {
      NPoint* pPoint = pItem->data( Qt::UserRole ).value< NPointPtr >();
      if( pPoint == null )
        return;
      pPoint->y = pSb->value();
    }
  }
  UpdatePreview();
  emit sgnlPropertyChanged();
}

void PropertyTableWidget::slotChangeCheck( int _row )
{
  QTableWidgetItem* pItem = item( _row, 0 );
  int userType = pItem->data( Qt::UserRole ).userType();
  QCheckBox* pCb = qobject_cast< QCheckBox* >( cellWidget( _row, 1 ) );
  if( userType == QMetaType::type( "BoolPtr" ) ) {
    bool* pValue = pItem->data( Qt::UserRole ).value< BoolPtr >();
    *pValue = pCb->isChecked();
  }
  else if( pItem->data( Qt::UserRole ).type() == QVariant::Int ) {
    int flag = pItem->data( Qt::UserRole ).toInt();
    bool value = pCb->isChecked();
    int intPtrType = QMetaType::type( "IntPtr" );
    QVariant userData;
    while( _row >= 0 ) {
      _row--;
      userData = item( _row, 0 )->data( Qt::UserRole );
      if( userData.userType() == intPtrType )
        break;
    }
    int* pValue = userData.value< IntPtr >();
    SETFLAG( *pValue, flag, value );
  }
  else
    return;
  UpdatePreview();
  emit sgnlPropertyChanged();
}

void PropertyTableWidget::slotChangeSelection( int _row )
{
  QTableWidgetItem* pItem = item( _row, 0 );
  int userType = pItem->data( Qt::UserRole ).userType();
  QComboBox* pCmb = qobject_cast< QComboBox* >( cellWidget( _row, 1 ) );
  if( userType != QMetaType::type( "IntPtr" ) )
    return;
  int* pValue = pItem->data( Qt::UserRole ).value< IntPtr >();
  *pValue = pCmb->itemData( pCmb->currentIndex() ).toInt();
  UpdatePreview();
  emit sgnlPropertyChanged();
}
