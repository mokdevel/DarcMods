# DarcMissions
A mission frame work for Arma Reforger. There are various missions premade and their behaviour can be modified by editing the configuration jsons.

TBD: 
- Change all missions to use pos and posName instead of posStart and locationName.
- MOVE ends up in LOITER
- Add rndCount for waypoints as a min,max parameter in core 
- Uncomment: float rndRange = 0;//Math.RandomInt(0, range/3); in WPHelper
- waypointGenType RANDOM to have MOVE and PATROL also.
- crashsite : The mission pos search 300, 500 to be set as json parameters
- Fix links to different chapters

## Installation
Just active the mod (and dependencies) and you're good to go.

## Vision
I wanted to create a simple drop-in mission package where there is minimal configuration needed to get it running. You can take the mod, play it in SP, MP or let it run on a dedicated server. Everything should be randomized so you should never know what is happening in the world. The behaviour of the mod should be close to vanilla and by doing that, you could be running this on top of your other game modes. I've been using this with [Escapists](https://reforger.armaplatform.com/workshop/5F16D7E4A1CBE075-Escapists) just to get a few additional surprises.

## Missions
- Hunter : AIs are hunting you and following you. They have some knowledge of where you are. You can outrun them or take them down. 
- Convoy : AIs are driving from a location A to B carrying valuable loot with them.
- Patrol : AIs are traveling either with a purpose to reach a location or to patrol areas.
- Crashsite : A helicopter carrying loot is damaged and is going to crash. Survivors will be protecting the wreck while waiting to be rescued.
- Occupation : A location is guarded by AIs with loot available. 

