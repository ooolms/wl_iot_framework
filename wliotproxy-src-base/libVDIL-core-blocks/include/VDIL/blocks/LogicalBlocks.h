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

#ifndef LOGICALBLOCKS_H
#define LOGICALBLOCKS_H

#include "VDIL/core/BaseBlock.h"

namespace WLIOTVDIL
{
	class NotBoolBlock
		:public BaseBlock
	{
	public:
		explicit NotBoolBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;

	protected:
		virtual void eval()override;

	public:
		static const QString mBlockName;

	private:
		BlockInput *in;
		BlockOutput *out;
	};

	class Base2InputsBoolBlock
		:public BaseBlock
	{
	public:
		explicit Base2InputsBoolBlock(quint32 bId);

	protected:
		virtual void eval()override;
		virtual bool calc(bool v1,bool v2)=0;

	public:
		static const QString mBlockName;

	private:
		BlockInput *in1;
		BlockInput *in2;
		BlockOutput *out;
	};

	class AndBoolBlock
		:public Base2InputsBoolBlock
	{
	public:
		explicit AndBoolBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;

	public:
		static const QString mBlockName;

	protected:
		virtual bool calc(bool v1,bool v2)override;
	};

	class OrBoolBlock
		:public Base2InputsBoolBlock
	{
	public:
		explicit OrBoolBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;

	public:
		static const QString mBlockName;

	protected:
		virtual bool calc(bool v1,bool v2)override;
	};

	class XorBoolBlock
		:public Base2InputsBoolBlock
	{
	public:
		explicit XorBoolBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;

	public:
		static const QString mBlockName;

	protected:
		virtual bool calc(bool v1,bool v2)override;
	};

	class AndNotBoolBlock
		:public Base2InputsBoolBlock
	{
	public:
		explicit AndNotBoolBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;

	public:
		static const QString mBlockName;

	protected:
		virtual bool calc(bool v1,bool v2)override;
	};

	class OrNotBoolBlock
		:public Base2InputsBoolBlock
	{
	public:
		explicit OrNotBoolBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;

	public:
		static const QString mBlockName;

	protected:
		virtual bool calc(bool v1,bool v2)override;
	};

	class XorNotBoolBlock
		:public Base2InputsBoolBlock
	{
	public:
		explicit XorNotBoolBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;

	public:
		static const QString mBlockName;

	protected:
		virtual bool calc(bool v1,bool v2)override;
	};
}

#endif // LOGICALBLOCKS_H
