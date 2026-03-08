# Flying helicopter

## Creating a flying helicopter
Use the Arma Reforger workbench to create your own helicopter mod. The default prefabs in Arma Reforger are without their engines running and will simply crash before the flight is possible.

NOTE: Below information is slightly outdated.

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
- Pilots/AI is added automatically to the helicopter. This is true for US, USSR and FIA. Other factions are not tested but might work.

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/choppersettings.png" width=30% height=30%>

## SDRC_ChopperComp values
### Chopper
The component has default values that work quite well for the vanilla helicopters.

### Flight settings
* ``Auto Start`` : (bool) When helicopter is spawned in the world, if enabled, it will automatically start to fly from spawn position to First Destination. For modding purposes or DarcMissions use, disable this.
* ``Throttle`` : (float) Arma Reforger default throttle. Affects physics. The speed gain aka acceleration. Higher value will make the helicopter reach the maximum speed faster. May also affect raise.
* ``Rotor Force0`` : (float) Arma Reforger default main rotor force. Affects physics. The force that pushes the helicopter upwards. A too high value will make the helicopter to raise too fast and does not look nice. The helicopter type/weight will affect so you may need to adjust this.
* ``Rotor Force1`` : (float) Arma Reforger default rear rotor force. Affects physics.
* ``Speed Min`` , ``Speed Max`` : (float) The min/max speed for flying. Speed is changing depending on the angle where the helicopter is moving.
  * When DarcMissions mission ends, the helicopter will accelerate to 150% of maximum speed.
* ``Fly Height Low`` , ``Fly Height High`` : (float) The min/max fly height (from ground/sea level). 
  * This is not an exact value and sometimes the helicopter may fly below or above the set value. If we're flying below the Low value, the helicopter will increase the rotor force up to gain height. The same if we're too high - the rotor force is decreased. We use physics for a natural flight so very steep hills may end up as objects where the helicopter may crash.
  * A DarcMissions helicopter will spawn between these values.
* ``Distance Low`` , ``Distance High`` : (float) The min/max distance for waypoint. This works in two different ways depending if value is over or below 1.0.
  * ``0.1 .. 0.99`` : The distance is worldsize * value (percentage). Having small values will keep the helicopter flying near the center of world.
  * ``1 .. n`` : The distance to fly to next.

### AI settings
* TBD

### Weapons
* ``Rocket Sector`` : (degrees) The sector in front of the rocket where the rocket can be shot. This is a cone with this angle left-right. For example: 10 degrees will result in cone of 20 degrees from the nose of the helicopter.
* ``Rocket Delay`` : (seconds) The time between rockets to be shot. 
* ``Rocket Position`` : Consists of three parameters
  * ``X``: The distance from helicopter nose to spawn the rocket
  * ``Y``: Target Y-position height modificator. Target position is usually on the ground and position is raised according to this value.
  * ``Z``: TBD - Location of helicopter rocket position left/right. Rockets will be spawned on the side of the helicopter instead of infront of the nose.

### Unsorted
* ``Fly Destinations`` : TO BE DOCUMENTED. You can create your own flight patterns here.

## Re-use in mods and game modes
The flight model is done as a component. You are free to use it in yours - credits are appreciated. In theory you can add that to any helicopter and they will gain autonomous flying capabilities. 

## HowTo
- Create a compatibility mod (or include in to your mod).
- Include DarcCore as a dependency.
- Choose the helicopter to use.
- Use the guide above to set the necessary values.
- Add the component.
- Spawn and let it fly.
- Dependency needed: [DarcCore](https://reforger.armaplatform.com/workshop/631EE12D448D7FCC-DarcCore) 

There are a few public functions that you can use. For flying to destination(s), use ``AddDestination()``. You can call it multiple times to create a fly path. The helicopter will not go exactly to the given point and this is by design. If you want it to fly over a location, set the point behind the location.

## Issues
There are various small things that are to be fixed in the future updates:
- Climb in certain cases is too fast and does not look natural.
- Flight path finding is simple for the moment. Steep turns sometimes look nice, sometimes not.
- Some times the helicopter flies nose up after a steep turn. Have not yet found how to avoid that.
