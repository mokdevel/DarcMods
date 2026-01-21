# Parameters
All configurations are done via .json files. The configuration files will be under your ```profile\DarcMods\<subDir>\*```, where the ``subDir`` is defined via ``dc_coreConfig.json``. 

## Version
Each file has a ``version`` parameter. This tells the version of the structure used for the specific file. This is an internal detail and is not to be used for the author's own versioning. When parameter structure has changed with additional (or deleted) items, the version number is increased. The mod knows which version to expect and will give an error in case a wrong version file is loaded. In these cases, you should delete your old file to receive a new version.

The mod is WIP so the files are getting version changes occasionally.

## Files
Core has common configurations for mods. See:
* [Core](./CORE.md)

The mission frame has the main definitions on mission spawn times, counts, etc. See:
* [Mission Frame](./MISSIONFRAME.md)
* [Mission Frame Config](./MISSIONCONFIG.md)

Missions are configurable via .json files. Each mission have their own configuration file, but some information is shared. There are:
* [General parameters](./P_COMMON.md) that are valid for all missions. 
* [Camps](./P_CAMP.md) is a common structure used in multiple missions.

Mission specific parameters for each mission type: 
* (1) [Hunter](./MISSION_HUNTER.md)
* (2) [Occupation](./MISSION_OCCUPATION.md)
* (3) [Convoy](./MISSION_CONVOY.md)
* (4) [Crashsite](./MISSION_CRASHSITE.md)
* (5) [Patrol](./MISSION_PATROL.md)
* (6) [Squatters](./MISSION_SQUATTERS.md)
* (7) [Roadblock](./MISSION_ROADBLOCK.md)
* (8) [HVT VIP](./MISSION_HVTVIP.md)
* (9) [HVT Item](./MISSION_HVTITEM.md)
* (10) [Stash](./MISSION_STASH.md)
* (11) [Chopper](./MISSION_CHOPPER.md)

Other parameter details available:
* [Lists](./P_LISTS.md) for automatic loot and enemy lists
* [Locations](./P_LOCATIONS.md)
* [Loot](./P_LOOT.md)
* [Non Valid Areas](./NONVALIDAREAS.md) aka safe zones
* [Structures](./P_STRUCTURE.md)
* [Buildings](./P_BUILDING.md)
* [QRF - Quick Reaction Force](./P_QRF.md)
* [Helicopters Crashing](./P_HELICOPTER_CRASH.md)
* [Helicopters Flying](./P_HELICOPTER_FLY.md)

# Configuration files
The configuration files will be under your ```profile\DarcMods\<subConfDir>```. The <subConfDir> is a name for the specific sub directory under DarcMods and can be changed in dc_coreConfig.json. The default directory will be called ```default```.
Depending on the usage, the profile is in your ArmaReforger, ArmaReforgerWorkbench or in your server profile directory. For example ```C:\Users\username\Documents\My Games\ArmaReforger\profile\DarcMods\default```.

For examples of configuration files see [ExampleConfigs](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions/ExampleConfigs).

Mission frame specific files
* [dc_missionConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig.json) :  The main mission frame configuration. 
* [dc_nonValidArea.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_nonValidArea_example.json) : Definition of areas where a mission can not spawn.
* [dc_lootList.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_LootList.json) : Definitions of loot for missions. 
* [dc_enemyList.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_enemyList.json) : Definitions of loot for missions. 
  
Mission specific files
* [dc_missionConfig_Hunter.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Hunter.json)
* [dc_missionConfig_Patrol.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Patrol.json)
* [dc_missionConfig_Occupation.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Patrol.json)
* [dc_missionConfig_Convoy.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Convoy.json)
* [dc_missionConfig_Crashsite.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Crashsite.json)
* [dc_missionConfig_Squatters.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Squatter.json)
* [dc_missionConfig_HvtItem.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_HvtItem.json)
* [dc_missionConfig_HvtVip.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_HvtVip.json)
* [dc_missionConfig_Stash.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Stash.json)

Dependency specific files
* [dc_coreConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_coreConfig.json) : Configuration file for core.
* [dc_compatFFConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissionsCompatFF/ExampleConfigs/dc_compatFFConfig.json) : Configuration file for core.