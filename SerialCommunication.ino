void SerialCommunication(){
  
  String content;
  String temp;
  int c = 0;
  int cClock = 0;
  int cState = 0;
  int cTrigg = 0;
  float tempFloat = 0;
  int index =  0;
  switch (serialComInt) {
    /////////////////////////
    // FREE RUN COMANDS 2X //
    /////////////////////////

    case 211: // set trigger high
      digitalWrite(pinTrigger,HIGH);
      break;
    case 210: // set trigger low
      digitalWrite(pinTrigger,LOW);
      break;   
    case 23: // get new state value
    {
      int testState = Serial.parseInt(); //get serial float number
      pinState = byte(testState);
      byte2pinMap();
      setResistors();
    }
      break;  
    case 22: // get new voltage value and interpolate
     {
      float testFloat = Serial.parseFloat(); //get serial float number
      pinState = volt2state(testFloat);
      byte2pinMap();
      setResistors();
     }
      break;

    //////////////////////
    // Timed Experiment //
    //////////////////////
    
    case 5:
     {
      content = Serial.readString();
      
      // get to the start
      index = content.indexOf(">"); //We find the next comma
      content = content.substring(index+1); //Remove the number from the string
      
      // cout everything after the end
      index = content.indexOf("<"); //We find the next comma
      content = content.substring(0,index).c_str();
      
      Serial.println(content);
      
      for(int i = 0; i < 240; i++){
        index = content.indexOf(","); //We find the next comma
        //Serial.println(content);
        c = i%3;
        
        switch (c){
          case 0:
          {
            
            Serial.print(cClock);
            Serial.print(",");
            temp = content.substring(0,index).c_str();
            timeArray[cClock] = temp.toFloat();
            cClock+=1;
            Serial.print(temp);
            Serial.print(",");
            Serial.println(timeArray[cClock]);
          }
          
          case 1:
          {
          
            temp = content.substring(0,index).c_str();
            triggerArray[cTrigg]= bool(temp.toInt());
            cTrigg+=1;
          }
          
          case 2:
          {
            temp = content.substring(0,index).c_str();
            tempFloat = temp.toFloat();
            stateArray[cState] = volt2state(tempFloat);
            cState+=1;
          }
          
        }//end switch statement
        content = content.substring(index+1); //Remove the number from the string
      }//end for statement

     SprintTimeTable();
     
    }// end case statement
    break;



      
    /////////////////////////////////
    // 12X Write Out Data Commands //
    /////////////////////////////////
    case 121: // turn serial data write out on
      writeOutFlag = true;
      break;
    
    case 120: // turn serial data write out off
      writeOutFlag = false;
      break;

    case 122: // reduced mode > clock pin-state trigger <
      writeOutMode = 1;
      break;
      
    case 123: // default human readable write out mode
      writeOutMode = 0;
      break;
        
    //////////////////////////////////////////////////
    // 13X switches betweeen different system modes //
    //////////////////////////////////////////////////
    
    case 130: //freee run mode
      sysMod = 0;
      break;
    case 131: // timed experiment mode
      sysMod = 1;
      break;
    case 132: // calibration mode
      sysMod = 2;
      break;

    //communication test
    case 1337:
      Serial.println("50 1337");
      break;
  }
}
