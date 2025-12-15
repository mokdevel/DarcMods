

# Version history

## WIP 

Changes:
* dc_missionConfig.json : Added ``showDynamicMissionMarker`` : Show/hide static mission markers. This will override any mission specific ``showMarker`` settings.
* Chopper mission: Added ``distanceToStart``, the distance from center of world for mission start.

## Next release (unreleased)
No changes in mission jsons. 

## 20251214
No changes in mission jsons. This is mainly a bug fix release.

Main features:
* Proper support for BallienCreatures Cultists faction. Setting ``BALLIEN_BC_FACTION`` as the enemy faction, will turn all mission enemies to cultists.
  * For proper support, delete your ``dc_enemyList.json`` to get an updated one.

Changes:
* Chopper mission: 
  * Mission starts closer to center of the world. Fixes the issue with Anizay.
  * Improved helicopter flight model. Smoother turning.
  * Added new helicopter: UH1H_armed_gunship_HEDP_sharkNose_Patrol 
  * Static missions will use ``activeTime`` defined in dc_missionConfig.json for the flight time.
* Roadblock mission: Previous fix of barbed wire removal - delete the file to receive the updated file.

Fixes: 

## 20251213
No changes in mission jsons. This updated introduces a new mission: Chopper.

Main features:
* Chopper mission: A flying helicopter is spawned on the edge of the map and will fly to the mission location. Look out, it's searching for you.

Changes:
* Roadblock mission: Removed some of the barbed wires as AIs were bleeding due to them.

## 20251128
No changes in mission jsons. This is mainly a bug fix release and preparation for next bigger update.

Main features:
* Proper support for Bacon Zombies. Setting ``BACON_622120A5448725E3_FACTION`` as the enemy faction, will turn all mission enemies to zombies. Storywise may not be perfect as the zombies will setup camps and roadblocks. They will also hunt you with cars (Convoy mission) as they don't know how to disembark a vehicle. 
  * For proper support, delete your ``dc_enemyList.json`` to get an updated one.
  
Fixes:
* Fix for location cache crash (hopefully).
* Fixed a race condition where DarcMissions started to spawn before DarcCore was properly initialized.

## 20251123
No changes in mission jsons. This is mainly a bug fix release and preparation for next bigger update.

Main features:
* Persistence (vanilla) handling. All entities (AI, vehicles, buildings) spawned will have the persistence disabled. This means that none of the items should end up in your save and become re-spawned when server boots.
* Known issues with the current implementation:
  * AI that was active in a fight, seems to be saved even if persistence was disabled. Sometimes the AI just spawns from the save and stands still like a statue.
  * The persistence disabling happens with a delay (a few seconds) to allow the entities to spawn properly. If your server crashes during this delay, they may end up in your save.
  
Changes:
* Added new loot boxes ```LootSupplyPortableContainers_01_large_item.et``` , ```LootEquipmentBoxWooden_Equipment_01_US.et```
* Requested missions (e.g. GM spawned) will ignore other missions and players nearby. You can drop many missions on the same spot if you like.
* Squatters mission: GM spawned mission will pick the nearest building.
* Crashsite mission: Additional randomization to flight pattern. Hopefully less landings on sea.
* Occupation mission: Additional randomization for the position.
* Convoy mission: Loot is spawned only after the mission is completed.
* Stash mission: Additional randomization for the position.
* HvtVip mission: New sub-mission.

Fixes:
* Removed MDT_RAILWAY from location search. Search took a long time on Gulfcoast Island map.
* It was possible to drag items like cars in the loot boxes which resulted in a crash.
* Some of the loot boxes were invisible.

### DarcDeathMarker
Compatibility compile for DarcCore.

### DarcMissionsCompatFF
Compatibility compile for DarcCore.

### DarcSpawner
Compatibility compile for DarcCore.


## 20251104
### DarcMissions
No changes in mission jsons. This is mainly a bug fix release and preparation for next bigger update.

Fixes:
* Added better support for Tactical Flava mod. If you use the mod, remove ``dc_enemyList.json`` to get a new one.
* Extended ``akas`` functionality in ``dc_enemyList.json`` to support more complex filtering. See docs.
* Clicking on the map icon did not show the intel / time left.
* Mission will fail if AI spawn fails.
* Occupation drop from GM had wrong mission type

## 20251102
### DarcMissions
No changes in mission jsons. This is mainly a bug fix release and preparation for next bigger update.

