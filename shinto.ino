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

const int   timeArrayLen = 240;
byte  stateArray[timeArrayLen];
bool  triggerArray[timeArrayLen];
float timeArray[timeArrayLen];

int sysMod = 2; // default  free run 1 timing 2 calibration
double calibrationStateTime = 4.0;

/////////////////////////////
// GENERAL TRAIN VARIABLES //
/////////////////////////////

int   phase = 0;
int   currentState = 0;
int   currentTrigger = LOW;
bool  singleStart = true;
bool  singleStop = true;

//////////////////////////
// STIM TRAIN VARIABLES //
//////////////////////////

bool  stimTrainRunning = false;
float preStimDur = 60.0;
float pulseDur = 5.0;
float IPI = 0.0;
float ITI = 60.0;
int   statePattern1[5] = {91, 78, 65, 77, 116};
int   statePattern2[4] = {91, 78, 65, 78};
int   numPulses1st = sizeof(statePattern1) / sizeof(int);
int   numPulses2nd = sizeof(statePattern2) / sizeof(int);
int   bonusPulses = 1;
int   maxPhases = 80;

////////////////////
// TEMPLATE TRAIN //
////////////////////

float preTemplateDur = 60.0;
float templateStepDur = 0.5;
int   stateTemplate1[5] = {91, 78, 65, 78, 91};
int   stateTemplate2[6] = {91, 78, 65, 78, 91, 116};
float templateIPI = 2.5;
float templateITI = 60.0;
int   templateRep = 10;

int   numMemberTemplate1 = sizeof(stateTemplate1) / sizeof(int);
int   numMemberTemplate2 = sizeof(stateTemplate2) / sizeof(int);

int templatePhases = templateRep * (numMemberTemplate1 + 1) + templateRep * (numMemberTemplate2 + 1);
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

        if (stimTrainRunning == false and singleStart == true) {
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
          digitalWrite(pinTrigger, currentTrigger);

          // start the train & reset clock
          stimTrainRunning = true;
          singleStart = false;
          singleStop = true;
          clockVar = 0.0;
          delay(0.5);
          writeOutMode=2;
          writeOutFlag=true;

        }
        else {

          if (clockVar > timeArray[timeArrayLen-1] and singleStop == true) {

              endTimedExperiment();

          }

          if (timeArray[phase] < clockVar and stimTrainRunning) {

            currentState = stateArray[phase];
            currentTrigger = triggerArray[phase];

            phase++;


            // set new resistor state
            pinState = byte(currentState);
            byte2pinMap();
            setResistors();

            // set trigger
            digitalWrite(pinTrigger, currentTrigger);
          }

        }




      }
      break;

    case 2:
      {

        // if calibrationMode is active the box cycles slowly through the 128 states @ 0.25 Hz.
        // you can measure the produced voltage at the  ShockPowerSupply with a multimeter.
        writeOutMode = 0;
        writeOutFlag = true;
        //turn on trigger so that there is always voltage on the ShockPowerSupply
        digitalWrite(pinTrigger, HIGH);


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

    case 3:
      {          
        if (stimTrainRunning == false and singleStart == true) {
          singleStart = false;
          writeOutFlag = false;
          getPatternTriggers();
          Serial.println("got triggers");
          getPatternTiming();
          Serial.println("got timing");
          getPatternStates();
          Serial.println("got states");
          
          writeOutMode=2;
          writeOutFlag=true;
        }

      }
      break;
    default:
      break;
  }
}
