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

#include "BaseProgramsControlCommand.h"
#include "StandardErrors.h"
#include "../CommandProcessor.h"

BaseProgramsControlCommand::BaseProgramsControlCommand(
	const QByteArray &cmdPrefix,BaseProgramsManager *mgr,CommandProcessor *p)
	:ICommand(p)
{
	mCmdPrefix=cmdPrefix;
	mMgr=mgr;
}

bool BaseProgramsControlCommand::processCommand(CallContext &ctx)
{
	if(ctx.cmd==mCmdPrefix+"_list")
	{
		QByteArrayList ids=mMgr->programIds(proc->uid());
		for(const QByteArray &id:ids)
		{
			BaseProgramConfigDb *cfgDb=mMgr->cfgDb(proc->uid(),id);
			if(!cfgDb)continue;
			writeCmdataMsg(ctx.callId,QByteArrayList()<<id<<cfgDb->programName()<<
				(mMgr->isWorking(proc->uid(),id)?"1":"0"));
		}
		return true;
	}
	else if(ctx.cmd==mCmdPrefix+"_create")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray name=ctx.args[0];
		QByteArray data=ctx.args[1];
		QByteArray id;
		if(!mMgr->addProgram(proc->uid(),name,data,id))
		{
			ctx.retVal.append("error when adding new program");
			return false;
		}
		ctx.retVal.append(id);
		return true;
	}
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray prgId=ctx.args[0];
	if(ctx.cmd==mCmdPrefix+"_start")
	{
		if(!mMgr->startStopProgram(proc->uid(),prgId,true))
		{
			ctx.retVal.append("no program found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd==mCmdPrefix+"_stop")
	{
		if(!mMgr->startStopProgram(proc->uid(),prgId,false))
		{
			ctx.retVal.append("no program found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd==mCmdPrefix+"_restart")
	{
		if(!mMgr->startStopProgram(proc->uid(),prgId,false)||!mMgr->startStopProgram(proc->uid(),prgId,true))
		{
			ctx.retVal.append("no program found: "+ctx.args[0]);
			return false;
		}
		return true;
	}
	else if(ctx.cmd==mCmdPrefix+"_update")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray data=ctx.args[1];
		if(!mMgr->updateProgram(proc->uid(),prgId,data))
		{
			ctx.retVal.append("error when updating program");
			return false;
		}
		return true;
	}
	else if(ctx.cmd==mCmdPrefix+"_rename")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray name=ctx.args[1];
		if(!mMgr->renameProgram(proc->uid(),prgId,name))
		{
			ctx.retVal.append("error when renaming program");
			return false;
		}
		return true;
	}
	else if(ctx.cmd==mCmdPrefix+"_remove")
	{
		if(!mMgr->removeProgram(proc->uid(),prgId))
		{
			ctx.retVal.append("error when removing program");
			return false;
		}
		return true;
	}
	else if(ctx.cmd==mCmdPrefix+"_get")
	{
		QByteArray data;
		if(!mMgr->getProgram(proc->uid(),prgId,data))
		{
			ctx.retVal.append("no program found: "+ctx.args[0]);
			return false;
		}
		ctx.retVal.append(data);
		return true;
	}
	return false;
}

QByteArrayList BaseProgramsControlCommand::acceptedCommands()
{
	return QByteArrayList()<<mCmdPrefix+"_start"<<mCmdPrefix+"_stop"<<mCmdPrefix+"_restart"<<
		mCmdPrefix+"_list"<<mCmdPrefix+"_update"<<mCmdPrefix+"_remove"<<mCmdPrefix+"_get"<<mCmdPrefix+"_rename";
}
