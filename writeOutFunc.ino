void writeOutFunc(){
  if (writeOutFlag == true){

    switch (writeOutMode){
      case 1: // reduced
      {
      Serial.print(">");
      Serial.print(clockVar);
      Serial.print(" ");   
      Serial.print(pinState);
      Serial.print(" ");   
      Serial.print(digitalRead(pinTrigger));
      Serial.println("<");
      }
      break;
      default:
      {
      Serial.print("clock: ");   
      Serial.print(clockVar);
      Serial.print(" | pinState: ");   
      Serial.print(pinState);
      Serial.print(" | pinTrigger: ");   
      Serial.print(digitalRead(pinTrigger));
      Serial.print(" | pinMap: ");  
      for(int bitI =0; bitI <= 6; bitI++ ){ 
        Serial.print(pinMap[bitI]);
      }
      Serial.println();
      }
      break;
    }
    
  }
}
