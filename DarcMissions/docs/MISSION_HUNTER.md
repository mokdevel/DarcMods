## Mission : Hunter
Example: [dc_missionConfig_Hunter.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Hunter.json)

AIs are hunting you and following you. They will receive regularly information about where the closest player. The location is known to them with an error of ```rndDistanceToPlayer```. You can outrun them by going further than ```maxDistanceToPlayer```.

```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
string title : See General parameters
string info : See General parameters
array<int> groupCount : See General parameters
array<string> groupTypes : See General parameters
int aiSkill : See General parameters
float aiPerception : See General parameters
int minDistanceToPlayer : Hunter group minimum distance to player for spawn
int maxDistanceToPlayer : ..max distance
int rndDistanceToPlayer : The error on the location where AI thinks you are. (0..rndDistanceToPlayer). 
```