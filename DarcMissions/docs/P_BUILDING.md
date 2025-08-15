# Building functionality

Mod has functionality to find floors in the building and spawn AI and the loot box on them. This is not perfect and sometimes you will find stuff under e.g. stairs or in the attic where there is no access. Different from other missions, you should *define individual characters as AI* (C_TYPE). See: [enemy lists](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LISTS.md#enemy-list-names-id). The reason is that the individual AI is spawned inside the building and the mod tries to find a free spot inside it. Behaviour set tries to keep the AI inside the building. They may come after you outside, but should be returning to the building in case they've not found the players.

The functionality can be configured in two different ways:
1) Find a building in a location by defining both ```locationTypes``` and ```buildingNames```. First the location is chosen and then the building is searched in it within ```buildingRadius```.
2) Find a certain building type from the map by defining only ```buildingNames```. Leave ```locationTypes``` empty. ```buildingRadius``` is ignored. NOTE: The mod DarcCore will cache the buildings found from the map for faster searching. The list ```buildingNames``` is used as an additional filter on the cached results.
