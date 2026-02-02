//SDRC_ChopperHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperHelper
{
	//------------------------------------------------------------------------------------------------	
	/*!	
	Add a destination forward. 
	Create a destination from current heli position, along it's 
	
	\param destination Next position to fly to. Multiple destinations can be defined by calling multiple times.
	\param type How to fly .. kinda. If set as FINAL, once reaching the destination, helicopter will stop flying. 
	*/
	static vector GetDestinationForward(IEntity owner, float distance)
	{
		vector direction = vector.Direction(owner.GetOrigin(), owner.GetTransformAxis(2));
		direction.Normalize();
		vector origin = owner.GetOrigin();
		vector position = origin + direction * distance;
		//Keep the flying flat
		position[1] = origin[1];
		return position;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Creates a random destination point
	*/	
	static vector GetRandomPosition(vector pos, float distanceLow, float distanceHigh)
	{
		float distance = SDRC_Misc.RandomFloat(distanceLow, distanceHigh);
		vector newpos;
		
		//If distance is under 1.0, use world percentage
		if (distance < 1.0)
		{
			newpos = SDRC_Misc.GetRandomWorldPosPercentage(distance);
		}
		else
		{
			newpos = SDRC_Misc.GetCoordinatesOnCircle(pos, distance, SDRC_Misc.RandomInt(0, 360));
		}
				
		return newpos;
	}	
	
	//------------------------------------------------------------------------------------------------	
	static void DrawLine(vector p0, vector p1, int color = Color.RED)
	{
		int shapeFlags = ShapeFlags.ONCE;
		vector p[2];
		p[0] = p0;
		p[1] = p1;		
		Shape.CreateLines(color, shapeFlags, p, 2);		
	}
	
	//------------------------------------------------------------------------------------------------
	static bool IsStillWorking(IEntity owner, bool inInit)
	{
		//If still in init, don't care if no pilots etc yet set.
		if (inInit)
		{
			return true;
		}

		//If working and at least one pilot, all good
		if ( (SDRC_VehicleHelper.IsWorking(owner)) && (SDRC_VehicleHelper.PilotCountAlive(owner) > 0) )
		{
			return true;
		}
		
		//Set damage so it should be destroyed on crash		
		SetHealth(owner, SDRC_Misc.RandomFloat(0, 0.05));
/*		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		if (damageManager)
		{
			float damage = SDRC_Misc.RandomFloat(0, 0.05);
			damageManager.SetHealthScaled(damage);		
		}*/
		
		//Make the chopper fly unsteadily
		VehicleHelicopterSimulation helicopter_s = VehicleHelicopterSimulation.Cast(owner.GetRootParent().FindComponent(VehicleHelicopterSimulation));
		if (helicopter_s)
		{
			float force = SDRC_Misc.RandomFloat(0, 0.1);
	        helicopter_s.RotorSetForceScaleState(0, force);
			force = SDRC_Misc.RandomFloat(0.1, 2.5);
	        helicopter_s.RotorSetForceScaleState(1, force);
			force = SDRC_Misc.RandomFloat(0.0, 0.1);
			helicopter_s.SetThrottle(force);
		}
				
		return false;
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Get helicopter altitude from ground. 

	Could use also float altitude = m_Helicopter_s.GetAltitudeAGL();
	*/	
	static float GetAltitude(IEntity owner)
	{
		float surfaceY = SDRC_Misc.GetSurfaceYWithWater(owner.GetOrigin());
		vector origin = owner.GetOrigin();
		float altitude = origin[1] - surfaceY;
		
		return altitude;		
	}
		
	//------------------------------------------------------------------------------------------------	
	// Damage settings
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	static float GetHealth(IEntity owner)
	{
		float health = -1;
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		if (damageManager)
		{
			health = damageManager.GetHealth();
		}
		
		return health;
	}
	
	//------------------------------------------------------------------------------------------------
	static void SetHealth(IEntity owner, float health)
	{
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(owner.FindComponent(DamageManagerComponent));
		if (damageManager)
		{
			damageManager.SetHealthScaled(health);
		}
		
		SDRC_Log.Add("[SDRC_ChopperHelper:SetHealth] Setting health: " + health, LogLevel.DEBUG);
	}
}