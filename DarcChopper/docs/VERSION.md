# Version history

## Next Version
Main features:
* Fly path lines can be hidden. Settings -> Gameplay -> 

Fixes:
* Rocket sector slightly increased.
* Min/Max speed increased to 15/40.

# 20260326
Main features:
* First version on behaviour/brain. Currently has states: normal, search and destroy, evac.
* New logic on waypoints.
  * Defend: Land and leave troops. Shut down engine.
  * Search and Destroy: Chopper will patrol the area and when an enemy is found, change it's route and try to attack. This is repeated for 10mins.
  * Suppressive Fire: The old S&D. Do one or more attack runs on location.
  *  Artillery Fire: Single attack on a location.
* Evac functionality: If chopper is damaged enough, AI will try to find a safe location to land. Troops get our and chopper will stop it's enginges. If safe location not found, chopper will fly away to avoid being destroyed.
Fixes:
* Added entity type parameter for future.

Known issues:
* Wheel brakes on MI28 don't stay on. 
* When chopper lands and shuts down engine, the physics may do a small jump. TBF.
* There probably are some combinations of behaviour and waypoint use that I have not thought about and will bug. TBF

# 20260324
Small fix update.

Main features:
* Modified speed effect on pitch.

Fixes:
* Landing parameter changes.
* Hover issue when landing where chopper was just idling above ground.
* Upright flying fixes.

# 20260323
Small fix update.

Fixes:
* Chopper is properly handling damage.
* Start up height calculated correctly.

## 20260315
Main features:
* Improved landing: Much smoother and quite well on target spot
* Improved flight model: Trying to smoothen the transitions between different states. Should reduce 'jumping up' when going to a new location.
* Waypoint DEFEND: Chopper will look for a good landing spot near the place where the waypoint was set. WIP, and surely has it's issues.
* Once landed, chopper will estimate the time needed to wait until crew has jumped out. Sometimes a few AI decide to stay inside and ignore orders- cowards!

Known issues (to be fixed):
* Whan unloading crew, chopper slightly rotates.
* On consoles the crew is most likely Dock Workers. This is by current design. 
* Landing is not perfect. 
* Transitions from unloading->hover and hover->normal flight are a bit janky.

## 20260312
Main features:
* Proper Get Out functionality for AI to disembark helicopter. (Credits: ATiM)
* The first flight direction of a heli is where the nose points. 
* Smoother landing.

Fixes:
* Crew consisted of a mix of various faction soldiers and not from the right faction.

## 202603xx - FIRST RELEASE
* The first release of the mod. See [documenation](https://github.com/mokdevel/DarcMods/tree/main/DarcChopper) for more details.

This is a new mod to enable functionality to let AIs use helicopters to fly around, attack with guns on the helicopter, use rockets, patrol an area, find enemies, land to bring troops. Functionality is implemented as a single re-usable component. It is very easy to make your favourite modded helicopter to fly by AIs.

Helicopters can be spawned via GM and can be commanded. Currently available commands are listed here: [Waypoints](https://github.com/mokdevel/DarcMods/tree/main/DarcChopper#waypoints)

Showcase and place for Q&A is here: https://discord.com/channels/105462288051380224/1467159734543978506

If you have been using the DEV version, please change to the official one. This version is compatible with DarcMissions.