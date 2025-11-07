## Mission : HVT VIP - High Value Target Very Important Person
Example: [dc_missionConfig_HvtVip.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_HvtVip.json)

An AI is defined as a High Value Target VIP. Go take him down. The VIP should be staying in place, but will activate (and defend) itself when a player is close by. The missions tries to find floors in the building and spawn AI and the loot box on them. Loot is spawned in th box when a win condition is achieved.

### GM requested mission
The building chosen will as close as possible to the location where mission was dropped. In case there are multiple houses beside each other, the mission location might not be exactly what you wanted.

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
array<int> hvtVipList : The indexes of HvtVips.
array<SDRC_Squatter> hvtVips : List of HvtVips
```

### SDRC_HvtVip
```
SDRC_MissionConfigGeneral general : See General parameters
SDRC_MissionConfigAi ai : See AI parameters
```
```
array<string> buildingNames : See Building parameters
string lootBox : The box for loot. See Loot box for compatible values.
SCR_DC_Loot loot : (optional) Loot found in the mission.
string target : The resourceName of the VIP to eliminate. Here you can use:
  C_TYPE of character defined in enemyLists
  ResourceName for a character. The character will be set to same faction as the mission enemy. For example:
  "{A517C72CEF150898}Prefabs/Characters/Factions/CIV/Businessman/Character_CIV_Businessman_2.et"
```
