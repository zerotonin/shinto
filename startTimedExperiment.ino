void startTimedExperiment() {

  // get timing, triggers, and states
  getStimTrainTiming();
  getStimTrainTriggers();
  getStimTrainStates();
  phase = 0;

  // set output to lowest state and turn off trigger
  currentState = 127;
  currentTrigger = LOW;
  // set new resistor state
  pinState = byte(currentState);
  byte2pinMap();
  setResistors();
  // set trigger
  digitalWrite(pinTrigger, currentTrigger);


  // start the train & reset clock
  stimTrainRunning = true;
  singleStart = false;
  singleStop = true;
  clockVar = 0.0;
  delay(0.5);
  writeOutMode = 2;
  writeOutFlag = true;
}
