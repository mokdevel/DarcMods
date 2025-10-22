# Camp structure
This is a class that is used by multiple missions.

See [General parameters](./P_COMMON.md) , [Loot](./P_LOOT.md) , [Structures](./P_STRUCTURE.md) , [Camp](./P_CAMP.md) , [Waypoints](./P_WAYPOINT.md)

### SDRC_Camp
```
SDRC_MissionConfigGeneral general : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<int> groupCount : See General parameters
array<int> waypointRange : See Waypoint parameters. NOTE: If loot is defined, the first group is spawned near the loot regardless of waypointRange values.
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
array<string> groupTypes : See General parameters
int aiSkill : See General parameters
float aiPerception : See General parameters
int emptySize : The size (radius) of the empty space needed to decide on a mission position.
SCR_DC_Loot loot : (optional) Loot found in the mission. 
array<SDRC_Structure> campItems  : (optional) The structure list for the camp.
```
