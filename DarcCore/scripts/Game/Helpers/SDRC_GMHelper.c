//Helpers SDRC_GMHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_GMHelper
{	
	static void AddMapCicrle(vector pos, float range, int color)
	{	
		SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
		if (gmComponent)
		{
			gmComponent.AddMapCicrle(pos, range, color);
		}
		else
		{
			SDRC_Log.Add("[SDRC_GMHelper:AddMapCicrle] SDRC_RplGMComp not found", LogLevel.ERROR);
		}		
	}
}
