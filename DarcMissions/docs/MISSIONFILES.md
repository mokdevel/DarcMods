# Basics
It is possible to define additional missions to be loaded from file system. You can drop new mission files in the ``subDir`` directory defined in dc_coreConfig.json. This is the same directory for all the mission files.

## Functionality
The configuration directory is searched for additional .json files that are of the same type. The filename shall follow the naming of the main mission file added with an ``_`` and your additional information. 
Example: A new patrol mission based on the original ``dc_missionConfig_Patrol.json`` shall be named as ``dc_missionConfig_Patrol_NewFile.json``.

The missions ``subIdx`` will be modified automatically. The list of ``missionList`` will be appended to already loaded list. Always use values starting from 0. 

**UNTESTED**: The files can be in a subdirectory under the ``subDir`` main directory.
``"missionFiles": ["dc_missionConfig_HvtItem_NewMissions.json"]`` - The file will be loaded from the same directory as the mission jsons.
``"missionFiles": ["new/dc_missionConfig_HvtItem_NewMissions.json"]`` - The file will be loaded from a ``new`` directory under the the json mission directory.

### Example
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
  
## Dependencies
I want to avoid 
  modList
  
## Mission packs
<TBD>

### How to create a mission packs
<TBD>