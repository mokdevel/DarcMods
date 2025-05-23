//Helpers SDRC_Log.c

//------------------------------------------------------------------------------------------------
/*!
Logging functionality that fits my needs
*/

enum DC_LogLevel
{
	NONE,	//0
	ERROR,	//1 - Prints FATAL, ERROR, WARNING
	NORMAL,	//2 - Prints NORMAL + all above
	DEBUG,	//3 - Prints DEBUG + all above
	ALL		//4 - Prints everything including VERBOSE and SPAM. It's a lot.
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
sealed class SDRC_Log
{
	static private DC_LogLevel m_LogLevel = DC_LogLevel.NORMAL;
	
/*	static void SDRC_Log()
	{
		m_LogLevel = DC_LogLevel.NORMAL;
		Print("[SDRC_Log] Started. (LogLevel = " + SCR_Enum.GetEnumName(DC_LogLevel, m_LogLevel), LogLevel.NORMAL);
	}*/
	
	//------------------------------------------------------------------------------------------------
	static void SetLogLevel(DC_LogLevel logLevel)
	{
		if (m_LogLevel != logLevel)
		{
			Print("[SDRC_Log] LogLevel changed to " + SCR_Enum.GetEnumName(DC_LogLevel, logLevel) + " (was: " + SCR_Enum.GetEnumName(DC_LogLevel, m_LogLevel) + ")", LogLevel.NORMAL);
		}
		else
		{
			Print("[SDRC_Log] LogLevel: " + SCR_Enum.GetEnumName(DC_LogLevel, m_LogLevel), LogLevel.NORMAL);
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

	
