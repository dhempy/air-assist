#include <ArduinoUnitTests.h>
#include "../air_assist/air_assist.h"

unittest(calcBPM)
{
  assertEqual("I don't know what", calcBPM());
}

unittest_main()
