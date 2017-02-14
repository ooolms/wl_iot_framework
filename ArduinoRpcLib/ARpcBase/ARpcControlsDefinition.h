#ifndef ARPCCONTROLSDEFINITION_H
#define ARPCCONTROLSDEFINITION_H

#include <QVariantMap>
#include <QSharedPointer>

class ARpcControlParam
{
public:
	enum Type
	{
		BAD_TYPE,
		CHECKBOX,
		TEXT_EDIT,
		SELECT,
		SLIDER,
		DIAL
	};

public:
	static QString typeToString(Type t);
	static Type typeFromString(const QString &s);

public:
	QString title;
	Type type=BAD_TYPE;
	QVariantMap constraints;
};

class ARpcControlsElement
{
public:
	virtual ~ARpcControlsElement(){}
};

class ARpcControlsCommand
	:public ARpcControlsElement
{
public:
	QString title;
	QString command;
	QList<ARpcControlParam> params;
	bool syncCall=false;
};

class ARpcControlsGroup
	:public ARpcControlsElement
{
public:
	enum Layout
	{
		VERTICAL,
		HORIZONTAL
	};

	class Element
	{
	public:
		enum Type{GROUP,CONTROL};

	public:
		explicit Element(ARpcControlsCommand *c);
		explicit Element(ARpcControlsGroup *g);
		bool isGroup()const;
		bool isControl()const;
		ARpcControlsGroup* group();
		ARpcControlsCommand* control();

	private:
		Type type;
		QSharedPointer<ARpcControlsElement> value;
	};

public:
	static bool parseJsonDescription(const QString &data,ARpcControlsGroup &controls);
	static bool parseXmlDescription(const QString &data,ARpcControlsGroup &controls);

public:
	QString title;
	Layout layout=VERTICAL;
	QList<Element> elements;
};

#endif // ARPCCONTROLSDEFINITION_H
