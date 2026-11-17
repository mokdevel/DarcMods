# Version history

## Next release (unreleased)

..empty..

## 20260829
Fixes:
* Sometimes helicopters were found floating with their engines off in the air.

## 20260812
Fixes:
* Disable streaming and persistence for choppers.

## 20260809
Main Feature:
* Support for friendly choppers.
  * Known issue: If you spawn a friendly chopper too far away from player, FF will remove the chopper. Same happens if they fly too far away. Something to fix but not for this release.
  * Known issue: There is an error ``The object ___ has no RplComponent. This will not be seen by players.`` which I'm investigating. Reason unknown and happens only with FF.

## 20260731
Fixes:
* Mod waits for gamemode to start before starting to run. This could have side effects.
* Configuration file load issues.
* dc_compatFFConfigChopper.json was created with test settings. Recommendation is to delete the file and receive an updated one at startup.
* Default chance for choppers arriving to battle is 30% + progression%.

## 20260421
First version