Fixes:
* Added better support for Rayzis Optics. Remove your dc_lootList.json to get a new updated one.
* Marker issue "NULL pointer to instance. Variable 'm_wMarkerTimestamp'" fixed
* Issue "(E): Math.RandomInt" fixes. Happened when min/max were the same.
* AI removal is immediate instead of setting first damage. Hopefully fixes some FPS drops reported.

## 20251030
### Update for Arma Reforger 1.6
Minor fixes and support for 1.6.

## 20251027
### DarcMissions
No changes in mission jsons. This is mainly a bug fix release and preparation for next bigger update.

Fixes:
* UTIL_MAGAZINE typos (was UTIL_MAGAZINES in a few places)
* fallbackEnemyFaction had issues when factions are not properly available setup
  * British Forces faction should work properly.

## 20251025
### DarcMissions
No changes in mission jsons. This is mainly an addition for Freedom Fighters compatibility.

Main features:
* Support for British Forces
  * If you're using the faction, delete dc_enemyList.json to get an updated one.

## 20251023
### DarcMissions
Major changes in the json files. The safest way is to backup your current ones if you have made a lot changes. If not, just delete the old confs.
* Hunters will not follow players to NonValidAreas. 
* Second wave: Added a stub for second wave functionality. Currently does nothing but eventually you can have a second wave of attackers after win (and/or lose).
* ``missionActiveDistance`` and ``missionActiveTimeToEnd`` shrinks slowly to avoid missions being stuck on populated servers.

Fixes:
* Crashsite could crash if the helicopter is destroyed before crash landing.
* Convoy completion issues.
* If no groups were spawned for Patrol, mission crashed.
* ``enemyFactions`` setting was not working properly.
* Improved floor finding in buildings. Trying to recognize attic in buildings to avoid spawning AI's and loot in non reachable places. Still not perfect, but better.

Changes:
* dc_coreConfig.json locationAkas - Functionality enabled. You can name location names to work as specific locationTypes. For example anything called "military" becomes type MDT_BASE (79) in addition to the original definition.
* dc_lootList.json
  * Added ``GEAR_VEST`` - vests
  * Added ``GEAR_HANDWEAR`` - gloves
  * Added ``GEAR_UNIFORM`` - military clothing
  * Added ``CLOTHING_UNIFORM`` - civilian clothers
* dc_missionConfig.json
  * Added ``missionActiveDistanceMul`` - multiplier to modify distance on every cycle when in win/lose state. With this you can make the active distance to shrink.
  * Added ``missionActiveTimeToEndMul`` - same as above but for time.
  * Moved difficulty settings under ``missionDifficulty``. 
  * Added ``lootChanceCoef`` 
  * Added ``lootCountCoef`` (currently setting is unused)
* Patrol and Hunter has a hardcoded 20 seconds as their ``missionActiveTimeToEnd``. Once the AI is dead, quickly remove the mission. 

Known issues:
* AI from other missions can be tricked to follow you to a NonValidArea.

### DarcSpawner
Uses faster cached location information for spawn.

### DarcMissionsCompatFF
A new mod for compatibility with Freedom Fighters
* Missions are not spawned close the hideouts
* Controlled areas have a percentage option to spawn missions. 
* Crashsite helicopters are cleaned from the map at server boot when a player joins the game. We don't want wrecks on the map.

## 20251012
### DarcMissions
Moved ``locationTypes`` under ``general`` as it's a common attribute for all missions. That said, the mission json files have changes. The safest way is to backup your current ones if you have made a lot changes. If not, just delete the old confs. Hopefully this is one of the last big changes for now.
NOTE: dc_coreConfig.json and dc_lootList.json have been updated too.

Fixes:
* Hunters spawn distance fixes.

Changes:
* dc_missionConfig_*.json
  * Moved ``locationTypes`` under General. This affects all missions. 
* dc_coreConfig.json added locationAkas - You can name location names to work as specific locationTypes. For example anything called "military" could be of type MDT_BASE (79). Functionality currently disabled.
* dc_lootList.json
  * Added ``GEAR_HEADGEAR`` - helmets and gas masks. More gear lists TBD.
  * Added ``CLOTHING_HEADGEAR`` - non military head gear like hats. More clothing lists TBD.
  * Support for MCX Spear 6.8. Included in ``WEAPON_RIFLE``.
  * Support for Gas Masks mod. Included in ``GEAR_HEADGEAR``.

## 20251009
### DarcMissions
Major changes in the json files. The safest way is to backup your current ones if you have made a lot changes. If not, just delete the old confs.

This release is 
* for bug fixing and clean up of configurations. 
* to get some run time before the new version of Arma Reforger is out.
* a preparation for new functionality needed for my future ideas.

