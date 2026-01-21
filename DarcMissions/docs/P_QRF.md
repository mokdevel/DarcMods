# QRF - Quick Reaction Force

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
