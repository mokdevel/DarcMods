# DarcMissions
A mission frame work for Arma Reforger


```
----|
    |--------------------------|--------------------------|------------------...
    |O----o----o----o----|O------o------o-----|O---o---o---o---o---|O----o---...
     ^                    ^
     mission is spawned   mission is spawned

    |--------------------| missionDelayBetweeen : Time between mission spawns 
|---| missionStartDelay : Time waited before first mission is spawned
     |----| missionCycleTime : Each mission has a cycle time it's running. It may differ from mission to mission
     |-------------------------| missionActiveTime : Mission is kept alive this amount of time.
                                 Note that if a player is within missionActiveDistance, the time will reset.      
```
