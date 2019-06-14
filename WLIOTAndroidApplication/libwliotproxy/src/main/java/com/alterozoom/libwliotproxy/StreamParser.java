package com.alterozoom.libwliotproxy;

import java.io.ByteArrayOutputStream;
import java.util.ArrayList;

public class StreamParser
{
	private enum StreamParserState
	{
		NORMAL,
		ESCAPE,//next symbol is escaped
		ESCAPE_HEX1,//next symbol is first of 2-letter hex code
		ESCAPE_HEX2//next symbol is second of 2-letter hex code
	}

	public static class StreamParserEventsCb
	{
		public void onStreamWasReset()
		{}

		public void onNewMessage(Message m)
		{}
	}

	private StreamParserState state;
	ByteArrayOutputStream nextMessageTitle;
	ArrayList<ByteArrayOutputStream> nextMessageArgs;
	ByteArrayOutputStream currentFilledStr;
	byte[] hexChars;
	ArrayList<StreamParserEventsCb> callbacks;

	public StreamParser()
	{
		state=StreamParserState.NORMAL;
		nextMessageTitle=new ByteArrayOutputStream();
		nextMessageArgs=new ArrayList<>();
		currentFilledStr=nextMessageTitle;
		hexChars=new byte[2];
	}

	public void addCb(StreamParserEventsCb cb)
	{
		callbacks.add(cb);
	}

	public void removeCb(StreamParserEventsCb cb)
	{
		callbacks.remove(cb);
	}

	public void pushData(byte[] data)
	{
		for(int i=0;i<data.length;++i)
			putByte(data[i]);
	}

	public void putByte(byte b)
	{
		if(b==0)
		{
			reset();
			for(StreamParserEventsCb cb:callbacks)
				cb.onStreamWasReset();
			return;
		}
		if(state==StreamParserState.ESCAPE)
			parseCharInEscapeState(b);
		else if(state==StreamParserState.ESCAPE_HEX1)
		{
			hexChars[0]=b;
			state=StreamParserState.ESCAPE_HEX2;
		}
		else if(state==StreamParserState.ESCAPE_HEX2)
		{
			hexChars[1]=b;
			state=StreamParserState.NORMAL;
			Integer cc=Integer.parseInt(ByteArray.fromBA(hexChars),16);
			if(cc!=null)
				currentFilledStr.write((byte)cc.intValue());
		}
		else //NORMAL
			parseCharInNormalState(b);
	}

	public void reset()
	{
		currentFilledStr=nextMessageTitle;
		nextMessageTitle.reset();
		nextMessageArgs.clear();
		state=StreamParserState.NORMAL;
	}

	private void parseCharInNormalState(byte c)
	{
		if(c=='\\')
			state=StreamParserState.ESCAPE;
		else if(c=='|')
		{
			ByteArrayOutputStream s=new ByteArrayOutputStream();
			nextMessageArgs.add(s);
			currentFilledStr=s;
		}
		else if(c=='\n')
		{
			Message m=new Message();
			m.title=new ByteArray(nextMessageTitle.toByteArray());
			for(ByteArrayOutputStream s:nextMessageArgs)
				m.args.add(new ByteArray(s.toByteArray()));
			currentFilledStr=nextMessageTitle;
			nextMessageTitle.reset();
			nextMessageArgs.clear();
			if(m.title.dataRef().length!=0)
				for(StreamParserEventsCb cb:callbacks)
					cb.onNewMessage(m);
		}
		else
			currentFilledStr.write(c);

	}

	private void parseCharInEscapeState(byte c)
	{
		if(c=='\\')
			currentFilledStr.write('\\');
		else if(c=='n')
			currentFilledStr.write('\n');
		else if(c=='|')
			currentFilledStr.write('|');
		else if(c=='0')
			currentFilledStr.write(0);
	else if(c=='x')
	{
		state=StreamParserState.ESCAPE_HEX1;
		return;
	}
	else
		currentFilledStr.write(c);
		state=StreamParserState.NORMAL;

	}
}
