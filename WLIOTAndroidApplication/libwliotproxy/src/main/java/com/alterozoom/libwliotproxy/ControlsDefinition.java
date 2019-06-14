package com.alterozoom.libwliotproxy;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public class ControlsDefinition
{
	public static class ControlParam
	{
		public enum Type
		{
			BAD_TYPE,
			CHECKBOX,
			TEXT_EDIT,
			SELECT,
			SLIDER,
			DIAL,
			RADIO,
			HIDDEN
		};

		public ByteArray title=new ByteArray();
		public Type type=Type.CHECKBOX;
		public boolean vLayout=true;
		HashMap<ByteArray,ByteArray> attributes;

		ControlParam(ControlParam t)
		{
			title=new ByteArray(t.title);
			type=t.type;
			vLayout=t.vLayout;
			attributes=new HashMap<>();
			for(Map.Entry<ByteArray,ByteArray> e:t.attributes.entrySet())
				attributes.put(new ByteArray(e.getKey()),new ByteArray(e.getValue()));
		}

		public boolean equals(ControlParam t)
		{
			return title==t.title&&type==t.type&&attributes==t.attributes&&vLayout==t.vLayout;
		}

		static ByteArray typeToString(Type t)
		{
			if(t==Type.CHECKBOX)
				return ByteArray.fromString("checkbox");
			else if(t==Type.TEXT_EDIT)
				return ByteArray.fromString("text_edit");
			else if(t==Type.SELECT)
				return ByteArray.fromString("select");
			else if(t==Type.SLIDER)
				return ByteArray.fromString("slider");
			else if(t==Type.DIAL)
				return ByteArray.fromString("dial");
			else if(t==Type.RADIO)
				return ByteArray.fromString("radio");
			else if(t==Type.HIDDEN)
				return ByteArray.fromString("hidden");
			else return new ByteArray();

		}
		static Type typeFromString(ByteArray s)
		{
			if(s.equals(ByteArray.fromString("checkbox")))
				return Type.CHECKBOX;
			else if(s.equals(ByteArray.fromString("text_edit")))
				return Type.TEXT_EDIT;
			else if(s.equals(ByteArray.fromString("select")))
				return Type.SELECT;
			else if(s.equals(ByteArray.fromString("slider")))
				return Type.SLIDER;
			else if(s.equals(ByteArray.fromString("dial")))
				return Type.DIAL;
			else if(s.equals(ByteArray.fromString("radio")))
				return Type.RADIO;
			else if(s.equals(ByteArray.fromString("hidden")))
				return Type.HIDDEN;
			else return Type.BAD_TYPE;
		}
	};

	public static class CommandControl
	{
		public ByteArray title=new ByteArray();
		public ByteArray buttonText=new ByteArray();
		public ByteArray command=new ByteArray();
		public ArrayList<ControlParam> params=new ArrayList<>();
		public boolean vLayout=true;
		public boolean forceBtn=false;

		CommandControl(CommandControl t)
		{
			title=new ByteArray(t.title);
			buttonText=new ByteArray(t.buttonText);
			command=new ByteArray(t.command);
			params=new ArrayList<>();
			for(ControlParam p:t.params)
				params.add(new ControlParam(p));
			vLayout=t.vLayout;
			forceBtn=t.forceBtn;
		}

		public boolean equals(CommandControl t)
		{
			return title==t.title&&command==t.command&&vLayout==t.vLayout&&forceBtn==t.forceBtn&&
				params==t.params&&buttonText==t.buttonText;
		}
	};

	public static class ControlsGroup
	{
		public static class Element
		{
			public enum Type
			{
				GROUP,
				CONTROL
			};

			private Type type;
			private Object value;

			public Element(CommandControl c)
			{
				type=Type.CONTROL;
				value=c;
			}

			public Element(ControlsGroup g)
			{
				type=Type.GROUP;
				value=g;
			}

			public boolean equals(Element t)
			{
				return type==t.type&&value.equals(t.value);
			}

			public boolean isGroup()
			{
				return type==Type.GROUP;
			}

			public boolean isControl()
			{
				return type==Type.CONTROL;
			}

			public ControlsGroup group()
			{
				if(type==Type.GROUP)
					return (ControlsGroup)value;
				return null;
			}

			public CommandControl control()
			{
				if(type==Type.CONTROL)
					return (CommandControl)value;
				return null;
			}
		};

		public ByteArray title=new ByteArray();
		public boolean vLayout=true;
		ArrayList<Element> elements=new ArrayList<>();

		public boolean equals(ControlsGroup t)
		{
			if(vLayout!=t.vLayout||!title.equals(t.title))return false;
			if(elements.size()!=t.elements.size())return false;
			for(int i=0;i<elements.size();++i)
				if(!elements.get(i).equals(t.elements.get(i)))
					return false;
			return true;
		}

		public static boolean parseJsonDescription(ByteArray data,ControlsGroup controls)
		{
			//TODO
			return false;
		}

		public static boolean parseXmlDescription(ByteArray data,ControlsGroup controls)
		{
			//TODO
			return false;
		}

		public static ByteArray dumpToJson(ControlsGroup controls)
		{
			//TODO
			return null;
		}

		public static ByteArray dumpToXml(	ControlsGroup controls,boolean shortTags)
		{
			//TODO
			return null;
		}

		public ArrayList<CommandControl> extractCommandsList()
		{
			ArrayList<CommandControl> retVal=new ArrayList<>();
			extractCommandsList(retVal);
			return retVal;
		}

		private void extractCommandsList(ArrayList<CommandControl> list)
		{
			for(Element elem:elements)
			{
				if(elem.isGroup())
					elem.group().extractCommandsList(list);
			else
				list.add(new CommandControl(elem.control()));
			}
		}

		/*private:
		static bool parseJsonCommand(const QJsonObject &controlObject,CommandControl &control,
		bool shortStrings,bool ignoreSomeErrors);
		static bool parseJsonGroup(const QJsonObject &groupObject,ControlsGroup &group,
		bool shortStrings,bool ignoreSomeErrors);
		static bool parseXmlCommand(QDomElement &commandElem,CommandControl &command,
		bool shortStrings,bool ignoreSomeErrors);
		static bool parseXmlGroup(QDomElement &groupElem,ControlsGroup &group,
		bool shortStrings,bool ignoreSomeErrors);
		static void dumpControlToJson(QJsonObject &controlObj,const CommandControl &c);
		static void dumpGroupToJson(QJsonObject &groupObj,const ControlsGroup &g);
		static void dumpControlToXml(QDomDocument &doc,QDomElement &controlElem,
		const CommandControl &c,bool shortTags);
		static void dumpGroupToXml(QDomDocument &doc,QDomElement &groupElem,const ControlsGroup &grp,bool shortTags);*/

	};
}
