# Helicopter details
This is a class that is used by multiple missions. It provides the information for helicopter functionality.

## SDRC_HelicopterInfo
This defines the helicopter parameters. The default prefabs in Arma Reforger are without their engines running and will simply crash before the flight is possible. For the flying helicopters the prefabs needs to be modified and currently only a couple of options are available.

```
string resource : The prefab for the helicopter. See supported values above.
float throttle : The 'speed' the helicopter is flying.
float rotorForce : Force of the main rotor.
float rotor2Force : Force of the second rotor.
float power : The gas pedal the AI is pressing. Bigger value will result in faster increase in acceleration.
float rotorForceUp : The force up of the rotor. Affects how quickly helicopter rises.
```

## Define a new flying helicopter prefab
Used by: Chopper mission
This defines the helicopter parameters. The default prefabs in Arma Reforger are without their engines running and will simply crash before the flight is possible. For the flying helicopters the prefabs needs to be modified and currently only a couple of options are available.

NOTE: The details are currently missing. *TBD*

Currently supported prefabs:
- ```"{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Flying_Patrol.et"```
- ```"{3815F0A6CA3FF790}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HEDP_Flying_Patrol.et"```
- ```"{96D1D7E22C123DEE}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_Patrol.et"```
- ```"{82704CE53C89C888}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying_Patrol.et"```

## Define a new crashing helicopter prefab
Used by: Crashsite mission
This defines the helicopter parameters. The default prefabs in Arma Reforger are without their engines running and will simply crash before the flight is possible. For the flying helicopters the prefabs needs to be modified and currently only a couple of options are available.

Currently supported prefabs:
- ```"{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et"```
- ```"{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et"```

### HowTo
- Duplicate the helicopter prefab to DarcMissions. In to the name, add "_flying" at the end.
- Edit prefab and add components ```SCR_FireplaceComponent```. This is only for the smoke.
- Modify the engine to be running immediately.
- Configure as in the image
<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/helicoptersettings.png" width=30% height=30%>

