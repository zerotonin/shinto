void getStimTrainTiming() {
  // First training session.
  timeArray[0] = preStimDur;
  for (int c = 0; c < numPulses1st; c++) {
    timeArray[c * 2 + 1] = timeArray[c * 2]     + pulseDur;
    timeArray[c * 2 + 2] = timeArray[c * 2 + 1] + IPI;
  }

  // Inter-training-interval, then second training session.
  int iter = numPulses1st * 2 + 1;
  timeArray[iter] = timeArray[iter - 1] + ITI;

  for (int c = 0; c < numPulses2nd; c++) {
    timeArray[c * 2 + iter + 1] = timeArray[c * 2 + iter]     + pulseDur;
    timeArray[c * 2 + iter + 2] = timeArray[c * 2 + iter + 1] + IPI;
  }
  iter = numPulses2nd * 2 + iter + 1;

  // Tail-fill all the way to timeArrayLen so the end criterion is legal.
  for (int c = iter; c < timeArrayLen; c++) {
    timeArray[c] = timeArray[c - 1];
  }
}
