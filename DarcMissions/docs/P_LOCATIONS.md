## Location parameters
A mission location position (see: [General parameters - pos](P_COMMON.md)) can be defined manually at certain coordinates or as "0 0 0". In the latter case, the mission position is chosen randomly from the map from locations defined in locationTypes. The different types are defined as [EMapDescriptorType](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/group__Map.html#ga18c4f596069370b50b7f842cf36d5686). 

Note that you 'overload' certain value by defining it multiple times. For example ```locationTypes = {59, 59, 59, 59, 60};``` will choose a CITY 80% of the time and only 20% time a village.
```
array<int> locationTypes : Location types as defined by EMapDescriptorType.
```

### EMapDescriptorType
Below are the typical values selected from [EMapDescriptorType](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/group__Map.html#ga18c4f596069370b50b7f842cf36d5686) .
```
58 = MDT_NAME_GENERIC
59 = MDT_NAME_CITY
60 = MDT_NAME_VILLAGE
61 = MDT_NAME_TOWN
62 = MDT_NAME_SETTLEMENT
63 = MDT_NAME_HILL
64 = MDT_NAME_LOCAL
65 = MDT_NAME_ISLAND
..
70 = MDT_NAME_RIDGE
71 = MDT_NAME_VALLEY
```
My humble wish for map makers is that please use the full range of ```EMapDescriptorTypes```. 