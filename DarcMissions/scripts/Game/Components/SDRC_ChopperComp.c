//SDRC_ChopperComp.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperCompClass : ScriptGameComponentClass { }
//SDRC_RplGMCompClass g_RplGMCompClass;

//------------------------------------------------------------------------------------------------
class SDRC_ChopperComp : ScriptGameComponent
{
	private static SDRC_ChopperComp s_Instance;	
	
	override void OnPostInit(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_ChopperComp] Starting SDRC_ChopperComp", LogLevel.NORMAL);
		s_Instance = this;				
		SetEventMask(owner, EntityEvent.FRAME | EntityEvent.POSTFRAME);
		Activate(owner);
	}
 
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	static SDRC_ChopperComp GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		
		VehicleHelicopterSimulation m_Vehicle_s = VehicleHelicopterSimulation.Cast(owner.FindComponent(VehicleHelicopterSimulation));
		
		if (!m_Vehicle_s)
		{
			Print("SDRC_ChopperComp No VehicleHelicopterSimulation");
		}

		vector pos = owner.GetOrigin();

//		owner.SetAngles("1 0 0");
//		owner.GetPhysics().SetVelocity("1 0 0");
//		owner.GetPhysics().SetAngularVelocity("1 0 0");
				
		//Draw velicity vector
		vector vVel = owner.GetPhysics().GetVelocity();
		vVel.Normalize();				
		DrawLine(pos, pos + (vVel * 30));
		
		//Draw angle vector
		vector vAng = owner.GetAngles();
		vAng.Normalize();				
		DrawLine(pos, pos + (vAng * 30), Color.GREEN);
		
	}

	void DrawLine(vector p0, vector p1, int color = Color.RED)
	{
		int shapeFlags = ShapeFlags.ONCE;
		vector p[2];
		p[0] = p0;
		p[1] = p1;		
		Shape.CreateLines(color, shapeFlags, p, 2);		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	
	*/
/*	override void EOnFrame(IEntity owner, float timeSlice)
	{
		Print("SDRC_ChopperComp");
	}*/
}