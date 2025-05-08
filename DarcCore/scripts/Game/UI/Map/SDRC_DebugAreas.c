//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class SDRC_DebugAreas : SDRC_MapCanvasLayer
{		
	override void Draw()
	{			
		m_DrawCommands.Clear();

		foreach(SDRC_DebugHelperPos mapCircle : SDRC_DebugHelper.m_MapCircle)
		{
			DrawCircle(mapCircle.pos, mapCircle.radius, mapCircle.color);
		}		
	}
/*	
	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
	}
	
	override void OnMapClose(MapConfiguration config)
	{	
		super.OnMapClose(config);
	}*/
}