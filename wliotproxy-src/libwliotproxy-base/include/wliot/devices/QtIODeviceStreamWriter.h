#ifndef QTIODEVICESTREAMWRITER_H
#define QTIODEVICESTREAMWRITER_H

#include "wliot/devices/IStreamWriter.h"
#include <QIODevice>

class QtIODeviceStreamWriter
	:public IStreamWriter
{
	Q_OBJECT
public:
	explicit QtIODeviceStreamWriter(QObject *parent=nullptr);

protected:
	virtual bool writeData(const QByteArray &data)override;

private:
	QIODevice *dev;
};

#endif // QTIODEVICESTREAMWRITER_H
