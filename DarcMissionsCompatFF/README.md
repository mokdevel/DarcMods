# DarcMissionsCompatFF
Compatibility mod for Freedom Fighters. DarcMissions and DarcMissionsCompatFF is thoroughly designed to work on any map without specific configs. 

## Functionality
* Missions are not spawned close the hideouts
* Controlled areas have a percentage option to spawn missions. 
* Crashsite helicopters are cleaned from the map at server boot when a player joins the game. We don't want wrecks on the map.

## Installation
Just activate the mod (and dependencies) and you're good to go.

## Configuration
Configuration is done in .json files.

# Configuration files
The configuration files will be under your ```profile\DarcMods\*```. Depending on the usage the profile is in your ArmaReforger, ArmaReforgerWorkbench or in your server profile directory. For example ```C:\Users\username\Documents\My Games\ArmaReforger\profile\DarcMods```.

## Spawner config
Example: [dc_compatFFConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissionsCompatFF/ExampleConfigs/dc_compatFFConfig.json)

```
int version : Version id of the file
string author : Author of the file
string comment : Generic comment. Not used in game.
int hideOutSafeZoneDistance : The radious of a 'safe zone' to a hide out where a mission shall not spawn.
float spawnRateForGreenZones : The percentage chance to spawn a mission to green zone (0.05 = 5%)
```

# Version history

## Next release (unreleased)

..empty..

## 20251023
A new mod for compatibility with Freedom Fighters. First release.
* Missions are not spawned close the hideouts
* Controlled areas have a percentage option to spawn missions. 
* Crashsite helicopters are cleaned from the map at server boot when a player joins the game. We don't want wrecks on the map.
