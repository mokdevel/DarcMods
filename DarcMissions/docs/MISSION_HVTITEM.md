## Mission : HVT Item - High Value Target Item
Example: [dc_missionConfig_HvtItem.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_HvtItem.json)

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/s_hvtitem.jpg" width=30% height=30%>

AIs have built a camp/installation which has a target that needs to be destroyed in order to complete the mission. Loot is spawned in the box when a win condition is achieved.

### GM requested mission
Location chosen will be close to where the mission was dropped. The mission location might not be exactly what you wanted as the it needs to have enough free space to spawn.

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
int buildingRadius : See Building parameters
array<int> missionList : See Common parameters
array<string> missionFiles : See Common parameters
```
```
# HvtItem specific
array<SDRC_HvtItem> subMissions : List of sub missions
```

### SDRC_HvtItem 
SDRC_HvtItem uses Camp functionality. 
```
# SDRC_HvtItem specific
SDRC_Camp
  campItems : (mandatory) Prefabs to spawn at the HvtItem missions. Note that first item shall be the loot box, but the item to destroy is defined with the targetIdx parameter.
int targetIdx : The item index in the camp that is to be destroyed for the mission to be completed.
```
