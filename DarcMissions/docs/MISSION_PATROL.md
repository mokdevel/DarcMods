## Mission : Patrol
Example: [dc_missionConfig_Patrol.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Patrol.json)

AI patrol is spawned to move around the map. The may have a destination where they're moving to or just randomly roam the map.

```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
int patrolingTime : (seconds) Time to patrol. Once this time has passed and not players nearby, despawn mission.
int distanceToPlayer : If no players this close to any players and patrolingTime has passed, despawn mission.
array<int> patrolList : The indexes of patrols.
array<SCR_DC_Patrol> patrols : List of patrols
```

### SCR_DC_Patrol
```
string comment : See General parameters
vector pos : See General parameters
vector posDestination : Destination for the patrol to go to. See also General parameters - pos.
string posName : See General parameters
string title : See General parameters
string info : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<int> groupCount : See General parameters
array<int> waypointRange : See Waypoint parameters. For patrols, the values should be quite big to have them move around the map.
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
  ROUTE : The patrol will go from posStart to posDestination. When destination has been reached, the patrol will LOITER.
  RADIUS, SCATTERED : The patrol will follow a path created with waypointRange starting from posStart. posDestination is ignored.
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
array<string> groupTypes : See General parameters
int aiSkill : See General parameters
float aiPerception : See General parameters
```