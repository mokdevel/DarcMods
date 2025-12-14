## Mission : Chopper
Example: [dc_missionConfig_Chopper.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Chopper.json)

A flying helicopter is spawned on the edges of the map and will fly to the mission location. Once the position is reached, the helicopter will continue to patrol randomly. If the helicopter is armed, AI shoot if an enemy is seen. The shooting is Arma Reforger functionality. Once the mission time is over, the helicopter will fly away (and despawn) ending the mission.

For the first release, there is no loot available. This is to be addressed in future updates.

### GM requested mission
The location is where the mission was dropped. The helicopter will fly first to this location.

### Parameters
Note: ```distanceToMission``` and ```distanceToPlayer``` overrides missionFrame settings. The initial helicopter position could start on top of a mission and it does not matter.

See [Common](./P_COMMON.md) , [AI](./P_COMMON.md#SDRC_MissionConfigAi), [Loot](./P_LOOT.md) , [Structures](./P_STRUCTURE.md) , [Camp](./P_CAMP.md), [Helicopter](./P_HELICOPTER.md)
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
int activeTime : (seconds) The time the mission should be running until the chopper flies away.
array<SDRC_HelicopterInfo> helicopterInfo : Helicopter definitions
array<SDRC_Chopper> subMissions : List of sub missions
```

### SDRC_Chopper
```
SDRC_MissionConfigGeneral general : See General parameters
SDRC_MissionConfigAi ai : See AI parameters
```
```
# Chopper specific
array<int> heliList : Indexes to helicopters defined in helicopterInfo. 
  For example: 0 will point to the first helicopter type defined in helicopterInfo. If you want to limit the mission to use only certain type of helicopter, change the values in heliList. You should not remove any of the definitions in helicopterInfo.
array<int> flyHeight : (min, max) The helicopter will fly between these values from the ground/sea. Helicopter spawns between these values.
array<int> speed : (min, max) Helicopter speed minimum/maximum.
array<float> flyDistance : (min, max) Distance for finding new positions.
  0.1 .. 0.99 : The distance is worldsize * value. Having small values will keep the helicopter flying near the center of world.
  1 .. n : The distance to fly to next
SDRC_EHeliWaypointGenerationType wpType : Helicopter way point functionality
  0 = NONE : Unused
  1 = RANDOM : Helicopter will find a new random destination depending on flyDistance
  2 = PATROL : Unused currentlt. *TBD*
```

## The flying mechanism
This is the short version and more documentation to be provided.

The functionality simulates (=fakes) how an AI would be flying the helicopter. The helicopter is provided with a destination or multiple, and a route is calculated to reach it. Depending on the curvature, distance and various other things, the helicopter is made to behave like it's actually flying.

Arma Reforger physics are used to some extent to modify the speed and how the helicopter descents or climbs. AI behaviour on if they see the player and decision to shoot are managed by Arma Reforger.

Parameters used for more natural flying:
- The steeper the turn, the more helicopter roll is affected.
- Pitch is modified depending on the speed.
- Curvature of the flight affects roll but the helicopter tries to return natural flat flying slowly.

### Re-use in mods
The flight model is done as a component. In theory you can add that to any helicopter and they will gain autonomous flying capabilities. For the moment I suggest not to use this and wait for a few update iterations.

### Issues
There are various small things that are to be fixed in the future updates:
- Climb in certain cases is too fast and does not look natural.
- Flight path finding is simple for the moment. Steep turns sometimes look nice, sometimes not.
