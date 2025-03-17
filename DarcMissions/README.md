# DarcMissions
A mission frame work for Arma Reforger. There are various missions premade and their behaviour can be modified by editing the configuration jsons.

## Missions
- Hunter : AIs are hunting you and following you. They have some knowledge of where you are. You can outrun them or take them down. 
- Convoy : AIs are driving from a location A to B carrying valuable loot with them.
- Patrol : AIs are traveling either with a purpose to reach a location or to patrol areas.
- Crashsite : A helicopter carrying loot is damaged and is going to crash. Survivors will be protecting the wreck while waiting to be rescued.
- Occupation : A location is guarded by AIs with loot available. 

# MissionFrame cycle
```
----|------------|------------|------------|------------|------------|------------|----...
    |-------------------------|-------------------------|-------------------------|----...
    |------------------------------|------------------------------|--------------------...
    |O----o----o----o----o----|O------o------o-----o----|O---o---o---o---o---|O----o---...
     ^                    ^
     mission is spawned   mission is spawned
     |----| missionCycleTime : Each mission has a cycle time it's running. It may differ between mission types
    |------------| missionFrameCycleTime : The cycle time to manage mission spawning, deletion etc...
    |-------------------------| missionDelayBetweeen : Time between mission spawns 
|---| missionStartDelay : Time waited before first mission is spawned
    |------------------------------| missionActiveTime : Mission is kept alive this amount of time.
                                     Note that if a player is within missionActiveDistance, the time will reset.      
```

# Thanks
- The work is inspired by [Defent's Mission System (DMS)](https://github.com/Defent/DMS_Exile) in Arma 3.
- [HunterKiller mod by Rabid Squirrel](https://reforger.armaplatform.com/workshop/597324ECFC025225-HunterKiller) for the initial kick to get into coding.
