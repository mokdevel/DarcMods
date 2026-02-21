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
AI behaviour on if they see the player and decision to shoot are managed by Arma Reforger.

