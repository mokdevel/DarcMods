

# Version history

## Next release (unreleased)

### DarcMissions
Main features:
* Change in behaviour: Loot is spawned only after win is reached. This is true for missions: Occupation, Squatters, HvtVip, Roadblock

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
