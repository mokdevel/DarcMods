//Helpers SCR_DC_Structure.c

//------------------------------------------------------------------------------------------------
/*!
Structure class
*/

class SCR_DC_Structure
{
	string m_Resource;
	vector m_Position;
	vector m_Rotation;

	void SCR_DC_Structure(string resource, vector position, vector rotation)
	{
		m_Resource = resource;
		m_Position = position;
		m_Rotation = rotation;
	};

    // Getter functions
    string GetResource()
    {
        return m_Resource;
    }

    vector GetPosition()
    {
        return m_Position;
    }

    vector GetRotation()
    {
        return m_Rotation;
    }

    // Setter functions
    void SetResource(string resource)
    {
        m_Resource = resource;
    }

    void SetPosition(vector position)
    {
        m_Position = position;
    }

    void SetRotation(vector rotation)
    {
        m_Rotation = rotation;
    }		
}
