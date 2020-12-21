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

#include "VDIL/xml/RandomSourceBlockXmlParser.h"
#include "VDIL/blocks/RandomSourceBlock.h"
#include <stdlib.h>

using namespace WLIOT;
using namespace WLIOTVDIL;

bool RandomSourceBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)
{
	((RandomSourceBlock*)block)->setBounds(
		QList<QPair<qint32,qint32>>()<<qMakePair(qint32(0),qint32(RAND_MAX)));
	if(!blockElem.hasAttribute("bounds"))
		return tryFixErrors;
	QByteArrayList arr=blockElem.attribute("bounds").toUtf8().split(',');
	if(arr.count()%2!=0)
		return tryFixErrors;
	bool ok=false;
	QList<QPair<qint32,qint32>> bounds;
	int c=arr.count()>>1;
	for(int i=0;i<c;++i)
	{
		qint32 min=arr[i<<1].toInt(&ok);
		if(!ok)return tryFixErrors;
		qint32 max=arr[(i<<1)+1].toInt(&ok);
		if(!ok)return tryFixErrors;
		bounds.append(qMakePair(min,max));
	}
	((RandomSourceBlock*)block)->setBounds(bounds);
	return true;
}

void RandomSourceBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	QList<QPair<qint32,qint32>> bounds=((const RandomSourceBlock*)block)->bounds();
	QByteArrayList strs;
	for(auto &b:bounds)
	{
		strs.append(QByteArray::number(b.first));
		strs.append(QByteArray::number(b.second));
	}
	blockElem.setAttribute("bounds",QString::fromUtf8(strs.join(',')));
}
