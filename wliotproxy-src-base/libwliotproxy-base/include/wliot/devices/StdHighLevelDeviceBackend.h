#ifndef STDHIGHLEVELDEVICEBACKEND_H
#define STDHIGHLEVELDEVICEBACKEND_H

#include "wliot/devices/IHighLevelDeviceBackend.h"
#include "wliot/devices/ILowLevelDeviceBackend.h"
#include "wliot/devices/StreamParser.h"

namespace WLIOT
{
	/**
	 * @brief Backend высокого уровня, который можно использовать поверх любого backend-а низкого уровня
	 */
	class StdHighLevelDeviceBackend
		:public IHighLevelDeviceBackend
	{
		Q_OBJECT
	public:
		explicit StdHighLevelDeviceBackend(ILowLevelDeviceBackend *le,QObject *parent=nullptr);
		virtual ~StdHighLevelDeviceBackend();
		virtual bool writeMessageToDevice(const Message &m)override;
		virtual bool isConnected()const override;
		virtual void forceDisconnect()override;
		virtual QByteArray backendType()const override;
		ILowLevelDeviceBackend* backend();
		virtual QString hwAddress()const override;

	private slots:
		void onNewData(const QByteArray &data);

	private:
		ILowLevelDeviceBackend *lowLvlBackend;
		StreamParser parser;
	};
}

#endif // STDHIGHLEVELDEVICEBACKEND_H
