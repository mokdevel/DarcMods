## Mission : Roadblock
Example: [dc_missionConfig_Roadblock.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Roadblock.json)

AIs have setup roadblocks on roads. They will patrol the location and may have loot with them. Loot is spawned when a win condition is achieved.

### GM requested mission
The location for the mission will be searched from the closest road. It will not be exactly where you put the mission.

### Parameters
The sub missions use Camp functionality. 

See [Common](./P_COMMON.md) , [AI](./P_COMMON.md#SDRC_MissionConfigAi), [Loot](./P_LOOT.md) , [Structures](./P_STRUCTURE.md) , [Camp](./P_CAMP.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
bool disableArsenal : See Common parameters
array<int> missionList : See Common parameters
array<string> missionFiles : See Common parameters
```
```
# Roadblock specific
array<SDRC_Camp> subMissions : List of sub missions
```
