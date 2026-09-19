//SDRC_ChopperCompModdedEnemy.c

//------------------------------------------------------------------------------------------------	
// Enemy related
//
// Enemy and attack related functions collected in this file.
//------------------------------------------------------------------------------------------------	

//------------------------------------------------------------------------------------------------
modded class SDRC_ChopperComp
{
	//------------------------------------------------------------------------------------------------	
	/*!
	Enable/Disable enemy searching
	*/		
	override void SetEnemySearchType(SDRC_EHeliEnemySearchType type)
	{
		m_EnemySearchType = type;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!
	Get last known enemy position
	*/
	override vector GetEnemyPosition()
	{
		return m_vAttackPosition;
	}

	//------------------------------------------------------------------------------------------------	
	/*!
	Sets attack position
	*/
	override void SetAttackPosition(vector pos)
	{
		//We will not reset attack position if there is still time left
		if ( (m_fAttackPositionSetTime > 0) && (pos == vector.Zero) )
		{
			return;
		}
		
		//Set time for attack position
		if (pos == vector.Zero)
		{
			m_fAttackPositionSetTime = 0;
		}
		else
		{
			m_fAttackPositionSetTime = params.enemyKnownTime;
		}
		
		//Set position to right height
		if (pos != vector.Zero)
		{		
			if (pos[1] == 0)
			{
				float y = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]);
				pos[1] = y;			
			}
		}
		
		if (m_vAttackPosition != vector.Zero)
		{
			m_vAttackPositionOld = m_vAttackPosition;
		}		
		m_vAttackPosition = pos;
		
		#ifdef WORKBENCH
			SDRC_DebugHelper.DeleteDebugSphere(m_sDid + "att");		
			if (pos != vector.Zero)
			{
				SDRC_DebugHelper.AddDebugSphere(pos, ARGB(32, 255, 0, 0), 4.0, m_sDid + "att");
			}
		#endif
	}
}