## Structure parameters
Structures are entities (prefabs) of e.g., furniture. Typically you would spawn multiple structures to create for example a camp with chairs, a tent, fireplace etc. 

### SCR_DC_Structure
```
string m_Resource : Prefab name
vector m_Position : Position in the world.
vector m_Rotation : Rotation vector. Currently only Yrot is supported ["Xrot YRot ZRot"].
```

### Creating a camp
You can use WB to set the wanted objects. You can turn them to make the camp to look better. I suggest you to create an own layer which you can then view as a file and convert to properly looking json file. Locate the camp on flat surface for example on an airfield if using a world like Arland (or use some simple flat world). If you have a loot defined and it needs to appear in a container like a box, ALWAYS define it as the first structure.

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/camp01.png" width=30% height=30%>

When the camp is spawned, the items will be set around a center location (origo) in the spawner script. The origo will be the position on the map and items will spawn around it.

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/camp02.png" width=30% height=30%>

### Example
Below is a an example camp with a box for loot, tent, fireplace and chair. The chair is rotated. The first item ```LootCrateWooden_01_blue``` is the container for loot. 
```
"campItems": [
  {
    "m_Resource": "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
    "m_Position": [
      1032.0379638671876,
      39.0,
      2478.923095703125
    ],
    "m_Rotation": [
      0.0,
      0.0,
      0.0
    ]
  },
  {
    "m_Resource": "{39C308BBB5945B85}Prefabs/Props/Military/Furniture/ChairMilitary_US_02.et",
    "m_Position": [
      1028.550048828125,
      39.0,
      2478.159912109375
    ],
    "m_Rotation": [
      0.0,
      119.33399963378906,
      0.0
    ]
  },
  {
    "m_Resource": "{D9842C11742C00CF}Prefabs/Props/Civilian/Fireplace_01.et",
    "m_Position": [
      1029.9000244140626,
      39.0,
      2477.43994140625
    ],
    "m_Rotation": [
      0.0,
      0.0,
      0.0
    ]
  },
  {
    "m_Resource": "{0511E95F422061BB}Prefabs/Props/Recreation/Camp/TentSmall_02/TentSmall_02_blue.et",
    "m_Position": [
      1029.9739990234376,
      39.0,
      2480.114013671875
    ],
    "m_Rotation": [
      0.0,
      0.0,
      0.0
    ]
  }
]
```