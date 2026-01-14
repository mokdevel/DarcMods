# Non valid areas
Example: [dc_nonValidArea_example.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_nonValidArea_example.json)

This a list of areas where missions shall not spawn. For example a safe zone would be listed here. Note that the default one is empty: [dc_nonValidArea.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_nonValidArea.json)
```
int version : See General parameters
string author : See General parameters
array<SDRC_NonValidArea> nonValidAreas : List of areas where missions shall not spawn.
```

## SDRC_NonValidArea
```
string worldName : ** DEPRECATED ** Will be removed in the future but left for compatibility reasons for now.
vector pos : Center position of area
float radius : Radius from the center where the missions shall not spawn.
string name : Your own name for the area. Not used by the mod.
```
