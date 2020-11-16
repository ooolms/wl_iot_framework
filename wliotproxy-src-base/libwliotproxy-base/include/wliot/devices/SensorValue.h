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

#ifndef SENSORVALUE_H
#define SENSORVALUE_H

#include "wliot/devices/SensorDef.h"
#include "wliot/devices/Message.h"
#include <QVector>

namespace WLIOT
{
	/**
	 * @brief Базовый класс для значения датчика
	 * @details Содержит набор (пакет) многомерных отсчетов и временную метку.
	 * SINGLE значения работают как пакет с одним отсчетом.
	 * Функции, содержащие в аргументах dimIndex и packIndex, позволяют доставать значения по номеру отсчета в пакете и
	 * размерности; функции, содержание в аргументах totalIndex, позволяют обращаться к набору
	 * отсчетов как к большому линейному массиву. Данный класс преобразует все числовые значения в double или
	 * quint64. Для доступа к данным напрямую нужно определить тип значений (type().numType) и преобразовать
	 * SensorValue* к указателю на нужных подкласс.
	 */
	class SensorValue
	{
	public:
		static SensorValue* createSensorValue(SensorDef::Type t);
		SensorValue* mkCopy()const;
		virtual ~SensorValue();
		bool isEmpty()const;
		bool operator==(const SensorValue &t)const;
		/**
		 * @brief Количество отсчетов в пакете
		 * @return
		 */
		quint32 samplesCount()const;
		/**
		 * @brief Тип значений
		 * @return
		 */
		const SensorDef::Type& type()const;
		/**
		 * @brief Общее количество значений (dim*samplesCount)
		 * @return
		 */
		quint32 totalCount()const;
		/**
		 * @brief Получить временную метку
		 * @return
		 */
		qint64 time()const;
		/**
		 * @brief Установить временную метку
		 * @param t временная метка
		 */
		void setTime(qint64 t);
		/**
		 * @brief Возвращает true, если совпадают типы и сами значения (может отличаться временная метка)
		 * @param t
		 * @return
		 */
		bool isDataEqual(const SensorValue &t);

		//dump/parse message args
		/**
		 * @brief Сконструировать значение датчика из аргументов сообщения meas,
		 * когда сами значения передаются в текстовом виде
		 * @param args агрументы сообщения meas
		 * @return
		 */
		bool parseMsgArgs(const QByteArrayList &args);
		/**
		 * @brief Сконструировать значение датчика из аргумента сообщения measb,
		 * когда сами значения передаются в бинарном виде упакованными в один аргумент сообщения
		 * @param data аргумент сообщения measb, содержащий данные
		 * @return
		 */
		bool parseBinary(const QByteArray &data);
		/**
		 * @brief Сконструировать значение датчика из аргумента сообщения measb64,
		 * когда сами значения передаются в бинарном виде упакованными в
		 * один аргумент сообщения и закодирвованными в base64
		 * @param data аргумент сообщения measb, содержащий данные в base64
		 * @return
		 */
		bool parseBase64(const QByteArray &data);
		/**
		 * @brief Преобразовать значение датчика в список агрументов как для сообщения meas
		 * @return
		 */
		QByteArrayList dumpToMsgArgs()const;
		/**
		 * @brief Преобразовать значение датчика в список агрументов как для сообщения meas,
		 * но без добавления временной метки в начале
		 * @return
		 */
		QByteArrayList dumpToMsgArgsNoTime()const;
		/**
		 * @brief Преобразовать значение датчика в бинарный вид как для сообщения measb
		 * @return
		 */
		QByteArray dumpToBinary()const;
		/**
		 * @brief Преобразовать значение датчика в бинарный вид как для сообщения measb,
		 * но без временной метки в начале
		 * @return
		 */
		virtual QByteArray dumpToBinaryNoTime()const=0;

		//Qt types conversions
		//access by packet index and dim index
		QByteArray valueToString(quint32 dimIndex,quint32 sampleIndex)const;
		virtual double valueToDouble(quint32 dimIndex,quint32 sampleIndex)const;
		virtual qint64 valueToS64(quint32 dimIndex,quint32 sampleIndex)const;
		//access by total index - from 0 to totalCount
		virtual QByteArray valueToString(quint32 totalIndex)const=0;
		virtual double valueToDouble(quint32 totalIndex)const=0;
		virtual qint64 valueToS64(quint32 totalIndex)const=0;

