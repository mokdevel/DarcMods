# FAQ

## Configuration related
### Q: The oneline jsons are hard to read/edit
The one line format comes from AR. If you use Notepad++, install JSON tools plugin and then press Ctrl+Alt+Shift+M. The file will look like: https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig.json . AR will read the better looking format just fine.

## Mission related
### Q: How to add both US and FIA factions as enemies?
If you're looking to have both US and FIA enemies randomly, just add "FIA" to ``enemyFactions`` in ``dc_missionConfig.json`` (example: ``"enemyFactions": [ "US", "FIA" ],``). Setting ``["US", "US", "US", "FIA"]`` would give 75% of missions to be US and 25% FIA.

### Q: I only want to use GM spawned missions - how?
If you set both static and dynamic missions count to zero, only GM spawned missions will spawn. GM spawned missions will always spawn even if the maximum dynamic missions has been reached. 

### Q: Sometimes the location name for a mission is [REDACTED]
If there is no name found close to a location, the intel will show it as redacted - blacked over. 