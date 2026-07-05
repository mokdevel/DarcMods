# Commands
This documentation is for modders who want to command the choppers from script of via prefab. From script, use ``AddDestination()``.

!!! THIS IS WIP !!!

## AddDestination
The function takes three parameters. Depending on the command used, the result may differ. 

* ``type`` : This is the command given to the chopper. The possible options are below.
* ``destination`` : The destination position where to go. 
* ``value`` : Additional value that you can provide with the specific command. Usually this is time to perform the action.
* ``index`` : This is internal and should not be used modders.

### FIFO
The commands are put in a First-In-First-Out array from where they're picked once the previous command has executed. The exception is ``WP_FLY`` which will be performed in a serie. 

## Tested (and should work)
* ``WP_FLY`` : Fly, normal flight pattern
* ``WP_FLY_AWAY`` : Fly away as a last move.
* ``WP_FLY_AWAY_IMMEDIATELY`` : Fly away immediately removing any added destinations. This will interrupt any existing flight plans.
* ``WP_FLY_IMMEDIATELY`` : Fly, but remove all already added destinations. This will interrupt any existing flight plans.
* ``WP_PATROL`` : Patrol around an area.
* ``WP_PATROL_ONCE`` : Do one patrol round around an area.
* ``WP_LAND`` : Order chopper to do landing sequence
* ``WP_WAIT`` : Wait, before moving to next state
* ``WP_WAIT_GETOUT`` : Same as WP_WAIT, but time set is dependent on crew count
* ``WP_RAISE``* : Raises the helicopter from the current position to given position while moving forward increasing speed.
* ``WP_HOVER``* : Hover at a certain altitude
* ``WP_HOVER_UP``* : Hover the helicopter up from the current height.
* ``WP_GET_OUT`` : Order AI to get out from the chopper
* ``WP_BRAKE``* : Brakes the helicopter speed to stand still.
* ``WP_END`` : Stop running SDRC_ChopperComp and let AR handle everything
* ``WP_DESPAWN`` : Despawn the helicopter. KNOWN: AI is not despawned so .. lot's of fun. :-)
* ``WP_STOP_ENGINE`` : Helicopter engine is stopped. Does the action and goes to WAIT state. 
* ``WP_ATTACK`` : Sets attack position to shoot at. This is performed once.
* ``WP_SEARCH_DESTROY``* : Search for enemy by patroling an area. If enemy is found, attack the location.

### WP_FLY
_TBD_

### WP_FLY_AWAY
_TBD_

### WP_FLY_AWAY_IMMEDIATELY
_TBD_

### WP_FLY_IMMEDIATELY
_TBD_

### WP_PATROL
_TBD_

### WP_PATROL_ONCE
_TBD_

### WP_LAND
_TBD_

### WP_WAIT
_TBD_

### WP_WAIT_GETOUT
_TBD_

### WP_RAISE
Raises the helicopter from the current position to given position while moving forward increasing speed.
* ``destination[0]`` : The relative distance to move forward.
  * If 0, defaults to 200
* ``destination[1]`` : The relative distance from ground to raise to. 
  * If -1, set the height to (minimum fly height + 5).
* ``destination[2]`` : Not used
* ``value`` : Not used

### WP_HOVER
Hover the helicopter at current height. You should always do a ``WP_BRAKE`` before this.
* ``destination`` : Not used.
* ``value`` : Time to stay hovering

### WP_HOVER_UP
Hover the helicopter up from the current height.
* ``destination[0]`` : Not used.
* ``destination[1]`` : The relative distance to raise up.
* ``destination[2]`` : Not used
* ``value`` : Time to use for upwards movement

### WP_GET_OUT
_TBD_

### WP_BRAKE
Brakes the helicopter speed to stand still.
* ``destination[0][2]`` : The position XZ to stop at.
* ``destination[1]`` : The relative height from ground to stop at. This is not exact, but rather a wish. 
  * If 0, normal flight pattern height is set.
* ``value`` : Distance to start braking. 
  * If 0, defaults to 200.

### WP_END
_TBD_

### WP_DESPAWN
_TBD_

### WP_STOP_ENGINE
_TBD_

### WP_ATTACK
_TBD_

### WP_SEARCH_DESTROY
This will set the behaviour of the chopper to SEARCH_AND_DESTROY_BEHAVIOUR for a given time before returning back to NORMAL_BEHAVIOUR. The chopper will arrive at the destination and start to patrol the area. Enemy is searched with a cycle of 2 seconds. If an enemy is found, current flight is interrupted, and a new flight pattern to attack the enemy is created. Chopper will stay in attack mode for 60 seconds and then resume to patroling. 
* ``destination`` : The position to keep an eye on. The helicopter will patrol around this area with a circle of 400m.  
* ``value`` : The time to be in SEARCH_AND_DESTROY_BEHAVIOUR. Once time has passed, we return to NORMAL_BEHAVIOUR.
  * GM default: 600

## Macro commands
Assigning single macro command will perform a set of single commands. 

* WP_M_LAND_TROOPS : Drop of troops to exact position, wait and leave
  * WP_LAND, WP_GET_OUT, WP_WAIT, WP_HOVER_UP, WP_RAISE
* WP_M_LAND_TO_FREE_SPOT : Drop of troops, but search for empty spot. Stop engine.
  * Search for safe spot, WP_LAND, WP_GET_OUT, WP_STOP_ENGINE, WP_END
* WP_M_EVAC_TROOPS : Drop troops to a safe spot and stop engine. This action is automatically done if helicopter is heavily damaged.
  * Search for safe spot, WP_LAND, WP_GET_OUT, WP_STOP_ENGINE, WP_END
* WP_M_ATTACK : For now, same as WP_M_SUPPRESSIVE. Will be changed!
* WP_M_SUPPRESSIVE : Bomb run on the area without the need to have an enemy
  * WP_ATTACK, WP_FLY, WP_FLY - randomly multiple times

## Internal commands
Not to be used for normal use.

* WP_UNDEFINED : Do not use
* WP_M_RESET : Reset destinations. Cut the current flight planned and pick the next destination in the list.
* WP_M_CUT : Cut the current flight planned and pick the next destination in the list.
* WP_M_TESTING : Just for testing

# Behaviour
The chopper can be given a behaviour. You should not use the SetBehaviour() API from your mod unless you really know what you're doing. 

* UNKNOWN_BEHAVIOUR : Well, unknown
* NORMAL_BEHAVIOUR : Normal flight pattern. In this behaviour we select the next destination from the list if any is specified. If not, we create a random flight destination.
* SEARCH_AND_DESTROY_BEHAVIOUR : The AI is alert and will do search and destroy if an enemy is found
* EVAC_BEHAVIOUR : The chopper is damaged and we're looking for a landing state.