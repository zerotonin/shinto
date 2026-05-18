// ────────────────────────────────────────────────────────────────────
//  shinto.ino  « Drosophila peak-end-rule shock controller »
// ────────────────────────────────────────────────────────────────────
//  Speaks the cuewire / PARROTard integer-command protocol over USB
//  serial.  All run-time parameters are host-settable; this file holds
//  the globals, setup(), and the mode dispatch in loop().
//
//  Companion Python host:  cuewire.shinto.ShintoRig
//                          (https://github.com/zerotonin/cuewire)

#include <FlexiTimer2.h>

// ── pin map ─────────────────────────────────────────────────────────
byte pinR[7]    = {13, 5, 10, 9, 8, 6, 12};
bool pinMap[7]  = { 0, 0, 0, 0, 0, 0, 0};
byte pinTrigger = 4;

// ── core flow state ─────────────────────────────────────────────────
double clockVar = 0.0;
byte   pinState = 0;

const int   timeArrayLen = 240;
byte        stateArray  [timeArrayLen];
bool        triggerArray[timeArrayLen];
float       timeArray   [timeArrayLen];

int   sysMod               = 0;     // 0 free  1 stim-train  2 calib  3 pattern
float calibrationStateTime = 4.0;   // seconds per calibration step

// ── timed-experiment lifecycle ──────────────────────────────────────
int  phase             = 0;
int  currentState      = 0;
int  currentTrigger    = LOW;
bool singleStart       = true;
bool singleStop        = true;
bool experimentRunning = false;
bool endEmitted        = false;

// ── stim-train parameters (Tully two-session) ───────────────────────
const int MAX_PATTERN_LEN = 16;
float preStimDur = 60.0;
float pulseDur   =  5.0;
float IPI        =  1.0;
float ITI        = 60.0;
byte  statePattern1[MAX_PATTERN_LEN] = {91, 78, 65, 78, 127};
byte  statePattern2[MAX_PATTERN_LEN] = {91, 78, 65, 78};
int   numPulses1st = 5;
int   numPulses2nd = 4;

// ── pattern parameters (peak-end-rule templates) ────────────────────
const int MAX_TEMPLATE_LEN = 16;
float preTemplateDur  = 60.0;
float templateStepDur =  0.25;
byte  stateTemplate1[MAX_TEMPLATE_LEN] = {67, 54, 67, 80, 92};
byte  stateTemplate2[MAX_TEMPLATE_LEN] = {67, 54, 67, 80, 92, 127};
int   numMemberTemplate1 = 5;
int   numMemberTemplate2 = 6;
float templateIPI = 3.75;
float templateITI = 60.0;
int   templateRep = 10;

// ── serial / write-out flags ────────────────────────────────────────
bool writeOutFlag = false;
int  serialComInt = 0;

// ── setup ───────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  for (int i = 0; i <= 6; i++) {
    pinMode(pinR[i], OUTPUT);
  }
  setResistors();

  pinMode     (pinTrigger, OUTPUT);
  digitalWrite(pinTrigger, LOW);

  FlexiTimer2::set(10, clockInt);     // 10 ms tick
  FlexiTimer2::start();
}

// ── main loop ───────────────────────────────────────────────────────
void loop() {
  if (Serial.available() != 0) {
    serialComInt = Serial.parseInt();
    SerialCommunication();
  }

  switch (sysMod) {

    case 1: {  // stim-train
      if (!experimentRunning && singleStart) {
        getStimTrainTiming();
        getStimTrainTriggers();
        getStimTrainStates();
        phase = 0;
        startTimedExperiment();
      } else {
        runTimedExperiment();
      }
      break;
    }

    case 2: {  // calibration — slow walk through all 128 states
      digitalWrite(pinTrigger, HIGH);
      if (clockVar > calibrationStateTime) {
        pinState = (pinState + 1) % 128;
        byte2pinMap();
        setResistors();
        clockVar = 0.0;
      }
      break;
    }

    case 3: {  // pattern (peak-end rule)
      if (!experimentRunning && singleStart) {
        getPatternTriggers();
        getPatternTiming();
        getPatternStates();
        phase = 0;
        startTimedExperiment();
      } else {
        runTimedExperiment();
      }
      break;
    }

    default:  // free run — pins respond only to direct commands
      break;
  }
}
