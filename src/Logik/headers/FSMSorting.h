#ifndef FSMSORTING_H
#define FSMSORTING_H

enum class FSMSortingStates { Paused, Idle, Passthrough, PukPresent, MetalMeasurement, Ejecting, RampFull };

class FSMSorting {
  public:
    FSMSorting(int32_t dispatcherConnectionId);
    ~FSMSorting();

    void raisePukEntrySorting();
    void raiseLBM1Interrupted();
    void raiseLBM1Open();
    void raisePukEjectorDistanceValid();
    void raiseLBR1Open();
    void raiseLBR1Interrupted();
    void raiseBGR1Interrupted();
    void raisePukDesired();
    void raisePukNotDesired();
    void raiseMSTrue();
    void raiseMSFalse();

    void raiseSystemOperationalIn();
    void raiseSystemOperationalOut();

    void onPukPresentIn(std::function<void(int32_t conId)> callBackFunctio);
    void onPukPresentOut(std::function<void(int32_t conId)> callBackFunction);

    void onPukEntryEgress(std::function<void(int32_t conId)> callBackFunction);
    void onPukEjectorDistanceValid(std::function<void(int32_t conId)> callBackFunctio);

    void onRampFullIn(std::function<void(int32_t conId)> callBackFunction);
    void onRampFullOut(std::function<void(int32_t conId)> callBackFunction);

    void onMetalMeasurementIn(std::function<void(int32_t conId)> callBackFunction);
    void onMetalMeasurementOut(std::function<void(int32_t conId)> callBackFunction);

    void onPassthroughOut(std::function<void(int32_t conId)> callBackFunction);
    void onEjectingOut(std::function<void(int32_t conId)> callBackFunction);

    void onSortingModuleActive(std::function<void(int32_t conId)> callBackFunction);
    void onSortingModuleResting(std::function<void(int32_t conId)> callBackFunction);

  private:
    bool rampFull;
    bool metalDetected;
    int32_t dispConnectionId;
    FSMSortingStates currentState;
    FSMSortingStates historyState;

    void setState(FSMSortingStates nextState);

    std::function<void(int32_t conId)> callbackPukEjectorDistanceValid;
    std::function<void(int32_t conId)> callbackPukEntryEgress;

    std::function<void(int32_t conId)> callbackMetalMeasurementIn;
    std::function<void(int32_t conId)> callbackMetalMeasurementOut;

    std::function<void(int32_t conId)> callbackPukPresentIn;
    std::function<void(int32_t conId)> callbackPukPresentOut;

    std::function<void(int32_t conId)> callbackRampFullIn;
    std::function<void(int32_t conId)> callbackRampFullOut;

    std::function<void(int32_t conId)> callbackPassthroughOut;
    std::function<void(int32_t conId)> callbackEjectingOut;

    std::function<void(int32_t conId)> callbackSortingModuleActive;
    std::function<void(int32_t conId)> callbackSortingModuleResting;
};

#endif