/////////////
// IMPORTS //
/////////////
#include <FlexiTimer2.h>

/////////////
// PINNING //
/////////////

int  pinR[7]   = {13, 5, 10, 9, 8, 6, 12};
bool pinMap[7] = { 0, 0, 0, 0,  0, 0,  0};
int  pinTrigger = 4;

////////////////////
// Flow Variables //
////////////////////

double clockVar = 0.0;
byte pinState = 0; // this variable describes the dampening value 0 ->128


byte  stateArray[80];
bool  triggerArray[80];
float timeArray[80];

int sysMod = 1; // default  free run 1 timing 2 calibration
double calibrationStateTime = 4.0;

//////////////////////////
// STIM TRAIN VARIABLES //
//////////////////////////

bool  stimTrainRunning = false;
float preStimDur = 60.0;
float pulseDur = 5.0;
float IPI = 0.0;
float ITI = 60.0;
int   statePattern1[5] = {91,78,65,77,116};
int   statePattern2[4] = {91,78,65,78};
int   numPulses1st = sizeof(statePattern1)/2;
int   numPulses2nd = sizeof(statePattern2)/2;
int   bonusPulses = 1;
int   maxPhases = 80;
int   phase = 0;
int   currentState = 0;
int   currentTrigger = LOW;
bool  singleFire = true;



/////////////////////////////
// Communication Variables //
/////////////////////////////
bool writeOutFlag = false;
int  writeOutMode = 0;
int serialComInt = 0;


///////////
// SETUP //
///////////

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);                              // Serielle Komunikation Starten
  // Intialise Resistor Ports/Pins
  for (int i = 0; i <= 6; i++) {
    pinMode     (pinR[i], OUTPUT);
  }
  setResistors();

  //Intialise Trigger
  pinMode     (pinTrigger, OUTPUT);
  digitalWrite(pinTrigger, LOW);


  FlexiTimer2::set(10, clockInt); // 10ms period
  FlexiTimer2::start();
}

///////////////
// MAIN LOOP //
///////////////

void loop() {
  // check if there is a serial command from MatLab
  if (Serial.available() == 0) {
  }
  else {
    serialComInt = Serial.parseInt();
    SerialCommunication();
  }
  switch (sysMod) {

    case 1:
      {

        if (stimTrainRunning == false and singleFire == true) {
          // get timing, triggers, and states
          getStimTrainTiming();
          getStimTrainTriggers();
          getStimTrainStates();
          phase = 0;


            currentState = 127;
            currentTrigger = LOW;
            // set new resistor state
            pinState = byte(currentState);
            byte2pinMap();
            setResistors();

            // set trigger
            digitalWrite(pinTrigger,currentTrigger);
        
          // start the train & reset clock
          stimTrainRunning = true;
          singleFire = false;
          clockVar = 0.0;
          delay(0.5);
        Serial.println(numPulses2nd);
        for (int c = 0; c < 80 ; c++) {
          Serial.print(timeArray[c]);
          Serial.print(' ');
          Serial.print(triggerArray[c]);
          Serial.print(' ');
          Serial.print(stateArray[c]);
          Serial.print(' ');
        Serial.println();
        }
        Serial.println("=====================");
          
        }
        else {
          
          if (clockVar > timeArray[79]){
              
          stimTrainRunning = false;
            digitalWrite(pinTrigger,LOW);
            pinState = byte(127);
            byte2pinMap();
            setResistors();
          
                      }
                      
          if (timeArray[phase]< clockVar and stimTrainRunning){
            
            currentState = stateArray[phase];
            currentTrigger = triggerArray[phase];
          
            phase++;
            
            
            // set new resistor state
            pinState = byte(currentState);
            byte2pinMap();
            setResistors();

            // set trigger
            digitalWrite(pinTrigger,currentTrigger);
          }

//          
//          Serial.print(clockVar);
//          Serial.print(": ");
//          Serial.print(phase);
//          Serial.print(" | ");
//          Serial.print(timeArray[phase]);
//          Serial.print(" | ");
//          Serial.print(currentTrigger);
//          Serial.print(" | ");
//          Serial.print(currentState);
//          Serial.println(" | ");
        }
        



      }
      break;

    case 2:
      // if calibrationMode is active the box cycles slowly through the 128 states @ 0.25 Hz.
      // you can measure the produced voltage at the  ShockPowerSupply with a multimeter.
      writeOutMode = 0;
      writeOutFlag = true;
      //turn on trigger so that there is always voltage on the ShockPowerSupply
      digitalWrite(pinTrigger, HIGH);
      {

        // test if 4 sec (default value of calibrationStateTime) are done and increase the
        // pinning state, reset the clock.
        if (clockVar > calibrationStateTime) {
          pinState += 1; // go to next higher dampening state
          pinState = pinState % 128; // make sure that the state cannot be outside 0 -> 127
          byte2pinMap(); // convert the bits of the byte into a vector of booleans which can be used to toggel the channels (pins)
          setResistors(); // write out the boolean vector to the digital channels
          clockVar = 0.0; // reset cloxk
        }
        // print out calibration progress
        writeOutFunc();
      }
      break;

    default:
      break;
  }
}
