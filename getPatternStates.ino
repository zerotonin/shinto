void getPatternStates(){
int c = 0;

  for (int templateI = 0; templateI < templateRep+1; templateI++) {
    for (int memberI = 0; memberI < numMemberTemplate1; memberI++) {
      
      stateArray[c] = stateTemplate1[memberI] ;
      c++;
    }
      stateArray[c] = 127;
      c++;
      
  }
  
  for (int templateI2 = 0; templateI2 < templateRep+1; templateI2++) {
    for (int memberI2 = 0; memberI2 < numMemberTemplate2; memberI2++) {


      stateArray[c] = stateTemplate2[memberI2];
      c++;
    }
    
      stateArray[c] = 127;
      c++;
  }
  for (int fillerI = c; fillerI<timeArrayLen;fillerI++){
    Serial.println(fillerI);
    stateArray[fillerI] = 127;
  }
}
