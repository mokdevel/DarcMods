## Waypoint parameters
The mission AI can be given general rules on how to create waypoints for their movement. You need to define the waypoint generation rule and the movement rule and the rest is created automatically.

```waypointRange``` is used as the range for example for RADIUS. The area with waypoints will be a value between min and max. If you have multiple groups, they most likely will have different values and different areas to roam.
```
array<int> waypointRange : (min, max) The random radius for waypoint creation.
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
  NONE        : Not used anywhere. The rest are names of the mission types.
  RANDOM      : Use one of these randomly: LOITER (1), SCATTERED (2), RADIUS (4), 
  SCATTERED   : Completely random waypoints without any logic. 
  RADIUS      : AI follow a path that is close to a circle with a radius. There is some additional randomization to avoid a perfect circle.
  ROUTE       : AI follow a route from A to B. The route is created with waypoints along the road. Once the destination is reached, the AI will LOITER
  LOITER      : Loitering is where AI hand around in the location. AR does not have this yet, so a DEFEND waypoint is created. 
  SLOTS       : AI goes from a slot to slot. NOTE: This will not work unless the map has slots (the S/M/L letters on map) defined.
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
  NONE        : Not used anywhere. The rest are names of the mission types.
  RANDOM      : Pick a random one from MOVECYCLE (1), PATROLCYCLE (4), MOVE (1), PATROL (1).
  MOVE        : Creates move waypoints. AI will LOITER once reaching the last waypoint.
  PATROL      : Same as MOVE but with patrol speed.
  MOVECYCLE   : Creates move waypoints in cycke. AI will restart the cycle once all waypoints are visited.
  PATROLCYCLE : Same as MOVECYCLE but with patrol speed.
  LOITER      : If LOITER is chosen for generation, LOITER will be enforced for the movetype too.
```