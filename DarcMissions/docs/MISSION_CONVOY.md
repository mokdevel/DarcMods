## Mission : Convoy
Example: [dc_missionConfig_Convoy.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Convoy.json)

AI patrol is spawned in a vehicle to move around the map. The convoy will go from ```posStart``` to ```posDestination```. When destination has been reached, the patrol will LOITER.

```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
int convoyTime : (seconds) Time to drive around. Once this time has passed and not players nearby, despawn mission.
int distanceToPlayer : If no players this close to the convoy and convoyTime has passed, despawn mission.
bool disableArsenal : See General parameters
array<int> convoyList : The indexes of convoys.
array<SCR_DC_Convoy> convoys : List of convoys
```

### SCR_DC_Convoy
```
string comment : See General parameters
vector pos : See General parameters. This is the starting point for the convoy 
vector posDestination : Destination for the convoy to go to. See also General parameters - pos.
string posName : See General parameters
string title : See General parameters
string info : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<string> groupTypes : See General parameters
int aiSkill : See General parameters
float aiPerception : See General parameters
array<string> vehicleTypes : The prefab names of vehicles. The vehicle is randomly picked from this list.
float cruiseSpeed : Speed to drive in km/h. 30 is a good value so that the convoy is not driving too fast.
SCR_DC_Loot loot : (optional) Loot found in the vehicle. 
```