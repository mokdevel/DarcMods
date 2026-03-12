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
	static int FindRightList(array<ref SDRC_List> lists, string listName)
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
}
