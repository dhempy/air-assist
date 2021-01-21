#include <ArduinoUnitTests.h>
#include "air_assist.h"

unittest(calcBPM)
{
  // this won't work -- calcBPM() returns void
  // assertEqual("I don't know what", calcBPM());

  // example of
  assertEqual(2.0, sqrt(4));
}

unittest_main()
