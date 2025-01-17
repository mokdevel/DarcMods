//Helpers SCR_DC_Log.c

//------------------------------------------------------------------------------------------------
/*!
Logging functionality that fits my needs
*/

enum DC_LogLevel
{
	NONE,
	ERROR,	//Prints FATAL, ERROR, WARNING
	NORMAL,	//Prints NORMAL + all above
	DEBUG,	//Prints DEBUG + all above
	ALL		//Prints everything
};

//Print method has these levels
/*
enum LogLevel
{
	SPAM,
	VERBOSE,
	DEBUG,
	NORMAL,
	WARNING,
	ERROR,
	FATAL,
}
*/

//------------------------------------------------------------------------------------------------
sealed class SCR_DC_Log
{
	static private DC_LogLevel m_LogLevel = DC_LogLevel.NORMAL;
	
	static void SCR_DC_Log()
	{
		m_LogLevel = DC_LogLevel.NORMAL;
		Print("[SCR_DC_Log] Started. (LogLevel = " + SCR_Enum.GetEnumName(DC_LogLevel, m_LogLevel), LogLevel.NORMAL);
//		m_LogLevel = DC_LogLevel.NONE;
	}
	
	//------------------------------------------------------------------------------------------------
	static void SetLogLevel(DC_LogLevel logLevel)
	{
		if (m_LogLevel != logLevel)
		{
			Print("[SCR_DC_Log] LogLevel changed to " + SCR_Enum.GetEnumName(DC_LogLevel, logLevel) + " (was: " + SCR_Enum.GetEnumName(DC_LogLevel, m_LogLevel) + ")", LogLevel.NORMAL);
		}
		m_LogLevel = logLevel;
	}

	static DC_LogLevel GetLogLevel()
	{
		return m_LogLevel;
	}

	//------------------------------------------------------------------------------------------------
	static void Add(string logEntry, LogLevel logLevel = LogLevel.NORMAL)
	{
		if (m_LogLevel == DC_LogLevel.NONE)
		{
			return;
		}
				
		switch (logLevel)
		{
			case LogLevel.SPAM:		
			case LogLevel.VERBOSE:
				if (m_LogLevel == DC_LogLevel.ALL)
				{
					Print(string.ToString(logEntry), LogLevel.NORMAL);
				}
				break;			
			case LogLevel.DEBUG:
				if ( (m_LogLevel == DC_LogLevel.ALL) || (m_LogLevel == DC_LogLevel.DEBUG) )
				{
					Print(string.ToString(logEntry), LogLevel.NORMAL);
				}
				break;			
			case LogLevel.WARNING:
			case LogLevel.ERROR:
			case LogLevel.FATAL:
				if ( (m_LogLevel != DC_LogLevel.ALL) && (m_LogLevel != DC_LogLevel.DEBUG) && (m_LogLevel != DC_LogLevel.ERROR))
				{
					break;
				}
			case LogLevel.NORMAL:
				Print(string.ToString(logEntry), logLevel);
			default:
		}
	}		
}

	
