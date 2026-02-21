# DarcChopper

<img src="./thumbnail.png" width=30% height=30%>

The mod introduces functionality to let AIs use helicopters to fly around, attack with guns on the helicopter, use rockets, patrol an area, find enemies, land to bring troops. The mod is still very much WIP.

Functionality is implemented as a single re-usable component.

# Re-use in mods and game modes
The flight model is done as a component (SDRC_ChopperComp). You are free to use it in yours - credits are appreciated. In theory you can add that to any helicopter and they will gain autonomous flying capabilities.

# Configuration parameters
See: [Parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_HELICOPTER_FLY.md)

# The flying mechanism
The functionality simulates (=fakes) how an AI would be flying the helicopter. The helicopter is provided with a destination or multiple, and a route is calculated to reach it. Depending on the curvature, distance and various other things, the helicopter is made to behave like it's actually flying.

Arma Reforger physics are used to some extent to modify the speed and how the helicopter descents or climbs. You can modify some of the parameters to simulate for example weight with lower rotor force.

Parameters are used for more natural flying:
- The steeper the turn, the more helicopter roll is affected.
- Pitch is modified depending on the speed.
- Curvature of the flight affects roll but the helicopter tries to return natural flat flying slowly.

# AI functionality
## Flying
First of all, the AIs do not fly the helicopter. There is no behaviour tree for flying. The pilot AI is a passenger and all the flying magic happen by the component. 

## Skill and perception
The skill and perception from Arma Reforger is used for finding enemies. The higher the perception value, the better the AI is able to find its target. A line of sight is needed so you can hide in a building and the AI will not see through walls. The higher the skill, the better the accuracy is.

## Finding enemies
Perception of the AI affects the capability to find an enemy. This is not controlled by the mod. The mod queries the AIs for knowledge of an enemy. If an enemy is found, the mod keeps this enemy as the high value target (HVT). This can be queried by external mods. After a while, similar query is done and the HVT may change. In case, no enemies are known by the AIs, the helicopter will forget the HVT knowledge.

## Shooting
### Machine Guns
AI shooting with MGs is controller by Arma Reforger and not by the mod. If targets are recognized, the AI will shoot by themselves with accuracy defined by the skill.

### Rockets
Arma Reforger does not provide functionality for AIs to use rockets. The mod fakes this by spawning a rocket at ``Rocket Position`` from the helicopter position. The rocket is launched with the capabilities provided by ``MissileMoveComponent``. After this, Arma Reforger handles the physics and related functionality.

Currently the helicopter is not queried for available weapons and their rocket ammo. You need to define the rockets for the helicopter to use in ``Rocket Prefabs``. At startup, one of the prefabs is chosen for use. 

Rocket prefabs tested:
```	
- {ECD8628EBF7E5F6B}Prefabs/Weapons/Ammo/Ammo_Rocket_Hydra70.et
- {072A755D5CB85D47}Prefabs/Weapons/Ammo/Ammo_Rocket_Hydra70_HE_M229.et
- {61AF60E0235DC3B1}Prefabs/Weapons/Ammo/Ammo_Rocket_Hydra70_HEDP_M247.et
	
- {C9A1612DC5340613}Prefabs/Weapons/Ammo/Ammo_Rocket_S5.et
- {EF17BED6DCEE4DE4}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_FRAG_S5MO.et
- {EE65544BA845C458}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_HEDP_S5KO.et
```
