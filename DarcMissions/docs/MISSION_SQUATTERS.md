## Mission : Squatters
Example: [dc_missionConfig_Squatters.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Squatter.json)

A building is guarded by AIs with loot available. The missions tries to find floors in the building and spawn AI and the loot box on them. Loot is spawned when a win condition is achieved.

Note on GM requested mission: The building chosen will as close as possible to the location where mission was dropped. In case there are multiple houses beside each other, the mission location might not be exactly what you wanted.

See [Common and General parameters](./P_COMMON.md) , [Loot](./P_LOOT.md) , [Building](./P_BUILDING.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
int buildingRadius : See Building parameters
array<int> squatterList : The indexes of squatters.
array<SDRC_Squatter> squatters : List of squatters
```

### SDRC_Squatter
```
SDRC_MissionConfigGeneral general : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<int> aiCount : (min, max) Amount of AI _characters_ to spawn.
array<string> aiTypes : The list of AI _characters_. The AI is randomly picked from this list. Here you can use:
  C_TYPE of character defined in enemyLists or a resourceName for a character. 
int aiSkill : See General parameters
float aiPerception : See General parameters
array<string> buildingNames : See Building parameters
string lootBox : The box for loot. See Loot box for compatible values.
SCR_DC_Loot loot : (optional) Loot found in the mission.  See Loot parameters.
```
