void getStimTrainTriggers(){
        int tempPhases = maxPhases;

          // set pre stimulus duration
        triggerArray[0] = HIGH;

        // set first training session
        for (int c = 0; c < numPulses1st; c++) {
          triggerArray[c * 2 + 1] = LOW;
          triggerArray[c * 2 + 2] = HIGH;
        }

        //set peri stimulus duration
        int iter = numPulses1st * 2 + 1;
        triggerArray[iter-1] =LOW ;
        triggerArray[iter] =HIGH ;



        // set second training session
        for (int c = 0; c < numPulses2nd; c++) {

          triggerArray[c * 2 + iter + 1] = LOW;
          triggerArray[c * 2 + iter + 2] =HIGH ;
        }

        // clear out rest of timeline
        iter = (numPulses2nd) * 2 + iter + 1;
        triggerArray[iter-1] =LOW ;
        tempPhases = tempPhases - iter;
        
        for (int c = 0; c < tempPhases; c++) {

          triggerArray[c + iter] = LOW;
       
        }
}
