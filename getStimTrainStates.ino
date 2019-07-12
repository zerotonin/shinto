void getStimTrainStates(){
        int tempPhases = maxPhases;
          // set pre stimulus duration
        stateArray[0] = statePattern1[0];

        // set first training session
        for (int c = 0; c < numPulses1st; c++) {
          stateArray[c * 2 + 1] = 127;
          stateArray[c * 2 + 2] = statePattern1[c+1];
        }

        //set peri stimulus duration
        int iter = numPulses1st * 2 + 1;
        stateArray[iter-1] = 127;
        stateArray[iter] = statePattern2[0];



        // set second training session
        for (int c = 0; c < numPulses2nd-1; c++) {

          stateArray[c * 2 + iter + 1] = 127;
          stateArray[c * 2 + iter + 2] = statePattern2[c+1];
        }

        // clear out rest of timeline
        iter = (numPulses2nd) * 2 + iter -1;
        tempPhases = tempPhases - iter;

        for (int c = 0; c < tempPhases; c++) {
          
          stateArray[c + iter] = 127;
       
        }
}
