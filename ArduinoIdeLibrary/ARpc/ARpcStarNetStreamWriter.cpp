#include "ARpcStarNetStreamWriter.h"
#include <string.h>

ARpcStarNetStreamWriter::ARpcStarNetStreamWriter(const ARpcUuid &srcId,ARpcIWriteCallback *wcb)
	:ARpcStreamWriter(wcb)
{
	destMode=BROADCAST;
	srcId.toHex(srcIdStr);
	memset(destIdStr,0,33);
}

void ARpcStarNetStreamWriter::setBroadcast()
{
	destMode=BROADCAST;
}

void ARpcStarNetStreamWriter::setDestId(const ARpcUuid &destId)
{
	destMode=DEST_DEV;
	destId.toHex(destIdStr);
}

void ARpcStarNetStreamWriter::writeMsgHeaders()
{
	writeArgNoEscape(srcIdStr);
	if(destMode==BROADCAST)
		writeArgNoEscape("#broadcast");
	else writeArgNoEscape(destIdStr);
}
