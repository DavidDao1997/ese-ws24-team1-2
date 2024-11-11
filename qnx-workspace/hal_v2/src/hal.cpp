#include <iostream>
using namespace std;


#include <stdint.h>
#include "sys/mman.h"
#include "sys/neutrino.h"
#include "hw/inout.h"



#include "header/I_Actuators.h"
#include "header/Actuators_Wrapper.h"


int main() {

	Actuators_Wrapper* actuators_test = new Actuators_Wrapper();

	if (!actuators_test->init()) {
		delete actuators_test;
	} else {

		while(1){
			actuators_test->yellowLampLightOn();

			uint8_t SMbit = 0;

			SMbit = actuators_test->readSortingModule();
			char buffer[50];
			sprintf(buffer, "readBit %d", SMbit);

			cout << buffer << endl;

			if (SMbit == SM_TYPE_EJECTOR){
				actuators_test->closeSortingModule();
				usleep(1000*500);
				actuators_test->openSortingModule();
				usleep(1000*500);
			} else {
				actuators_test->openSortingModule();
				usleep(1000*500);
				actuators_test->closeSortingModule();
				usleep(1000*500);
			}

			usleep(1000*1000);

			actuators_test->yellowLampLightOff();
			actuators_test->motorStop();
			actuators_test->yellowLampLightOff();
			actuators_test->redLampLightOff();
			actuators_test->greenLampLightOff();
			usleep(1000*500);
			/*
			cout << "new" << endl;
			actuators_test->redLampLightOn();
			usleep(1000*500);
			//cout << "red light on" << endl;
			actuators_test->yellowLampLightOn();
			usleep(1000*500);
			actuators_test->greenLampLightOn();
			usleep(1000*500);
			actuators_test->yellowLampLightOff();
			usleep(1000*500);
			actuators_test->greenLampLightOn();
			usleep(1000*500);
			*/

			/*
			//cout << "motor right" << endl;
			actuators_test->runRight();
			usleep(1000*500);
			//cout << "motor stop" << endl;
			actuators_test->runLeft();
			usleep(1000*500);
			actuators_test->motorStop();
			usleep(1000*500);
			actuators_test->runSlow();
			actuators_test->runLeft();
			usleep(1000*500);
			actuators_test->runFast();
			*/



		}

		delete actuators_test;
	}
	
	return 0;

}
