#include "header/HeightSensorControl.h"

#include <thread>
#include <vector> // Für das Speichern der Werte in einem Array

// Deklaration von Variablen
// std::vector<int> heights;  // Array für Höhen
// std::vector<int> sampleCounts;  // Array für die Anzahl der Samples für jede Höhe

bool receivingRunning = false;
int bandHeight = 0;
bool firstValue = false;
int lastValue = -1;
int stableCount = 0;
int countSample = 0; // Zähler für Samples

// Constructor
HeightSensorControl::HeightSensorControl(const std::string channelName, const std::string dispatcherName, const uint8_t festoID) {
    hsControllerChannel = createNamedChannel(channelName);
    channelID = hsControllerChannel->chid;
    running = false;
    dispatcherConnectionID = name_open(dispatcherName.c_str(), 0);
    if ((festoID == FESTO1) || (festoID == FESTO2)){
        festoNr = festoID;
    } else {
        Logger::getInstance().log(LogLevel::ERROR, "Wrong Festo Nr in Constructor...", "HeightSensorControl");
    }

    tsc = new TSCADC();
    adc = new ADC(*tsc);
    adc->registerAdcISR(ConnectAttach(0, 0, channelID, _NTO_SIDE_CHANNEL, 0), PULSE_ADC_SAMPLE);
    adc->sample();
}

// Destructor
HeightSensorControl::~HeightSensorControl() { 
    // TODO disconnect from dispatcher
    if (0 > ConnectDetach(dispatcherConnectionID)){
        Logger::getInstance().log(LogLevel::ERROR, "Disconnection from Dispatcher failed...", "HeightSensorControl");
    }
    // TODO How to end thread if blocked in MsgReveivePulse and return avlue?
    Logger::getInstance().log(LogLevel::DEBUG, "destroying own channel...", "HeightSensorControl");
    destroyNamedChannel(channelID, hsControllerChannel); 
}

// global to store Height information into a array
// std::vector<SampleData> HeightSensorControl::heightData;

// Need some ErrorHandling
// void HeightSensorControl::initHS() {
//    uintptr_t port1BaseAddr = setupGPIO();
////    TSCADC tsc;
////    ADC *adc = new ADC(tsc);
////    adc->registerAdcISR(ConnectAttach(0, 0, channelID, _NTO_SIDE_CHANNEL, 0), PULSE_ADC_START_SAMLING);
//
//    //start Thread
//    std::thread receivingThread(&HeightSensorControl::receivingRoutine,
//    this, chanID, adc);
//    adc->sample();
//
//    //cleanUp need to be put somewhere else
//    cleanupResources(chanID, ConnectAttach(0, 0, chanID, _NTO_SIDE_CHANNEL,
//    0), port1BaseAddr, receivingThread);
//}

// Initialisiere den Kanal
// int HeightSensorControl::initializeChannel() {
//    int chanID = ChannelCreate(0);
//    if (chanID < 0) {
//        perror("Could not create a channel!\n");
//        exit(EXIT_FAILURE);
//    }
//    return chanID;
//}

// Setup der GPIO
// uintptr_t HeightSensorControl::setupGPIO() {
//    uintptr_t port1BaseAddr = mmap_device_io(GPIO_REGISTER_LENGHT, GPIO_BANK_1);
//    if (port1BaseAddr == MAP_DEVICE_FAILED) {
//        perror("Failed to map GPIO port!");
//        exit(EXIT_FAILURE);
//    }
//    return port1BaseAddr;
//}

// clean up all ressource from Height Sensor
// void HeightSensorControl::cleanupResources(int chanID, int conID) {
//    // MsgSendPulse(conID, -1, PULSE_STOP_THREAD, 0);
//    // receivingThread.join();
//    if (ConnectDetach(conID) != EOK) perror("Detaching channel failed!");
//
//    if (ChannelDestroy(chanID) != EOK) perror("Destroying channel failed!");
//}

bool HeightSensorControl::stop(){
	int coid = connectToChannel(channelID);
    if (0 > MsgSendPulse(coid, -1, PULSE_STOP_RECV_THREAD, 0)) {
        Logger::getInstance().log(LogLevel::ERROR, "Shutting down Msg Receiver failed...", "HeightSensorControl");
        return false;
    }
    // disconnect the connection to own channel
    Logger::getInstance().log(LogLevel::DEBUG, "Shutting down PULSE send...", "HeightSensorControl");
    if (0 > ConnectDetach(coid)){
        Logger::getInstance().log(LogLevel::ERROR, "Stop Detach failed...", "HeightSensorControl");
        return false;
    }
    return true;
}

