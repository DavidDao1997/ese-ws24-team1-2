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
HeightSensorControl::HeightSensorControl(const std::string channelName, const std::string dispatcherName) {
    hsControllerChannel = createNamedChannel(channelName);
    channelID = hsControllerChannel->chid;
    running = false;
    dispatcherConnectionID = name_open(dispatcherName.c_str(), 0);

    tsc = new TSCADC();
    adc = new ADC(*tsc);
    adc->registerAdcISR(ConnectAttach(0, 0, channelID, _NTO_SIDE_CHANNEL, 0), PULSE_ADC_SAMPLE);
    adc->sample();
}

// Destructor
HeightSensorControl::~HeightSensorControl() { std::cout << "HwAdcDemo object destroyed." << std::endl; }

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
            perror("HSCONTROLLER: MsgReceivePulse failed!");
            exit(EXIT_FAILURE);
        }

        // printf("Iam into Routine HeightPulse with Code: %d\n", msg.code );
        if (msg.code == PULSE_ADC_SAMPLE) {
            int32_t currentValue = msg.value.sival_int;
            processSample(currentValue, secondChance, candidatesSend, candidateValue, adc);
            // check if currentValue matches previousValue

            // this_thread::sleep_for(chrono::milliseconds(10));
            // adc->sample();
        }

        // processSample(currentValue, secondChance, candidateValue, adc);
    }
    printf("HSCONTROLLER: Message thread stops...\n");
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
            if (MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS_SAMPLING_DONE, currentValue)) {
                perror("HSCONTROLLER: Send failed.");
            }
            candidatesSend = false;
        }
    } else if (abs(currentValue - lastValue) <= THRESHOLD) {
        // std::cout << "HSCONTROL: value is: " << currentValue << std::endl;
        // std::cout << "HSCONTROL: CounterValue is: " << countSample << std::endl;
        if (MsgSendPulse(dispatcherConnectionID, -1, PULSE_HS_SAMPLE, currentValue)) {
            perror("HSCONTROLLER: Send failed.");
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
