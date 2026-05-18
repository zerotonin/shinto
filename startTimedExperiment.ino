void startTimedExperiment() {
  // Park output: lowest dampening state (127) and trigger LOW.
  currentState   = 127;
  currentTrigger = LOW;
  pinState = byte(currentState);
  byte2pinMap();
  setResistors();
  digitalWrite(pinTrigger, currentTrigger);

  experimentRunning = true;
  singleStart       = false;
  singleStop        = true;
  endEmitted        = false;
  clockVar          = 0.0;

  delay(0.5);
  writeOutFlag = true;
}
