## Mission : Chopper
Example: [dc_missionConfig_Chopper.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Chopper.json)

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/s_chopper.jpg" width=30% height=30%>

A flying helicopter is spawned on the edges of the map and will fly to the mission location. If the helicopter is armed, AI shoot if an enemy is seen. The shooting is Arma Reforger functionality. 

Once the missions position is reached, the helicopter will behave depending on the ``wpType``:
* ``RANDOM`` : Continue to patrol randomly.
* ``PATROL`` : Patrol around the mission location.
* ``SEARCH`` : Search for players while flying around. If players are found, the mission is over and Hunters are sent to clear out the area. The search is done by the AI itself and finding is depending on the perception value. 

Once the mission time is over, the helicopter will fly away (and despawn) ending the mission.

For the first release, there is no loot available. This is to be addressed in future updates.

### GM requested mission
The location is where the mission was dropped. The helicopter will fly first to this location.

### Parameters
Note: ```distanceToMission``` and ```distanceToPlayer``` overrides missionFrame settings. The initial helicopter position could start on top of a mission and it does not matter.

See [Common](./P_COMMON.md) , [AI](./P_COMMON.md#SDRC_MissionConfigAi), [Loot](./P_LOOT.md) , [Flying Helicopter](https://github.com/mokdevel/DarcMods/blob/main/DarcChopper/docs/P_HELICOPTER_FLY.md#creating-a-flying-helicopter)
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
int distanceToMission : RESERVED_FOR_FUTURE - Distance to mission when searching for a mission pos. Overrides missionFrame settings.
int distanceToPlayer : RESERVED_FOR_FUTURE - Distance to player when searching for a mission pos. Overrides missionFrame settings.
float distanceToStart : (percentage) How far from the world center shall the helicopter spawn.
int activeTime : (seconds) The time the mission should be running until the chopper flies away.
array<SDRC_Chopper> subMissions : List of sub missions
```

### SDRC_Chopper
```
SDRC_MissionConfigGeneral general : See General parameters
SDRC_MissionConfigAi ai : See AI parameters
```
```
# Chopper specific
array<string> heliList : Prefab to helicopter with SDRC_ChopperComp
array<int> flyHeight : (min, max) The helicopter will fly between these values from the ground/sea. Helicopter spawns between these values.
array<int> speed : (min, max) Helicopter speed minimum/maximum.
array<float> flyDistance : (min, max) Distance for finding new positions.
  0.1 .. 0.99 : The distance is worldsize * value. Having small values will keep the helicopter flying near the center of world.
  1 .. n : The distance to fly to next
SDRC_EHeliWaypointGenerationType wpType : Helicopter way point functionality
  0 = NONE : Unused
  1 = RANDOM : Helicopter will find a new random destination depending on flyDistance
  2 = PATROL : Helicopter patrols around a given area.
  3 = SEARCH : Helicopter flies and searches for players. Once player is found, mission completes and Hunters are sent to kill players.   
SDRC_EHeliEnemySearchType enemyType : Which targets are considered enemies
  0 = NONE : Ignore all targets
  1 = ANY : Any target seen is considered as an enemy.
  2 = PLAYER : Only players are considered targets
  3 = ANY_CHAR : Any character (player and AI) is considered as an enemy.
```

## Supported helicopters
By default there are vanilla helicopters supported. For modded ones, you need a compatibility mod. Currently supported prefabs for Chopper mission are below. You can add the modded ones in the ``heliList`` parameter even if the mod is not available. In this case, the mod will browse through the whole ``heliList`` to find a one available. Suggestion is to add atleast one vanilla helicopter to get the mission to spawn.

### Un-armed
- ``"{435F663B9456C29E}Prefabs/Vehicles/Helicopters/UH1H/UH1H_civ_livery_v1_Patrol.et"``
- ``"{5BBDA2DACF9CDCA4}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_Patrol.et"``

### Armed
- ``"{446634BB04ED3705}Prefabs/Vehicles/Helicopters/UH1H/SP02_GUNSHIP_Patrol.et"``
- ``"{96D1D7E22C123DEE}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_Patrol.et"``
- ``"{4CFDE3580182C452}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_gunship_HEDP_sharkNose_Patrol.et"``
- ``"{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Patrol.et"``
- ``"{3815F0A6CA3FF790}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HEDP_Patrol.et"``

### Modded
Modded helicopters are supported through a compatibility mod. Currently available:
* https://reforger.armaplatform.com/workshop/672F2BB6523FBA29-WZHelisforDarcMissions
* https://reforger.armaplatform.com/workshop/6720D3B2BEBC691E-Mi24and28forDarcMissions
* https://reforger.armaplatform.com/workshop/6850D5F667CEFF94-AH-6MforDarcMissions
* https://reforger.armaplatform.com/workshop/684F3C94BD457F85-KA-52forDarcMissions
