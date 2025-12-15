# DarcCore
Core and common functions for DarcMods. 

# Configuration parameters
For examples of configuration files see [ExampleConfigs](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions/ExampleConfigs).

## Core config
Example: [dc_coreConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_coreConfig.json)
```
int version : Version number of the file.
string author : Author of the file.
DC_LogLevel logLevel : Loglevel of debugging information created in to logs.
  0 = NONE   : Nothing is printed
  1 = ERROR  : Only errors and warnings (FATAL, ERROR, WARNING)
  2 = NORMAL : Normal information in to log + all above
  3 = DEBUG  : Debug details + all above
  4 = ALL    : Prints everything including VERBOSE and SPAM. It's a lot.
string subDir : The sub directory where to store configs (subDir=myconf -> $profile:/DarcMods/myconf/ )
bool debugShowWaypoints : Only in WB. Show waypoints for AI.
bool debugShowMarks : Only in WB. Show various markers in the world like mission location and non Valid Areas.
bool debugShowSpheres : Only in WB. Show/hide spheres.
bool debugShowLines : Only in WB. Show/hide lines.
bool debugShowInfo : Only in WB. Show/hide info.
string fallbackEnemyFaction : (USSR) The default faction to assign to AI in case the proper faction is not available. For example: If FIA is not assigned but a FIA AI is spawned, it's set as USSR
bool showOnGMMapNonValidArea : When enabled, nonValidAreas are shown on the GM map.
bool showOnGMMapMissionMarker : When enabled, mission markers are shown on the GM map.
SDRC_EHintPosition hintPosition : Position of the hint/popup. 
  0 = UP_LEFT : Upper left corner 
  1 = DOWN_LEFT : Lower left corner 
array<string>buildingExcludeFilter : Exclude these prefab names from the cached building list. The list of buildings are cached to avoid a rescan on the map.
SDRC_EmptyPos emptyPos
  int limit : See section: Empty position finding
  array<string> ignoreFilter : See section: Empty position finding
  array<string> stopFilter : See section: Empty position finding
  array<string> classFilter : See section: Empty position finding
  array<string> objectFilter : See section: Empty position finding
array<SDRC_LocationAka> locationAkas : Add locations with a certain type to the list. 
array<SDRC_LocationAka> buildingAkas : Add buildings with a certain type to the list.
```

## SDRC_LocationAka
[EMapDescriptorType](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/group__Map.html#ga18c4f596069370b50b7f842cf36d5686) .

```
EMapDescriptorType type : The type to use for names.
array<string> names : Names that are to be added to the location list with selected type.
```

# Empty position finding
When searching for an empty position for an spawnable item (mission camp, building, vehicle, ..), we need to do some extra checks in addition to the BI provided functionality. The ``FindEmptyTerrainPosition`` returns areas that are e.g. in the woods or has big objects on it (buildings, rocks, ..). The mod does additional filtering. Once a possible position is found, the mod queries for items within the range. If there are more than ``limit`` items that are considered blockers, the area is not empty and this not suitable for spawning. 

## SDRC_EmptyPos
Under ``emptyPos`` you will find these parameters:
* ``limit`` : The amount of blocking objects that define the area not to be empty.  
* ``ignoreFilter`` : Objects that are not considered as blockers. For example particles.
* ``stopFilter`` : Objects that immediately will define that the position is not good. For example buildings or large rocks.
* ``classFilter`` : The classes of objects that might be considered as blockers. More filtering done in the next step.
* ``objectFilter`` : If an object matching the listed classes is found, we check that the object to match this list. If ``limit`` or more is found within the area, the area is not suitable for spawning.
