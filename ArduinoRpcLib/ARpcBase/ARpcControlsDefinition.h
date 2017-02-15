#ifndef ARPCCONTROLSDEFINITION_H
#define ARPCCONTROLSDEFINITION_H

#include <QMap>
#include <QString>
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
	QMap<QString,QString> constraints;
};

class ARpcControlsElement
{
public:
	virtual ~ARpcControlsElement(){}
};

//TODO add layout to command
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
		const ARpcControlsGroup* group()const;
		const ARpcControlsCommand* control()const;

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
