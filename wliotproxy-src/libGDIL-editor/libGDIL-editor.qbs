import qbs

StaticLibrary
{
	targetName: "libGDIL-editor"

	Depends {name: "cpp"}
	Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
	Depends {name: "libGDIL"}
	cpp.includePaths: ["./include","./include-private","./ui"]

	Export
	{
		Depends {name: "cpp"}
		Depends {name: "Qt"; submodules: ["core","gui","widgets"]}
		Depends {name: "libGDIL"}
		cpp.includePaths: "./include"
	}


	files:[
        "Resources/GDILEditor.qrc",
        "include-private/BaseBlocksGroupEditorsFactory.h",
        "include-private/BlockGraphicsItem.h",
        "include-private/BlockGraphicsItemConfigButton.h",
        "include-private/BlockGraphicsItemHeader.h",
        "include-private/BlockGraphicsItemPort.h",
        "include-private/CoreBlocksGroupEditorsFactory.h",
        "include-private/EditorColors.h",
        "include-private/EditorInternalApi.h",
        "include-private/EditorScene.h",
        "include-private/GDILEditorRcInit.h",
        "include-private/LinkGraphicsItem.h",
        "include-private/LogicalBlocksGroupEditorsFactory.h",
        "include-private/block_editors/CommandBlockEditor.h",
        "include-private/block_editors/ComparationBlockEditor.h",
        "include-private/block_editors/DebugBlockEditor.h",
        "include-private/block_editors/DefaultBlockEditor.h",
        "include-private/block_editors/DimChangeBlockEditor.h",
        "include-private/block_editors/NormingBlockEditor.h",
        "include-private/block_editors/StaticSourceBlockEditor.h",
        "include-private/block_editors/StorageSourceBlockEditor.h",
        "include/GDIL/editor/BlocksEditingFactory.h",
        "include/GDIL/editor/Editor.h",
        "include/GDIL/editor/IBlockEditor.h",
        "include/GDIL/editor/IBlocksGroupEditorsFactory.h",
        "src/BaseBlocksGroupEditorsFactory.cpp",
        "src/BlockGraphicsItem.cpp",
        "src/BlockGraphicsItemConfigButton.cpp",
        "src/BlockGraphicsItemHeader.cpp",
        "src/BlockGraphicsItemPort.cpp",
        "src/BlocksEditingFactory.cpp",
        "src/CoreBlocksGroupEditorsFactory.cpp",
        "src/Editor.cpp",
        "src/EditorColors.cpp",
        "src/EditorInternalApi.cpp",
        "src/EditorScene.cpp",
        "src/GDILEditorRcInit.cpp",
        "src/IBlockEditor.cpp",
        "src/LinkGraphicsItem.cpp",
        "src/LogicalBlocksGroupEditorsFactory.cpp",
        "src/block_editors/CommandBlockEditor.cpp",
        "src/block_editors/ComparationBlockEditor.cpp",
        "src/block_editors/DebugBlockEditor.cpp",
        "src/block_editors/DefaultBlockEditor.cpp",
        "src/block_editors/DimChangeBlockEditor.cpp",
        "src/block_editors/NormingBlockEditor.cpp",
        "src/block_editors/StaticSourceBlockEditor.cpp",
        "src/block_editors/StorageSourceBlockEditor.cpp",
        "ui/CommandBlockEditorUi.ui",
        "ui/CommandBlockEditorWidget.cpp",
        "ui/CommandBlockEditorWidget.h",
        "ui/ComparationBlockEditorUi.ui",
        "ui/ComparationBlockEditorWidget.cpp",
        "ui/ComparationBlockEditorWidget.h",
        "ui/DebugBlockEditorWidget.cpp",
        "ui/DebugBlockEditorWidget.h",
        "ui/DimChangeBlockEditorWidget.cpp",
        "ui/DimChangeBlockEditorWidget.h",
        "ui/NormingBlockEditorUi.ui",
        "ui/NormingBlockEditorWidget.cpp",
        "ui/NormingBlockEditorWidget.h",
        "ui/StaticSourceBlockEditorUi.ui",
        "ui/StaticSourceBlockEditorWidget.cpp",
        "ui/StaticSourceBlockEditorWidget.h",
        "ui/StorageSourceBlockEditorUi.ui",
        "ui/StorageSourceBlockEditorWidget.cpp",
        "ui/StorageSourceBlockEditorWidget.h",
    ]
}