Main features:
* New mission: Stash - A secret stash is available for picking. May or may not have guards near it.
* Clicking on an active mission on map will show remaining time.

Fixes:
* Hunters work properly on Kunar map.
* Various crash fixes here and there.
* Convoys have passengers.

Configuration changes:
* dc_missionConfig.json
  * Difficulty can be configured in DarcMissions config. See: ``missionDifficulty``.

* dc_missionConfig_*.json
  * Faction definition for missions. You can force certain missions to spawn enemies from specific faction. See: ``faction``
  * Missions loadable from multiple files. See: ``missionFiles``.
  * Introduced difficulty option for missions. See: ``difficulty``.
  * Common options in missions use the same structure. See: ``Common parameters``
  * Mission index renamed to subIdx to eventually work as a unique ID.

## 20250824
Updated json: dc_enemyList.json - remove your existing file.

Fixes:
* HvtItem and HvtVip missions occasionally created a static win marker on map.
* Better support for factions: BALLIEN_BC_FACTION, BACON_622120A5448725E3_FACTION

## 20250822
Updated jsons: dc_enemyList.json, dc_lootList.json - remove your existing files.

### DarcMissions
Main features:
* New mission: High Value Target Item - A structure is to be destroyed.
* Change in behaviour: Loot is spawned only after win is reached. This is true for missions: Occupation, Squatters, HvtVip, HvtItem, Roadblock
* Added convoys with armored vehicles and cars with turrets.
* lootList support for mods: M4BlockII, RAYZIOPTICSPACK, RISLaserAttachments, BarrettM82, BaconDesertEagle
* New lootList keyword: WEAPON_RIFLE_BIG

Changes: 
* LootList structure changed.
* Renamed loot keys: UTIL_OPTICS to UTIL_OPTIC, UTIL_MAGAZINES to UTIL_MAGAZINE
* More randomization for location position searching. Avoid having the mission always in same place.
* Fixed the weird crash with Patrol mission

Fixes:
* Win condition not set properly for some of the missions

### DarcSpawner
Configuration json has changed. The safest way is to backup your current ones if you have made a lot changes. If not, just delete the old confs.

Main features:
* Supports spawning loot containers on designated spots. 

## 20250801
Major changes in the json files. The safest way is to backup your current ones if you have made a lot changes. If not, just delete the old confs.

### DarcMissions
Main features:
* New mission: High Value Target VIP - An AI is defined as a High Value Target VIP. Go take him down.
* Changes in missions related to win condition. Notice that depending on the (sub-)mission, for a win you may not need to kill all AIs. So, be careful out there.
* Game master map view:
  * Missions and NonValidAreas are shown on the map.
  * Missions can be deleted by pressing 'delete'.
* For all missions
  * A message is shown when a mission succeeds (``winMessage``) or fails (``loseMessage``).
  * First phase of win conditions is introduced. Currently win conditions are related to AI killing (100%, 75%, 50% or random value 30%-100%, High Value Target elimination)  
  * Mission icon will change to win or lose once finished.
* Enemy list have new entries to support Bacon Zombies and BallienCreatures. This is very untested so be careful. Entries:
  * G_ZOMBIE_SMALL
  * G_ZOMBIE_MEDIUM
  * G_ZOMBIE_LARGE
  * C_DEMON
  * C_DEMON_BOSS
* Added a new road block
* Hunter mission follows the same structure as others. You can define multiple types of hunters.
* Leaving ``locationTypes`` empty, will pick a completely random location from map. 
* New loot box: Briefcase

Changes:
* dc_enemyList.json and dc_lootList.json changes. 
* dc_nonValidArea.json removed showOnMap
* dc_coreConfig.json added showOnGMMapNonValidArea, showOnGMMapMissionMarker
* dc_Mission*.json added markerType, markerIdx, showMessage
* dc_Mission*.json moved winCondition, winMessage, loseMessage, xp under each individual mission item.

### DarcSpawner
Main features:
* You can have multiple spawnSets which are randomly chosen
* Configuration structure follows the same way as missions. This means that a new json is to be used. Delete your old one.

## 20250801
Minor update to properly find enemies when using TOH ReCharacters. Update *should not* affect existing installs. If you're using TOH ReCharacters, delete your dc_enemyList.json and a new one will be created.

Main features:
* Support for TOH ReCharacters
  * MEI supported
  * MEC is not supported
* Checked compatibility with EveronXArland and Novka

