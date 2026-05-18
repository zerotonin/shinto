void endTimedExperiment() {
  experimentRunning = false;
  digitalWrite(pinTrigger, LOW);
  pinState = byte(127);
  byte2pinMap();
  setResistors();
  singleStop = false;
}
