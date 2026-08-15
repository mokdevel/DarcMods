# DarcDeathMarker
<img src="https://github.com/mokdevel/DarcMods/blob/main/DarcDeathMarker/thumbnail.png" width=30% height=30%>

The mod creates a map marker on the position where you died. The marker is active 10 minutes and disappears after that. If you die again, another marker replaces the previous one.

## Installation
Just activate the mod (and dependencies) and you're good to go.

## Configuration
No configuration options.

## Core config
Example: [dc_deathMarkerConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcDeathMarker/ExampleConfigs/dc_deathMarkerConfig.json)
```
jsonVersion : Version of the json structure. Do not modify.
int version : Version number of the file for author use. Not used in game.
string author : Author of the file. Not used in game.
string comment : Generic comment to describe the file. Not used in game.
int markerLifeTime : (seconds) How long the marker is visible on map.
bool visibleOnlyToFaction : If true, the markers will only be visible for players in the same faction.
```

# Version history

## 20260725
Fixes:
* Mod waits for gamemode to start before starting to run. This could have side effects.

## 20260601
Fixes:
* Mod can be run stand alone. There was a dependency to DarcChopper.

## 20260111
Compatibility compile for DarcCore. New json fileformat.

## 20251213
Compatibility compile for DarcCore.

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