Fixes:
* Convoy mission: When vehicle spawn fails, the mission fails .. instead of crashing.
* Squatters mission: Building selection was not working properly.
* Mission position searching much faster. When mission was starting, servers took quite some time to find a spot on large maps.
* Crashes with EveronXArland when searching for mission position. 

## 20250630
Minor update to properly find enemies when using RHS. Update *should not* affect existing installs. If you're using RHS, delete your dc_enemyList.json and a new one will be created.

Main features:
* Support for RHS enemies
  * RHS_USAF and RHS_AFRF supported
  * RHS_ION is not supported
  
## 20250627
Some changes in the jsons. The safest way is to backup your current ones if you have made a lot changes. If not, just delete the old confs.

Main features: 
* New mission: Roadblock
* Added FAQ to documentation

Fixes:
* DarcDeathMarker was not possible to run without DarcMissions. 

Internal things:
* New icons with easier icon creation.

### Conf changes
* dc_missionConfig.json parameter changes
  * Moved ``missionDelayBetween`` in to the mission specific structures and renamed to ``delayBetween``. You can define the delay separately for dynamic and static.
  * Added ``showStaticMissionMarker`` that shows/hides static mission markers. 
* dc_coreConfig.json parameter changes
  * ``emptyPos.objectFilter`` : Less strict filtering for Anizay map
* dc_missionConfig_*.json
  * Added ``xp``, ``winCondition``, ``winMessage``, ``loseMessage`` as placeholders. WIP.

## 20250607
Some changes in the jsons. The safest way is to backup your current ones if you have made a lot changes. If not, just delete the old confs.

Main features: 
* You can spawn missions as a GM. You can find the mission under DarcMissions. 
* Static mission behaviour changed; these spawn immediately when server starts.
* AI will start from their first assigned waypoint.
* Improved floor finding for buildings
* Filtering rules for maps: Kunar, Anizay
* More smoke for Crashsites
* Magazines spawned in loot (0-4 with itemChance%)

### Conf changes
* dc_coreConfig.json parameter changes
  * Added ``emptyPos.limit``, ``emptyPos.ignoreFilter``, ``emptyPos.stopFilter``, ``emptyPos.classFilter``, ``emptyPos.objectFilter``
  * Fixed issue where the file was in two locations
* dc_missionConfig_Occupation.json
  * ``emptySize`` moved inside SDRC_Occupation
* dc_missionConfig.json parameter changes
  * NEW: Introduced ``missionDynamic`` and ``missionStatic`` structures
  * Moved ``missionCount``, ``missionActiveTime`` and ``missionTypeArray`` in to the mission specific structures  
  * Mission count can be defined as a value or dependent on the map size. See ``count``
  * Removed ``staticTryLimit``
* dc_lootList.json
  * ``WEAPON_ATTACHMENT`` renamed to ``UTIL_ATTACHMENT``
  * ``WEAPON_OPTICS`` renamed to ``UTIL_OPTICS``
  * ``UTIL_MAGAZINES`` : NEW - Adds a random magazine
  * ``UTIL_AMMO`` : NEW - Adds a random rocket, shell, flare, ..

## 20250526
Some changes in the jsons. The safest way is to backup your current ones if you have made a lot changes. If not, just delete the old confs.

Main feature: You can define the enemies via ```enemyFactions```. Just configure a faction, and that faction is used for enemies. USSR as default.

* Two new loot lists ```WEAPON_SHOTGUN``` , ```WEAPON_MG```. Supports BigChungus guns.
* Cars become persistent once you are close to them for a moment.
* No more duplicate conf files. Dir defined in core with ```subDir``` parameter.
* Mods wait for core to start first.
* Kunar map
  * POS_IN_WATER issue fixed
  * Added mosques and minarets as squatter spawns
* Core provides some additional game information.
* dc_coreConfig.json parameter changes
  * Added ```subDir```. This defines the subdir under profile\DarcMods where the confs go.
  * Added ```fallbackEnemyFaction``` . In case a faction is not available, set AI to this one.
  * Added ```enemyFactions``` . You can define the factions considered as enemies. Enemy AI will spawn from these factions unless a mission has defined a specific factions. The default missions use the new enemy list functionality
* dc_missionConfig.json parameter changes
  * ```missionProfile``` removed
* Enemylist functionality : Instead of defining specific enemy prefab, you can use the available keywords to define the type of AI to spawn. The enemy is selected to represent the faction found in ```enemyFactions```.
* All missions use enemylists. You should delete your existing jsons.
* Available keywords:
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
	G_SMALL : Small groups with two units. Mixed arms.

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
