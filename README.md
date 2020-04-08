# air-assist
Arduino code to run the Air Assist open-source ventilator.

More info: https://www.vent.guide/
Code repo: https://github.com/dhempy/air-assist

## User Inputs:

### "Respiratory Cycle Time" in seconds.
 - Defaults to 3.0 seconds.
 - Range of 2.0 - 6.0 seconds, precision of 0.1 seconds.
 - Also display Breath/minute (e.g. 12, 20)

### "I:E" ratio:
 - Defaults to 1:2. e.g. twice as long in expiration vs inspiration.
 - Range of [1:1.5 - 1:3.0], with a precision of 0.1.

## Outputs:

 - Computes inspiration time and expiration time in ms,
   based on cycle_time and ie_ratio.
 - Drives solenoids based on those times.
