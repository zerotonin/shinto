void getStimTrainTriggers() {
  // First training session — trigger HIGH during each pulse, LOW between.
  triggerArray[0] = HIGH;
  for (int c = 0; c < numPulses1st; c++) {
    triggerArray[c * 2 + 1] = LOW;
    triggerArray[c * 2 + 2] = HIGH;
  }

  // Second training session.
  int iter = numPulses1st * 2 + 1;
  triggerArray[iter - 1] = LOW;
  triggerArray[iter]     = HIGH;

  for (int c = 0; c < numPulses2nd; c++) {
    triggerArray[c * 2 + iter + 1] = LOW;
    triggerArray[c * 2 + iter + 2] = HIGH;
  }
  iter = numPulses2nd * 2 + iter + 1;
  triggerArray[iter - 1] = LOW;

  // Tail-fill the rest of the array to LOW.
  for (int c = iter; c < timeArrayLen; c++) {
    triggerArray[c] = LOW;
  }
}
