//Helpers SDRC_ListHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various list related things
*/

//------------------------------------------------------------------------------------------------
class SDRC_ListHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Find the index for the list with listName
	*/
	static int FindListIndex(array<ref SDRC_List> lists, string listName)
	{
		int index = -1;
		
		//Find the right list index		
		for (int i = 0; i < lists.Count(); i++)		
		{
			if (lists[i].id == listName)
			{
				index = i;
				break;
			}
		}
		
		return index;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find a listName that has populated items
	
	This will go through the testList items to find a list that has items in it. The idea is to find a 
	list with items in it. 
	
	- When a prefab name starting with "{" is found, it will be picked.
	- For the tag names (like VEHICLE_TRACKED_ARMED), we check the _lists_ contents to find the right list and check 
	  that it has items to use.
	
	\return Empty is returned if none found.
	*/
	static string FindPopulatedList(array<ref SDRC_List> lists, array<string> testList)
	{
		int ri = testList.GetRandomIndex();
		string listName = "";
		
		//Is the provided list empty
		if (ri == -1)
		{
			return listName;
		}
		
		for (int i = 0; i < testList.Count() - 1; i++)
		{
			string l_name = testList[ri];
			
			//If it is a prefab name, try loading it 
			if (l_name[0] == "{")
			{
				//If success, we can continue.
				Resource resource = Resource.Load(l_name);
				if (resource.IsValid())
				{
					return l_name;
				}
			}

			int l_idx = FindListIndex(lists, l_name);
			
			if (!lists[l_idx].items.IsEmpty())
			{
				listName = lists[l_idx].id;
				break;
			}
			
			//Try next one.
			ri++;
			if (ri == testList.Count())
			{
				ri = 0;
			}			
		}
		
		return listName;
	}
}
