# Commands
This documentation is for modders who want to command the choppers from script of via prefab. From script, use ``AddDestination()``.

## AddDestination
The function takes three parameters. Depending on the command used, the result may differ. 

* ``type`` : This is the command given to the chopper. The possible options are below.
* ``destination`` : The destination position where to go. 
* ``value`` : Additional value that you can provide with the specific command. Usually this is time to perform the action.
* ``index`` : This is internal and should not be used modders.

### FIFO
The commands are put in a First-In-First-Out array from where they're picked once the previous command has executed. The exception is ``WP_FLY`` which will be performed in a serie. 

List of available commands:
* ``WP_UNDEFINED`` : Undefined - not to be used
* ``WP_FLY`` : Fly, normal flight pattern
* ``WP_FLY_IMMEDIATELY`` : Fly, but remove all already added destinations. This will interrupt any existing flight plans.
* ``WP_FLY_AWAY`` : Fly away as a last move.
* ``WP_FLY_AWAY_IMMEDIATELY`` : Fly away immediately removing any added destinations. This will interrupt any existing flight plans.
* ``WP_PATROL`` : Patrol around an area.
* ``WP_PATROL_ONCE`` : Do one patrol round around an area.
* ``WP_M_LAND`` : Order chopper to do landing sequence. (WP_LAND is not to be used, deprecated!)
* ``WP_LAND_VERTICAL`` : Order chopper to do a vertical landing sequence
* ``WP_WAIT`` : Wait, before moving to next state
* ``WP_WAIT_GETOUT`` : Same as WP_WAIT, but time set is dependent on crew count
* ``WP_RAISE`` : Raises the helicopter from the current position to given position while moving forward increasing speed.
* ``WP_HOVER`` : Hover at a certain altitude
* ``WP_HOVER_UP`` : Hover the helicopter up from the current height.
* ``WP_HOVER_DOWN`` : Hover the helicopter down from the current height.
* ``WP_BRAKE`` : Brakes the helicopter speed to stand still.
* ``WP_END`` : Stop running SDRC_ChopperComp and let AR handle everything
* ``WP_DESPAWN`` : Despawn the helicopter. KNOWN: AI is not despawned so .. lot's of fun. :-)

* ``WP_GET_OUT`` : Order AI to get out from the chopper
* ``WP_STOP_ENGINE`` : Helicopter engine is stopped. Does the action and goes to WAIT state. 
* ``WP_ATTACK`` : Sets attack position to shoot at. This is performed once.
* ``WP_SEARCH_DESTROY`` : Search for enemy by patroling an area. If enemy is found, attack the location.

### WP_FLY
Fly, normal flight pattern. If you have multiple WP_FLY assigned, these will be processed in a serie to create a longer flight pattern.
* ``destination`` : Position to fly to.
* ``value`` : Not used.

### WP_FLY_IMMEDIATELY
Interrupts and clears all existing flight destinations assigned and assigns a single destination to fly to immediately. 
* ``destination`` : Position to fly to.
* ``value`` : Not used.

### WP_FLY_AWAY
Assigns an "end" destination far away close to map border. This is command to tell a chopper that soon it's time to despawn.
* ``destination`` : Not used in normal cases. Will generate a position far away. If destination is set, will fly there.
* ``value`` : Not used.

### WP_FLY_AWAY_IMMEDIATELY
Interrupts and clears all existing flight destinations assigned and assigns a single "end" destination to fly to immediately. 
* ``destination`` : Not used in normal cases. Will generate a position far away. If destination is set, will fly there.
* ``value`` : Not used.

### WP_PATROL
Creates a flight pattern to circle around a position. This will do a random amount of circling around the position. If you want longer patroling, set multiple WP_PATROL after each other.
* ``destination`` : Position to circle around.
* ``value`` : The radius of the cirle.
  * If 0 or empty, defaults to internal chopper type parameter patrolRadius
  
### WP_PATROL_ONCE
Creates a flight pattern to circle around a position. This will do a single circle around the position. If you want longer patroling, use WP_PATROL.
* ``destination`` : Position to circle around.
* ``value`` : The radius of the cirle.
  * If 0 or empty, defaults to internal chopper type parameter patrolRadius

