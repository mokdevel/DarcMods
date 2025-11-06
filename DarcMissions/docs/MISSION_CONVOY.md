## Mission : Convoy
Example: [dc_missionConfig_Convoy.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Convoy.json)

AI patrol is spawned in a vehicle to move around the map. The convoy will go from ```posStart``` to ```posDestination```. When destination has been reached, the patrol will LOITER.

Note on GM requested mission: The location for the mission will be searched from the closest road. If no road is found, the mission will not spawn. The start location is what you define, destination is random unless defined in ``posDestination``.

See [Common](./P_COMMON.md) , [Loot](./P_LOOT.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
bool disableArsenal : See Common parameters
array<int> missionList : See Common parameters
int distanceToPlayer : If no players this close to the convoy and convoyTime has passed, despawn mission.
array<ref SDRC_Convoy> subMissions : The sub missions
```

### SDRC_Convoy
```
SDRC_MissionConfigGeneral general : See General parameters
SDRC_MissionConfigAi ai :
```
```
# SDRC_Convoy specific
array<string> vehicleTypes : The prefab names of vehicles. The vehicle is randomly picked from this list.
float cruiseSpeed : Speed to drive in km/h. 30 is a good value so that the convoy is not driving too fast.
SCR_DC_Loot loot : (optional) Loot found in the vehicle. 
```
