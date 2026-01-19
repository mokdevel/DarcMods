# DarcSpawner
Spawns vehicles or other containers randomly on map with some loot. Once everything is spawned, the mod will stop running.

The position for a spawn is searched from locations listed.

## Installation
Just activate the mod (and dependencies) and you're good to go.

## Configuration
Configuration is done in .json files.

# Configuration files
The configuration files will be under your ```profile\DarcMods\*```. Depending on the usage the profile is in your ArmaReforger, ArmaReforgerWorkbench or in your server profile directory. For example ```C:\Users\username\Documents\My Games\ArmaReforger\profile\DarcMods```.

For examples of configuration files see [ExampleConfigs](https://github.com/mokdevel/DarcMods/tree/main/DarcSpawner/ExampleConfigs).

## How does the spawn happen?
A random index is chosen from ``spawnSetList`` and the indexed ``spawnSet`` is selected. Depending on the parameters, random spawning will happen. A random item from ``containers`` is selected. A location is chosen from the map of type ``locationTypes``. The exact position is A) randomized with ``spawnRndRadius`` or B) if ``spawnOnRoad`` is set, a position on a road is searched (only for vehicles). ``spawnCount`` is the maximum amount of containers to spawn. Loot items are then added to the container.

## Spawner config
Example: [dc_spawnerConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcSpawner/ExampleConfigs/dc_spawnerConfig.json)

```
int version : Version id of the file
string author : Author of the file
bool spawnOnRoad : Spawn the cars on road. If no road network manager found, cars spawned around the map.
int spawnRndRadius : Random radius where the spawnName spawns. Once a location is found, additional randomization is done to avoid vehicles to be always in the same spot.
int containerCount : Amount of containers (cars, lootboxes, etc..) to spawn.
float spawnWorldSizeMultiplier : If containerCount = 0, we search for the world size in km and multiple with this. For example: 4km wide map with spawnWorldSizeMultiplier = 2 results in spawnCount = 8 (4*2)
bool disableArsenal : Disable arsenal for vehicles so that only defined loot items are found.
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
array<string> containers : What container to spawn; cars, box, .. All of these will be spawned with spawnChance chance
SDRC_Loot loot : Loot found in the mission. See Loot parameters
```

## Related documentation:
Some parameters are described in DarcMissions documentation
- [Location parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LOCATIONS.md)
- [Loot](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LOOT.md)
- [LootLists](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LISTS.md#loot-lists)

# Version history
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