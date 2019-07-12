void getPatternTiming() {

  int c = 0;
  float timeSec = preTemplateDur;

  for (int templateI = 0; templateI < templateRep; templateI++) {
    for (int memberI = 0; memberI < numMemberTemplate1 + 1; memberI++) {
      timeArray[c] = true;
      c++;
      timeSec += templateStepDur;
    }
    timeSec += templateIPI;
  }

  timeSec += templateITI;


  for (int templateI = 0; templateI < templateRep; templateI++) {
    for (int memberI = 0; memberI < numMemberTemplate2 + 1; memberI++) {
      timeArray[c] = true;
      c++;
      timeSec += templateStepDur;
    }
    timeSec += templateIPI;
  }

}
