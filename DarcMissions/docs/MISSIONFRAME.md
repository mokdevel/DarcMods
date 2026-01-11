# Basics
Once the mod starts to run it will wait for ```missionStartDelay``` before the missions start to spawn. The missions have their own life cycle with a defined cycle time. Once a mission is completed and no players are near by, the missions will despawn. The missions are active a minimum of ```missionActiveTime```. The time is reset if there is a player within ```missionActiveDistance```. For more details, see [Mission Frame](#mission-frame).

## Mission position
Mission position is either defined or randomized. See [General parameters - pos](P_COMMON.md) and [Location parameters](P_LOCATIONS.md). Once a position is chosen, it's checked for validity. The position shall not be .. 
- .. too close to another mission
- .. too close to any player
- .. in water
- .. in [non valid area](NONVALIDAREAS.md)

In the case position is not usable, another try is made. Currently searching is limited to five tries before deciding that no position has been found. In this case, mission will not spawn.

GM requested missions generally follow the same rule. Depends slightly on the mission requested.

## Dynamic vs Static mission
In the configuration you can define missions either as a dynamic mission (```missionDynamic```) or static mission (```missionStatic```). The missions are the same but spawning is different. 

* Static missions will spawn immediately when the server starts. Once all static missions are spawned, no new ones will be spawned during the session. If a static mission fails to start, it will not be tried again. For example, if you define 30 missions as static, on a small map there may not be locations that fit the missions criteria. You may end up with 20 missions running. For static missions ``activeTime`` should be long. NOTE: No popup will be provided to players for static mission spawns. Marker visibility can be defined with ``showStaticMissionMarker``.

* Dynamic missions will spawn on steady pace. The ``activeTime`` should be relatively short so that the missions despawn and new ones are brought to players. Note that no dynamic missions, including GM requested ones, will spawn if static missions are still spawning.

## Requested mossions
GM can drop in more missions to the map. These are dynamic missions and will add to the total of dynamic missions. Requested missions do not respect the mission limits soy ou can spawn missions as a GM even if you have reached the maximum count of dynamic missions. GM requested missions are spawned with a higher priority compared to normal dynamic missions. If you set both static and dynamic missions to zero, only GM spawned missions will spawn.

Requested missions do not check if a mission position is valid. You can overload an area with multiple missions if that is wanted.

## Mission end (win or lose)
Each mission has a win condition. The normal case is to eliminate a certain amount of AI to reach the goal. Once the win condition has been reached, the mission is kept alive for ```missionActiveTimeToEnd``` seconds. The time is reset if there is a player within ```missionActiveDistance```. At the end of a mission, all spawned items will despawn (for example camps) but also vehicles. 
Available win conditions:
```
  0 = NONE 			  : Unused
  1 = AI_KILL_ALL     : Kill all AIs to win
  2 = AI_KILL_75      : 75%
  3 = AI_KILL_50      : 50%
  4 = AI_KILL_RANDOM  : 30%-100% of AIs to kill. This is set randomly when mission starts.
  5-9: RESERVED       : Reserved for future use
  10 = HVT_KILL_VIP   : Kill the target VIP - shall be used only with HVTVIP mission.
```

## MissionFrame cycle
```
----|------------|------------|------------|------------|------------|------------|----...
    |-------------------------|-------------------------|-------------------------|----...
    |------------------------------| missionActiveTime
                              |------------------------------| missionActiveTime
                                                        |------------------------------...
    |O----o----o----o----o----|O------o------o-----o----|O---o---o---o---o---|O----o---...
     ^                         ^                         ^
     mission is spawned        mission is spawned        mission is spawned 
     |----| missionCycleTime : Each mission has a cycle time it's running. It may differ between mission types
    |------------| missionFrameCycleTime : The cycle time to manage mission spawning, deletion etc...
    |-------------------------| missionDelayBetweeen : Time between mission spawns 
|---| missionStartDelay : Time waited before first mission is spawned
    |------------------------------| missionActiveTime : Mission is kept alive this amount of time.
                                     Note that if a player is within missionActiveDistance, the time will reset.      
```
