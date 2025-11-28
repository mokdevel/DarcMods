# DarcDeathMarker
The mod creates a map marker on the position where you died. The marker is active 10 minutes and disappears after that. If you die again, another marker replaces the previous one.

## Installation
Just activate the mod (and dependencies) and you're good to go.

## Configuration
No configuration options.

## Core config
Example: [dc_deathMarkerConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcDeathMarker/ExampleConfigs/dc_deathMarkerConfig.json)
```
int version : Version number of the file.
string author : Author of the missions.
int markerLifeTime : (seconds) How long the marker is visible on map.
bool visibleOnlyToFaction : If true, the markers will only be visible for players in the same faction.
```

# Version history

## 20251128
Compatibility compile for DarcCore.

## 20251123
Compatibility compile for DarcCore.

## 20251030
### Update for Arma Reforger 1.6
Minor fixes and support for 1.6.

## 20250910
### DarcDeathMarker
Main features:
* Configuration options added.
* DeathMarker visibility can be limited to the player faction.

