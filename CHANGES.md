Release 1.02 (11 June 2021) -- compared to 1.01
----------------------------------------------
* Fix bug about profiles flags set to ON which were lost (i.e. set to OFF) for the box shown after a computation run (never the first run)
* Fix the rejection of values > 999.999, from Ui, for micmac step values and spatial resolution
* Improve micmac step values and spatial resolution numeric format compliance check from .jstackprof
* Reject thresholdRejection values from .jstackprof below 0 or greater than 999.999 and add numeric format compliance check
* Fix the display of valid reloaded threshold value and button state if an invalid value was typed and Cancel button used

Release 1.01 (30 May 2021) -- compared to 1.0
--------------------------------------------------
* Fix the behavior after usage of 'set pixel conversion factors'
  The application now considers correctly when no change was made about it, instead of considering as always changed

Release 1.0 (28 May 2021) 
--------------------------------------------------
Initial commit. No Tag.