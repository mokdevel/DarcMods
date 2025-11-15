## Mission : HVT Item - High Value Target Item
Example: [dc_missionConfig_HvtItem.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_HvtItem.json)

AIs have built a camp/installation which has a target that needs to be destroyed in order to complete the mission. Loot is spawned in the box when a win condition is achieved.

### GM requested mission
Location chosen will be close to where the mission was dropped. The mission location might not be exactly what you wanted as the it needs to have enough free space to spawn.

### Parameters
See [Common](./P_COMMON.md) , [AI](./P_COMMON.md#SDRC_MissionConfigAi), [Loot](./P_LOOT.md) , [Building](./P_BUILDING.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
int buildingRadius : See Building parameters
array<int> missionList : See Common parameters
array<sring> missionFiles : See Common parameters
```
```
# HvtVip specific
array<SDRC_HvtVip> subMissions : List of sub missions
```

### SDRC_HvtVip
```
SDRC_MissionConfigGeneral general : See General parameters
SDRC_MissionConfigAi ai : See AI parameters
```
```
# HvtVip specific
array<string> buildingNames : See Building parameters
string lootBox : The box for loot. See Loot box for compatible values.
SCR_DC_Loot loot : (optional) Loot found in the mission.
string target : The resourceName of the VIP to eliminate. Here you can use:
  C_TYPE of character defined in enemyLists
  ResourceName for a character. The character will be set to same faction as the mission enemy. For example:
  "{A517C72CEF150898}Prefabs/Characters/Factions/CIV/Businessman/Character_CIV_Businessman_2.et"
```
