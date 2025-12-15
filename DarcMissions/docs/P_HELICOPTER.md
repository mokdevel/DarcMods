# Helicopter details
This is a class that is used by multiple missions. It provides the information for helicopter functionality.

## SDRC_HelicopterInfo
This defines the helicopter parameters. The default prefabs in Arma Reforger are without their engines running and will simply crash before the flight is possible. For the flying helicopters the prefabs needs to be modified and currently only a couple of options are available.

```
string comment : Generic comment to provide details. Not used in game.
string resource : The prefab for the helicopter. See supported values above.
float throttle : The 'speed' the helicopter is flying.
float rotorForce : Force of the main rotor.
float rotor2Force : Force of the second rotor.
float power : The gas pedal the AI is pressing. Bigger value will result in faster increase in acceleration.
float rotorForceUp : The force up of the rotor. Affects how quickly helicopter rises.
```