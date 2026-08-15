# DarcSpawner
<img src="https://github.com/mokdevel/DarcMods/blob/main/DarcSpawner/thumbnail.png" width=30% height=30%>

Spawns vehicles, items or other entities randomly on map with or without loot. Once everything is spawned, the mod will stop running.

The position for a spawn is searched from locations listed.

## Installation
Just activate the mod (and dependencies) and you're good to go.

## Configuration
Configuration is done in .json files.

# Configuration files
The configuration files will be under your ```profile\DarcMods\*```. Depending on the usage the profile is in your ArmaReforger, ArmaReforgerWorkbench or in your server profile directory. For example ```C:\Users\username\Documents\My Games\ArmaReforger\profile\DarcMods```.

For examples of configuration files see [ExampleConfigs](https://github.com/mokdevel/DarcMods/tree/main/DarcSpawner/ExampleConfigs).

## How does the spawn happen?
A random index is chosen from ``spawnSetList`` and the indexed ``spawnSet`` is selected. A random item from ``entities`` is selected. 

From the ``spawnSet`` location is chosen from the map of type ``locationTypes`` or if coordinates are defined in ``positions``, that position is used. Location type is information found from the map itself. Map makers set a parameter called ``EMapDescriptorType`` to an object to describe what the specific location is. For example, for a city, an object would have the ``EMapDescriptorType`` defined with value ``MDT_NAME_CITY``. With this information we know that this area is considered as a city. For more, see: [Location parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LOCATIONS.md)

The exact position is found with the position of the ``locationTypes`` or as defined in ``positions``. The position is then..
  * randomized with ``spawnRndRadius`` or 
  * if ``spawnOnRoad`` is set, a position on a road is searched. 

Randomization is recommended as we don't want the spawn to happen always in the same spot.

``spawnCount`` is the maximum amount of containers to spawn. If loot is defined, loot items are added to the entities which has the capability to store items.

## Spawner config
Example: [dc_spawnerConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcSpawner/ExampleConfigs/dc_spawnerConfig.json)

```
jsonVersion : Version of the json structure. Do not modify.
int version : Version number of the file for author use. Not used in game.
string author : Author of the file. Not used in game.
string comment : Generic comment to describe the file. Not used in game.
int spawnCount : Amount of entities (cars, lootboxes, etc..) to spawn.
float spawnWorldSizeMultiplier : If containerCount = 0, we search for the world size in km and multiple with this. For example: 4km wide map with spawnWorldSizeMultiplier = 2 results in spawnCount = 8 (4*2)
array<int> spawnSetList : The indexes of spawnsets.
array<SCR_DC_SpawnSet> spawnSets : List of spawners
```

### SCR_DC_SpawnSet
```
string comment : See General parameters
bool showMarker : Show marker on map. False will disable markers.
string markerType : The marker type to use. Default is "DARC_MISSION".
  For Arma Reforger default ones, you can use "PLACED_MILITARY". Other mods may extend this.
int markerIdx : The index of the icon of defined markerType
array<EMapDescriptorType> locationTypes : See Location parameters in DarcMissions
array<string> entities : What entities to spawn; cars, box, .. The entity to spawn is chosen randomly from the list.
int spawnRndRadius : Random radius where the spawnSet spawns. Once a location is found, additional randomization is done to avoid containers to be always in the same spot.
bool spawnOnRoad : Spawn the entities on a road. If no road network manager found, entities will be spawned as if this parameter would be false.
bool disableArsenal : Disable arsenal for vehicles so that only defined loot items are found.
SDRC_Loot loot : Loot found in the mission. See Loot parameters. Set to null if you are spawning entities that are not to be considered as loot containers.
```

## Related documentation:
Some parameters are described in DarcMissions documentation
- [Location parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LOCATIONS.md)
- [Loot](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LOOT.md)
- [LootLists](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LISTS.md#loot-lists)

# Addons
DarcSpawner supports addons to create additional configurations. Addon functionality is WIP so there might be issues.

## Animals
<img src="https://github.com/mokdevel/DarcMods/blob/main/DarcSpawner_Animals/thumbnail.png" width=30% height=30%>

Spawns animals randomly on map. 

All credits to Holden Shift for the amazing French Animals mod:
[French Animals](https://reforger.armaplatform.com/workshop/6A04798F752CBE57-BAR_FrenchAnimals)

Example: [dc_spawnerConfig_Animals.json](https://github.com/mokdevel/DarcMods/blob/main/DarcSpawner/ExampleConfigs/dc_spawnerConfig_Animals.json)

### Notes
* Currently spawns individual animals: Wolf, Deer, Doe, Rabbit, Fox
* Works on all maps just by enabling the mod.
* All configuration done via json files located in your profiles directory (after first run).

# Version history

## 20260815

### DarcSpawner
Main Features:
* Addons functionality.

Fixes:
* Improved position finding for items to be spawned.

### DarcSpawner_Animals
* First release

## 20260812
Fixes:
* Loot parameter was not possible to leave empty.

## 20260725
Fixes:
* Mod waits for gamemode to start before starting to run. This could have side effects.

## 20260601
Fixes:
* Mod can be run stand alone. There was a dependency to DarcChopper.
* Loot box spawning crashes (fix for 1.7).

## 20260502
Compatibility compile for DarcCore.

## 20260118
Fixed loot crate prefabs and added more loot.

## 20260115
Compatibility compile for DarcCore.

## 20260111
Compatibility compile for DarcCore. New json fileformat.

## 20251213
Compatibility compile for DarcCore.

## 20251128
Compatibility compile for DarcCore.

## 20251123
Compatibility compile for DarcCore.

## 20251102
Fixes:
- Default text for map marker was shown.

## 20251030
### Update for Arma Reforger 1.6
Minor fixes and support for 1.6.
