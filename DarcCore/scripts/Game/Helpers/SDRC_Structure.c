//Helpers SDRC_Structure.c

//------------------------------------------------------------------------------------------------
/*!
Structure class
*/

class SDRC_Structure
{
	private string resource;
	private vector position;
	private vector rotation;	//"Xrot YRot ZRot"

	void Set(string resource_, vector position_, vector rotation_ = "0 0 0")
	{
		resource = resource_;
		position = position_;
		rotation = rotation_;
	};

    // Getter functions
    string GetResource()
    {
        return resource;
    }

    vector GetPosition()
    {
        return position;
    }

    vector GetRotation()
    {
        return rotation;
    }

	float GetRotationY()
    {
        return rotation[1];
    }

    // Setter functions
    void SetResource(string resource_)
    {
        resource = resource_;
    }

    void SetPosition(vector position_)
    {
        position = position_;
    }

    void SetRotation(vector rotation_)
    {
        rotation = rotation_;
    }
}
