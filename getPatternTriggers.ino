void getPatternTriggers() {
  
  int c = 0;

  for (int templateI = 0; templateI < templateRep+1; templateI++) {
    for (int memberI = 0; memberI < numMemberTemplate1; memberI++) {
      triggerArray[c] = true;
      c++;
    }
    triggerArray[c] = false;
    c++;
  }

    for (int templateI = 0; templateI < templateRep+1; templateI++) {
    for (int memberI = 0; memberI < numMemberTemplate2; memberI++) {
      triggerArray[c] = true;
      c++;
    }
    triggerArray[c] = false;
    c++;
  }
  

}
