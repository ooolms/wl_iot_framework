import qbs

StaticLibrary
{
	targetName: "libVDIL-editor"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
	Depends {name: "libVDIL"}
	Depends {name: "libwliotproxy-ui-config"}
	cpp.includePaths: ["./include","./include-private","./ui"]
	cpp.defines:
	{
		var r=["QT_RESTRICTED_CAST_FROM_ASCII"];
		if(qbs.buildVariant=="debug")
			r.push("DEBUG");
		return r;
	}

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
		Depends {name: "libVDIL"}
		Depends {name: "libwliotproxy-ui-config"}
		cpp.includePaths: "./include"
	}


	files:[
        "Resources/VDILEditorRc.qrc",
        "include-private/BaseBlocksGroupEditorsFactory.h",
        "include-private/BlockGraphicsItem.h",
        "include-private/BlockGraphicsItemConfigButton.h",
        "include-private/BlockGraphicsItemHeader.h",
        "include-private/BlockGraphicsItemPort.h",
        "include-private/CoreBlocksGroupEditorsFactory.h",
        "include-private/EditorColors.h",
        "include-private/EditorInternalApi.h",
        "include-private/EditorScene.h",
        "include-private/ProgramConfigDialog.h",
        "include-private/VDILEditorRcInit.h",
        "include-private/LinkGraphicsItem.h",
        "include-private/LogicalBlocksGroupEditorsFactory.h",
        "include-private/block_editors/ArrayCombineBlockEditor.h",
        "include-private/block_editors/ArraySelectBlockEditor.h",
        "include-private/block_editors/CommandBlockEditor.h",
        "include-private/block_editors/ComparationBlockEditor.h",
        "include-private/block_editors/DateTimeSourceBlockEditor.h",
        "include-private/block_editors/DebugBlockEditor.h",
        "include-private/block_editors/DefaultBlockEditor.h",
        "include-private/block_editors/DelayBlockEditor.h",
        "include-private/block_editors/DevicePresenceSourceBlockEditor.h",
        "include-private/block_editors/DeviceStateSourceBlockEditor.h",
        "include-private/block_editors/DimChangeBlockEditor.h",
        "include-private/block_editors/NormingBlockEditor.h",
        "include-private/block_editors/RandomSourceBlockEditor.h",
        "include-private/block_editors/RuntimeSourceBlockEditor.h",
        "include-private/block_editors/RuntimeStoreBlockEditor.h",
        "include-private/block_editors/StaticSourceBlockEditor.h",
        "include-private/block_editors/StorageSourceBlockEditor.h",
        "include-private/block_editors/TimerBlockEditor.h",
        "include-private/block_editors/VDevCommandSourceBlockEditor.h",
        "include-private/block_editors/VDevSensorSendBlockEditor.h",
        "include/VDIL/editor/BlocksEditingFactory.h",
        "include/VDIL/editor/DataUnitEdit.h",
        "include/VDIL/editor/Editor.h",
        "include/VDIL/editor/IBlockEditor.h",
        "include/VDIL/editor/IBlocksGroupEditorsFactory.h",
        "include/VDIL/editor/IEditorHelper.h",
        "include/VDIL/editor/TimerConfigEdit.h",
        "src/BaseBlocksGroupEditorsFactory.cpp",
        "src/BlockGraphicsItem.cpp",
        "src/BlockGraphicsItemConfigButton.cpp",
        "src/BlockGraphicsItemHeader.cpp",
        "src/BlockGraphicsItemPort.cpp",
        "src/BlocksEditingFactory.cpp",
        "src/CoreBlocksGroupEditorsFactory.cpp",
        "src/DataUnitEdit.cpp",
        "src/Editor.cpp",
        "src/EditorColors.cpp",
        "src/EditorInternalApi.cpp",
        "src/EditorScene.cpp",
        "src/ProgramConfigDialog.cpp",
        "src/VDILEditorRcInit.cpp",
        "src/IBlockEditor.cpp",
        "src/LinkGraphicsItem.cpp",
        "src/LogicalBlocksGroupEditorsFactory.cpp",
        "src/TimerConfigEdit.cpp",
        "src/block_editors/ArrayCombineBlockEditor.cpp",
        "src/block_editors/ArraySelectBlockEditor.cpp",
        "src/block_editors/CommandBlockEditor.cpp",
        "src/block_editors/ComparationBlockEditor.cpp",
        "src/block_editors/DateTimeSourceBlockEditor.cpp",
        "src/block_editors/DebugBlockEditor.cpp",
        "src/block_editors/DefaultBlockEditor.cpp",
        "src/block_editors/DelayBlockEditor.cpp",
        "src/block_editors/DevicePresenceSourceBlockEditor.cpp",
        "src/block_editors/DeviceStateSourceBlockEditor.cpp",
        "src/block_editors/DimChangeBlockEditor.cpp",
        "src/block_editors/NormingBlockEditor.cpp",
        "src/block_editors/RandomSourceBlockEditor.cpp",
        "src/block_editors/RuntimeSourceBlockEditor.cpp",
        "src/block_editors/RuntimeStoreBlockEditor.cpp",
        "src/block_editors/StaticSourceBlockEditor.cpp",
        "src/block_editors/StorageSourceBlockEditor.cpp",
        "src/block_editors/TimerBlockEditor.cpp",
        "src/block_editors/VDevCommandSourceBlockEditor.cpp",
        "src/block_editors/VDevSensorSendBlockEditor.cpp",
        "ui/ArrayCombineBlockEditorWidget.cpp",
        "ui/ArrayCombineBlockEditorWidget.h",
        "ui/ArraySelectBlockEditorWidget.cpp",
        "ui/ArraySelectBlockEditorWidget.h",
        "ui/CommandBlockEditorWidget.cpp",
        "ui/CommandBlockEditorWidget.h",
        "ui/CommandBlockEditorWidget.ui",
        "ui/ComparationBlockEditorWidget.cpp",
        "ui/ComparationBlockEditorWidget.h",
        "ui/ComparationBlockEditorWidget.ui",
        "ui/DataUnitEdit.ui",
        "ui/DataUnitValueEdit.cpp",
        "ui/DataUnitValueEdit.h",
        "ui/DateTimeSourceBlockEditorWidget.cpp",
        "ui/DateTimeSourceBlockEditorWidget.h",
        "ui/DebugBlockEditorWidget.cpp",
        "ui/DebugBlockEditorWidget.h",
        "ui/DelayBlockEditorWidget.cpp",
        "ui/DelayBlockEditorWidget.h",
        "ui/DevicePresenceSourceBlockEditorWidget.cpp",
        "ui/DevicePresenceSourceBlockEditorWidget.h",
        "ui/DevicePresenceSourceBlockEditorWidget.ui",
        "ui/DeviceStateSourceBlockEditorWidget.cpp",
        "ui/DeviceStateSourceBlockEditorWidget.h",
        "ui/DeviceStateSourceBlockEditorWidget.ui",
        "ui/DimChangeBlockEditorWidget.cpp",
        "ui/DimChangeBlockEditorWidget.h",
        "ui/NormingBlockEditorWidget.cpp",
        "ui/NormingBlockEditorWidget.h",
        "ui/NormingBlockEditorWidget.ui",
        "ui/OneStringVarSelectWidget.cpp",
        "ui/OneStringVarSelectWidget.h",
        "ui/ProgramConfigDialog.ui",
        "ui/RandomSourceBlockEditorWidget.cpp",
        "ui/RandomSourceBlockEditorWidget.h",
        "ui/StaticSourceBlockEditorWidget.cpp",
        "ui/StaticSourceBlockEditorWidget.h",
        "ui/StorageSourceBlockEditorWidget.cpp",
        "ui/StorageSourceBlockEditorWidget.h",
        "ui/StorageSourceBlockEditorWidget.ui",
        "ui/TimerBlockEditorWidget.cpp",
        "ui/TimerBlockEditorWidget.h",
        "ui/TimerConfigEdit.ui",
        "ui/TriggersEditDialog.ui",
    ]
}
