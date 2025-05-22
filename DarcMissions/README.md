# DarcMissions
A mission frame work for Arma Reforger. There are various missions premade and their behaviour can be modified by editing the configuration jsons.

## Installation
Just activate the mod (and dependencies) and you're good to go.

## Vision
I wanted to create a simple drop-in mission package where there is minimal configuration needed to get it running. You can take the mod, play it in SP, MP or let it run on a dedicated server. Everything should be randomized so you should never know what is happening in the world. The behaviour of the mod should be close to vanilla and by doing that, you could be running this on top of your other game modes. I've been using this with [Escapists](https://reforger.armaplatform.com/workshop/5F16D7E4A1CBE075-Escapists) just to get a few additional surprises.

## Missions
- Hunter : AIs are hunting you. They have some knowledge of where you are. You can outrun them or take them down. 
- Convoy : AIs are driving from a location A to B carrying valuable loot with them.
- Patrol : AIs are patroling an area or traveling with a purpose to reach a location.
- Crashsite : A helicopter carrying loot is damaged and is crashing. Survivors will be protecting the wreck while waiting to be rescued.
- Occupation : A location is guarded by AIs with loot available.
- Squatters : A building is guarded by AIs with loot available.

## Thanks
- The work is inspired by [Defent's Mission System (DMS)](https://github.com/Defent/DMS_Exile) in Arma 3.
- [HunterKiller mod by Rabid Squirrel](https://reforger.armaplatform.com/workshop/597324ECFC025225-HunterKiller) for the initial kick to get into coding.
- The great Arma Reforger community and #enfusion_scripting

## Future plans
- Waypoint functionality for AI to roam buildings in a city
- New missions: Kill a target, steal smth and bring the loot somewhere, choppers hunting you

## Known issues
- Vehicles spawned for missions (convoy, ...) will despawn once mission is cleared. (TBD: Plans to change this behavior).
- No ammo spawns in loot. (TBD: weapon compatible ammo functionality)

# Parameters
All configurations are done via .json files. 

The mission frame has the main definitions on mission spawn times, counts, etc. See:
* [Mission Frame](./docs/MISSIONFRAME.md)

Missions are configurable via .json files. Each mission have their own configuration file, but some information is shared. There are:
* [Common parameters](./docs/P_COMMON.md) that are valid for all missions. 
* [General parameters](./docs/P_COMMON.md#general-parameters) that are used in multiple mods.

Mission specific parameters for each mission type: 
* [Hunter](./docs/MISSION_HUNTER.md)
* [Convoy](./docs/MISSION_CONVOY.md)
* [Patrol](./docs/MISSION_PATROL.md)
* [Crashsite](./docs/MISSION_CRASHSITE.md)
* [Occupation](./docs/MISSION_OCCUPATION.md)
* [Squatters](./docs/MISSION_SQUATTERS.md)

Other parameter details available:
* [Lists](./docs/P_LISTS.md) for automatic loot and enemy lists
* [Locations](./docs/P_LOCATIONS.md)
* [Loot](./docs/P_LOOT.md)
* [Non Valid Areas](./docs/NONVALIDAREAS.md) aka safe zones
* [Structures](./docs/P_STRUCTURE.md)
* [Waypoints](./docs/P_WAYPOINTS.md)

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

Dependency specific files
* [dc_coreConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_coreConfig.json) : Configuration file for core.

# Version history

## Next release (unreleased)
* Two new loot lists ```WEAPON_SHOTGUN``` , ```WEAPON_MG```. Supports BigChungus guns.
* Cars become persistent once you are close to them for a moment.
* No more duplicate conf files. Dir defined in core with ```subDir``` parameter.
* Mods wait for core to start first.
* Kunar map issue fixed (POS_IN_WATER).
* Core provides some additional game information.
* dc_coreConfig.json: Added ```subDir``` parameter
* dc_coreConfig.json: Added ```fallbackEnemyFaction``` . In case a faction is not available, set AI to this one.
* dc_coreConfig.json: Added ```enemyFactions``` . You can define the factions considered as enemies. Enemy AI will spawn from these factions unless a mission has defined a specific factions. The default missions use the new enemy list functionality
* dc_missionConfig.json : ```missionProfile``` removed
* Enemylist functionality : Instead of defining specific enemy prefab, you can use the available keywords to define the type of AI to spawn. The enemy is selected to represent the faction found in ```enemyFactions```. Available keywords:
  ```
	//Groups
	G_LIGHT : Small arms units like rifle men.
	G_HEAVY : Units with bigger guns for example machine guns
	G_SNIPER : Units with rifles and typically with scopes
	G_LAUNCHER : Units with launchers
	G_ADMIN : Officers and similar higher ranking units
	G_MEDICAL : Medical units
	G_RECON : Units defined as recon units
	G_SPECIAL : Special Forces units.
	G_SMALL	: Small groups with two units. Mixed arms.

	//Characters
	C_RIFLEMAN
	C_HEAVY
	C_RECON
	C_OFFICER
	C_CREW
	C_SNIPER
	C_LAUNCHER
	C_MEDIC
	C_SPECIAL
  ```
  
## 20250522 - Nothing here
