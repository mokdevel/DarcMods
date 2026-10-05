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
		//If position is reset, reset also known time and return to flight mode
		if (pos == vector.Zero)
		{
			m_fAttackPositionKnownTime = 0;
			m_vAttackPosition = pos;
			return;
		}
		
		//We will not reset attack position if there is still time left
		if (m_fAttackPositionKnownTime > 0)
		{
			return;
		}

		//If attack is set to same position as before, don't reset timers. Just return to continue hitting the previous spot.
		if (pos == m_vAttackPosition)
		{
			return;
		}
		
		//Set time for attack position
		m_fAttackPositionKnownTime = params.enemyKnownTime;
		
		//Set position to right height
		if (pos[1] == 0)
		{
			float y = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]);
			pos[1] = y;			
		}

		m_vAttackPosition = pos;
		
		#ifdef WORKBENCH
			SDRC_DebugHelper.DeleteDebugSphere(m_sDid + "att");		
			SDRC_DebugHelper.DeleteDebugPos(m_sDid + "att");		
			if (pos != vector.Zero)
			{
				SDRC_DebugHelper.AddDebugPos(pos, ARGB(32, 255, 0, 0), 4.0, m_sDid + "att", 2.0);
			}
		#endif
	}
}