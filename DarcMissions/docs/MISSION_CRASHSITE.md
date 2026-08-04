## Mission : Crashsite
Example: [dc_missionConfig_Crashsite.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Crashsite.json)

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/s_crashsite.jpg" width=30% height=30%>

A flying helicopter is spawned in a random location flying towards a random location. The helicopter either crashes or if the speed is reduced enough, it will be destroyed via script. Once the helicopter has crashed, AI will be spawned to protect the crashsite. 
NOTE: There is no parameter for waypoints and AIs will ```LOITER```. Loot can be added in to the loot box carried. Additional structures can be spawned around the crashed helicopter. 

The location for the initial chopper spawn is random and only checks for mission distance and player distance. The destination is toward map center with randomization. This is to avoid the direction to be towards the map edges. There will be cases where the helicopter flies towards the sea and this is just unfortunate randomization (read: this is by design).

### GM requested mission
The location is where the mission was dropped.

### Parameters
Note: ```distanceToMission``` and ```distanceToPlayer``` overrides missionFrame settings. The initial helicopter position could start on top of a mission and it does not matter.

See [Common](./P_COMMON.md) , [AI](./P_COMMON.md#SDRC_MissionConfigAi), [Loot](./P_LOOT.md) , [Structures](./P_STRUCTURE.md) , [Camp](./P_CAMP.md) , [Crashing Helicopter](./P_HELICOPTER_CRASH.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
bool disableArsenal : See Common parameters
array<int> missionList : See Common parameters
array<string> missionFiles : See Common parameters
```
```
# Crashsite specific
int distanceToMission : Distance to mission when searching for a mission pos. Overrides missionFrame settings.
int distanceToPlayer : Distance to player when searching for a mission pos. Overrides missionFrame settings.
array<int> flyHeight : (min, max) The helicopter is spawned between these height values. The higher the values, the longer flight. Setting to zero will create the crashsite immediately.
array<SDRC_Crashsite> subMissions : List of sub missions
```

### SDRC_Crashsite
SDRC_Crashsite uses Camp functionality. 
```
# SDRC_Crashsite specific
SDRC_Camp
  loot : (optional) The loot found in the box spawned near the crash
  campItems : (optional) Prefabs to spawn at the crashsite. Note that first item shall be the loot box.
array<SDRC_HelicopterInfo> helicopterInfo : The helicopter is selected randomly.
```

### SDRC_HelicopterInfo
This defines the helicopter parameters. The default prefabs in Arma Reforger are without their engines running and will simply crash before the flight is possible. For the flying helicopters the prefabs needs to be modified and currently only a couple of options are available.

```
string comment : Generic comment to provide details. Not used in game.
string resource : The prefab for the helicopter. See supported values above.
float throttle : The 'speed' the helicopter is flying.
float rotorForce : Force of the main rotor.
float rotor2Force : Force of the second rotor.
```

## Supported helicopters
By default there are a few vanilla helicopters supported. For modded ones, you need a compatibility mod. Currently supported prefabs for CrashSite mission are below. 

### Un-armed
- ``{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_crashing.et``
- ``{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_crashing.et``