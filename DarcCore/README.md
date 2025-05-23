# DarcCore
Core functions for Darc Mods

# Configuration parameters
For examples of configuration files see [ExampleConfigs](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions/ExampleConfigs).

The configuration files will be under your ```profile\DarcMods_conf\*```.

## Core config
Example: [dc_coreConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_coreConfig.json)
```
int version : Version number of the file.
string author : Author of the missions.
DC_LogLevel logLevel : Loglevel of debugging information created in to logs.
  0 = NONE   : Nothing is printed
  1 = ERROR  : Only errors and warnings (FATAL, ERROR, WARNING)
  2 = NORMAL : Normal information in to log + all above
  3 = DEBUG  : Debug details + all above
  4 = ALL    : Prints everything including VERBOSE and SPAM. It's a lot.
string subDir : The sub directory where to store configs (subDir=myconf -> $profile:/DarcMods/myconf/ )
bool debugShowWaypoints : Only in WB. Show waypoints for AI.
bool debugShowMarks : Only in WB. Show various markers in the world like mission location and non Valid Areas.
string defaultEnemyFaction : (USSR) The default faction to assign to AI in case the proper faction is not available. For example: If FIA is not assigned but a FIA AI is spawned, it's set as USSR
array<string>buildingExcludeFilter : Exclude these prefab names from the cached building list. The list of buildings are cached to avoid a rescan on the map.
```
