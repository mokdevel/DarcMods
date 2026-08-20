# DarcChopperCompatFF
<img src="https://github.com/mokdevel/DarcMods/blob/main/DarcChopperCompatFF/thumbnail.png" width=30% height=30%>

Compatibility mod for Freedom Fighters that integrates DarcChoppers to game play. DarcMissions and DarcMissionsCompatFF is thoroughly designed to work on any map without specific configs.

## Functionality
* Choppers join battle with a random chance.

When a battle starts, chance to spawn one or more helicopters is checked. By default the spawn chance ``spawnChance`` is 30%. Campaign progress percentage is added to the value and this is the total chance for spawning. 
  Example: With a 20% progression in the campaing, the total default chance is 50%.

If a request to spawn is made, a delay of ``spawnDelay`` (min/max) defined minutes is set for each helicopter. Eventually the helicopter will arrive and do a 'search and destroy' activity around the area. This action will happen for ``attackTime`` (min/max) defined minutes. Once the time is over, the helicopter will fly away and despawn.

See also: [Search and destroy](https://github.com/mokdevel/DarcMods/blob/main/DarcChopper/docs/P_WP_COMMANDS.md#wp_search_destroy)

## Installation
Just activate the mod (and dependencies) and you're good to go.

## Configuration
Configuration is done in .json files.

# Configuration files
The configuration files will be under your ```profile\DarcMods\*```. Depending on the usage the profile is in your ArmaReforger, ArmaReforgerWorkbench or in your server profile directory. For example ```C:\Users\username\Documents\My Games\ArmaReforger\profile\DarcMods```.

## Parameters
Example: [dc_compatFFConfigChopper.json](https://github.com/mokdevel/DarcMods/blob/main/DarcChopperCompatFF/ExampleConfigs/dc_compatFFConfigChopper.json)

```
int version : Version id of the file
string author : Author of the file
string comment : Generic comment. Not used in game.
array<string> factions : Factions to use for choppers. "FF" uses Freedom Fighters default.
float spawnChance : Chance percentage that is added to campaign progress. 
array<int> spawnDistance : Distance min/max to spawn the attacking chopper.
array<int> spawnDelay : (minutes) Min/max delay before spawning the attacking chopper.
array<int> chopperCount : The amount choppers to spawn.
array<int> attackTime : (minutes) Min/max time to attack an area.
array<ref int> attackList : The list of mission indexes.
array<ref SDRC_ChopperCompatFF> attacks : List of attacks
```

# Version history

## Next Version

## 202608xx
Fixes:
* Sometimes helicopters were found floating with their engines off in the air.

## 20260812
Fixes:
* Disable streaming and persistence for choppers.

## 20260809
Main Feature:
* Support for friendly choppers.
  * Known issue: If you spawn a friendly chopper too far away from player, FF will remove the chopper. Same happens if they fly too far away. Something to fix but not for this release.
  * Known issue: There is an error ``The object ___ has no RplComponent. This will not be seen by players.`` which I'm investigating. Reason unknown and happens only with FF.

## 20260731
Fixes:
* Mod waits for gamemode to start before starting to run. This could have side effects.
* Configuration file load issues.
* dc_compatFFConfigChopper.json was created with test settings. Recommendation is to delete the file and receive an updated one at startup.
* Default chance for choppers arriving to battle is 30% + progression%.

## 20260421
First version