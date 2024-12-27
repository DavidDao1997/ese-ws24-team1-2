#ifndef HEIGHTSENSORCONTROL_H_
#define HEIGHTSENSORCONTROL_H_

using namespace std;

#include <stdio.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include <iostream>
#include <thread>
#include <chrono>

// #include "simqnxgpioapi.h"
// #include "simqnxirqapi.h"

#include <sys/mman.h>
#include <hw/inout.h>
#include "../../Util/headers/Util.h"

/* Code configuration */
#define DEMO true         // true for demo time, false to spin forever.
#define DEMO_DURATION 30 // Duration of demo time in minutes.

#include "../../HAL/headers/ADC.h"
#include "../../HAL/headers/TSCADC.h"

#include <vector>   // für den Vektor
#include <algorithm> // für std::sort, wenn Median benötigt wird

#include "../../HAL/headers/HALConfig.h"
#include "../../Dispatcher/headers/PulseMsgConfig.h"
#include "../../Dispatcher/headers/PulseMsgHandler.h"


#include "../../Logging/headers/Logger.h"



/* GPIO port addresses (spruh73l.pdf S.177 ff.) */
#define ADC_BASE 0x44E0D000

/* GPIO port registers length */
#define ADC_LENGTH 0x2000


/* TSC_ADC register offsets (spruh73l.pdf S.1747) */
#define ADC_IRQ_ENABLE_SET 0x2c
#define ADC_IRQ_ENABLE_CLR 0x30
#define ADC_IRQ_STATUS 0x28
#define ADC_CTRL 0x40
#define ADC_DATA 0x100

/* ADC irq pin mask */
#define ADC_IRQ_PIN_MASK 0x2


/* Threshold */
#define THRESHOLD 180 // Hysterese-Bereich.
#define MAX_PRINT_COUNT 100 // Maximale Bestätigungsanzahl.

// Struktur für das Speichern von Höhen- und Sample-Daten
struct SampleData {
    int countSample;
    int medianHeight;
};

class HeightSensorControl: public PulseMsgHandler {
public:
    // Constructor and Destructor
    HeightSensorControl(const std::string channelName, const std::string dispatcherName);
    virtual ~HeightSensorControl();

    void initHS();

    // Public Methods
    //Example to save Height information

//    void  printHeightData();
//    static std::vector<SampleData> heightData;
//    void storeHeightData(int countSample, int medianHeight);  // Methode zum Speichern der Daten
//    int calculateMedian(std::vector<int>& values);            // Methode zur Berechnung des Medians
    //void initHS();

    // thread for the HeightSensor
    void handleMsg() override;
    void sendMsg() override;
    int32_t getChannel() override;

    bool stop();

private:
    int32_t channelID;
    name_attach_t *hsControllerChannel;
    int32_t dispatcherConnectionID;
    bool running;

    TSCADC *tsc;
    ADC *adc;


    //static int8_t numOfPulses;
    //static int8_t pulses[ACTUATOR_CONTROLLER_NUM_OF_PULSES];
    // Member Variables
    // bool receivingRunning;    // Indicates if the receiving routine is running
    // int bandHeight;           // Height of the band threshold
    // bool firstValue;          // Flag for the first value detection
    // int changeCounter;        // Counts value changes
    // int lastValue;            // Stores the last processed value
    // int stableCount;          // Counts consecutive stable values

    // Vektoren zum Speichern von Höhen- und Sample-Daten
//    std::vector<SampleData> heightData;  // Speichert Höhen und zugehörige Sample-Daten
//    std::vector<int> heights;            // Speichert nur die Höhenwerte
//    std::vector<int> sampleCounts;       // Speichert die Sample Counts für jede Höhe

    // ADC and Channel Initialization
    // int initializeChannel();
    // uintptr_t setupGPIO();

    // // Cleanup Resources
    // void cleanupResources(
    //     int chanID,
    //     int conID,
    //     uintptr_t port1BaseAddr,
    //     std::thread &receivingThread
    // );




    // Helper Functions for Processing
    void processSample(
        int currentValue,
        bool &secondChance,
        bool  &candidatesSend,
        int &candidateValue,
        ADC * adc
    );

    void handleBandHeightReached(
        bool &secondChance
    );

    void handleNewValue(
        int currentValue,
        bool &secondChance,
        int &candidateValue
    );
};

#endif /* HEIGHTSENSORCONTROL_H_ */
