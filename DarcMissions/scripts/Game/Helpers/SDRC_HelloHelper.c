//Helpers SDRC_MissionHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for missions. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_HelloHelper
{	
	static void Hello()
	{
		#ifndef SDRC_RELEASE
			Print("SDRC Hello: vanilla");
		#endif
	}
}