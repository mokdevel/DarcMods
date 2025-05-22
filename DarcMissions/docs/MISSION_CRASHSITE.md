## Mission : Crashsite
Example: [dc_missionConfig_Crashsite.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Crashsite.json)

A flying helicopter is spawned in a random location flying towards a random location. The helicopter either crashes or if the speed is reduced enough, it will be destroyed via script. Once the helicopter has crashed, AI will be spawned to protect the crashsite. NOTE: There is no parameter for waypoints and AIs will ```LOITER```. Loot can be added in to the loot box carried. Additional structures can be spawned around the crashed helicopter. 

The location for the initial chopper spawn is random and only checks for mission distance and player distance. The destination is toward map center with randomization. This is to avoid the direction to be towards the map edges. There will be cases where the helicopter flies towards the sea and this is just unfortunate randomization (read: this is by design).

Note: ```distanceToMission``` and ```distanceToPlayer``` overrides missionFrame settings. The initial helicopter position could start on top of a mission and it does not matter.
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
int distanceToMission : Distance to mission when searching for a mission pos. Overrides missionFrame settings.
int distanceToPlayer : Distance to player when searching for a mission pos. Overrides missionFrame settings.
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
int aiSkill : See General parameters
float aiPerception : See General parameters
array<SCR_DC_HelicopterInfo> helicopterInfo : The helicopter is selected randomly.
SCR_DC_Loot loot : (optional) Loot found in the box. 
array<SCR_DC_Structure> siteItems : (optional) Prefabs to spawn at the crashsite. Note that first item shall be the loot box.
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
- Duplicate the helicopter prefab to DarcMissions. In to the name, add "_flying" at the end.
- Edit prefab and add components ```SCR_FireplaceComponent```. This is only for the smoke.
- Modify the engine to be running immediately.
- Configure as in the image
<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/helicoptersettings.png" width=30% height=30%>