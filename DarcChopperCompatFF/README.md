# DarcChopperCompatFF
Compatibility mod for Freedom Fighters that integrates DarcChoppers to game play. DarcMissions and DarcMissionsCompatFF is thoroughly designed to work on any map without specific configs. 

<img src="https://github.com/mokdevel/DarcMods/blob/main/DarcChopperCompatFF/thumbnail.png" width=30% height=30%>

## Functionality
* Choppers join battle with a random chance.

When a battle starts, chance to spawn one or more helicopters is checked. By default the spawn chance is ``spawnChance``. Campaign progress percentage is added to this and this is the total chance for spawning. If a request to spawn is made, a delay of ``spawnDelay`` (min/max) defined minutes is set for each helicopter. Eventually the helicopter will arrive and do a 'search and destroy' activity around the area. This action will happen for ``attackTime`` (min/max) defined minutes. Once the time is over, the helicopter will fly away and despawn.

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
float spawnChance : Chance percentage that is added to campaign progress. This is the total percentage to spawn choppers to join the fight.
array<int> spawnDistance : Distance min/max to spawn the attacking chopper.
array<int> spawnDelay : (minutes) Min/max delay before spawning the attacking chopper.
array<int> chopperCount : The amount choppers to spawn.
array<int> attackTime : (minutes) Min/max time to attack an area.
array<ref int> attackList : The list of mission indexes.
array<ref SDRC_ChopperCompatFF> attacks : List of attacks
```

# Version history

## 20260730
Fixes:
*  dc_compatFFConfigChopper.json was created with test settings. Optionally you could delete the file and receive an updated one at startup.

## 20260421
First version