	protected:
		explicit SensorValue(SensorDef::Type t);
		explicit SensorValue(const SensorValue &t);
		SensorValue& operator=(const SensorValue &t);
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)=0;
		virtual void createData()=0;
		virtual void freeData()=0;
		virtual void copyDataFrom(const SensorValue *from);
		virtual bool valueIsEqual(const SensorValue *t,quint32 index)const=0;
		virtual bool copyFromBinaryData(const char *data)=0;

	private:
		bool parseDataFromMsgArgs(const QByteArrayList &args);
		bool parseDataFromBinary(const QByteArray &data);

	protected:
		SensorDef::Type mType;
		qint64 mTime;
		quint32 mSamplesCount;
	};

	/**
	 * @brief Базовый шаблонный класс для значений датчика, содержащий числовые данные.
	 * @details От него наследуется множество подклассов для разных типов числовых значений
	 */
	template<typename T>
	class SensorValueNumeric
		:public SensorValue
	{
	protected:
		explicit SensorValueNumeric(SensorDef::Type t)
			:SensorValue(t)
		{
			createData();
		}

		SensorValueNumeric(const SensorValueNumeric &t)
			:SensorValue(t)
		{}

		virtual ~SensorValueNumeric()
		{
			freeData();
		}

	public:
		T get(quint32 dimIndex,quint32 packIndex)const
		{
			return mData[packIndex*mType.dim+dimIndex];
		}

		T getT(quint32 totalIndex)const
		{
			return mData[totalIndex];
		}

		void set(quint32 dimIndex,quint32 packIndex,const T &v)
		{
			mData[packIndex*mType.dim+dimIndex]=v;
		}

		void setT(quint32 totalIndex,const T &v)
		{
			mData[totalIndex]=v;
		}

		virtual double valueToDouble(quint32 totalIndex)const override
		{
			return (double)mData.value(totalIndex);
		}

		virtual qint64 valueToS64(quint32 totalIndex)const override
		{
			return (qint64)mData.value(totalIndex);
		}

		QVector<T> getSample(quint32 packIndex)const
		{
			QVector<T> v;
			v.resize(mType.dim);
			quint32 offset=mType.dim*packIndex;
			for(quint32 i=0;i<mType.dim;++i)
				v[i]=mData[offset+i];
			return v;
		}

		QVector<T> getSamples(quint32 from,quint32 count)const
		{
			return mData.mid(from*mType.dim,count*mType.dim);
		}

		bool setSample(const QVector<T> &v,quint32 packIndex)
		{
			if(v.size()!=mType.dim||packIndex>=mSamplesCount)return false;
			quint32 offset=mType.dim*packIndex;
			for(quint32 i=0;i<mType.dim;++i)
				mData[offset+i]=v[i];
			return true;
		}

		bool addSample(const QVector<T> &v)
		{
			if(v.size()!=(int)mType.dim)return false;
			mData.append(v);
			++mSamplesCount;
			return true;
		}

		virtual QByteArray valueToString(quint32 totalIndex)const override
		{
			return QByteArray::number(mData[totalIndex]);
		}

		virtual QByteArray dumpToBinaryNoTime()const override
		{
			return QByteArray((const char*)mData.data(),(int)(mType.dim*mSamplesCount*sizeof(T)));
		}

		bool setData(const QVector<T> &d)
		{
			if(mType.packType==SensorDef::SINGLE)
			{
				if((quint32)d.size()!=mType.dim)
					return false;
			}
			else
			{
				if(d.size()%mType.dim!=0)
					return false;
				mSamplesCount=d.size()/mType.dim;
			}
			mData=d;
			return true;
		}

		const QVector<T>& data()const
		{
			return mData;
		}

		using SensorValue::valueToString;
		using SensorValue::valueToDouble;
		using SensorValue::valueToS64;

	protected:
		virtual void createData()override
		{
			mData.resize(mType.dim*mSamplesCount);
		}

		virtual void freeData()override
		{
			mData.clear();
		}

		virtual void copyDataFrom(const SensorValue *from)override
		{
			SensorValue::copyDataFrom(from);
			const SensorValueNumeric<T> *tt=(const SensorValueNumeric<T>*)from;
			mData=tt->mData;
		}
		virtual bool valueIsEqual(const SensorValue *t,quint32 index)const override
		{
			const SensorValueNumeric<T> *tt=(const SensorValueNumeric<T>*)t;
			return mData[index]==tt->mData[index];
		}

		virtual bool copyFromBinaryData(const char *data)override
		{
			memcpy(mData.data(),data,mType.dim*mSamplesCount*sizeof(T));
			return true;
		}

	protected:
		QVector<T> mData;
	};

	class SensorValueF32
		:public SensorValueNumeric<float>
	{
	public:
		explicit SensorValueF32(SensorDef::Type t):SensorValueNumeric<float>(t){}
		SensorValueF32(const SensorValueF32 &t):SensorValueNumeric<float>(t){}

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
		virtual QByteArray valueToString(quint32 index)const override;
	};

	class SensorValueF64
		:public SensorValueNumeric<double>
	{
	public:
		explicit SensorValueF64(SensorDef::Type t):SensorValueNumeric<double>(t){}
		SensorValueF64(const SensorValueF64 &t):SensorValueNumeric<double>(t){}

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
		virtual QByteArray valueToString(quint32 index)const override;
	};

	class SensorValueS8
		:public SensorValueNumeric<qint8>
	{
	public:
		explicit SensorValueS8(SensorDef::Type t):SensorValueNumeric<qint8>(t){}
		SensorValueS8(const SensorValueS8 &t):SensorValueNumeric<qint8>(t){}

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
	};

	class SensorValueU8
		:public SensorValueNumeric<quint8>
	{
	public:
		explicit SensorValueU8(SensorDef::Type t):SensorValueNumeric<quint8>(t){}
		SensorValueU8(const SensorValueU8 &t):SensorValueNumeric<quint8>(t){}

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
	};

	class SensorValueS16
		:public SensorValueNumeric<qint16>
	{
	public:
		explicit SensorValueS16(SensorDef::Type t):SensorValueNumeric<qint16>(t){}
		SensorValueS16(const SensorValueS16 &t):SensorValueNumeric<qint16>(t){}

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
	};

	class SensorValueU16
		:public SensorValueNumeric<quint16>
	{
	public:
		explicit SensorValueU16(SensorDef::Type t):SensorValueNumeric<quint16>(t){}
		SensorValueU16(const SensorValueU16 &t):SensorValueNumeric<quint16>(t){}

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
	};

	class SensorValueS32
		:public SensorValueNumeric<qint32>
	{
	public:
		explicit SensorValueS32(SensorDef::Type t):SensorValueNumeric<qint32>(t){}
		SensorValueS32(const SensorValueS32 &t):SensorValueNumeric<qint32>(t){}

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
	};

	class SensorValueU32
		:public SensorValueNumeric<quint32>
	{
	public:
		explicit SensorValueU32(SensorDef::Type t):SensorValueNumeric<quint32>(t){}
		SensorValueU32(const SensorValueU32 &t):SensorValueNumeric<quint32>(t){}

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
	};

	class SensorValueS64
		:public SensorValueNumeric<qint64>
	{
	public:
		explicit SensorValueS64(SensorDef::Type t):SensorValueNumeric<qint64>(t){}
		SensorValueS64(const SensorValueS64 &t):SensorValueNumeric<qint64>(t){}

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
	};

	class SensorValueU64
		:public SensorValueNumeric<quint64>
	{
	public:
		explicit SensorValueU64(SensorDef::Type t):SensorValueNumeric<quint64>(t){}
		SensorValueU64(const SensorValueU64 &t):SensorValueNumeric<quint64>(t){}

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
	};

	/**
	 * @brief Класс для значений датчиков, содержащих текстовые данные
	 */
	class SensorValueText
		:public SensorValue
	{
	public:
		explicit SensorValueText(SensorDef::Type t);
		virtual ~SensorValueText();
		SensorValueText(const SensorValueText &t);
		virtual double valueToDouble(quint32 totalIndex)const override;
		virtual qint64 valueToS64(quint32 totalIndex)const override;
		virtual QByteArray valueToString(quint32 totalIndex)const override;
		virtual QByteArray dumpToBinaryNoTime()const override;
		QByteArray get(quint32 dimIndex,quint32 packIndex)const;
		QByteArray getT(quint32 totalIndex)const;

		using SensorValue::valueToString;
		using SensorValue::valueToDouble;
		using SensorValue::valueToS64;

	protected:
		virtual bool valueFromString(quint32 totalIndex,const QByteArray &data)override;
		virtual void createData()override;
		virtual void freeData()override;
		virtual void copyDataFrom(const SensorValue *from)override;
		virtual bool valueIsEqual(const SensorValue *t,quint32 index)const override;
		bool copyFromBinaryData(const char *data)override;

	private:
		QVector<QByteArray> mData;
	};
}

#endif // SENSORVALUE_H
