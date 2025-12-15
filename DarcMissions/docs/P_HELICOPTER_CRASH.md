# Crashing helicopter
Used by: Crashsite mission

## Supported prefabs
Currently supported prefabs:
- ``"{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et"``
- ``"{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et"``

## Define a new crashing helicopter prefab

The default prefabs in Arma Reforger are without their engines running and will simply crash before the flight is possible.

### HowTo
- Duplicate the helicopter prefab to your mod. In to the name, add "_flying" at the end.
- Edit prefab and add components ``SCR_FireplaceComponent``. This is only for the smoke.
- Modify the engine to be running immediately.
- Configure as in the image
<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/helicoptersettings.png" width=30% height=30%>

