## Loot parameters
Loot is the reward of a mission. It is usually in a container (vehicle, box, ..) that is spawned in to the mission. The loot box is (in most cases) empty from the start and *only* after mission success, it's filled. There might a small delay and this is by design. The idea is that you should not be able to steal loot without compeliting mission. 

The box and crate prefabs are special for the mod. If you want to define additional ones, please see [loot box](#loot-box) and [howto](#define-a-new-loot-box).

### SCR_DC_Loot
```
IEntity box : (set as null) The container where the items are put. This can be a box, vehicle or similar. Do not specify anything here as this will be autofilled. For concoy mission this will be the vehicle, for occupation this will be the first item listed in the structure. 
float itemChance : (0-1, equals to percentage) The chance of each item appearing in the box. 
array<string> items : List of items. This can be prefabs or an lootList.
```

### Example
The below example shows the different ways you can define the loot. 
- "box": null : Leave as null. For convoys this will be automatically assigned to the vehicle. For missions with structures, the first structue is considered as the loot target.
- There is a 75% chance for each of the items to appear in the box. For a 100% chance, chnage the value to 1.0.
- Items will include a saline bag and a map. In addition a random rifle and a random general item is added. See [lootList]
- If you have defined prefabs that are not available (for example, due to a missing mod), these items will not be spawned. No items will be replacing them.
(https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_LISTS.md#loot-lists)
```
"loot": {
  "box": null,
  "itemChance": 0.75,
  "items": [
    "{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
    "{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
    "WEAPON_RIFLE",
    "ITEM_GENERAL"
  ]
}
```

### Loot box
The loot box (vehicle, box, ..) needs to have certain components available for them to work. Please see [this](#define-a-new-loot-box).
- Vehicles : Typically works.
- Box : The default arsenal boxes are not supported and you will need to use special ones. Currently supported values: 
```
"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et"
"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et"
"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et"
"{14B16D7580478D1A}Prefabs/Props/Civilian/LootSuitcase_01.et"
"{D15A294D5138ADFF}Prefabs/Props/Military/AmmoBoxes/US/LootEquipmentBoxWooden_Equipment_01_US.et"
"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et"
"{DBC8E6A4DD948C96}Prefabs/Props/Military/SupplyBox/SupplyPortableContainers/SupplyPortableContainer_01/LootSupplyPortableContainers_01_large_item.et"
"{973BB83B14B97689}PrefabsEditable/Auto/Props/Civilian/LootE_SackUniversal_01.et"
```

### Define a new loot box
- Duplicate the container prefab to DarcMissions. In theory you could use anything. In to the name, add "Loot" in the beginning.
- Edit prefab and add components 
  - ```UniversalInventoryStorageComponent```
  - ```ScriptedInventoryStorageManagerComponent```
  - ```ActionsManagerComponent```
  - ```SCR_ArsenalComponent``` : without this your box will allow for example vehicles to be inserted
- Configure as in the image
<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/lootboxsettings.png" width=30% height=30%>
