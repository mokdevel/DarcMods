# Basics
Once the mod starts to run it will wait for ```missionStartDelay``` before the missions start to spawn. The missions have their own life cycle with a defined cycle time. Once a mission is completed and no players are near by, the missions will despawn. The missions are active a minimum of ```missionActiveTime```. The time is reset if there is a player within ```missionActiveDistance```. For more details, see [Mission Frame](#mission-frame).

## Mission position
Mission position is either defined or randomized. See [General parameters - pos](./P_COMMON.md) and [Location parameters](./P_LOCATIONS.md). Once a position is chosen, it's checked for validity. The position shall not be .. 
- .. too close to another mission (``minDistanceToMission``)
- .. too close to any player (``minDistanceToPlayer``)
- .. too far away from any player (``maxDistanceToPlayer``)
- .. in water
- .. in [non valid area](./P_NONVALIDAREAS.md)

In the case position is not usable, another try is made. Currently searching is limited to five tries before deciding that no position has been found. In this case, mission will not spawn.

GM spawned missions will only check for mission not being in water or under map. If you want, you can spawn missions in for example non valid areas. 

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/missionposition.jpg" width=30% height=30%>

## Dynamic vs Static mission
In the configuration you can define missions either as a dynamic mission (```missionDynamic```) or static mission (```missionStatic```). The missions are the same but spawning is different. 

* Static missions will spawn immediately when the server starts. Once all static missions are spawned, no new ones will be spawned during the session. If a static mission fails to start, it will not be tried again. For example, if you define 30 missions as static, on a small map there may not be locations that fit the missions criteria. You may end up with 20 missions running. For static missions ``activeTime`` should be long. NOTE: No popup will be provided to players for static mission spawns. Marker visibility can be defined with ``showStaticMissionMarker``.

* Dynamic missions will spawn on steady pace. The ``activeTime`` should be relatively short so that the missions despawn and new ones are brought to players. Note that no dynamic missions, including GM requested ones, will spawn if static missions are still spawning.

## Requested missions
GM can drop in more missions to the map. These are dynamic missions and will add to the total of dynamic missions. Requested missions do not respect the mission limits soy ou can spawn missions as a GM even if you have reached the maximum count of dynamic missions. GM requested missions are spawned with a higher priority compared to normal dynamic missions. If you set both static and dynamic missions to zero, only GM spawned missions will spawn.

Requested missions do not check if a mission position is valid. You can overload an area with multiple missions if that is wanted.

## Mission count
Various aspects affect the mission count spawned on the map. There are two main types of missions, Static and Dynamic missions (see above). Both have a section for definitions in file ``dc_missionConfig.json`` which you can find under ``missionDynamic`` and ``missionStatic``. The parameter ``count`` defines the maximum amount of missions that will spawn for Dynamic and Static types. 

The mission types that can spawn are defined in ``missionTypeArray``. This is separate for Dynamic and Static missions so you can define if certain types are available at start up or during gameplay. The mission to spawn is randomly picked from the list, so you never know what missions will be available. 

Certain types of missions have a tendency to find a spawn position easier than others. For example, Crashsite location could happen anywhere so when it's picked from the list, it most likely is spawned. A Hunter mission location needs to have players near by and this is not true for all positions in the world. This could lead to a situation where you have a world full of Crashsites and other missions are not spawning as you would expect. 

To correct this, a parameter ``missionLimit`` is available. With this you can define that a certain ``missionType`` is only spawned to a certain count. For the above example, you could define a limit for Crashsites to e.g. ``2``. This will make it sure that you don't get spammed with spawned Crashsites. 

NOTE: If you spawn missions via GM, it will not limit the types nor counts. You as a GM, have made a decision to spawn a mission, it will be respected. So, you could spawn multiple Crashsites even if it has been limited via ``missionLimit``. GM spawned missions will be counted to the total ``count`` so the mod will not itself spawn more than the mission total or mission limit. 

Note from the developer: I like to spawn certain types of missions as Static in to the world to fill it with surprises. Especially Patrol, Convoy and Roadblock missions with a few Chopper missions are good to have in the world. Everything is random, so you never know what is spawned where.

Related documentation:
* [Mission configuration](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/MISSIONCONFIG.md#sdrc_missiontypeconfig) , separate for Dynamic and Static.
* [dc_missionConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/MISSIONCONFIG.md)

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
  10 = HVT_KILL_VIP   : Kill the target VIP. Shall be used only with HVTVIP mission.
  20 = HVT_DESTROY_ITEM : Destroy mission target. Even if you kill all AI, mission will not be a win. Shall be used only with HVTITEM mission.
  30 = FIND_IN_15     : Reach the mission location within 15 minutes to populate the loot.
  31 = FIND_IN_30     : .. 30 minutes .. 
  32 = FIND_IN_45     : .. 45 minutes .. 
  33 = FIND_IN_60     : .. 60 minutes .. 
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
