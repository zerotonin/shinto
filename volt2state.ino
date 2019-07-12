int volt2state(float volt){

  int stateInterp = int(round((volt-150.52)/-0.77805));
  
  return byte(stateInterp);
}
