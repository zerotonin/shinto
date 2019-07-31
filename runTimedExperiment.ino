void runTimedExperiment() {
  if (clockVar > timeArray[timeArrayLen - 1] and singleStop == true) {

    endTimedExperiment();

  }

  if (timeArray[phase] < clockVar and stimTrainRunning) {

    currentState = stateArray[phase];
    currentTrigger = triggerArray[phase];

    phase++;


    // set new resistor state
    pinState = byte(currentState);
    byte2pinMap();
    setResistors();

    // set trigger
    digitalWrite(pinTrigger, currentTrigger);
  }

}
