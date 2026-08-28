# Basics
It is possible to define additional missions to be loaded from file system. You can drop new mission files in the ``subDir`` directory defined in dc_coreConfig.json. This is the same directory for all the mission files.

## Functionality
The configuration directory is searched for additional .json files that are of the same type. The filename shall follow the naming of the main mission file added with an ``_`` and your additional information. 
Example: A new patrol mission based on the original ``dc_missionConfig_Patrol.json`` shall be named as ``dc_missionConfig_Patrol_NewFile.json``.

The missions ``subIdx`` will be modified automatically. The list of ``missionList`` will be appended to already loaded list. Always use values starting from 0. 

**UNTESTED**: The files can be in a subdirectory under the ``subDir`` main directory.
``"missionFiles": ["dc_missionConfig_HvtItem_NewMissions.json"]`` - The file will be loaded from the same directory as the mission jsons.
``"missionFiles": ["new/dc_missionConfig_HvtItem_NewMissions.json"]`` - The file will be loaded from a ``new`` directory under the the json mission directory.

**NOTE:** All mission types are not prepared for this yet. Occupation, Stash and HvtItem works.

## Example
You plan to create a new mission file with Patrol missions.
* You create a new mission json file that has the same parameters as ``dc_missionConfig_Patrol.json``. Make a copy of an existing one and modify.
* You rename it as ``dc_missionConfig_Patrol_MyPatrolMissions.json``
* You create ``subMissions`` with subIdx ``0,1,2``. Three new missions. 
* You define ``"missionList": [0,1,1,2,2]``
* There are already two other missions in other files (subIdx 0 and 1) with definition ``"missionList": [0,1]``.
* When your file is loaded, your subIdx will be renumbered and ``missionList`` combined. 
  * Your subIdx 0 will become 2 as 0 and 1 are already defined.
  * The list will look like this internally: ``"missionList": [0,1,2,3,3,4,4]``. ``2,3,4`` are your old ``0,1,2``.
  
There is limited error checking in the file loading so be careful when defining the missions.
  
# Dependencies
I want to avoid the situation where you need to have a lot of mods just to run DarcMissions. The minimum shall be just DarcMissions and you will get a default set of missions. By adding additional mods, the system will enable additional missions. 

Each submission has a parameter ``modList``. See: [P_COMMON](https://github.com/mokdevel/DarcMods/blob/91b541bdb11c11b4ed2561ab8aeea8417162b214/DarcMissions/docs/P_COMMON.md#sdrc_missionconfig)
```
array<string> modList : List of mods needed for this mission.
```
If the submission has defined a modList, the mods are checked to be available. If the mods are not available, the subMission is not available for players. You will see this as a warning in your logs.

```
[SDRC_MissionConfig:LoadMissionFiles] For index 0: Zombie and Demon characters. (dc_missionConfig_Occupation_Horror.json) to work, a mod is needed: $TheModWithZombiesInIt:
                                           <--- comment in you submission --->   <--- filename ------------------------>                             <--- missing mod --->
```

The mod name is found <TBD>.

# Supported mods

## French Animals 
* [BAR_FrenchAnimals](https://reforger.armaplatform.com/workshop/6A04798F752CBE57-BAR_FrenchAnimals)
If you add this mod in to your server list, this will enable additional animal related missions of type Occupation. 

It is recommended to use also DarcSpawner_Animals to get random wild life in to the world.
* [DarcSpawner_Animals](https://reforger.armaplatform.com/workshop/6A18782245D36F45-DarcSpawner_Animals)
  
# Mission packs
* TBD

### How to create a mission packs
* TBD