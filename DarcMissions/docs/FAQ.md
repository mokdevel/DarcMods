# FAQ

## General
### Q: Suddenly the mod stopped working/generates errors/behaves oddly/..
It's possible there was an update and the json files are not compatible with the new version. Backup your current ones and then delete the old confs. New ones will be created at startup. 

### Q: Missions are not spawning
Most likely an update changed the jsons which are not compatible anymore. Backup your current ones and then delete the old confs. New ones will be created at startup. 

## Configuration related
### Q: Where are the configuration json files
All configurations are done via .json files. The configuration files will be under your ``profile\DarcMods\<subDir>\*``, where the subDir is defined via dc_coreConfig.json. 
Alternatively, users can pass a command line argument/startup parameter like this ``-darcSubDir yourSubDirHere`` to override the subDir set in ``dc_coreConfig.json``. DarcCore will look in ``profile\DarcMods\<subDir>`` first for config files, then look in ``profile\DarcMods\default`` if they are not found.

Examples below:
* Single player: ``C:\Users\username\Documents\My Games\ArmaReforger\profile\DarcMods``
* Workbench: ``C:\Users\username\Documents\My Games\ArmaReforgerWorkbench\profile\DarcMods``
* Dedicated server: ``C:\serverpath\profile\DarcMods``. The profile dir is defined with ``-profile=`` startup parameter.

### Q: My configurations stopped loading
Check that float values are in the format ``0.0`` instead of ``0`` without the .0. AR has an issue to load float values properly if they are integers. See [ticket](https://feedback.bistudio.com/T193209) .

## Mission related
### Q: How to add both US and FIA factions as enemies?
If you're looking to have both US and FIA enemies randomly, just add "FIA" to ``enemyFactions`` in ``dc_missionConfig.json`` (example: ``"enemyFactions": [ "US", "FIA" ],``). Setting ``["US", "US", "US", "FIA"]`` would give 75% of missions to be US and 25% FIA.

### Q: How to use RHS factions as enemies?
Set ``RHS_USAF`` or ``RHS_AFRF`` as the enemy faction in ``enemyFactions`` in ``dc_missionConfig.json``.
See: Factions in [mission config](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/MISSIONCONFIG.md#factions)

### Q: I only want to use GM spawned missions - how?
If you set both static and dynamic missions count to zero, only GM spawned missions will spawn. GM spawned missions will always spawn even if the maximum dynamic missions has been reached. 

### Q: Sometimes the location name for a mission is [REDACTED]
If there is no name found close to a location, the intel will show it as redacted - blacked over. 

### Q: How to add more loot
Loot is defined in the mission configuration files (for example: dc_missionConfig_Occupation.json). In each sub-mission there is a section called ``loot``. It has ``items`` and ``itemChance``. Add more items to the list to increase the amount. Increasing the chance, it is more likely to receive more loot (100% = 1.0). Note that mission difficulty affects both of these values.
There is no need to edit dc_lootList.json.
See: [loot](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LOOT.md) and [difficulty](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/MISSIONCONFIG.md#difficulty)

## Old and fixed

### Q: The oneline jsons are hard to read/edit
The one line format comes from AR. If you use Notepad++, install JSON tools plugin and then press Ctrl+Alt+Shift+M. The file will look like: https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig.json . AR will read the better looking format just fine.

