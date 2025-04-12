# DarcSpawner
Spawns vehicles (could work with containers too) randomly on map with some loot. Once everything is spawned, the mod will stop running.

## Installation
Just active the mod (and dependencies) and you're good to go.

## Configuration
Configuration is done in .json files.

# Configuration files
The configuration files will be under your ```profile\DarcMods_conf\*```. Depending on the usage the profile is in your ArmaReforger, ArmaReforgerWorkbench or in your server profile directory. For example ```C:\Users\username\Documents\My Games\ArmaReforger\profile\DarcMods_conf```.

For examples of configuration files see [ExampleConfigs](https://github.com/mokdevel/DarcMods/tree/main/DarcSpawner/ExampleConfigs).

## Spawner config
Example: [dc_spawnerConfig_Convoy.json](https://github.com/mokdevel/DarcMods/blob/main/DarcSpawner/ExampleConfigs/dc_spawnerConfig.json)

```
int version : See Common parameters
string author : See Common parameters
bool showMarker : See Common parameters
int spawnSetID : The spawner to use. -1 = random pick of spawner from spawnSets
bool spawnOnRoad : Spawn the cars on road. If no road network manager found, cars spawned around the map.
int spawnRndRadius : Random radius where the spawnName spawns. 
float spawnWorldSizeMultiplier : If spawnCount = 0, we search for the world size in km and multiple with this. For example: 4km wide map with spawnWorldSizeMultiplier = 2 results in spawnCount = 8 (4*2)
array<SCR_DC_SpawnSet> spawnSets : List of possible spawners
```

### SCR_DC_SpawnSet
```
string comment : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<string> spawnNames : What resource to spawn; cars, box, .. All of these will be spawned with spawnChance chance
float spawnChance : The change to spawn an item from spawnNames. 0.5 = 50% chance
int spawnCount : The maximum amount of spawnNames to spawn. For example: 10 with 0.5 chance would spawn 5 items on average. 0 = count a value depending on mapsize.
array<string> itemNames : Items (prefabs) to add to each spawnNames
float itemChance : The chance to spawn and item from itemNames. 0.5 = 50% chance
```

## Related documentation:
Some parameters are described in DarcMissions documentation
- [Common parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#common-parameters)
- [General parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#general-parameters)
- [Location parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#location-parameters)
- [LootLists](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#loot-lists)
