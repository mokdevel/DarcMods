class SCR_DC_MapMarkerEntityClass : GenericEntityClass {}

class SCR_DC_MapMarkerEntity : GenericEntity
{
	protected bool m_bIsTurnedOn;					// this value is edited only on authority's side

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Authority_Method(bool turningOn)
	{
		Print("authority-side code");

//		if (turningOn == m_bIsTurnedOn)				// the authority has authority
//			return;									// prevent useless network messages

//		m_bIsTurnedOn = turningOn;

		Rpc(RpcDo_Broadcast_Method, turningOn);		// send the music broadcast request
		Rpc(RpcDo_Owner_Method);					// run specific code on the owner's entity (that may or may not be the authority)
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_Owner_Method()
	{
		Print("owner-side code - RpcDo_Owner_Method");
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_Broadcast_Method(bool turningOn)
	{
		Print("proxy-side code - RpcDo_Broadcast_Method");
//		PlayMusic(turningOn);
	}

	// public methods
	void TurnOn()
	{
		Rpc(RpcAsk_Authority_Method, true);
	}

	void TurnOff()
	{
		Rpc(RpcAsk_Authority_Method, false);
	}
}

/*

class SCR_DC_MapMarkerEntity : GenericEntity
{
	[RplProp(onRplName: "OnTurnedOnUpdated")]
	protected int m_bIsTurnedOn = 0;			// this value is to be updated by the authority, not set locally by proxies (even owner)
													// if it is set locally, the change will not broadcast and there will be a difference between the proxy and the authority
													// this state discrepancy will last until authority's next update broadcast

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Authority_Method(bool turningOn)
	{
//		if (turningOn == m_bIsTurnedOn)
//			return;									// prevent useless network messages

		Print("authority-side code");
		m_bIsTurnedOn++;							// m_bIsTurnedOn is changed only in an authority-targeting method
													// it will broadcast over the network automatically due to Replication setting (line 3)

		SetLedLightColour();						// SetLedLightColour is not automatically called on the authority

		Replication.BumpMe();						// tell the Replication system this entity has changes to be broadcast
													// the Replication system will update the member variable AFTER RpcAsk_Authority_Method is done
	}

	protected void OnTurnedOnUpdated()
	{
		Print("proxy-side code");					// this method is called on proxies when m_bIsTurnedOn is updated through Replication
													// it is NOT called on the authority
		SetLedLightColour();
	}

	protected void SetLedLightColour()
	{
//		if (m_bIsTurnedOn)
//			SomeLightClass.SetLedLightColour(this, Color.Green);
//		else
//			SomeLightClass.SetLedLightColour(this, Color.Red);
	}

	// public methods
	void TurnOn()
	{
//		if (m_bIsTurnedOn)							// m_bIsTurnedOn can be read from any entity
//			return;

		Rpc(RpcAsk_Authority_Method, true);
	}

	void TurnOff()
	{
//		if (!m_bIsTurnedOn)
//			return;

		Rpc(RpcAsk_Authority_Method, false);
	}
}

*/