## Mission : Convoy
Example: [dc_missionConfig_Convoy.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Convoy.json)

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/s_convoy.jpg" width=30% height=30%>

AI patrol is spawned in a vehicle to move around the map. The convoy will travel between two points. When destination has been reached, the patrol will LOITER.

NOTE that ``pos`` parameter has additional functionality:
* If ``pos`` is left empty or with zero values, route will be picked from ``locationTypes``.
* If ``pos`` has multiple values, they are considered as pairs. The first one is start and the second is destination. 

Example:
``[0,0,0],[0,0,0],`` : A random route based on locationTypes
``[x0,y0,z0],[a0,b0,c0],`` : A route from x0 to a0
``[x1,0,z1],[a1,0,c1],`` : A route from x1 to a1, the Z-component is not necessary
``[x2,y2,z2],`` : This is an error as the second pair is missing. This will be used as: ``[x2,y2,z2],[0,0,0]`` where the route is from x2 to a random location based on locationTypes

### GM requested mission
The location for the mission will be searched from the closest road. If no road is found, the mission will not spawn. The start location is what you define, destination is random unless defined in ``posDestination``.

### Parameters
See [Common](./P_COMMON.md) , [AI](./P_COMMON.md#SDRC_MissionConfigAi), [Loot](./P_LOOT.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
bool disableArsenal : See Common parameters
array<int> missionList : See Common parameters
array<sring> missionFiles : See Common parameters
```
```
# Convoy specific
int distanceToPlayer : If no players this close to the convoy and mission time has passed, despawn mission.
array<SDRC_Convoy> subMissions : List of sub missions
```

### SDRC_Convoy
```
SDRC_MissionConfigGeneral general : See General parameters
SDRC_MissionConfigAi ai : See AI parameters
```
```
# SDRC_Convoy specific
array<string> vehicleTypes : The prefab names of vehicles. The vehicle is randomly picked from this list.
float cruiseSpeed : Speed to drive in km/h. 30 is a good value so that the convoy is not driving too fast.
SCR_DC_Loot loot : (optional) Loot found in the vehicle. 
```
