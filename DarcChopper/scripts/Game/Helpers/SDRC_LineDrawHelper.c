//SDRC_LineDrawHelper.c

enum SDRC_ELineDrawCommand
{
	SET_COLOR = -1,
	END = -2,
}

//------------------------------------------------------------------------------------------------
class SDRC_LineDrawHelper
{
	static void CreateDrawCommandsFromData(array<vector> lineData, array<ref CanvasWidgetCommand> drawCommands)
	{
		array<float> drawVertices = {};
		drawCommands.Clear();
		
		if (!SDRC_MenuHelper.GetShowFlyPath())
		{
			return;
		}
		
		// ----------------		
		// Do the drawing on canvas
		
		//Do the line drawing
		int color = -1;
		vector prevPos = vector.Zero;
		foreach (vector pos : lineData)
		{
			//If an end signal is found, we either end or start a new line.
			if (pos[0] == SDRC_ELineDrawCommand.END)
			{
				color = -1;
				prevPos = vector.Zero;
				continue;
			}

			//Change color
			if (pos[0] == SDRC_ELineDrawCommand.SET_COLOR)
			{
				color = pos[1];
				continue;
			}
			
			//If this is the first point we found, let's use that as the previous position. This is the start of the line.
			if (prevPos == vector.Zero)
			{
				prevPos = pos;
				continue;
			}
			
			AddVertice(prevPos, drawVertices);
			AddVertice(pos, drawVertices);
			prevPos = pos;
			//Insert into pool of draw commands
			drawCommands.Insert(AddLines(drawVertices, color));			
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Calculate the vertice screen coords and add to vertices list
	*/
	static void AddVertice(vector pos, out array<float> vertices)
	{
		//Calculate screen position of point
		BaseWorld m_World = GetGame().GetWorld();
		WorkspaceWidget m_Workspace = GetGame().GetWorkspace();
		vector x0 = m_Workspace.ProjWorldToScreenNative(pos, m_World);
		if (x0[2] > 0)
		{
			vertices.Insert(x0[0]);
			vertices.Insert(x0[1]);
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Add vertices to LineDrawCommand and return it
	*/
	static LineDrawCommand AddLines(out array<float> vertices, int color = Color.DARK_GREEN)
	{		
		//Create draw command
		ref LineDrawCommand line = new LineDrawCommand();	
		line.m_Vertices = {};
		line.m_iColor = color;
		line.m_fOutlineWidth = 0;
		line.m_fWidth = 1.0;
		line.m_Vertices.Copy(vertices);
		vertices.Clear();
		return line;
	}	
}