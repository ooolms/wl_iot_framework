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

#ifndef SOURCEBLOCK_H
#define SOURCEBLOCK_H

#include "VDIL/core/BaseBlock.h"

namespace WLIOTVDIL
{
	/*
	 * две копии data решают проблему с многозадачностью (хранилище не thread safe),
	 * extractData вызывается из основного потока, затем вызывается prepareWorkData,
	 * которая просто копирует next в work (это должно быть защищено мьютексом),
	 * которая потом используется в рабочем потоке
	*/
	class SourceBlock
		:public BaseBlock
	{
	public:
		explicit SourceBlock(quint32 id);
		virtual ~SourceBlock();
		bool extractNextData();//call from main thread
		bool prepareWorkData();//call from work thread
		virtual bool isSourceBlock()const override;
		void setTriggerActivated();

	protected:
		virtual void eval()override final;
		virtual DataUnit extractDataInternal()=0;
		virtual void evalInternal(const DataUnit &data,bool triggerActivated);

	private:
		DataUnit mNextData,mWorkData;
		bool mNextTriggerActivated,mWorkTriggerActivated;
	};
}

#endif // SOURCEBLOCK_H
