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
bool debugShowWaypoints : Only in WB. Show waypoints for AI.
bool debugShowMarks : Only in WB. Show various markers in the world like mission location and non Valid Areas. 
```
