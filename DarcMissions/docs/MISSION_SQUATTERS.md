## Mission : Squatters
Example: [dc_missionConfig_Squatters.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Squatter.json)

A building is guarded by AIs with loot available. The missions tries to find floors in the building and spawn AI and the loot box on them. Loot is spawned when a win condition is achieved.

### GM requested mission
The building chosen will as close as possible to the location where mission was dropped. In case there are multiple houses beside each other, the mission location might not be exactly what you wanted.

### Parameters
In Squatters mission it is recommended to use characters instead of groups. The AI have a waypoint to return back to the house. Spawning individual characters is easier in buildings.

See [Common](./P_COMMON.md) , [AI](./P_COMMON.md#SDRC_MissionConfigAi), [Loot](./P_LOOT.md), [Building](./P_BUILDING.md)
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
# Squatters specific
array<SDRC_Squatter> subMissions : List of sub missions
```

### SDRC_Squatter
```
SDRC_MissionConfigGeneral general : See General parameters
SDRC_MissionConfigAi ai : See AI parameters
```
```
# Squatters specific
array<string> buildingNames : See Building parameters
string lootBox : The box for loot. See Loot box for compatible values.
SCR_DC_Loot loot : (optional) Loot found in the mission.  See Loot parameters.
```
Notes on AI parameters:
```
types : The list of AI _characters_. The AI is randomly picked from this list. Here you can use:
  C_TYPE of character defined in enemyLists or a resourceName for a character.
```