## Thanks
- The work is inspired by [Defent's Mission System (DMS)](https://github.com/Defent/DMS_Exile) in Arma 3.
- [HunterKiller mod by Rabid Squirrel](https://reforger.armaplatform.com/workshop/597324ECFC025225-HunterKiller) for the initial kick to get into coding.

# Basics
Once the mod starts to run it will wait for ```missionStartDelay``` before the missions start to spawn. The missions have their own life cycle with a defined cycle time. Once a mission is completed and no players are near by, the missions will despawn. The missions are active a minimum of ```missionActiveTime```. The time is reset if there is a player within ```missionActiveDistance```.

## MissionFrame cycle
```
----|------------|------------|------------|------------|------------|------------|----...
    |-------------------------|-------------------------|-------------------------|----...
    |------------------------------| missionActiveTime
                              |------------------------------| missionActiveTime
                                                        |------------------------------...
    |O----o----o----o----o----|O------o------o-----o----|O---o---o---o---o---|O----o---...
     ^                         ^                         ^
     mission is spawned        mission is spawned        mission is spawned 
     |----| missionCycleTime : Each mission has a cycle time it's running. It may differ between mission types
    |------------| missionFrameCycleTime : The cycle time to manage mission spawning, deletion etc...
    |-------------------------| missionDelayBetweeen : Time between mission spawns 
|---| missionStartDelay : Time waited before first mission is spawned
    |------------------------------| missionActiveTime : Mission is kept alive this amount of time.
                                     Note that if a player is within missionActiveDistance, the time will reset.      
```

## Mission position
Mission position is either defined or randomized. See [General parameters - pos](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions#general-parameters) and [Location parameters](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions#location-parametes). Once a position is chosen, it's checked for validity. The position shall not be .. 
- .. too close to another mission
- .. too close to any player
- .. in water
- .. in [non valid area](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions#non-valid-areas---dc_nonvalidareajson)

In the case position is not usable, another try is made. Currently searching is limited to five tries before deciding that no position has been found. In this case, mission will not spawn.

## Dynamic vs Static mission
In the configuration you can define missions either as a dynamic mission (missionTypeArrayDynamic) or static mission (missionTypeArrayStatic). The missions are the same but static missions have priority in being spawned. Static missions are such that there should always be the given count of static missions running and if there are mission slots still available, these will be populated with dynamic missions. Note that both static and dynamic missions are counted towards the total count.

This enables you to have a count of certain types of missions always running. For example, you may want to have three patrols always roaming the map. To achieve this, you define the patrol mission three times in the missionTypeArrayStatic array. If you define multiple types of missions in the static list, the missions are picked at random. Having for example three patrol missions and one convoy mission in the list, makes sure that four static missions are running, but this may be any combination due to random picking. The patrol mission has a higher chance to be chosen.

## Configuration parameters
Configuration is done in .json files.

# Configuration files
For examples of configuration files see [ExampleConfigs](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions/ExampleConfigs).

The configuration files will be under your ```profile\DarcMods_conf\*```. Depending on the usage the profile is in your ArmaReforger, ArmaReforgerWorkbench or in your server profile directory. For example ```TBD: Fix the names above and add link to find these in your users dir```.

Mission frame specific files
* dc_missionConfig.json ([link](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#dc_missionconfigjson---missionframe)) :  The main mission frame configuration. 
* dc_nonValidArea.json ([link](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#non-valid-areas---dc_nonvalidareajson)) : Definition of areas where a mission can not spawn.
* dc_lootList.json ([link](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions#loot-lists---dc_lootlistjson)) : Definitions of loot for missions. 
* dc_coreConfig.json ([link](https://github.com/mokdevel/DarcMods/blob/main/DarcCore/README.md#dc_coreconfigjson)) : Configuration file for core.
  
Mission specific files
* dc_missionConfig_Hunter.json ([link](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#hunter---dc_missionconfig_hunterjson))
* dc_missionConfig_Patrol.json ([link](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions#patrol---dc_missionconfig_patroljson))
* dc_missionConfig_Occupation.json
* dc_missionConfig_Convoy.json ([link](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#convoy---dc_missionconfig_convoyjson))
* dc_missionConfig_Crashsite.json ([link](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#mission--crashsite)

## Mission frame
Example: [dc_missionConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig.json)

The main configuration file for mission frame.
```
int version : See General parameters
string author : See General parameters
bool recreateConfigs : If set to true, all configs are written to disk. Should be run only first time.
string missionProfile : Directory specifying a certain conf for play. For example "Escapists" will result in configs under ```profile\DarcMods_conf\Escapists\*```
int missionStartDelay : (seconds) Time to wait before spawning the first mission.
int missionDelayBetweeen : (seconds) Time delay between mission spawns.
int missionCount : Maximum amount of missions (both static and dynamic) to be active at the same time. 
int missionFrameCycleTime : (seconds) The cycle time to manage mission spawning, deletion etc...
int missionActiveTime : (seconds) Time to keep the mission active.
int missionActiveTimeStatic : (seconds) Time to keep the static mission active (seconds). This typically is much longer than for dynamic.
int missionActiveDistance : The distance to a player to keep the mission active.
int missionHintTime : (seconds) Time to show mission hints to players. 0 disables ALL hints.
int minDistanceToMission : Distance to another mission. Two missions shall not be too close to each other.
int minDistanceToPlayer : Mission shall not spawn too close to a player.
array<int> missionTypeArrayDynamic : List mission types that spawn randomly. (DC_EMissionType)
  0 = NONE       : Not used anywhere. The rest are names of the mission types.
  1 = HUNTER
  2 = OCCUPATION
  3 = CONVOY
  4 = CRASHSITE
  5 = PATROL
array<int> missionTypeArrayStatic : List mission types that are always active. See missionTypeArrayDynamic for values.
```

## Non valid areas
Example: [dc_nonValidArea.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_nonValidArea.json)

This a list of areas where missions shall not spawn. For example a safe zone would be listed here. 
```
int version : See General parameters
string author : See General parameters
array<SCR_DC_NonValidArea> nonValidAreas : List of areas where missions shall not spawn.
```

### SCR_DC_NonValidArea
```
string worldName : The world name this should be affecting. For example Arland or Eden. If left empty, area will be valid for all maps. Note that Arland and GM_Arland are considered different maps.
vector pos : Center position of area
float radius : Radius from the center where the missions shall not spawn.
string name : Your own name for the area. Not used by the mod.
```

## Loot lists
Example: [dc_lootList.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_LootList.json)

The mod is capable of creating automatic loot lists to be used with missions. You can also define your own sets by following the same notation as in the example. Do not reuse the same names as is already available as a lootListName.

This supports additional mods that you can define in the modList parameter. The typical notation is ```"$ModName:PreFabs"```. A properly created mod follows the same structure and keeps prefabs in the right places. 
```
int version : See General parameters
string author : See General parameters
array<string> modList : The mods to search for loot.
array<SCR_DC_LootList> lootLists : The defined lootlists
```
Currently available lootListNames are below. For a complete list, please check [SCR_DC_LootListJsonApi.c](https://github.com/mokdevel/DarcMods/blob/main/DarcCore/scripts/Game/Helpers/SCR_DC_LootListJsonApi.c)
```
WEAPON_RIFLE : A random rifle from any faction.
WEAPON_HANDGUN : A random handgun from any faction.
WEAPON_LAUNCHER : A random launcher from any faction.
WEAPON_GRENADE : Grenades available
WEAPON_ATTACHMENT : Various attachments excluding optics
WEAPON_OPTICS : Optic attachments
ITEM_MEDICAL : Medical items
ITEM_GENERAL : General items
```

### SCR_DC_LootList
The functionality goes through the modDir and gets every file with the ending of ```.et```. The list is first filtered with ```include``` words and then filtered with ```exclude```words. Note that any item you may have defined manually goes through the same filtering and may be removed if the ```exclude``` word matches.
```
string lootListName : The name given for the lootList. This is the name you use for adding loot. 
string modDir : Prefab directory inside mod.
array<string> include : Items having these words are included in the lootList.
array<string> exclude : Items with these words will be removed from the lootList.
array<ResourceName> itemList : The list of items. This is autofilled, but you can pre-define items if needed.
```

### Example
The below example with the name WEAPON_RIFLE will search three mods for items matching the include and exclude filters. The intial search path is ```$Modname:Prefabs```. Under the Prefabs dir we use the dir ```/Weapons/Rifles``` for the more detailed search. The full path for the search is ```$Modname:Prefabs/Weapons/Rifles``` and internally we're searching for all files ending in ```et```. Initally all items will be listed. 

TBD: Explanation on how the filtering works.

```
"version": 1,
"author": "darc",
"modList": [
  "$ArmaReforger:Prefabs",
  "$WCS_Armaments:Prefabs",
  "$M110MarksmanRifle:Prefabs"
  ],
"lootLists": [
  {
    "lootListName": "WEAPON_RIFLE",
    "lootListType": 0,
    "modDir": "/Weapons/Rifles",
    "include": [
      "Rifle"
    ],
    "exclude": [
      "_Base",
      "***TBD: ADD SOME PREFAB***"
    ],
    "itemList": [
      "***TBD: ADD SOME PREFAB***"
    ]
  }
  ]
}
```

# Mission parameters
## Mission : Hunter
Example: [dc_missionConfig_Hunter.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Hunter.json)

AIs are hunting you and following you. They will receive regularly information about where the closest player. The location is known to them with an error of ```rndDistanceToPlayer```. You can outrun them by going further than ```maxDistanceToPlayer```.


```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
string title : See General parameters
string info : See General parameters
int minDistanceToPlayer : Hunter group minimum distance to player for spawn
int maxDistanceToPlayer : ..max distance
int rndDistanceToPlayer : The error on the location where AI thinks you are. (0..rndDistanceToPlayer). 
array<int> groupCount : See General parameters
array<string> groupTypes : See General parameters
int AISkill : See General parameters
float AIperception : See General parameters
```

## Mission : Patrol
Example: [dc_missionConfig_Patrol.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Patrol.json)

AI patrol is spawned to move around the map. The may have a destination where they're moving to or just randomly roam the map.

```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
int patrolingTime : (seconds) Time to patrol. Once this time has passed and not players nearby, despawn mission.
int distanceToPlayer : If no players this close to any players and patrolingTime has passed, despawn mission.
array<int> patrolList : The indexes of patrols.
array<SCR_DC_Patrol> patrols : List of patrols
```

### SCR_DC_Patrol
```
string comment : See General parameters
vector posStart : See General parameters - pos. 
vector posDestination : Destination for the patrol to go to. See also General parameters - pos.
string locationName : See General parameters - posName
string title : See General parameters
string info : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<int> groupCount : See General parameters
array<int> waypointRange : See Waypoint parameters. For patrols, the values should be quite big to have them move around the map.
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
  ROUTE : The patrol will go from posStart to posDestination. When destination has been reached, the patrol will LOITER.
  RADIUS, SCATTERED : The patrol will follow a path created with waypointRange starting from posStart. posDestination is ignored.
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
array<string> groupTypes : See General parameters
int AISkill : See General parameters
float AIperception : See General parameters
```

## Mission : Convoy
Example: [dc_missionConfig_Convoy.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Convoy.json)

AI patrol is spawned in a vehicle to move around the map. 
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
int convoyTime : (seconds) Time to drive around. Once this time has passed and not players nearby, despawn mission.
int distanceToPlayer : If no players this close to the convoy and convoyTime has passed, despawn mission.
array<int> convoyList : The indexes of convoys.
array<SCR_DC_Convoy> convoys : List of convoys
```

### SCR_DC_Convoy
```
string comment : See General parameters
vector posStart : See General parameters - pos. 
vector posDestination : Destination for the patrol to go to. See also General parameters - pos.
string locationName : See General parameters - posName
string title : See General parameters
string info : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
  ROUTE : The convoy will go from posStart to posDestination. When destination has been reached, the patrol will LOITER.
  RADIUS, SCATTERED : The concoy will follow a path created with waypointRange starting from posStart. posDestination is ignored.
array<string> vehicleTypes : The prefab names of vehicles. The vehicle is randomly picked from this list.
float cruiseSpeed : Speed to drive in km/h. 30 is a good value so that the convoy is not driving too fast.
array<string> groupTypes : See General parameters
int AISkill : See General parameters
float AIperception : See General parameters
SCR_DC_Loot loot : (optional) Loot found in the vehicle. 
```

## Mission : Crashsite
Example: [dc_missionConfig_Crashsite.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Crashsite.json)

A flying helicopter is spawned in a random location flying towards a random location. The helicopter either crashes or if the speed is reduced enough, it will be destroyed via script. Once the helicopter has crashed, AI will be spawned to protect the crashsite. Loot can be added in to the loot box carried. Additional structures can be spawned around the crashed helicopter. 

The location for the initial chopper spawn is random and only checks for mission distance and player distance. The destination is toward map center with randomization. This is to avoid the direction to be towards the map edges. There will be cases where the helicopter flies towards the sea and this is just unfortunate randomization (read: this is by design).
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
array<int> flyHeight : (min, max) The helicopter is spawned between these height values. The higher the values, the longer flight. Setting to zero will create the crashsite immediately.
array<int> crashsiteList : The indexes of crashsites.
array<SCR_DC_Crashsite> crashsites : List of crashsites
```

### SCR_DC_Crashsite
```
string comment : See General parameters
string title : See General parameters
string info : See General parameters
array<string> groupTypes : See General parameters
int AISkill : See General parameters
float AIperception : See General parameters
array<SCR_DC_HelicopterInfo> helicopterInfo : The helicopter is selected randomly.
//Optional settings
array<SCR_DC_Structure> siteItems : Prefabs to spawn at the crashsite. Note that first item shall be the loot box.
SCR_DC_Loot loot : (optional) Loot found in the vehicle. 
```

### SCR_DC_HelicopterInfo
This defines the helicopter parameters. The default prefabs in Arma Reforger are without their engines running and will simply crash before the flight is possible. For the flying helicopters the prefabs needs to be modified and currently only a couple of options are available.

Currently supported prefabs:
- ```"{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et"```
- ```"{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et"```

```
string resource : The prefab for the helicopter. See supported values above.
float throttle : The 'speed' the helicopter is flying.
float rotorForce : Force of the main rotor.
float rotor2Force : Force of the second rotor.
```

### Define a new flying helicopter prefab
- Duplicate the helicopter prefab
- Set <TBD: motor running>
- ...etc

## Mission : Occupation
Example: [dc_missionConfig_Occupation.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Patrol.json)

AI patrol is spawned to a location like city. They will patrol the location and may have loot with them.

```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
int emptySize : The size (radius) of the empty space needed to decide on a mission position.
array<int> occupationList : The indexes of occupations.
array<SCR_DC_Occupation> occupations : List of oocupations.
```

### SCR_DC_Occupation
```
string comment : See General parameters
vector locationPos : See General parameters - pos. 
string locationName : See General parameters - posName
string title : See General parameters
string info : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<int> groupCount : See General parameters
array<int> waypointRange : See Waypoint parameters. For patrols, the values should be quite big to have them move around the map.
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
array<string> groupTypes : See General parameters
int AISkill : See General parameters
float AIperception : See General parameters
SCR_DC_Loot loot : (optional) Loot found in the mission. 
array<ref SCR_DC_Structure> campItems  : (optional) The structure list for the camp.
```

# Common mission parameters
## Common parameters
These parameters are in every mission.
```
int version : See General parameters
string author : See General parameters
int missionCycleTime : (seconds) The cycle time the mission states are run
bool showMarker : Show mission marker on map. False will disable markers.
bool showHint : Show players a hint with details about the missions. False will disable hints.
```

## General parameters
```
int version : Version number of the file.
string author : Author of the missions.
string comment : Generic comment to describe the mission. Not used in game.
vector pos : Position for mission. "0 0 0" used for random location chosen from locationTypes.
string posName : Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes. "" will leave the information empty.
string title : Title for the hint shown for players
string info : Details for the hint shown for players
array<int> groupCount : Amount of AI groups of characters to spawn. (min, max) 
array<string> groupTypes : The prefab names of AI groups or characters. The AI is randomly picked from this list.
  Example : {
            "{ADB43E67E3766CE7}Prefabs/Characters/Factions/OPFOR/USSR_Army/Spetsnaz/Character_USSR_SF_Sharpshooter.et",
            "{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"
            }
int AISkill : Skill for AI (0-100). See SCR_AICombatComponent and EAISkill.
  0 = NONE
  10 = NOOB
  20 = ROOKIE
  50 = REGULAR
  70 = VETERAN
  80 = EXPERT
  100 = CYLON
float AIperception : How quickly AI reacts to danger. See SCR_AICombatComponent for details on perception factors.
```

## Location parameters
A mission location position (see: General parameters - pos) can be defined manually at certain coordinates or as "0 0 0". In the latter case, the mission position is chosen randomly from the map from locations defined in locationTypes. The different types are defined as [EMapDescriptorType](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/group__Map.html#ga18c4f596069370b50b7f842cf36d5686). 

Note that you 'overload' certain value by defining it multiple times. For example ```locationTypes = {59, 59, 59, 59, 60};``` will choose a CITY 80% of the time and only 20% time a village.
```
array<int> locationTypes : Location types as defined by EMapDescriptorType.
```

### EMapDescriptorType
Below are the typical values selected from [EMapDescriptorType](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/group__Map.html#ga18c4f596069370b50b7f842cf36d5686) .
```
58 = MDT_NAME_GENERIC 
59 = MDT_NAME_CITY 
60 = MDT_NAME_VILLAGE 
61 = MDT_NAME_TOWN 
62 = MDT_NAME_SETTLEMENT 
63 = MDT_NAME_HILL 
64 = MDT_NAME_LOCAL 
65 = MDT_NAME_ISLAND 
..
70 = MDT_NAME_RIDGE 
71 = MDT_NAME_VALLEY 
```
My humble wish for map makers is that please use the full range of EMapDescriptorTypes. 

## Waypoint parameters
The mission AI can be given general rules on how to create waypoints for their movement. You need to define the waypoint generation rule and the movement rule and the rest is created automatically.
```
array<int> waypointRange : (min, max) 
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
  NONE        : Not used anywhere. The rest are names of the mission types.
  RANDOM      : Use one of these randomly: SCATTERED (1), RADIUS (3), LOITER (1)
  SCATTERED   : Completely random waypoints without any logic. 
  RADIUS      : AI follow a path that is close to a circle with a radius. There is some additional randomization to avoid a perfect circle.
  ROUTE       : AI follow a route from A to B. The route is created with waypoints along the road. Once the destination is reached, the AI will LOITER
  LOITER      : Loitering is where AI hand around in the location. AR does not have this yet, so a DEFEND waypoint is created. 
  SLOTS       : AI goes from a slot to slot. NOTE: This will not work unless the map has slots (the S/M/L letters on map) defined.
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
  NONE        : Not used anywhere. The rest are names of the mission types.
  RANDOM      : Pick a random one from MOVECYCLE (1) and PATROLCYCLE (3).
  MOVE        : Creates move waypoints. AI will LOITER once reaching the last waypoint.
  PATROL      : Same as MOVE but with patrol speed.
  MOVECYCLE   : Creates move waypoints in cycke. AI will restart the cycle once all waypoints are visited.
  PATROLCYCLE : Same as MOVECYCLE but with patrol speed.
  LOITER      : If LOITER is chosen for generation, LOITER will be enforced for the movetype too.
```

## Loot parameters
Loot is the reward of a mission. It is usually in a container (vehicle, box, ..) that is spawned in the mission. 

NOTE: The box entity needs to have <***COMPONENT***> available. See [loot box](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#loot-box). 

### SCR_DC_Loot
```
IEntity box : (set as null) The container where the items are put. This can be a box, vehicle or similar. Do not specify anything here as this will be autofilled. For concoy mission this will be the vehicle, for occupation this will be the first item listed in the structure. 
float itemChance : (0-1, equals to percentage) The chance of each item appearing in the box. 
array<string> items : List of items. This can be prefabs or an lootList.
```

### Example
The below example shows the different ways you can define the loot. 
- "box": null : Leave as null. For convoys this will be automatically assigned to the vehicle. For missions with structures, the first structue is considered as the loot target.
- There is a 75% chance for each of the items to appear in the box
- Items will include a saline bag and a map. In addition a random rifle and a random general item is added. See [lootList](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions#loot-lists---dc_lootlistjson)
```
"loot": {
  "box": null,
  "itemChance": 0.75,
  "items": [
    "{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
    "{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
    "WEAPON_RIFLE",
    "ITEM_GENERAL"
  ]
}
```

### Loot box 
The loot box (vehicle, box, ..) needs to have <TBD: component> available. If it's not available, no loot can be added. 
- Vehicles : Typically works.
- Box : The default arsenal boxes are not supported and you will need to use special ones. Currently supported values: 
```
"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et"
"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et"
```

### Define a new loot box
- Duplicate the box prefab
- Set <TBD: component>
- Remove <TBD: component>
- ...etc

## Structure parameters
Structures are entities (prefabs) of e.g., furniture. Typically you would spawn multiple structures to create for example a camp with chairs, a tent, fireplace etc. 

### SCR_DC_Structure
```
string m_Resource : Prefab name
vector m_Position : Position in the world.
vector m_Rotation : Rotation vector. Currently only Yrot is supported ["Xrot YRot ZRot"].
```

### Creating a camp
You can use WB to set the wanted objects. You can turn them to make the camp to look better. I suggest you to create an own layer which you can then view as a file and convert to properly looking json file. Locate the camp on flat surface for example on an airfield if using a world like Arland (or use some simple flat world). When the camp is spawned, the items will be set around a center location (origo) in the spawner script. The origo will be the position on the map and items will spawn around it.

If you have a loot defined and it needs to appear in a container like a box, ALWAYS define it as the first structure.

TBD: Image of a camp
TBD: Image of camp from the above with origo marked.

### Example
Below is a an example camp with a box for loot, tent, fireplace and chair. The chair is rotated. The first item ```LootCrateWooden_01_blue``` is the container for loot. 
```
"campItems": [
  {
    "m_Resource": "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
    "m_Position": [
      1032.0379638671876,
      39.0,
      2478.923095703125
    ],
    "m_Rotation": [
      0.0,
      0.0,
      0.0
    ]
  },
  {
    "m_Resource": "{39C308BBB5945B85}Prefabs/Props/Military/Furniture/ChairMilitary_US_02.et",
    "m_Position": [
      1028.550048828125,
      39.0,
      2478.159912109375
    ],
    "m_Rotation": [
      0.0,
      119.33399963378906,
      0.0
    ]
  },
  {
    "m_Resource": "{D9842C11742C00CF}Prefabs/Props/Civilian/Fireplace_01.et",
    "m_Position": [
      1029.9000244140626,
      39.0,
      2477.43994140625
    ],
    "m_Rotation": [
      0.0,
      0.0,
      0.0
    ]
  },
  {
    "m_Resource": "{0511E95F422061BB}Prefabs/Props/Recreation/Camp/TentSmall_02/TentSmall_02_blue.et",
    "m_Position": [
      1029.9739990234376,
      39.0,
      2480.114013671875
    ],
    "m_Rotation": [
      0.0,
      0.0,
      0.0
    ]
  }
]
```

# TBD
- Waypoint functionality to roam buildings in a city
- Give AIs the capability to guard buildings (for example in occupation missions)
- New missions: Kill a target, steal smth and bring the loot somewhere, choppers hunting you
