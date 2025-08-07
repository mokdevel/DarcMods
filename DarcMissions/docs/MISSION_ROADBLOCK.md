## Mission : Roadblock
Example: [dc_missionConfig_Roadblock.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Roadblock.json)

AIs have setup roadblocks on roads. They will patrol the location and may have loot with them.

Note on GM requested mission: The location for the mission will be searched from the closest road. It will not be exactly where you put the mission.

See [Common and General parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_COMMON.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
bool disableArsenal : See General parameters
array<int> roadblockList : The indexes of roadblocks.
array<SDRC_Occupation> roadblocks : List of roadblocks.
```

NOTE: The roadblocks use the same ``SDRC_Occupation`` structure as occupations . See [SDRC_Occupation](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/MISSION_OCCUPATION.md#sdrc_occupation)
