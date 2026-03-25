# Version history

## Next Version
Main features:
* Fly path lines can be hidden. Settings -> Gameplay -> 
* When chopper is damaged enough, it will try to make a evacuation landing in a safe place.

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