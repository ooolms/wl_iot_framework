#ifndef SUBPROGRAMBLOCKEDITORWIDGET_H
#define SUBPROGRAMBLOCKEDITORWIDGET_H

#include "VDIL/core/DataUnit.h"
#include <QWidget>

namespace Ui
{
	class SubProgramBlockEditorWidget;
}

class QTreeWidgetItem;

namespace WLIOTVDIL
{
	class SubProgramBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit SubProgramBlockEditorWidget(QWidget *parent=nullptr);
		virtual ~SubProgramBlockEditorWidget();
		const QList<TypeAndDim>& inputsTypes();
		const QStringList& inputsTitles();
		const QList<TypeAndDim>& outputsTypes();
		const QStringList& outputsTitles();
		void setParams(const QList<TypeAndDim> &iTypes,const QStringList &iTitles,
			const QList<TypeAndDim> &oTypes,const QStringList &oTitles);

	signals:
		void editContent();

	private slots:
		void onAddInputClicked();
		void onDelInputClicked();
		void onAddOutputClicked();
		void onDelOutputClicked();
		void onInputsListDClicked(QTreeWidgetItem *item,int column);
		void onOutputsListDClicked(QTreeWidgetItem *item,int column);

	private:
		static QString typeStr(const WLIOTVDIL::TypeAndDim &t);

	private:
		Ui::SubProgramBlockEditorWidget *ui;
		QList<TypeAndDim> mInputsTypes;
		QStringList mInputsTitles;
		QList<TypeAndDim> mOutputsTypes;
		QStringList mOutputsTitles;
	};
}

#endif // SUBPROGRAMBLOCKEDITORWIDGET_H
