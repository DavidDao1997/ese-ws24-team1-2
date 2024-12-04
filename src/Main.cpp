#include <iostream>
// #include "festoheader/Decoder.h"
#include "Dispatcher/header/Dispatcher.h"
// #include "festoheader/ActuatorController.h"
#include <string>

int main() {
	std::string dispatcherChannel = "dispatcher";
	Dispatcher dispatcher(dispatcherChannel);
	// dispatcher->addSubSbrber(int32_t coid, uint_8[])
	
	// string decoderChannel = "decoder";
	// Decoder decoder(decoderChannel);

	// string actuatorControllerChannel = "actuatorController";
	// ActuatorController actuatorController(actuatorControllerChannel);
	// dispatcher::addSubscriber(
	// 	actuatorController::getChannelId,
	// 	actuatorController::pulses,
	// 	ActuatorController::numberOfPulses
	// );
	// construct actuatorControllerMaster
	// construct decoder
	// construct ... (eg FSMs, Heartbeat, EStop)

	// start thread dispatcher
	// start thread ...

	// thread.join()...

	// maybe loop and dont die or dont die

	return 0;
}
