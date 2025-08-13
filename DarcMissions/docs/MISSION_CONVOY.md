## Mission : Convoy
Example: [dc_missionConfig_Convoy.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Convoy.json)

AI patrol is spawned in a vehicle to move around the map. The convoy will go from ```posStart``` to ```posDestination```. When destination has been reached, the patrol will LOITER.

Note on GM requested mission: The location for the mission will be searched from the closest road. If no road is found, the mission will not spawn. The start location is what you define, destination is random unless defined in ``posDestination``.

See [Common and General parameters](./P_COMMON.md) , [Loot](./P_LOOT.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
int convoyTime : (seconds) Time to drive around. Once this time has passed and not players nearby, despawn mission.
int distanceToPlayer : If no players this close to the convoy and convoyTime has passed, despawn mission.
bool disableArsenal : See General parameters
array<int> convoyList : The indexes of convoys.
array<SDRC_Convoy> convoys : List of convoys
```

### SDRC_Convoy
```
string comment : See General parameters
vector pos : See General parameters. This is the starting point for the convoy 
vector posDestination : Destination for the convoy to go to. See also General parameters - pos.
string posName : See General parameters
string title : See General parameters
string info : See General parameters
DC_EMissionWinCondition winCondition : See General parameters
string winMessage : See General parameters
string loseMessage : See General parameters
int xp = 0 : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<string> groupTypes : See General parameters
int aiSkill : See General parameters
float aiPerception : See General parameters
array<string> vehicleTypes : The prefab names of vehicles. The vehicle is randomly picked from this list.
float cruiseSpeed : Speed to drive in km/h. 30 is a good value so that the convoy is not driving too fast.
SCR_DC_Loot loot : (optional) Loot found in the vehicle. 
```
