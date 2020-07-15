/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include "VDIL/core/BlockInput.h"
#include "VDIL/core/BlockOutput.h"
#include "VDIL/core/IEngineHelper.h"
#include "VDIL/core/IEngineCallbacks.h"
#include <QList>
#include <QPoint>
#include <QPointer>

namespace WLIOTVDIL
{
	class Program;

	class BaseBlock
	{
	public:
		explicit BaseBlock(quint32 bId);
		virtual ~BaseBlock();
		virtual QString groupName()const=0;
		virtual QString blockName()const=0;
		virtual bool isSourceBlock()const;
		QByteArrayList configOptions()const;
		bool setConfigOption(const QByteArray &key,const DataUnit &value);
		DataUnit configOptionValue(const QByteArray &key);
		Program* program();
		const Program* program()const;

		//for program
		int inputsCount()const;
		BlockInput* input(int index);
		int inputIndex(const BlockInput *in)const;
		int outputsCount()const;
		BlockOutput* output(int index);
		int outputIndex(const BlockOutput *out)const;
		quint32 blockId()const;

		//for work
		void evalIfReady();
		void cleanupAfterEval();

	protected:
		virtual void eval()=0;
		virtual void cleanupAfterEvalInternal();
		virtual void evalOnTimer();
		virtual void onInputTypeSelected(BlockInput *b);
		virtual void onConfigOptionChanged(const QByteArray &key);
		BlockInput* mkInput(TypeConstraints suppTypes,DataUnit::Type currType,const QString &title,int index=-1);
		BlockOutput* mkOutput(DataUnit::Type type,quint32 dim,const QString &title,int index=-1);
		void rmInput(int index);
		void rmInput(BlockInput *in);
		void rmOutput(int index);
		void rmOutput(BlockOutput *out);
		void evalOnTimerInMsec(quint32 msec);
		void addConfigOption(const QByteArray &key,const DataUnit &defaultValue);
		void rmConfigOption(const QByteArray &key);
		IEngineHelper* helper()const;
		IEngineCallbacks* engineCallbacks()const;
		virtual QList<QUuid> usedDevices()const;


	private:
		void onTimer();
		void cleanupInputs();

	public:
		//editing attributes
		QString title;
		QPointF position;

	protected:
		Program *prg;

	private:
		QList<BlockInput*> inputs;
		QList<BlockOutput*> outputs;
		QPointer<QTimer> evalTimer;
		QMap<QByteArray,DataUnit> mConfigOptions;

	private:
		quint32 mBlockId;
		friend class BlockOutput;
		friend class BlockInput;
		friend class Program;
	};
}

#endif // BASEBLOCK_H
