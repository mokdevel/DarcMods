# Flying helicopter
Used by: Chopper mission

## Supported prefabs
Currently supported prefabs for Chopper mission:

### Un-armed
- ``"{70A03633AAE61492}Prefabs/Vehicles/Helicopters/UH1H/UH1H_civ_base_Patrol.et"``
- ``"{5BBDA2DACF9CDCA4}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_Patrol.et"``

### Armed
- ``"{446634BB04ED3705}Prefabs/Vehicles/Helicopters/UH1H/SP02_GUNSHIP_Patrol.et"``
- ``"{96D1D7E22C123DEE}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_Patrol.et"``
- ``"{4CFDE3580182C452}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_gunship_HEDP_sharkNose_Patrol.et"``
- ``"{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Patrol.et"``
- ``"{3815F0A6CA3FF790}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HEDP_Patrol.et"``

# Creating a flying helicopter
Use the Arma Reforger workbench to create your own helicopter mod. The default prefabs in Arma Reforger are without their engines running and will simply crash before the flight is possible.

NOTE: For the moment, there are no pilots/AI added automatically to the helicopter.

- Open Arma Reforger Tools
- Create a mod with DarcCore as a dependency
- Select the helicopter you want to use
- Duplicate the helicopter prefab to your mod. In to the name, add "_Patrol" at the end.
- Edit prefab and add component ``SDRC_ChopperComp``.
- Modify the engine to be running immediately.
- Configure as in the image
- Open World Editor
- Add the helicopter you just created and raise it from ground to the initial flight height.
- Run. The helicopter should start to fly.

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/choppersettings.png" width=30% height=30%>

## SDRC_ChopperComp values
The component has default values that work quite well for the vanilla helicopters.
* ``Auto Start`` : (bool) When helicopter is spawned in the world, if enabled, it will automatically start to fly from spawn position to First Destination.
* ``Throttle`` : (float) Arma Reforger default throttle. Affects physics. The speed gain aka acceleration. Higher value will make the helicopter reach the maximum speed faster. May also affect raise.
* ``Rotor Force0`` : (float) Arma Reforger default main rotor force. Affects physics. The force that pushes the helicopter upwards. A too high value will make the helicopter to raise too fast and does not look nice. The helicopter type/weight will affect so you may need to adjust this.
* ``Rotor Force1`` : (float) Arma Reforger default rear rotor force. Affects physics.
* ``First Destination`` : (vector) First destination to fly to from the spawn position. The helicopter will continue fly autonomously after that.

Values modified by DarcMissions when spawning a mission.
* ``Speed Min`` , ``Speed Max`` : (float) The min/max speed for flying. Speed is changing depending on the angle where the helicopter is moving.
  * When DarcMissions mission ends, the helicopter will accelerate to 150% of maximum speed.
* ``Fly Height Low`` , ``Fly Height High`` : (float) The min/max fly height (from ground/sea level). 
  * This is not an exact value and sometimes the helicopter may fly below or above the set value. If we're flying below the Low value, the helicopter will increase the rotor force up the gain height. The same if we're too high - the rotor force is decreased. We use physics for a natural flight so very steep hills may end up as objects where the helicopter may crash.
  * A DarcMissions helicopter will spawn between these values.
* ``Distance Low`` , ``Distance High`` : (float) The min/max distance for waypoint. This works in two different ways depending if value is over or below 1.0.
  * ``0.1 .. 0.99`` : The distance is worldsize * value (percentage). Having small values will keep the helicopter flying near the center of world.
  * ``1 .. n`` : The distance to fly to next.
* ``SDRC_EHeliWaypointGenerationType m_fWpType`` : Waypoint type. Currently hidden.

# The flying mechanism
This is the short version and more documentation to be provided.

The functionality simulates (=fakes) how an AI would be flying the helicopter. The helicopter is provided with a destination or multiple, and a route is calculated to reach it. Depending on the curvature, distance and various other things, the helicopter is made to behave like it's actually flying.

Arma Reforger physics are used to some extent to modify the speed and how the helicopter descents or climbs. AI behaviour on if they see the player and decision to shoot are managed by Arma Reforger.

Parameters used for more natural flying:
- The steeper the turn, the more helicopter roll is affected.
- Pitch is modified depending on the speed.
- Curvature of the flight affects roll but the helicopter tries to return natural flat flying slowly.

## Re-use in mods and game modes
The flight model is done as a component. You are free to use it in yours - credits are appreciated. In theory you can add that to any helicopter and they will gain autonomous flying capabilities. 

## HowTo
- Create a compatibility mod (or include in to your mod).
- Include DarcCore as a dependency.
- Choose the helicopter to use.
- Use the guide above to set the necessary values.
- Add the component.
- Spawn and let it fly.

There are a few public functions that you can use. For flyin to destination(s), use ``AddDestination()``. You can call it multiple times to create a fly path. The helicopter will not go exactly to the given point and this is by design. If you want it to fly over a location, set the point behind the location.

## Issues
There are various small things that are to be fixed in the future updates:
- Climb in certain cases is too fast and does not look natural.
- Flight path finding is simple for the moment. Steep turns sometimes look nice, sometimes not.
- Some times the helicopter flies nose up after a steep turn. Have not yet found how to avoid that.