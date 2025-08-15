## Mission : HVT VIP - High Value Target VIP
Example: [dc_missionConfig_HvtVip.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_HvtVip.json)

An AI is defined as a High Value Target VIP. Go take him down. The VIP 

The missions tries to find floors in the building and spawn AI and the loot box on them. 

Note on GM requested mission: The building chosen will as close as possible to the location where mission was dropped. In case there are multiple houses beside each other, the mission location might not be exactly what you wanted.

See [Common and General parameters](./P_COMMON.md) , [Loot](./P_LOOT.md) , [Building](./P_BUILDING.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
int buildingRadius : The radius to search for suitable buildings.
array<int> hvtVipList : The indexes of HvtVips.
array<SDRC_Squatter> hvtVips : List of HvtVips
```

### SDRC_HvtVip
```
SDRC_MissionConfigGeneral general : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<int> groupCount : (min, max) Amount of AI groups to spawn.
array<string> groupTypes : The resourceName names of AI groups. The AI is randomly picked from this list.
int aiSkill : See General parameters
float aiPerception : See General parameters
array<string> buildingNames : Names of the buildings. This works as a wildcard. For example:
  "Barracks_" will include all barrack types on the map.
  "PubVillage_E_1L01" will include this type of pub and will ignore the others.
string lootBox : The box for loot. See Loot box for compatible values.
SCR_DC_Loot loot : (optional) Loot found in the mission.
string target : The resourceName if the VIP to eliminate.
```
