//Helpers SDRC_MissionHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for missions. 
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_HelloHelper
{
	override static void Hello()
	{
		#ifndef SDRC_RELEASE		
			super.Hello();
			Print("SDRC Hello: compat");
		#endif
	}
}