# QRF - Quick Reaction Force

## SDRC_MissionConfigQrf
The setup for QRF details for a sub-mission.

See: [Difficulty](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/MISSIONCONFIG.md#sdrc_missiondifficulty)
```
array<int> subIdx : subIdx from which to choose
  Example: "subIdx":[0,0,1,1,2] - QRF 0 and 1 will be chosen 40% chance and 2 with 20%
SDRC_EMissionSuccess activation : Which success activates the check for QRF
  SDRC_EMissionSuccess.WIN : Mission was won. Safe to use for all missions.
  SDRC_EMissionSuccess.LOSE : Mission was lost. Can be selectively be used with some types of missions.
  SDRC_EMissionSuccess.WIN_OR_LOSE : Mission either won or lost. Can be selectively be used with some types of missions.
float chance : The chance for the sub-mission to get a QRF. Note that difficulty will modify this value.
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
```
int qrfIdx : Unique index for the qrf. 
string comment : Generic comment to describe the mission. Not used in game.
string vehicle : Vehicle resourcename for QRF. 
string info : Details for the hint shown for players
array<SDRC_EMissionDifficulty> difficulty : Difficulty for qrf
int xp : Experience or other reward given
SDRC_MissionConfigAi ai : See AI parameters
array<string> modList : (WIP, does nothing) The mods to search for loot. If left empty, all mods enabled will be searched. 
```
