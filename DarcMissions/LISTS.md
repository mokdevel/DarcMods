# Lists
The mod is capable of creating automatic lists to be used with missions. You can also define your own sets by following the same notation as in the example. Do not reuse the same ids as is already available as a ```lootListName```.

## SCR_DC_List
The functionality goes through the modDir and gets every file with the ending of ```.et```. The list is first filtered with ```include``` words and then filtered with ```exclude```words. Note that any item you may have defined manually goes through the same filtering and may be removed if the ```exclude``` word matches.
```
string id : The name given for the listist. This is the name you use for adding loot or selecting enemies. 
string modDir : Prefab directory inside mod.
array<string> include : Items having these words are included in the lootList.
array<string> exclude : Items with these words will be removed from the lootList.
array<ResourceName> items : The list of items. This is autofilled, but you can pre-define items if needed.
```

# Loot lists
Example: [dc_lootList.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_LootList.json)

This supports additional mods that you can define in the modList parameter. The typical notation is ```"$ModName:PreFabs"```. A properly created mod follows the same structure and keeps prefabs in the right places. 
```
int version : See General parameters
string author : See General parameters
array<string> modList : The mods to search for loot. If left empty, all mods enabled will be searched.
array<SCR_DC_LootList> lootLists : The defined lootlists
```
## Loot list names (id)
Currently available loot names are below. For a complete list, please check [SCR_DC_LootListJsonApi.c](https://github.com/mokdevel/DarcMods/blob/main/DarcCore/scripts/Game/Helpers/SCR_DC_LootListJsonApi.c)
```
WEAPON_RIFLE : A random rifle from any faction.
WEAPON_HANDGUN : A random handgun from any faction.
WEAPON_LAUNCHER : A random launcher from any faction.
WEAPON_GRENADE : Grenades available
WEAPON_ATTACHMENT : Various attachments excluding optics
WEAPON_OPTICS : Optic attachments
ITEM_MEDICAL : Medical items
ITEM_GENERAL : General items
```

## Example for lootList
The below example with the name ```WEAPON_RIFLE``` will search three mods for items matching the include and exclude filters. The intial search path is ```$Modname:Prefabs```. Under the Prefabs dir we use the dir ```/Weapons/Rifles``` for the more detailed search. The full path for the search is ```$Modname:Prefabs/Weapons/Rifles``` and internally we're searching for all files ending in ```.et```. Initally all items will be listed. 

The .json configuration as an example:
```
"version": 1,
"author": "darc",
"modList": [
  "$ArmaReforger:Prefabs",
  "$WCS_Armaments:Prefabs",
  "$M110MarksmanRifle:Prefabs"
  ],
"lootLists": [
  {
    "lootListName": "WEAPON_RIFLE",
    "lootListType": 0,
    "modDir": "/Weapons/Rifles",
    "include": [
      "Rifle"
    ],
    "exclude": [
      "_Base",
      "SVD",
      "{29DD901C0664FC61}Prefabs/Weapons/Rifles/M16/Variants/Rifle_M16A2_suppressor_4x20_OliveGreen_Sand_Stripes.et"
    ],
    "itemList": [
      "{E8F00BF730225B00}Prefabs/Weapons/Grenades/Grenade_M67.et"
    ]
  }
  ]
}
```
All items under each mods ```/Weapons/Rifles``` will be collected initially. The list could be very long but below is a section.
```
"{C63227C0E70EA62E}Prefabs/Weapons/Rifles/M16/Rifle_M16A2_base.et
"{29DD901C0664FC61}Prefabs/Weapons/Rifles/M16/Variants/Rifle_M16A2_suppressor_4x20_OliveGreen_Sand_Stripes.et"
"{923D948AB0D57A50}Prefabs/Weapons/Rifles/AK74/Rifle_AK74_long_base.et"
"{B036BA26CCFE9E5D}Prefabs/Weapons/Rifles/M16/Variants/Rifle_M16A2_OliveGreen_Sand_Stripes.et"
"{FA0E25CE35EE945F}Prefabs/Weapons/Rifles/AKS74U/Rifle_AKS74UN.et"
"{8BA0D0DE316D1B44}Prefabs/Weapons/Rifles/AKS74U/Rifle_AKS74U_base.et"
"{6415B7923DE28C1B}Prefabs/Weapons/Rifles/SVD/Rifle_SVD_PSO.et"
```
The ```include``` filter ("Rifle") matches the list so everything is included. The ```exclude``` filter picks out anything with "_Base", the SVD's and the specific M16A2 with suppressor. A grenade was specified in the itemList so it is included. The resulting list will be: 
```
"{E8F00BF730225B00}Prefabs/Weapons/Grenades/Grenade_M67.et"
"{B036BA26CCFE9E5D}Prefabs/Weapons/Rifles/M16/Variants/Rifle_M16A2_OliveGreen_Sand_Stripes.et"
"{FA0E25CE35EE945F}Prefabs/Weapons/Rifles/AKS74U/Rifle_AKS74UN.et"
```

# Enemy lists
## Example for enemyList
