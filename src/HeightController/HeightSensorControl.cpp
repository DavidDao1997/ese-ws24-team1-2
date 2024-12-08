#include "header/HeightSensorControl.h"
#include <thread>
#include <vector> // Für das Speichern der Werte in einem Array

// Deklaration von Variablen
std::vector<int> heights; // Array für Höhen
std::vector<int> sampleCounts; // Array für die Anzahl der Samples für jede Höhe

bool receivingRunning = false;
int bandHeight = 0;
bool firstValue = false;
int lastValue = -1;
int stableCount = 0;
int countSample = 0; // Zähler für Samples

using namespace std;

// Constructor
HeightSensorControl::HeightSensorControl() {
    std::cout << "HwAdcDemo object created." << std::endl;

}

// Destructor
HeightSensorControl::~HeightSensorControl() {
    std::cout << "HwAdcDemo object destroyed." << std::endl;
}

//global to store Height information into a array
//std::vector<SampleData> HeightSensorControl::heightData;

// Need some ErrorHandling
//void HeightSensorControl::initHS() {
////    int chanID = initializeChannel();
////    uintptr_t port1BaseAddr = setupGPIO();
////    TSCADC tsc;
////    ADC *adc = new ADC(tsc);
////    adc->registerAdcISR(ConnectAttach(0, 0, chanID, _NTO_SIDE_CHANNEL, 0), PULSE_ADC_START_SAMLING);
//
//    //start Thread
//    //std::thread receivingThread(&HeightSensorControl::receivingRoutine, this, chanID, adc);
//    //adc->sample();
//
//    //cleanUp need to be put somewhere else
//    //cleanupResources(chanID, ConnectAttach(0, 0, chanID, _NTO_SIDE_CHANNEL, 0), port1BaseAddr, receivingThread);
//}

// Initialisiere den Kanal
int HeightSensorControl::initializeChannel() {
    int chanID = ChannelCreate(0);
    if (chanID < 0) {
        perror("Could not create a channel!\n");
        exit(EXIT_FAILURE);
    }
    return chanID;
}

// Setup der GPIO
uintptr_t HeightSensorControl::setupGPIO() {
    uintptr_t port1BaseAddr = mmap_device_io(IO_MEM_LEN, GPIO_BANK_1);
    if (port1BaseAddr == MAP_DEVICE_FAILED) {
        perror("Failed to map GPIO port!");
        exit(EXIT_FAILURE);
    }
    return port1BaseAddr;
}

// clean up all ressource from Height Sensor
void HeightSensorControl::cleanupResources(int chanID, int conID, uintptr_t port1BaseAddr, thread &receivingThread) {
//    MsgSendPulse(conID, -1, PULSE_STOP_THREAD, 0);
//    receivingThread.join();
    if (ConnectDetach(conID) != EOK)
        perror("Detaching channel failed!");

    if (ChannelDestroy(chanID) != EOK)
        perror("Destroying channel failed!");

}

// thread that received msg
void HeightSensorControl::initRoutine() {
    int channelID = initializeChannel();
    setupGPIO();
    TSCADC tsc;
    ADC *adc = new ADC(tsc);
    adc->registerAdcISR(ConnectAttach(0, 0, channelID, _NTO_SIDE_CHANNEL, 0), PULSE_ADC_SAMPLE);

    int candidateValue = 0;
    bool secondChance = false;

    ThreadCtl(_NTO_TCTL_IO, 0);
    _pulse msg;
    receivingRunning = true;

    //need to switch to a switch case variant if more pulse msg are available
    while (receivingRunning) {
    	printf("Iam into Routine\n");
        if (MsgReceivePulse(channelID, &msg, sizeof(_pulse), nullptr) < 0) {
            perror("MsgReceivePulse failed!");
            exit(EXIT_FAILURE);
        }
    	printf("Iam into Routine HeightPulse\n");
//
//        if (msg.code == PULSE_STOP_THREAD) {
//            printf("Thread kill code received!\n");
//            receivingRunning = false;
//        }
        //is now a Enum? PULSE_ADC_START_SAMPLING?
        if (msg.code == PULSE_ADC_SAMPLE) {
            int currentValue = msg.value.sival_int;
            processSample(currentValue, secondChance, candidateValue, adc);
        }
    }
    printf("Message thread stops...\n");
}

// Evaluate sample
void HeightSensorControl::processSample(int currentValue, bool &secondChance, int &candidateValue, ADC *adc) {
    if (!firstValue) {
        bandHeight = currentValue;
        firstValue = true;
    }

    if (abs(currentValue - bandHeight) <= THRESHOLD) {
        handleBandHeightReached(secondChance);
    }
    else if (abs(currentValue - lastValue) <= THRESHOLD) {
        printf("%d\n", currentValue);
    }
    else {
        handleNewValue(currentValue, secondChance, candidateValue);
    }
    //take a sample every 10ms
    this_thread::sleep_for(chrono::milliseconds(10));
    adc->sample();
}

// If bandheight is reached
void HeightSensorControl::handleBandHeightReached(bool &secondChance) {
    // Höhe wird erreicht - Speichern der Höhe
    if (countSample > 0 && !heights.empty()) {
        heights.push_back(bandHeight);
        sampleCounts.push_back(countSample);
    }

    secondChance = false;
    countSample = 0;
}

// Wenn ein neuer Wert erkannt wird
void HeightSensorControl::handleNewValue(int currentValue, bool &secondChance, int &candidateValue) {
    if (!secondChance) {
        secondChance = true;
        candidateValue = currentValue;
    }
    else if (abs(currentValue - candidateValue) <= THRESHOLD) {
        lastValue = currentValue;
        secondChance = false;
        countSample = 1;  // Sample wird gezählt
    }
    else {
        secondChance = false;
        countSample = 0;
    }
}

//void HeightSensorControl::printHeightData() {
//    if (heightData.empty()) {
//        std::cout << "No height data available!" << std::endl;
//    } else {
//        for (const auto& data : heightData) {
//            std::cout << "Count Sample: " << data.countSample << ", Median Height: " << data.medianHeight << std::endl;
//        }
//    }
//}




