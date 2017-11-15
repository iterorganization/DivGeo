QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#unix:!mac { LIBS += -Wl,-rpath=\\\$$ORIGIN/libs }

QMAKE_CXXFLAGS += -Wall -posix -fno-rtti

RC_FILE = dg.rc
TARGET = dg3
TEMPLATE = app

HEADERS += \
    main/MainWnd.h \
    main/Console.h \
    main/ModelInfo.h \
    main/ResourcesManager.h \
    main/ModelViewManager.h \
    main/ResizableWidget.h \
    main/ModelViewProxy.h \
    main/StringsManager.h \
    main/SettingsManager.h \
    main/SessionData.h \
    main/ToolsPanel.h \
    main/MouseToolPushButton.h \
    main/HelpManager.h \
    main/UserPreferences.h \
    main/ProjectSettings.h \
    main/ModelTree.h \
    main/ActionStackInfo.h \
    main/ModelAgent.h \
    main/DgDataModel.h \
    main/DgDataModel.Token.h \
    main/DgDataModel.Tree.h \
    main/DgDataModel.Token.hpp \
    main/DgDataModel.TokenNames.h \
\
    view/EditorWnd.h \
    view/ViewWnd.h \
    view/ToolData.h \
    view/Modes.h \
    view/ViewScene.h \
    view/ViewItemsDecl.h \
    view/ViewTopItemsDecl.h \
    view/ViewParameters.h \
    view/IViewScene.h \
    view/ViewTopScene.h \
    view/ViewStateRecord.h \
    view/ViewState.h \
\
    view/items/IViewItem.h \
    view/items/ViewMeshElementItem.h \
    view/items/ViewMeshCellItem.h \
    view/items/ViewElementItem.h \
    view/items/ViewChordItem.h \
    view/items/ViewEquilItem.h \
    view/items/ViewNodeItem.h \
    view/items/ViewTemplateItem.h \
    view/items/ViewXPointTestItem.h \
    view/items/ViewXPointSegItem.h \
    view/items/ViewGridPointExItem.h \
    view/items/ViewGridPointSegItem.h \
    view/items/ViewSurfaceExItem.h \
    view/items/ViewAxesItem.h \
    view/items/ViewGridItem.h \
    view/items/ViewHighlightedRectItem.h \
    view/items/ViewRotateToolItem.h \
    view/items/ViewSonnetDataItem.h \
    view/items/ViewSeparatorItem.h \
    view/items/ViewSourceItem.h \
    view/items/ViewMeshPointQuasiItem.h \
    view/items/ViewShapeItem.h \
    view/items/ViewLabelItem.h \
    view/items/ViewRadialGridItem.h \
    view/items/ViewChordRadialTopItem.h \
    view/items/ViewElementRadialTopItem.h \
    view/items/ViewMeshRadialTopItem.h \
    view/items/ViewSourceRadialTopItem.h \
    view/items/ViewChordTopItem.h \
    view/items/ViewLegendItem.h \
    view/items/ViewCommentItem.h \
\
    model/core/Model.h \
    model/core/IAction.h \
    model/core/IComponent.h \
    model/core/CoreConstants.h \
    model/core/Actions.h \
    model/core/ListContainer.h \
    model/core/IValueHolder.h \
    model/core/IVarItem.h \
    model/core/IVarOrigin.h \
    model/core/IObject.h \
    model/core/IObject.t.hpp \
    model/core/IManager.h \
    model/core/DgTokens.h \
    model/core/ActionStack.h \
\
    common/strlib.h \
    common/mathlib.h \
    common/Types.h \
    common/Point.h \
    common/Macroes.h \
    common/Includes.h \
    common/StdStringList.h \
    common/Version.h \
    common/ShowFlags.h \
    common/Resources.Dialogs.h \
    common/Resources.Menu.h \
    common/Resources.Msg.h \
    common/Resources.Gui.h \
    common/Resources.Macro.h \
    common/Constants.h \
    common/ResourceTree.h \
    common/Spline.h \
    common/PointConverters.h \
    common/StringConverters.h \
    common/Metatypes.h \
    common/ListExt.h \
    common/Resources.Help.h \
    common/Resources.Log.h \
    common/IModelAgent.h \
    common/UpdateInfo.h \
\
    model/diagnostics/DiagnosticsDecl.h \
    model/diagnostics/Sources.h \
    model/diagnostics/Comment.h \
\
    model/flux/FluxModel.h \
    model/flux/Topology.h \
    model/flux/CreatorId.h \
    model/flux/XPointTest.h \
    model/flux/XPointSeg.h \
    model/flux/SurfaceZone.h \
    model/flux/SurfaceEx.h \
    model/flux/GridPointSeg.h \
    model/flux/GridPointEx.h \
    model/flux/EquilDecl.h \
    model/flux/Equil.h \
\
    model/material/MaterialDecl.h \
    model/material/Template.h \
    model/material/Node.h \
    model/material/Element.h \
    model/material/Element.t.hpp \
    model/material/Structure.h \
    model/material/Chords.h \
    model/material/Separator.h \
