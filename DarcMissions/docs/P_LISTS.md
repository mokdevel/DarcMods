# Lists
The mod is capable of creating automatic lists to be used with missions. You can also define your own sets by following the same notation as in the example. Do not reuse the same ``id`` as is already available as a ``lootListName``, ``enemyListName`` or ``vehicleListName``.

Example: [dc_lootList.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_LootList.json) , [dc_enemyList.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_enemyList.json) , [dc_vehicleList.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_vehicleList.json)

By default all mods are searched, but you can define a limited selection of mods to search in. The typical notation is ``"$ModName:"``. A properly created mod follows the same structure and keeps prefabs in the right places. 
```
int jsonVersion : See General parameters
string author : See General parameters
array<string> modList : The mods to search for loot. If left empty, all mods enabled will be searched.
  Example:
    "modList": [],
    "modList": ["$ArmaReforger:", "$WCS_Armaments:", "$M110MarksmanRifle:"],
array<SDRC_List> lists : The defined lists. See details at he bottom of the page.
array<ref SDRC_Aka> akas : The defined 'akas'. See details at he bottom of the page.
```

## SDRC_List
The functionality goes through the modDir and gets every file with the ending of ```.et```. The list is first filtered with ```include``` words and then filtered with ```exclude```words. Note that any item you may have defined manually goes through the same filtering and may be removed if the ```exclude``` word matches.
```
string id : The name given for the listist. This is the name you use for adding loot or selecting enemies. 
array<string> modDir : Prefab directories inside mod. All mods will be searched for these directories.
array<string> include : Items having these words are included in the lootList.
array<string> exclude : Items with these words will be removed from the lootList.
array<ResourceName> items : The list of items. This is autofilled, but you can pre-define items if needed.
```

## SDRC_Aka
Also known as - other names used for example for factions. In some cases/mods the prefab name don't have the faction name included. The first item in the array is the *faction* and the rest of the names are the name used for filtering. Note that case matters (US != us).
```
array<string> names : The faction name and the filtering names in this order.
```

### Example
This is the case for example with RHS. RHS uses the faction name ``RHS_USAF`` but the naming convention with AI prefabs uses ``USAF_USMC``. With the definition, the groups and characters are searched with the 'also known as' name.
```
"akas": [
  {
    "names": [
      "RHS_USAF",
      "USAF_USMC"
    ]
  }
```

This is the case for example with Tactical Flava. The characters/groups are of faction ``USSR``, but have a naming ``TF_RF``. With the definition below, enemies from USSR faction will be collected from enemies with USSR and TF_RF in their name. The second aka is the same for ``US``.
```
"akas": [
  {
    "names": [
      "USSR",
      "USSR", "TF_RF"
    ]
  },
  {
    "names": [
      "US",
      "US", "TF_US"
    ]
  }
```

