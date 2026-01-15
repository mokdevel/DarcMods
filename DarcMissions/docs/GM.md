# GM functionality
Some mod functionality is available for GM use. In order for this to work correctly, make sure you have ``admin`` permission in your server conf.

```
	"rcon": {
		"permission": "admin",
	},
```

## Drop entities on the map
From the entity browser, you can drop..
- Missions to the world. They will be started after a while. For details on how mission will be spawned, is described in the different mission type documentation.
- NonValidAreas to the world. They will be activated after a while and the icon disappears from the world.

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/s_gm_entitylist.jpg" width=30% height=30%>

## Map information
Show information by clicking on a symbol
- Missions that are in ACTIVE state. Missions that are still spawning are not shown.
- NonValidAreas.

## Map functionality

### Mission markers
- Click on an icon, to get some intel about the mission
- Keep cursor on top of the icon and press delete-key. The mission will end and despawn.

### Non Valid Areas
These are shown as red circles on the map.
- Click on an icon, to get some intel about the area.
- Edit the size of a NonValidArea in GM view. Hover on top of the red circle on map. Hold Left Control and use scroll wheel to change size. 
  - Known issue: You need to re-press Left Control if you change the mouse wheel direction. 
- You can delete a NonValidArea in GM view. Hover on top of the red circle and press DELETE on keyboard.
- Once you leave the map view, the file is saved on the server folder.

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/s_gm_nva01.jpg" width=30% height=30%>
