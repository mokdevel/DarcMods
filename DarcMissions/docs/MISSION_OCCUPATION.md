## Mission : Occupation
Example: [dc_missionConfig_Occupation.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Patrol.json)

AI patrol is spawned to a location like city. They will patrol the location and may have loot with them.

```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
int emptySize : The size (radius) of the empty space needed to decide on a mission position.
bool disableArsenal : See General parameters
array<int> occupationList : The indexes of occupations.
array<SCR_DC_Occupation> occupations : List of oocupations.
```

### SCR_DC_Occupation
```
string comment : See General parameters
vector pos : See General parameters
string posName : See General parameters
string title : See General parameters
string info : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<int> groupCount : See General parameters
array<int> waypointRange : See Waypoint parameters. NOTE: If loot is defined, the first group is spawned near the loot regardless of waypointRange values.
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
array<string> groupTypes : See General parameters
int aiSkill : See General parameters
float aiPerception : See General parameters
SCR_DC_Loot loot : (optional) Loot found in the mission. 
array<ref SCR_DC_Structure> campItems  : (optional) The structure list for the camp.
```