// thread that received msg
void HeightSensorControl::handleMsg() {

    //    int channelID = initializeChannel();
    //    setupGPIO();

    int candidateValue = 0;
    bool secondChance = false;

    ThreadCtl(_NTO_TCTL_IO, 0);
    _pulse msg;
    receivingRunning = true;

    int32_t previousValue;
    bool candidatesSend = false;
    // need to switch to a switch case variant if more pulse msg are available
    while (receivingRunning) {
        // printf("Iam into Routine\n");
        if (MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr) < 0) {
            Logger::getInstance().log(LogLevel::ERROR, "MsgReceivePulse failed...", "HeightSensorControl");
            //exit(EXIT_FAILURE);
        }

        // printf("Iam into Routine HeightPulse with Code: %d\n", msg.code );
        if (msg.code == PULSE_ADC_SAMPLE) {
            int32_t currentValue = msg.value.sival_int;
            processSample(currentValue, secondChance, candidatesSend, candidateValue, adc);
            // check if currentValue matches previousValue

            // this_thread::sleep_for(chrono::milliseconds(10));
            // adc->sample();
        } else if(msg.code == PULSE_STOP_RECV_THREAD) {
            Logger::getInstance().log(LogLevel::DEBUG, "received PULSE_STOP_RECV_THREAD...", "HeightSensorControl");
            receivingRunning = false;  
        }

        // processSample(currentValue, secondChance, candidateValue, adc);
    }
    Logger::getInstance().log(LogLevel::DEBUG, "Message thread stops...", "HeightSensorControl");
}

void HeightSensorControl::sendMsg() {}

int32_t HeightSensorControl::getChannel() { return channelID; }

// Evaluate sample
void HeightSensorControl::processSample(
    int currentValue, bool &secondChance, bool &candidatesSend, int &candidateValue, ADC *adc
) { //, ADC *adc) {
    if (!firstValue) {
        bandHeight = currentValue;
        firstValue = true;
    }

    if ((abs(currentValue - bandHeight) <= THRESHOLD)) {
        handleBandHeightReached(secondChance);
        if (candidatesSend) {
            if (festoNr == FESTO1){
                if (MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS1_SAMPLING_DONE, currentValue)) {
                    Logger::getInstance().log(LogLevel::WARNING, "Send failed...", "HeightSensorControl");
                }
            } else if (festoNr == FESTO2) {
                if (MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS2_SAMPLING_DONE, currentValue)) {
                    Logger::getInstance().log(LogLevel::WARNING, "Send failed...", "HeightSensorControl");
                }
            } else {
                Logger::getInstance().log(LogLevel::ERROR, "Festo Setup Wrong...", "HeightSensorControl");
            }
            candidatesSend = false;
        }
    } else if (abs(currentValue - lastValue) <= THRESHOLD) {
        // std::cout << "HSCONTROL: value is: " << currentValue << std::endl;
        // std::cout << "HSCONTROL: CounterValue is: " << countSample << std::endl;
        if (festoNr == FESTO1){
            if (MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS1_SAMPLE, currentValue)) {
                Logger::getInstance().log(LogLevel::WARNING, "Send failed...", "HeightSensorControl");
            }
        } else if (festoNr == FESTO2) {
            if (MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS2_SAMPLE, currentValue)) {
                Logger::getInstance().log(LogLevel::WARNING, "Send failed...", "HeightSensorControl");
            }
        } else {
            Logger::getInstance().log(LogLevel::ERROR, "Festo Setup Wrong...", "HeightSensorControl");
        }
        candidatesSend = true;
    } else {
        handleNewValue(currentValue, secondChance, candidateValue);
    }

    this_thread::sleep_for(chrono::milliseconds(10));
    adc->sample();
}

// // If bandheight is reached
void HeightSensorControl::handleBandHeightReached(bool &secondChance) {
    // Höhe wird erreicht - Speichern der Höhe
    /*
    if (countSample > 0 && !heights.empty()) {
        heights.push_back(bandHeight);
        sampleCounts.push_back(countSample);
    }
    */

    secondChance = false;
    countSample = 0;
}

// Wenn ein neuer Wert erkannt wird
void HeightSensorControl::handleNewValue(int currentValue, bool &secondChance, int &candidateValue) {
    if (!secondChance) {
        secondChance = true;
        candidateValue = currentValue;
    } else if (abs(currentValue - candidateValue) <= THRESHOLD) {
        lastValue = currentValue;
        secondChance = false;
        countSample += 1; // Sample wird gezählt
    } else {
        secondChance = false;
        countSample = 0;
    }
}

// void HeightSensorControl::printHeightData() {
//     if (heightData.empty()) {
//         std::cout << "No height data available!" << std::endl;
//     } else {
//         for (const auto& data : heightData) {
//             std::cout << "Count Sample: " << data.countSample << ", Median
//             Height: " << data.medianHeight << std::endl;
//         }
//     }
// }
