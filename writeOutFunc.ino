void writeOutFunc() {
  if (writeOutFlag == true) {

    switch (writeOutMode) {
      case 1: // reduced
        {
          Serial.print(">");
          Serial.print(clockVar);
          Serial.print(" ");
          Serial.print(pinState);
          Serial.print(" ");
          Serial.print(digitalRead(pinTrigger));
          Serial.print(" ");
          Serial.print(sysMod);
          Serial.println("<");
        }
        break;

      case 2:
      {
      // This is the write out function to see the stimulus succesion for classic associative conditioning

        Serial.println("=========================");
        if (sysMod == 3){
         
        Serial.println("Pattern Experiment Protocol");
        
        }
        else
        {
         
        Serial.println("Timed Experiment Protocol");
        }
        
        Serial.println("=========================");
        SprintTimeTable();
        Serial.println("=========================");
        writeOutMode = 1;
        writeOutFlag=false;
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
          for (int bitI = 0; bitI <= 6; bitI++ ) {
            Serial.print(pinMap[bitI]);
          }
          Serial.println();
        }
        break;
    }

  }
}