### WP_M_LAND
Land the helicopter to destination following a generated flight path. The chopper tries to land exactly at the destination spot, but it's not guaranteed.
* ``destination`` : Position to land to. 
* ``value`` : Not used.

NOTE: This was called WP_LAND earlier and is now deprecated. WP_LAND has been left for compatibility and will behave as WP_M_LAND.

### WP_LAND_VERTICAL
Land the helicopter to the spot where it is currently. You should use WP_BRAKE before using WP_LAND_VERTICAL to stop the flight.
* ``destination`` : Not used.
* ``value`` : Not used.

### WP_WAIT
Orders the helicopter to wait in its current state 
* ``destination`` : Not used.
* ``value`` : Time in seconds to wait until moving to next WP_command.

### WP_WAIT_GETOUT
Defines a time to wait for the crew to get out. This activates WP_WAIT but calculates the time depending on the crew count. This should be called after ``WP_GET_OUT`` to set the delay or you can use ``WP_WAIT`` to set it yourself.
* ``destination`` : Not used.
* ``value`` : Not used.

### WP_RAISE
Raises the helicopter from the current position to given position while moving forward increasing speed.
* ``destination[0]`` : The relative distance to move forward.
  * If 0 or empty, defaults to 200
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

### WP_HOVER_DOWN
Hover the helicopter down from the current height.
* ``destination[0]`` : Not used.
* ``destination[1]`` : The relative distance to raise up.
* ``destination[2]`` : Not used
* ``value`` : Time to use for upwards movement

### WP_GET_OUT
_TBD_

### WP_BRAKE
Brakes the helicopter speed to stand still.
* ``destination[0][2]`` : The position XZ to stop at.
* ``destination[1]`` : The height from ground to stop at. This is not exact, but rather a wish. 
  * If -999, the height will be set to helicopter current height.
* ``value`` : Distance to start braking. 
  * If -1 or empty, defaults to 200.

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

* ``WP_M_LAND`` : Land on the provided position
  * WP_BRAKE, WP_LAND_VERTICAL
* ``WP_M_LAND_TROOPS`` : Drop of troops to position, wait and leave
  * WP_LAND, WP_GET_OUT, WP_WAIT, WP_HOVER_UP, WP_RAISE
* ``WP_M_LAND_TO_FREE_SPOT`` : Drop of troops, but search for empty spot. Stop engine.
  * Search for safe spot, WP_LAND, WP_GET_OUT, WP_STOP_ENGINE, WP_END
* ``WP_M_EVAC_TROOPS`` : Drop troops to a safe spot and stop engine. This action is automatically done if helicopter is heavily damaged.
  * Search for safe spot, WP_LAND, WP_GET_OUT, WP_STOP_ENGINE, WP_END
* ``WP_M_ATTACK`` : For now, same as WP_M_SUPPRESSIVE. Will be changed!
* ``WP_M_SUPPRESSIVE`` : Bomb run on the area without the need to have an enemy
  * WP_ATTACK, WP_FLY, WP_FLY - randomly multiple times

## Internal commands
Not to be used for normal use.

* ``WP_UNDEFINED`` : Do not use
* ``WP_M_RESET`` : Reset destinations. Cut the current flight planned and pick the next destination in the list.
* ``WP_M_CUT`` : Cut the current flight planned and pick the next destination in the list.
* ``WP_M_TESTING`` : Just for testing

# Behaviour
The chopper can be given a behaviour. You should not use the SetBehaviour() API from your mod unless you really know what you're doing. 

* UNKNOWN_BEHAVIOUR : Well, unknown
* NORMAL_BEHAVIOUR : Normal flight pattern. In this behaviour we select the next destination from the list if any is specified. If not, we create a random flight destination.
* SEARCH_AND_DESTROY_BEHAVIOUR : The AI is alert and will do search and destroy if an enemy is found
* EVAC_BEHAVIOUR : The chopper is damaged and we're looking for a landing state.