## Mission : Hunter
Example: [dc_missionConfig_Hunter.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Hunter.json)

AIs are hunting you and following you. They will receive regularly information about where the closest player. The location is known to them with an error of ```rndDistanceToPlayer```. You can outrun them by going further than ```maxDistanceToPlayer```.

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
# Hunter specific
int minDistanceToPlayer : Hunter group minimum distance to player for spawn
int maxDistanceToPlayer : ..max distance
int rndDistanceToPlayer : The error on the location where AI thinks you are. (0..rndDistanceToPlayer).
array<SDRC_Hunter> subMissions : List of sub missions
```

## SDRC_Hunter
```
SDRC_MissionConfigGeneral general : See General parameters
SDRC_MissionConfigAi ai : See AI parameters
```
Hunter AI is defined as any other AI, but will not use any of the parameters below.
```
waypointRange
waypointGenType
waypointMoveType
```
