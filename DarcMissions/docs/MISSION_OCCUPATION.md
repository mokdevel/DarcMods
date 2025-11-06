## Mission : Occupation
Example: [dc_missionConfig_Occupation.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Occupation.json)

AI patrol is spawned to a location like city. They will patrol the location and may have loot with them. Loot is spawned when a win condition is achieved.

Note on GM requested mission: The location is where the mission was dropped.

See [Common](./P_COMMON.md) , [Loot](./P_LOOT.md) , [Structures](./P_STRUCTURE.md) , [Camp](./P_CAMP.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
bool disableArsenal : See General parameters
array<int> occupationList : The indexes of occupations.
array<SDRC_Camp> occupations : List of occupations.
```
