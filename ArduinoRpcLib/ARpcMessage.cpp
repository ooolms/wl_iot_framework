#include "ARpcMessage.h"

ARpcMessage::ARpcMessage()
{

}

void ARpcMessage::parse(const QString &str)
{
	title.clear();
	args.clear();
	int index=0,newIndex=0;
	newIndex=str.indexOf('|',index);
	if(newIndex==-1)
	{
		title=str;
		return;
	}
	title=str.mid(0,newIndex);
	index=newIndex;
	while(newIndex!=-1)
	{
		newIndex=str.indexOf('|',index+1);
		args.append(str.mid(index+1,newIndex-index-1));
		index=newIndex;
	}
}

QString ARpcMessage::dump()
{
	if(args.isEmpty())return title;
	return title+"|"+args.join("|");
}

