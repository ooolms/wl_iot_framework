#include "block_editors/DebugBlockEditor.h"
#include "DebugBlockEditorWidget.h"
#include "GDIL/blocks/DebugBlock.h"

QWidget *DebugBlockEditor::mkEditingWidget(EditorInternalApi *editor,QWidget *parent)
{
	return new DebugBlockEditorWidget(parent);
}

void DebugBlockEditor::loadParamsFromBlock(QWidget *editingWidget,const BaseBlock *block)
{
	DebugBlockEditorWidget *w=(DebugBlockEditorWidget*)editingWidget;
	const DebugBlock *b=(const DebugBlock*)block;
	w->setParams(b->debugString(),b->inCount());
}

void DebugBlockEditor::saveParamsToBlock(QWidget *editingWidget,BaseBlock *block)
{
	DebugBlockEditorWidget *w=(DebugBlockEditorWidget*)editingWidget;
	DebugBlock *b=(DebugBlock*)block;
	b->setParams(w->debugString(),w->inCount());
}

QPixmap DebugBlockEditor::previewImage()const
{
	return QPixmap(":/GDIL/editor/blocks/debug.png");
}

QString DebugBlockEditor::description()const
{
	return "writing debug message to log";
}

QString DebugBlockEditor::typeName()const
{
	return "debug block";
}
