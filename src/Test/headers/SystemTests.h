#ifndef TESTS_SYSTEMTESTS_H_
#define TESTS_SYSTEMTESTS_H_

#include <iostream>
#include <stdint.h>

#include "../../Actuatorcontroller/headers/ActuatorController.h"
#include "../../Dispatcher/headers/Dispatcher.h"
// #include "../../Logik/FSM/headers/FSMController.h"
#include "../../Util/headers/Util.h"

class SystemTests {
  public:
    SystemTests();
    ~SystemTests();

    int8_t Run();

  private:
    bool running;
    int8_t setup();
    int8_t cleanup();

    bool testEstopStopsMotorAndTurnsOnLamps();
    bool testEnteringServiceModeMakesGreenLampBlink();
    bool testInitialPukTurnsOnMotor();
};

#endif /* TESTS_SYSTEMTESTS_H_ */