void getPatternTiming() {

  int c = 0;
  float timeSec = preTemplateDur;

  for (int templateI = 0; templateI < templateRep+1; templateI++) {
    for (int memberI = 0; memberI < numMemberTemplate1 + 1; memberI++) {
      
      timeArray[c] = timeSec;
      c++;
      timeSec += templateStepDur;
    }
    timeSec += templateIPI;
  }
  timeSec += templateITI;


  for (int templateI2 = 0; templateI2 < templateRep+1; templateI2++) {
    for (int memberI2 = 0; memberI2 < numMemberTemplate2 + 1; memberI2++) {


      timeArray[c] = timeSec;
      c++;
      timeSec += templateStepDur;
    }
    timeSec += templateIPI;
  }
}
