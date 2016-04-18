#include "ARpcConfig.h"

ARpcConfig::ARpcConfig()
{
	funcCallErrMsgTitle="err";
	funcCallOkMsgTitle="ok";
	funcCallSyncMsgTitle="sync";
	boardResetMsgTitle="ready";
	infoMsgTitle="info";
	msgDelim='\n';
	argDelim='|';
}

