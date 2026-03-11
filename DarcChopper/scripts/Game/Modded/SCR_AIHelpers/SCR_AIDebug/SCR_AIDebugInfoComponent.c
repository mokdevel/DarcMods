  /* ------------------------------------------------------------------------------------------------ /
 / CRX-EAI Modded Class ( #CRX-EAIModdedClass )                                                      /
/ ------------------------------------------------------------------------------------------------ */
/*

//------------------------------------------------------------------------------------------------
modded class SCR_AIDebugInfoComponent : ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	override bool UpdateUI()
	{
		bool updateUI;
		
		// m_FixedAI = null;
		
		// return super.UpdateUI();
		
		// ///////////////////////////////////////////////////////////////////////////////////////////////
		#ifdef CRX_DEBUG
		SetSelectedAIAgentOrEntity();
		#endif
		// ///////////////////////////////////////////////////////////////////////////////////////////////
		
		// ///////////////////////////////////////////////////////////////////////////////////////////////
		#ifdef CRX_DEVELOPMENT
		
		if (m_FixedAI)
			m_TargetAI = m_FixedAI;
		else
		{
			if (m_CameraManager)
			{
				m_Camera = m_CameraManager.CurrentCamera();
				
				if (m_Camera)
				{
					IEntity targetAI = m_Camera.GetCursorTarget();
					
					if (targetAI && targetAI.IsInherited(ChimeraCharacter))
						m_TargetAI = targetAI;
				}
			}
		}
		
		if (m_TargetAI)
		
		// if (m_FixedAI)
		{
			// m_TargetAI = m_FixedAI;
			
			ChimeraCharacter chimeraCharacter = ChimeraCharacter.Cast(m_TargetAI);
			
			// DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_SELECT_FIXED_AGENT, true);
			
			if (chimeraCharacter)
			{
				AIControlComponent controlComponent = chimeraCharacter.GetAIControlComponent();
				
				if (controlComponent)
				{
					AIAgent agent = controlComponent.GetAIAgent();
					
					if (agent)
					{
						SCR_ChimeraAIAgent chimeraAIAgent = SCR_ChimeraAIAgent.Cast(agent);
						
						if (chimeraAIAgent)
						{
							m_InfoComponent = chimeraAIAgent.m_InfoComponent;
							
							m_UtilityComponent = chimeraAIAgent.m_UtilityComponent;
							
							if (m_UtilityComponent)
								m_CombatComponent = m_UtilityComponent.m_CombatComponent;
						}
					}
				}
			}
			
			// m_wRoot.SetVisible(true);
			
			updateUI = super.UpdateUI();
			
			if (m_FixedAI)
			{
				vector boundMin, boundMax;
				
				m_TargetAI.GetWorldBounds(boundMin, boundMax);
				
				vector targetCenter = (boundMax + boundMin) * 0.5;
				
				vector position = GetGame().GetWorkspace().ProjWorldToScreen(targetCenter, m_TargetAI.GetWorld());
				
				FrameSlot.SetPos(m_wRoot.GetChildren(), position[0] + OFFSET_X, position[1]);
			}
			
			return updateUI;
		}
		
		m_wRoot.SetVisible(false);
		
		// ///////////////////////////////////////////////////////////////////////////////////////////////
		#else
		// ///////////////////////////////////////////////////////////////////////////////////////////////
		
		updateUI = super.UpdateUI();
		
		#endif
		// ///////////////////////////////////////////////////////////////////////////////////////////////
		
		return updateUI;
	}
	
	// ///////////////////////////////////////////////////////////////////////////////////////////////
	#ifdef CRX_DEBUG
	
	//------------------------------------------------------------------------------------------------
	void SetSelectedAIAgentOrEntity()
	{
		bool inPlayMode = GetGame().InPlayMode();
		
		SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
		
		// inPlayMode = true;
		
		if (inPlayMode)
		{
			AIAgent selectedAgent;
			IEntity selectedEntity;
			
			bool selectedAIAgentOrEntity = GetSelectedAIAgentOrEntity(selectedAgent, selectedEntity);
			
			m_FixedAI = selectedEntity;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetSelectedAIAgentOrEntity(out AIAgent outAgent, out IEntity outEntity)
	{
		set<SCR_EditableEntityComponent> selectedEntities = new set<SCR_EditableEntityComponent>();
		SCR_BaseEditableEntityFilter baseEditableEntityFilter = SCR_BaseEditableEntityFilter.GetInstance(EEditableEntityState.SELECTED);
		
		baseEditableEntityFilter.GetEntities(selectedEntities);
		
		if (selectedEntities.IsEmpty())
		{
			outAgent = null;
			outEntity = null;
		}
		else
		{
			SCR_EditableCharacterComponent editableCharacterComponent = SCR_EditableCharacterComponent.Cast(selectedEntities[0]);
			
			if (editableCharacterComponent)
			{
				AIAgent agent = editableCharacterComponent.GetAgent();
				
				if (agent)
				{
					outAgent = agent;
					outEntity = agent.GetControlledEntity();
				}
			}
			else
			{
				foreach (SCR_EditableEntityComponent editableEntityComponent : selectedEntities)
				{
					SCR_EditableGroupComponent editableGroupComponent = SCR_EditableGroupComponent.Cast(editableEntityComponent);
					
					if (editableGroupComponent)
					{
						outAgent = null;
						outEntity = null;
						
						SCR_AIGroup group = SCR_AIGroup.Cast(editableGroupComponent.GetOwner());
						
						if (group)
						{
							outAgent = group.GetLeaderAgent();
							outEntity = group.GetLeaderEntity();
							
							return true;
						}
					}
				}
				
				outAgent = null;
				outEntity = null;
				
				// outEntity = selectedEntities[0].GetOwner();
			}
		}
		
		return true;
	}
	
	#endif
	// ///////////////////////////////////////////////////////////////////////////////////////////////	
}
*/