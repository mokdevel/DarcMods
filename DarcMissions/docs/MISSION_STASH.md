## Mission : Stash
Example: [dc_missionConfig_Stash.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Stash.json)

A stash is spawned to a location. It may be up for grabs or have AI protecting it. The stash missions usually have a time limit to reach the stash to complete it regardless of AI being there or not. To get a win state, you need to get close enough of the stash.

### GM requested mission
The location is where the mission was dropped.

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
```
```
# Stash specific
int activeDistance : Distance that defines if mission is a win (loot spawns). Same distance used for keeping the mission active.
array<SDRC_Camp> subMissions : List of sub missions
```
