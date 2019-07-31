void SprintTimeTable(){
  
      for(int i = 0; i < timeArrayLen; i++){
        Serial.print(">");
        Serial.print(timeArray[i]);
        Serial.print(" ");
        Serial.print(triggerArray[i]);
       // Serial.print(" ");
       // Serial.print(stateArray[i]);
        Serial.println("<");
        
      }
}
