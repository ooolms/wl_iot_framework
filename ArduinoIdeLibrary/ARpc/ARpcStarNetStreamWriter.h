#ifndef ARPCSTARNETSTREAMWRITER_H
#define ARPCSTARNETSTREAMWRITER_H

#include "ARpcStreamWriter.h"
#include "ARpcUuid.h"

class ARpcStarNetStreamWriter
	:public ARpcStreamWriter
{
public:
	explicit ARpcStarNetStreamWriter(const ARpcUuid *srcId,ARpcIWriteCallback *wcb);
	void setBroadcast();
	void setDestId(const ARpcUuid &destId);

protected:
	virtual void writeMsgHeaders();

private:
	enum
	{
		BROADCAST,
		DEST_DEV
	}destMode;
	char srcIdStr[33];
	char destIdStr[33];
};

#endif // ARPCSTARNETSTREAMWRITER_H
