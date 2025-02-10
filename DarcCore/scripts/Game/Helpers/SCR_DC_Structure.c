//Helpers SCR_DC_Structure.c

//------------------------------------------------------------------------------------------------
/*!
Structure class
*/

class SCR_DC_Structure
{
	string m_Resource;
	vector m_Position;
	vector m_Rotation;	//"Xrot YRot ZRot"

	void Set(string resource, vector position, vector rotation = "0 0 0")
	{
		m_Resource = resource;
		m_Position = position;
		m_Rotation = rotation;
	};

	//TBD: As the things are not private, setters/getters are unnecessary
	
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

	float GetRotationY()
    {
        return m_Rotation[1];
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