# Loot lists
## Loot list names (id)
Currently available names are below. For a complete list, please check [SDRC_LootListConfig.c](https://github.com/mokdevel/DarcMods/blob/main/DarcCore/scripts/Game/Conf/SDRC_LootListConfig.c)
```
WEAPON_RIFLE : A random (assault) rifle from any faction.
WEAPON_RIFLE_BIG : A random "large rifle" from any faction. These for example SVDs and bolt action rifles
WEAPON_HANDGUN : A random handgun from any faction.
WEAPON_LAUNCHER : A random launcher from any faction.
WEAPON_GRENADE : Grenades available
WEAPON_SHOTGUN : Shotguns (use e.g. Big Chungus mods)
WEAPON_MG : Machine guns (use e.g. Big Chungus mods)
ITEM_MEDICAL : Medical items
ITEM_GENERAL : General items
UTIL_ATTACHMENT : Various attachments excluding optics
UTIL_OPTIC : Optic attachments
UTIL_MAGAZINE : Adds a random magazine
UTIL_AMMO : Adds a random rocket, shell, flare, ..
GEAR_HEADGEAR : Head Gear including helmets, gas masks
GEAR_VEST : Vests
GEAR_HANDWEAR : Gloves etc
GEAR_UNIFORM : Jackets, pants
GEAR_BAG : Jackets, pants
CLOTHING_HEADGEAR : Head Gear including hats
CLOTHING_UNIFORM : Jackets, pants, shirts, 

//Freedom Fighters specific items
FF_ITEMS_LOW,			//Low tier loot in FF
FF_ITEMS_MID,			//Mids tier loot in FF
FF_ITEMS_HIGH,			//High tier loot in FF
```

# Enemy lists
## Enemy list names (id)
Currently available names are below. For a complete list, please check [SDRC_EnemyListConfig.c](https://github.com/mokdevel/DarcMods/blob/main/DarcCore/scripts/Game/Conf/SDRC_EnemyListConfig.c)
```
//Groups
G_LIGHT : Small arms units like rifle men.
G_HEAVY : Bigger guns for example machine guns
G_SNIPER : Units with rifles and typically with scopes
G_LAUNCHER : Units with launchers
G_ADMIN : Officers and similar higher ranking units
G_MEDICAL : Medical units
G_RECON : Units defined as recon units
G_SPECIAL : Special Forces units.
G_SMALL	: Small groups with two units. Mixed arms.

//Characters
C_RIFLEMAN : Basic rifle men
C_HEAVY : Units with bigger guns for example large machine guns
C_RECON : Units defined as recon units
C_OFFICER : Commander units
C_CREW : Pilots or other crew units
C_SNIPER : Units with sniper gear e.g. larger rifles
C_LAUNCHER : Units with launchers or other types of explosives
C_MEDIC : Units with medical gear
C_SPECIAL : Special forces 
C_RANDOMIZED : This is a collection of C_RIFLEMAN + C_HEAVY + C_RECON. This is generated runtime.

//Zombies and creatures (untested!)
G_ZOMBIE_SMALL
G_ZOMBIE_MEDIUM
G_ZOMBIE_LARGE
C_ZOMBIE
C_DEMON
C_DEMON_BOSS
```

# Vehicle lists
Vehicle lists are automatically created lists of different types of vehicles with their faction affiliation. This makes it possible to use ``id`` instead of a prefab name to define the mission vehicle. When selecting a vehicle from the generated lists, the vehicle is checked for the faction so that right prefab is selected. In the case where a faction has no vehicles defined, a random one is picked from the list.

## Vehicle list names (id)
Currently available names are below. For a complete list, please check [SDRC_VehicleListConfig.c](https://github.com/mokdevel/DarcMods/blob/main/DarcCore/scripts/Game/Conf/SDRC_VehicleListConfig.c). 

```
VEHICLE_WHEELED_ALL : All vehicles found.
VEHICLE_WHEELED_MILITARY_ALL : All non-CIV faction vehicles.
VEHICLE_WHEELED_CIVILIAN_ALL : All CIV faction vehicles.
VEHICLE_WHEELED_ARMED : All vehicles with weapons. This includes cars and armor.
VEHICLE_WHEELED_UNARMED : All vehicles without weapons.
VEHICLE_WHEELED_ARMOR : All armored vehicles.
VEHICLE_WHEELED_CIVILIAN_TRUCK : All CIV faction trucks. Vehicle type is any type of truck.
VEHICLE_WHEELED_MILITARY_TRUCK : All non-CIV faction trucks. Vehicle type is any type of truck.
VEHICLE_WHEELED_CIVILIAN_CAR : All CIV faction cars. Vehicle type is CAR.
VEHICLE_WHEELED_MILITARY_CAR : All non-CIV faction cars. Vehicle type is CAR.

VEHICLE_HELICOPTER_ALL : All helicopters found.
VEHICLE_CHOPPER_ALL : All choppers found. These have the SDRC_ChopperComp component.
VEHICLE_CHOPPER_TRANSPORT : All choppers found with 'transport' in their name. These have the SDRC_ChopperComp component.
VEHICLE_CHOPPER_ARMED : All choppers found with '_gunship' or '_armed' in their name. These have the SDRC_ChopperComp component.
```

The filtering is done with keywords. 
* ``WHEELED`` : The vehicle has VehicleWheeledSimulation component.
* ``MILITARY`` : Vehicle does not have trait FACTION_CIV.
* ``CIVILIAN`` : Vehicle has trait FACTION_CIV.
* ``ARMED`` : Vehicle has trait TRAIT_ARMED.
* ``UNARMED`` : Vehicle does not have trait TRAIT_ARMED.
* ``CARD`` : Vehicle is of type CAR
* ``TRUCK`` : Vehicle is of type TRUCK, COMM_TRUCK, FUEL_TRUCK, SUPPLY_TRUCK 
* ``HELICOPTER`` : The vehicle has VehicleHelicopterSimulation component.
* ``CHOPPER`` : The vehicle has SDRC_ChopperComp component.

# Examples
## Example lootList
The below example with the name ```WEAPON_RIFLE``` will search three mods for items matching the include and exclude filters. The full path for the search is ```$Modname:Prefabs/Weapons/Rifles``` where the ``$Modname`` parameter is picked from the modList. Internally we're searching for all files ending in ```.et```. Initally all items will be listed. 

The .json configuration as an example:
```
"jsonVersion": 1,
"author": "darc",
"modList": [
  "$ArmaReforger:",
  "$WCS_Armaments:",
  "$M110MarksmanRifle:"
  ],
"lists": [
  {
	"id": "WEAPON_RIFLE",
	"modDir": [
      "Prefabs/Weapons/Rifles"
	],
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

## Example enemyList
The below example with the name ```G_LIGHT``` will search all available mods for items matching the include and exclude filters. The full path for the search is ```$Modname:Prefabs/Groups``` where the ``$Modname`` parameter is picked from the modList. internally we're searching for all files ending in ```.et```. Initally all items will be listed. All items matching the ```include``` filter will be included. The ```exclude``` filter picks out anything with "_Base", "_NotSpawned", .. from the list.

The .json configuration as an example:

```
{
"jsonVersion": 1,
"author": "darc",
"modList": [],
"lists": [
  {
    "id": "G_LIGHT",
    "modDir": [
    "Prefabs/Groups"
    ],
    "include": [
      "LightFire",
      "FireTeam",
      "FireGroup",
      "RifleSquad",
      "SentryTeam"
    ],
    "exclude": [
      "_Base",
      "_NotSpawned",
      "_Remnants",
      "_Random"
    ],
    "items": []
  }
  ]
}
```