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

#ifndef PROGRAMRUNTIMEVARS_H
#define PROGRAMRUNTIMEVARS_H

#include "VDIL/core/DataUnit.h"
#include "VDIL/core/IProgramRuntime.h"

namespace WLIOTVDIL
{
	class Program;

	class ProgramRuntimeVars
		:public IProgramRuntimeInstance
	{
	public:
		explicit ProgramRuntimeVars(Program *p);

		//configuration
		void setupVar(const QString &name,const DataUnit &defaultValue);//creates or update
		void removeVar(const QString &name);
		DataUnit defaultValue(const QString &name)const;
		QStringList allVars()const;

		//runtime
		DataUnit runtimeValue(const QString &name)const;
		void setRuntimeValue(const QString &name,const DataUnit &value);
		virtual void prepareToStart()override;
		virtual void cleanupAfterStop()override;

	private:
		QMap<QString,DataUnit> mDefaultValues;
		QMap<QString,DataUnit> mRuntimeValues;
	};
}

#endif // PROGRAMRUNTIMEVARS_H
