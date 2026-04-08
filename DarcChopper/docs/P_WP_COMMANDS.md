# Commands
This documentation is for modders who want to command the choppers from script of via prefab. From script, use ``AddDestination()``.

!!! THIS IS WIP !!!

## Tested (and should work)
* WP_FLY : Fly, normal flight pattern
* WP_FLY_IMMEDIATELY : Fly, but remove all already added destinations. This will interrupt any existing flight plans.
* WP_FLY_AWAY : Fly away as a last move.
* WP_FLY_AWAY_IMMEDIATELY : Fly away immediately removing any added destinations. This will interrupt any existing flight plans.
* WP_STOP_ENGINE : Helicopter engine is stopped. Does the action and goes to WAIT state. 
* WP_ATTACK : Sets attack position to shoot at. This is performed once.
* WP_SEARCH_DESTROY : Search for enemy by patroling an area. If enemy is found, attack the location.

## Macro commands
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
* WP_UNDEFINED : Do not use
* WP_PATROL : Patrol around an area
* WP_LAND : Order chopper to do landing sequence
* WP_WAIT : Wait, before moving to next state
* WP_RAISE : Raise from ground to lowest flight height
* WP_HOVER : Hover at a certain altitude
* WP_GET_OUT : Order AI to get out from the chopper
* WP_END : Do end actions
* WP_DESPAWN : Despawn the chopper
* WP_HOVER_UP : Does the action and goes to HOVER state
* WP_M_RESET : Reset destinations
* WP_M_TESTING : Just for testing