# Non valid areas
Example: [dc_nonValidArea_example.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_nonValidArea_example.json)

This a list of areas where missions shall not spawn. For example a safe zone would be listed here. Note that the default one is empty: [dc_nonValidArea.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_nonValidArea.json)
```
int version : See General parameters
string author : See General parameters
bool showOnMap : Show a red area where non valid area is. Helps to properly define the areas. (Currently works only in WB)
array<SCR_DC_NonValidArea> nonValidAreas : List of areas where missions shall not spawn.
```

## SCR_DC_NonValidArea
```
string worldName : The world name this should be affecting. For example Arland or Eden. If left empty, area will be valid for all maps. Note that Arland and GM_Arland are considered different maps.
vector pos : Center position of area
float radius : Radius from the center where the missions shall not spawn.
string name : Your own name for the area. Not used by the mod.
```