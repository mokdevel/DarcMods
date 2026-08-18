# Location parameters
A mission location position (see: [General parameters - pos](P_COMMON.md)) can be defined manually at certain coordinates or as "0 0 0". In the latter case, the mission position is chosen randomly from the map from locations defined in locationTypes. The different types are defined as [EMapDescriptorType](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/group__Map.html#ga18c4f596069370b50b7f842cf36d5686). 

## Location type
Location type is information found from the map itself. Map makers set a parameter called ``EMapDescriptorType`` to an object to describe what the specific location is. For example, for a city, an object would have the ``EMapDescriptorType`` defined with value ``MDT_NAME_CITY``. With this information we know that this area is considered as a city. 

You 'overload' certain value by defining it multiple times. For example ```locationTypes = {59, 59, 59, 59, 60};``` will choose a CITY 80% of the time and only 20% time a village.

### Location caching
At startup, DarcCore will search for locations and buildings to include in the location cache. This is to avoid rescan every time a location is needed. 

### Special location type
Certain types are not available to find from the map so the location cache does additional searching. The special rules are found in the configuration as ``locationAkas`` and ``buildingAkas``. See: [SDRC_LocationAka](CORE.md#SDRC_LocationAka)

## EMapDescriptorType
Below are the typical values selected from [EMapDescriptorType](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/group__Map.html#ga18c4f596069370b50b7f842cf36d5686) . Some types are searched during the caching and have special rules to find them. These are marked as comments below.
```
 7 = MDT_FORESTTRIANGLE 
 8 = MDT_FORESTSQUARE
 9 = MDT_CALVARY 
10 = MDT_CHURCH 	//Special: Includes all buildings with 'Church', 'Mosque_' and 'Minaret' in their name
11 = MDT_CHAPEL 
..
14 = MDT_BUNKER
15 = MDT_FORTRESS

18 = MDT_VIEWPOINT
19 = MDT_TOWER
20 = MDT_VIEWTOWER
21 = MDT_WATERTOWER
22 = MDT_LIGHTHOUSE
..
25 = MDT_FUELSTATION
26 = MDT_HOSPITAL
..
32 = MDT_BUSSTATION
..
35 = MDT_CRANE
36 = MDT_TRANSFORMER
37 = MDT_TRANSMITTER
..
53 = MDT_POLICE		//Special: Includes all buildings with '_Police' in their name
..
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
72 = MDT_PARKING
..
75 = MDT_CONSTRUCTION_SITE
..
79 = MDT_BASE		//Special: Includes all 'military' location names
80 = MDT_PORT 		//Special: Includes all 'harbour' and 'harbor' location names
81 = MDT_AIRPORT	//Special: Includes all 'airport' location names
..
84 = MDT_RADIO
```

My humble wish for map makers is that please use the full range of ```EMapDescriptorTypes```. 
