## Mission : Patrol
Example: [dc_missionConfig_Patrol.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Patrol.json)

AI patrol is spawned to move around the map. The may have a destination where they're moving to or just randomly roam the map.

### GM requested mission
The location is where the mission was dropped.

### Parameters
See [Common](./P_COMMON.md) , [AI](./P_COMMON.md#SDRC_MissionConfigAi)
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
# Patrol specific
int distanceToPlayer : If no players this close to any players and patrolingTime has passed, despawn mission.
array<SDRC_Patrol> subMissions : List of sub missions
```

### SDRC_Patrol
```
SDRC_MissionConfigGeneral general : See General parameters
SDRC_MissionConfigAi ai : See AI parameters
```
Notes on AI parameters:
```
waypointRange : For patrols, the values should be quite big to have them move around the map.
waypointGenType :
  ROUTE : The patrol will go from posStart to posDestination. When destination has been reached, the patrol will LOITER.
  RADIUS, SCATTERED : The patrol will follow a path created with waypointRange starting from posStart. posDestination is ignored.
```
