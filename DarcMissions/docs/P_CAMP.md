# Camp functionality
This is a class that is used by multiple missions. It provides a similar structure of configuration for them and a mission may extend it with mission specific topics. 

See [Common](./P_COMMON.md) , [Loot](./P_LOOT.md) , [Structures](./P_STRUCTURE.md) , [Camp](./P_CAMP.md)

### SDRC_Camp
```
SDRC_MissionConfigGeneral general : See General parameters
SDRC_MissionConfigAi ai : See General parameters
SDRC_MissionConfigSecondWave secondWave : See General parameters
SDRC_Loot loot : (optional) Loot found in the mission. 
array<ref SDRC_Structure> campItems : (optional) The structure list for the camp.
```
