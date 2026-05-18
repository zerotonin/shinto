void runTimedExperiment() {
  // Natural end of the loaded timeline — emit "end" once, then clean up.
  if (clockVar > timeArray[timeArrayLen - 1] && singleStop) {
    if (!endEmitted) {
      Serial.print("end");          // no newline — host treats as sentinel
      endEmitted = true;
    }
    endTimedExperiment();
  }

  // Advance to the next phase whenever its scheduled time elapses.
  if (timeArray[phase] < clockVar && experimentRunning) {
    currentState   = stateArray[phase];
    currentTrigger = triggerArray[phase];
    phase++;

    pinState = byte(currentState);
    byte2pinMap();
    setResistors();

    digitalWrite(pinTrigger, currentTrigger);
  }
}
