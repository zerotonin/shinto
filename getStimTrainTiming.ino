void getStimTrainTiming(){
  
        int tempPhases = maxPhases;
        // set pre stimulus duration
        timeArray[0] = preStimDur;

        // set first training session
        for (int c = 0; c < numPulses1st; c++) {
          timeArray[c * 2 + 1] = timeArray[c * 2] + pulseDur;
          timeArray[c * 2 + 2] = timeArray[c * 2 + 1] + IPI;
        }

        //set peri stimulus duration
        int iter = numPulses1st * 2 + 1;
        timeArray[iter] = timeArray[iter - 1] + ITI;



        // set second training session
        for (int c = 0; c < numPulses2nd; c++) {

          timeArray[c * 2 + iter + 1] = timeArray[c * 2 + iter] + pulseDur;
          timeArray[c * 2 + iter + 2] = timeArray[c * 2 + iter + 1] + IPI;
        }

        // clear out rest of timeline
        iter = (numPulses2nd) * 2 + iter + 1;
        tempPhases = tempPhases - iter;

        for (int c = 0; c < tempPhases; c++) {

          timeArray[c + iter] = timeArray[c + iter - 1];
       
        }
}