\
    model/mesh/MeshPoint.h \
    model/mesh/MeshElement.h \
    model/mesh/MeshDecl.h \
    model/mesh/MeshConstants.h \
    model/mesh/MeshCell.h \
    model/mesh/Mesh.h \
    model/mesh/SonnetDecl.h \
    model/mesh/SonnetData.h \
\
    model/vars/VarsManager.h \
    model/vars/VarSetDef.h \
    model/vars/VarSet.h \
    model/vars/VarDef.h \
    model/vars/VarsDecl.h \
    model/vars/VarConst.h \
    model/vars/Var.h \
\
    dialogs/DistrGraph.h \
    dialogs/DialogsDecl.h \
    dialogs/DlgMoveRotate.h \
    dialogs/DlgCreateChord.h \
    dialogs/DlgCreateGridPoint.h \
    dialogs/DlgCreatePoint.h \
    dialogs/DlgImportTopology.h \
    dialogs/DlgExportElements.h \
    dialogs/DlgCreateSurface.h \
    dialogs/DlgGluePoints.h \
    dialogs/DlgGlueElems.h \
    dialogs/DlgStretch.h \
    dialogs/DlgSetAngle.h \
    dialogs/DlgInvalidVars.h \
    dialogs/DlgVarsEdit.h \
    dialogs/DlgVsdList.h \
    dialogs/DlgCreateName.h \
    dialogs/DlgVsdEdit.h \
    dialogs/DlgVdEdit.h \
    dialogs/DlgVdHelpEdit.h \
    dialogs/DlgTopology.h \
    dialogs/DlgMeshHeader.h \
    dialogs/DlgOutputMode.h \
    dialogs/DlgMeshOptions.h \
    dialogs/DlgStatictics.h \
    dialogs/DlgSettings.h \
    dialogs/Assistant.h \
    dialogs/DlgAbout.h \
    dialogs/DlgVarHelp.h \
    dialogs/DlgNewModel.h \
    dialogs/DlgProgress.h \
    dialogs/DlgCommentEdit.h \
    dialogs/PropertyTableWidget.h \
    dialogs/DoubleSpinBoxExt.h

SOURCES += \
    main/main.cpp \
    main/MainWnd.Tools.cpp \
    main/MainWnd.Slots.cpp \
    main/MainWnd.Draw.cpp \
    main/MainWnd.cpp \
    main/Console.cpp \
    main/ModelInfo.cpp \
    main/MainWnd.Slots.File.cpp \
    main/MainWnd.Slots.Edit.cpp \
    main/ResourcesManager.cpp \
    main/MainWnd.Slots.View.cpp \
    main/MainWnd.Slots.Command.cpp \
    main/ModelViewManager.cpp \
    main/MainWnd.Slots.Variables.cpp \
    main/MainWnd.Slots.Options.cpp \
    main/ModelViewProxy.cpp \
    main/SettingsManager.cpp \
    main/StringsManager.Log.cpp \
    main/StringsManager.Menus.cpp \
    main/StringsManager.Strings.cpp \
    main/StringsManager.Dialogs.cpp \
    main/StringsManager.Gui.cpp \
    main/ToolsPanel.cpp \
    main/HelpManager.cpp \
    main/UserPreferences.cpp \
    main/ProjectSettings.cpp \
    main/ModelTree.cpp \
    main/ModelTree.Branches.cpp \
    main/ActionStackInfo.cpp \
    main/ModelAgent.cpp \
    main/DgDataModel.cpp \
    main/DgDataModel.Load.cpp \
    main/DgDataModel.Tree.cpp \
    main/DgDataModel.TokenNames.cpp \
\
    view/ViewWnd.Draw.cpp \
    view/ViewWnd.cpp \
    view/ViewScene.cpp \
    view/ViewParameters.cpp \
    view/IViewScene.cpp \
    view/ViewTopScene.cpp \
    view/EditorWnd.cpp \
    view/ViewWnd.Mouse.cpp \
    view/ViewWnd.Tools.Geometry.cpp \
    view/ViewWnd.Tools.Helpers.cpp \
    view/ViewWnd.Tools.Objects.cpp \
    view/ViewWnd.Tools.Transform.cpp \
    view/ViewWnd.Tools.Mesh.cpp \
\
    view/items/IViewItem.cpp \
    view/items/ViewMeshElementItem.cpp \
    view/items/ViewMeshCellItem.cpp \
    view/items/ViewElementItem.cpp \
    view/items/ViewChordItem.cpp \
    view/items/ViewEquilItem.cpp \
    view/items/ViewNodeItem.cpp \
    view/items/ViewTemplateItem.cpp \
    view/items/ViewXPointTestItem.cpp \
    view/items/ViewXPointSegItem.cpp \
    view/items/ViewGridPointExItem.cpp \
    view/items/ViewGridPointSegItem.cpp \
    view/items/ViewSurfaceExItem.cpp \
    view/items/ViewAxesItem.cpp \
    view/items/ViewGridItem.cpp \
    view/items/ViewHighlightedRectItem.cpp \
    view/items/ViewRotateToolItem.cpp \
    view/items/ViewSonnetDataItem.cpp \
    view/items/ViewSeparatorItem.cpp \
    view/items/ViewSourceItem.cpp \
    view/items/ViewShapeItem.cpp \
    view/items/ViewLabelItem.cpp \
    view/items/ViewRadialGridItem.cpp \
    view/items/ViewChordRadialTopItem.cpp \
    view/items/ViewElementRadialTopItem.cpp \
    view/items/ViewMeshRadialTopItem.cpp \
    view/items/ViewSourceRadialTopItem.cpp \
    view/items/ViewChordTopItem.cpp \
    view/items/ViewLegendItem.cpp \
    view/items/ViewCommentItem.cpp \
