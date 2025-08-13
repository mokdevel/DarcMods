# DarcSpawner
Spawns vehicles or other containers randomly on map with some loot. Once everything is spawned, the mod will stop running.

The position for a spawn is searched from locations listed.

## Installation
Just activate the mod (and dependencies) and you're good to go.

## Configuration
Configuration is done in .json files.

# Configuration files
The configuration files will be under your ```profile\DarcMods_conf\*```. Depending on the usage the profile is in your ArmaReforger, ArmaReforgerWorkbench or in your server profile directory. For example ```C:\Users\username\Documents\My Games\ArmaReforger\profile\DarcMods_conf```.

For examples of configuration files see [ExampleConfigs](https://github.com/mokdevel/DarcMods/tree/main/DarcSpawner/ExampleConfigs).

## How does the spawn happen?
A random index is chosen from ``spawnSetList`` and the indexed ``spawnSet`` is selected. Depending on the parameters, random spawning will happen. A random item from ``containers`` is selected. A location is chosen from the map of type ``locationTypes``. The exact position is A) randomized with ``spawnRndRadius`` or B) if ``spawnOnRoad`` is set, a position on a road is searched (only for vehicles). ``spawnCount`` is the maximum amount of containers to spawn. Loot items are then added to the container.

## Spawner config
Example: [dc_spawnerConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcSpawner/ExampleConfigs/dc_spawnerConfig.json)

```
int version : See Common parameters
string author : See Common parameters
bool spawnOnRoad : Spawn the cars on road. If no road network manager found, cars spawned around the map.
int spawnRndRadius : Random radius where the spawnName spawns. Once a location is found, additional randomization is done to avoid vehicles to be always in the same spot.
int containerCount : Amount of containers (cars, lootboxes, etc..) to spawn.
float spawnWorldSizeMultiplier : If spawnCount = 0, we search for the world size in km and multiple with this. For example: 4km wide map with spawnWorldSizeMultiplier = 2 results in spawnCount = 8 (4*2)
bool disableArsenal : See General parameters
array<int> spawnSetList : The indexes of squatters.
array<SCR_DC_SpawnSet> spawnSets : List of spawners
```

### SCR_DC_SpawnSet
```
string comment : See General parameters
bool showMarker : See Common parameters
string markerType : Marker type of SCR_EMapMarkerType. DARC_MISSION by default. You could use for example PLACED_MILITARY or any of the other defined ones.
int markerIdx : The index of the icon in the map marker configuration (check in workbench). 
array<EMapDescriptorType> locationTypes : See Location parameters
array<string> containers : What container to spawn; cars, box, .. All of these will be spawned with spawnChance chance
SDRC_Loot loot : Loot found in the mission. 
```

## Related documentation:
Some parameters are described in DarcMissions documentation
- [Common parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_COMMON.md#common-parameters)
- [General parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_COMMON.md#general-parameters)
- [Location parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LOCATIONS.md)
- [Loot](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LOOT.md)
- [LootLists](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LISTS.md#loot-lists)
