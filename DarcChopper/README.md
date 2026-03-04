# DarcChopper

<img src="./thumbnail.png" width=30% height=30%>

The mod introduces functionality to let AIs use helicopters to fly around, attack with guns on the helicopter, use rockets, patrol an area, find enemies, land to bring troops. The mod is still very much WIP.

Functionality is implemented as a single re-usable component.

* [Version history](VERSION.md)

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

## Flight path
The flight path is a spline defined by multiple points (destinations). The helicopter follows it by trying to stay close to it. It's purpose is not to be exact as it's not a train on a track. For example sharp corners will be rounded and additional points are added to make the flight smoother. Sometimes you will see that the destination you want to reach, is not flown through a shortest route. This is by design. AI is making it's own decisions with recommendations coming from you.

## Hills and obstacles
Flying tries to avoid crashing, but unfortunately this sometimes happens. Like humans, AIs make occasionally bad decisions. Some adjustments are done when being too close to a hill: the raise power is increased and speed is decreased. 

NOTE: Currently there is no ray casting involved to find obstacles in front of the helicopter. An obstacle could be a steep hill or another flying helicopter and crashes sometimes happen.

## Destruction
The component is active as long as the helicopter is functional and there is a minimum of one pilot. When the helicopter is deemed non-fuctional, the mod will stop controlling it. Arma Reforger will take control and handle the end of life. The helicopter will fly according to AR physics and this may end up in interesting and funny situations. This is by design.

# AI functionality
A crew is needed for the helicopter to fly.  

## Spawning crew
If the helicopter prefab does not have crew a assigned, the mod will assign crew members. For pilots, members from C_CREW is selected. The rest of the passengers will be from C_RIFLEMAN. See: [EnemyLists](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LISTS.md#enemy-lists). 

## Factions
The helicopter itself has a faction attached to it. For example MI8's are assigned for USSR faction and the crew is selected from the same faction. Safe factions to use are US, USSR and FIA, but others should work too.

When the helicopter is spawned, the faction is first checked from ``Faction`` setting in the SDRC_ChopperComp. By default the value is empty and the helicopter assigned faction is used. If the parameter is set, the faction is used for crew spawning. 

NOTE: There is no check for correctness currently. So, a faction defined as USRS will result in errors.

## Skill and perception
The skill and perception from Arma Reforger is used for finding enemies. The higher the perception value, the better the AI is able to find its target. A line of sight is needed so you can hide in a building and the AI will not see through walls. The higher the skill, the better the accuracy is.

Skill is define with ``AI Skill`` and percetion by ``AI Perception`` parameters.

## Finding enemies
Perception of the AI affects the capability to find an enemy. This is not controlled by the mod. The mod queries the AIs for knowledge of an enemy. If an enemy is found, the mod keeps this enemy as the high value target (HVT). This can be queried by external mods. After a while, similar query is done and the HVT may change. In case, no enemies are known by the AIs, the helicopter will forget the HVT knowledge.

## Flying
First of all, the AIs do not fly the helicopter. There is no behaviour tree for flying. The pilot AI is a passenger and all the flying magic happen by the component. 

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

# GM Functionality
You can spawn choppers as a GM and they will start to fly around the world randomly. You can control them with waypoints.

## Entity browser
The helicopters piloted by AIs can be found by filtering with DarcChopper. There are variants ready made for faction US, USSR and FIA. Drag and drop in to the world, use ALT-key to change altitude. 

NOTE: You can not set the helicopter on ground and let it lift off and start to fly. This is by design.

## First flight
When spawned, helicopter will check it's altitude to make sure it's above ``Fly Height Low``. If not, the helicopter will be moved and this may look like an ugly jump. A random fly destination is chosen and helicopter is rotated towards it. Then we're ready for flying. 

## Flight path
A flight path is assigned to the helicopter and it will start to follow it. The flight path is shown with green lines on screen and once reaching the end, a new flight path is created. The flight path will be set between ``Fly Height Low`` and ``Fly Height High`` if the location is on ground or too high up. 

## Lines: Green and Blue
You will see green and blue lines on screen when the helicopter is flying. 

* Green: Shows the flight path for the helicopter. This is a spline that is followed while flying. This shows were we're generally going to end up.
* Blue: If you've set waypoints for the helicopter, blue straight lines will show the future route. When the helicopter reaches the end of a green line, a check for waypoints (blue lines) is made. If waypoints are available, a new route is created via them and you will see the blue straight lines changing to a green flight path.
* Gray: Shows the flight path towards the area to patrol. Once reaching the destination, a patroling flight path will be created.
* Red: Location to attack with rockets. This is more like a bombing run where the target location is shot with rockets regardless of if there are enemies or not.

NOTE: The lines are drawn on a canvas so they will be on top of the screen items. This is something work on and improve.

# Waypoints
You can assign waypoints for the helicopter. Just pick any AI/group or even the helicopter, select a waypoint and drop it on the map. Use ALT-key to change the altitude of the waypoint, but don't worry, if it's on the ground by mistake, it will be set to right flight height. You can set multiple waypoints as a chain and future flight will follow it.

Once a waypoint is dropped, the helicopter will pick it from the map and it will disappear. This means that it has been considered for future use.

<img src="../pics/waypoints.png">

Check the supported waypoints below. Other waypoints you define, will disappear from the map and will be discarded.

## Move
The helicopter is requested to fly to this destination. You can set multiple ones to create a longer route.

## Force Move
The helicopter is requested to fly to this destination immediately. Any existing plans will be discarded and the new destination is accepted.

## Move Relaxed
The helicopter is requested to fly to this destination and do rounds around the area. We will circle around the area for a while until continuing normal flight. You can set multiple points for longer patrol times.

## 