\
    dialogs/DistrGraph.cpp \
    dialogs/DlgMoveRotate.cpp \
    dialogs/DlgCreateChord.cpp \
    dialogs/DlgCreateGridPoint.cpp \
    dialogs/DlgCreatePoint.cpp \
    dialogs/DlgCreateSurface.cpp \
    dialogs/DlgGluePoints.cpp \
    dialogs/DlgGlueElems.cpp \
    dialogs/DlgStretch.cpp \
    dialogs/DlgSetAngle.cpp \
    dialogs/DlgInvalidVars.cpp \
    dialogs/DlgVarsEdit.cpp \
    dialogs/DlgVsdList.cpp \
    dialogs/DlgCreateName.cpp \
    dialogs/DlgVsdEdit.cpp \
    dialogs/DlgVdEdit.cpp \
    dialogs/DlgVdHelpEdit.cpp \
    dialogs/DlgTopology.cpp \
    dialogs/DlgMeshHeader.cpp \
    dialogs/DlgOutputMode.cpp \
    dialogs/DlgMeshOptions.cpp \
    dialogs/DlgStatictics.cpp \
    dialogs/DlgSettings.cpp \
    dialogs/PropertyTableWidget.cpp \
    dialogs/Assistant.cpp \
    dialogs/DlgAbout.cpp \
    dialogs/DlgVarHelp.cpp \
    dialogs/DlgNewModel.cpp \
    dialogs/DlgCommentEdit.cpp \
    dialogs/DoubleSpinBoxExt.cpp \
\
    model/core/Model.Write.cpp \
    model/core/Model.Read.cpp \
    model/core/Model.Objects.cpp \
    model/core/Model.cpp \
    model/core/CoreConstants.cpp \
    model/core/Actions.Equil.cpp \
    model/core/Actions.Material.cpp \
    model/core/Actions.cpp \
    model/core/IAction.cpp \
    model/core/Actions.Vars.cpp \
    model/core/IVarOrigin.cpp \
    model/core/IVarItem.cpp \
    model/core/IObject.cpp \
    model/core/DgTokens.cpp \
    model/core/IManager.cpp \
    model/core/ActionStack.cpp \
\
    common/strlib.cpp \
    common/mathlib.cpp \
    common/ShowFlags.cpp \
    common/Spline.cpp \
    common/ResourceTree.cpp \
    common/PointConverters.cpp \
    common/StringConverters.cpp \
    common/Point.cpp \
    common/UpdateInfo.cpp \
\
    model/diagnostics/Sources.cpp \
    model/diagnostics/Comment.cpp \
\
    model/flux/XPointTest.cpp \
    model/flux/XPointSeg.cpp \
    model/flux/SurfaceZone.cpp \
    model/flux/SurfaceEx.cpp \
    model/flux/GridPointSeg.cpp \
    model/flux/GridPointEx.cpp \
    model/flux/Equil.cpp \
    model/flux/Equil.Splines.cpp \
    model/flux/CreatorId.cpp \
    model/flux/FluxModel.cpp \
    model/flux/Topology.cpp \
    model/flux/FluxModel.Output.cpp \
    model/flux/Topology.InOut.cpp \
    model/flux/Equil.InOut.cpp \
    model/flux/FluxModel.Surfaces.cpp \
    model/flux/FluxModel.GridPoints.cpp \
    model/flux/Topology.Zones.cpp \
    model/flux/Topology.Segments.cpp \
    model/flux/Topology.XPoints.cpp \
    model/flux/Topology.Separatrix.cpp \
\
    model/material/Template.cpp \
    model/material/Node.cpp \
    model/material/Element.cpp \
    model/material/Chords.cpp \
    model/material/Separator.cpp \
    model/material/Structure.cpp \
    model/material/Structure.InOut.cpp \
\
    model/mesh/MeshPoint.cpp \
    model/mesh/MeshElement.cpp \
    model/mesh/MeshCell.cpp \
    model/mesh/Mesh.cpp \
    model/mesh/SonnetData.cpp \
\
    model/vars/VarsManager.cpp \
    model/vars/VarsManager.InOut.cpp \
    model/vars/VarsManager.Check.cpp \
    model/vars/VarSetDef.cpp \
    model/vars/VarSet.cpp \
    model/vars/VarDef.cpp \
    model/vars/VarConst.cpp \
    model/vars/Var.cpp

OTHER_FILES += \
    dg.rc \
    ../strings.dgr \
    ../help.dgh \
    ../config.dgc

RESOURCES += \
    dg.qrc


