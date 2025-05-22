## Mission : Squatters
Example: [dc_missionConfig_Squatters.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Squatter.json)

A building is guarded by AIs with loot available.

The missions tries to find floors in the building and spawn AI and the loot box on them. This is not perfect and sometimes you will find stuff under e.g. stairs or in the attic where there is no access. Different from other missions, you should define individual characters as AI. They should be returning to the building in case they've left it to hunt you. 

The mission can be configured in two different ways:
1) Find a building in a location by defining both ```locationTypes``` and ```buildingNames```. First the location is chosen and then the building is searched in it within ```buildingRadius```.
2) Find a certain building type from the map by defining only ```buildingNames```. Leave ```locationTypes``` empty. ```buildingRadius``` is ignored. NOTE: The mod DarcCore will cache the buildings found from the map for faster searching. The list ```buildingNames``` is used as an additional filter on the cached results.

```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
int buildingRadius : The radius to search for suitable buildings.
array<int> squatterList : The indexes of squatters.
array<SCR_DC_Squatter> squatters : List of squatters
```

### SCR_DC_Squatter
```
string comment : See General parameters
vector pos : See General parameters
string posName : See General parameters
string title : See General parameters
string info : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<int> aiCount : (min, max) Amount of AI _characters_ to spawn.
array<string> aiTypes : The prefab names of AI _characters_. The AI is randomly picked from this list.
int aiSkill : See General parameters
float aiPerception : See General parameters
array<string> buildingNames : Names of the buildings. This works as a wildcard. For example:
  "Barracks_" will include all barrack types on the map.
  "PubVillage_E_1L01" will include this type of pub and will ignore the others.
string lootBox : The box for loot. See Loot box for compatible values.
SCR_DC_Loot loot : (optional) Loot found in the mission. 
```