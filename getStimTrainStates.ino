void getStimTrainStates() {
  // First training session — alternating pattern voltage / "off" (127).
  stateArray[0] = statePattern1[0];
  for (int c = 0; c < numPulses1st; c++) {
    stateArray[c * 2 + 1] = 127;
    stateArray[c * 2 + 2] = statePattern1[c + 1];
  }

  // Second training session.
  int iter = numPulses1st * 2 + 1;
  stateArray[iter - 1] = 127;
  stateArray[iter]     = statePattern2[0];

  for (int c = 0; c < numPulses2nd - 1; c++) {
    stateArray[c * 2 + iter + 1] = 127;
    stateArray[c * 2 + iter + 2] = statePattern2[c + 1];
  }
  iter = numPulses2nd * 2 + iter - 1;

  // Tail-fill to "off" (127) for the remainder of the array.
  for (int c = iter; c < timeArrayLen; c++) {
    stateArray[c] = 127;
  }
}
