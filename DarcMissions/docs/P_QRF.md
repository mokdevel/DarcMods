# QRF - Quick Reaction Force
A QRF or quick reaction force is an additional attack wave sent to a mission location once the mission is about to end.

* The mission ends with a WIN or LOSE condition.
* The ``activation`` rule is checked for the mission. If it matches, we check for chance.
* The ``chance`` is modified by the difficult ``qrfChance``. The values are multiplied and the value represents percentage (1.0 = 100%).
  Example: A mission with HARD difficulty has a ``qrfChance`` of 1.2. A 0.5 ``chance`` results in a 60% chance to get a QRF (0.5*1.2).
* A location within ``distance`` is searched. If found, the AI with the possible vehicle is spawned.
* The QRF attack will wait for ``delay`` time seconds before spawning.
* QRF AI will have a destination waypoint to come to the mission location.
  * Choppers will have a search and destroy objective at the mission location. They will patrol the area for a certain time, before flying away.

## Pre defined QRFs
Currently defined QRFs:
```
0, "index 0: Light group"
1, "index 1: Snipers"
2, "index 2: Heavy team"
3, "index 3: Spec ops"
4, "index 4: MG team"
5, "index 5: Multiple small groups"
6, "index 6: Deadly recon team"
7, "index 7: Not so sharp snipers"
30, "index 30: Group in a car"
31, "index 31: Armored vehicle"
32, "index 32: Armed vehicle"
60, "index 60: Armed chopper"
```

## SDRC_MissionConfigQrf
The setup for QRF details for a sub-mission.

See: [Difficulty](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/MISSIONCONFIG.md#sdrc_missiondifficulty)
```
array<int> subIdx : unique index for a QRF
  Example: "subIdx":[0,0,1,1,2] - QRF 0 and 1 will be chosen 40% chance and 2 with 20%
SDRC_EMissionSuccess activation : Which success activates the check for QRF
  SDRC_EMissionSuccess.WIN : Mission was won. Safe to use for all missions.
  SDRC_EMissionSuccess.LOSE : Mission was lost. Can be selectively be used with some types of missions.
  SDRC_EMissionSuccess.WIN_OR_LOSE : Mission either won or lost. Can be selectively be used with some types of missions.
float chance : The chance percentage for the sub-mission to get a QRF. Note that difficulty will modify this value.
array<int> delay : (seconds) Delay min-max before spawning QRF,
```

## SDRC_QrfConfig
```
jsonVersion : Version of the json structure. Do not modify.
int version : Version number of the file for author use. Not used in game.
string author : Author of the file. Not used in game.
string comment : Generic comment to describe the file. Not used in game.
array<SDRC_Qrf> qrfs : The list of different QRF waves.
```

## SDRC_Qrf
See: [AI](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_COMMON.md#SDRC_MissionConfigAi)

```
int qrfIdx : Unique index for the QRF. 
string comment : Generic comment to describe the mission. Not used in game.
string info : Details for the hint shown for players
array<int> distance : Spawn distance min/max for QRF.
string vehicle : Vehicle resourcename for QRF. 
int speed : Vehicle cruising speed
array<SDRC_EMissionDifficulty> difficulty : Difficulty for QRF
int xp : (WIP, does nothing) Experience or other reward given
SDRC_MissionConfigAi ai : See AI parameters
array<string> modList : (WIP, does nothing) List of mods needed for this mission.
```
