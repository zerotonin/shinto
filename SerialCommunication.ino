// ────────────────────────────────────────────────────────────────────
//  SerialCommunication.ino  « PARROTard 7-digit command dispatcher »
// ────────────────────────────────────────────────────────────────────
//  Each case matches an entry in cuewire.constants.COMMAND_IDS.
//  Follow-numbers are pulled via waitOnInt() / waitOnFloat() with a
//  20-second timeout, mirroring the cuewire/PARROTard idiom.
//  Frame uploads (>>v0,v1,...<<) are pulled via readIntListFrame() /
//  uploadTimeTable() and terminated by '\n'.

void SerialCommunication() {

  switch (serialComInt) {

    // ── write-out & clock ───────────────────────────────────────────
    case 13370001:                       // writeOutTrue
      writeOutFlag = true;
      break;

    case 13370000:                       // writeOutFalse
      writeOutFlag = false;
      break;

    case 13379999:                       // resetClock
      clockVar = 0.0;
      break;

    case 1337:                           // communicationTest
      Serial.println("50 1337");
      break;

    // ── shock subsystem (13374xxx) ──────────────────────────────────
    case 13374000: {                     // SHOCKsetState_int
      int s = waitOnInt(int(pinState));
      pinState = byte(s);
      byte2pinMap();
      setResistors();
      break;
    }

    case 13374001: {                     // SHOCKsetVoltage_float
      float v = waitOnFloat(0.0);
      pinState = volt2state(v);
      byte2pinMap();
      setResistors();
      break;
    }

    case 13374010:                       // SHOCKtriggerOn
      digitalWrite(pinTrigger, HIGH);
      break;

    case 13374011:                       // SHOCKtriggerOff
      digitalWrite(pinTrigger, LOW);
      break;

    case 13374020:                       // SHOCKsetCalibDwell_float
      calibrationStateTime = waitOnFloat(calibrationStateTime);
      break;

    // ── mode selection (13375xxx) ──────────────────────────────────
    case 13375000:                       // MODEsetFreeRun
      sysMod = 0;
      break;

    case 13375001:                       // MODEsetStimTrain
      sysMod = 1;
      break;

    case 13375002:                       // MODEsetCalibration
      sysMod = 2;
      break;

    case 13375003:                       // MODEsetPattern
      sysMod = 3;
      break;

    // ── experiment flow (13372xxx, shared with PARROTard) ──────────
    case 13372000:                       // EXPabort
      endTimedExperiment();
      break;

    case 13372001:                       // EXPstart
      singleStart = true;
      singleStop  = true;
      break;

    case 13372999:                       // EXPgetParameter_return
      writeOutParameterFrame();
      break;

    // ── time-table upload (13376xxx) ───────────────────────────────
    case 13376000:                       // TIMETABLE_upload
      uploadTimeTable();
      break;

    // ── stim-train parameters (13377xxx) ───────────────────────────
    case 13377000:                       // STIMTRAIN_setPreDur_float
      preStimDur = waitOnFloat(preStimDur);
      break;

    case 13377001:                       // STIMTRAIN_setPulseDur_float
      pulseDur = waitOnFloat(pulseDur);
      break;

    case 13377002:                       // STIMTRAIN_setIPI_float
      IPI = waitOnFloat(IPI);
      break;

    case 13377003:                       // STIMTRAIN_setITI_float
      ITI = waitOnFloat(ITI);
      break;

    case 13377010: {                     // STIMTRAIN_setStatePattern1_intlist
      int n = readByteListFrame(statePattern1, MAX_PATTERN_LEN);
      if (n > 0) numPulses1st = n;
      break;
    }

    case 13377011: {                     // STIMTRAIN_setStatePattern2_intlist
      int n = readByteListFrame(statePattern2, MAX_PATTERN_LEN);
      if (n > 0) numPulses2nd = n;
      break;
    }

    // ── pattern parameters (13378xxx) ──────────────────────────────
    case 13378000:                       // PATTERN_setPreDur_float
      preTemplateDur = waitOnFloat(preTemplateDur);
      break;

    case 13378001:                       // PATTERN_setStepDur_float
      templateStepDur = waitOnFloat(templateStepDur);
      break;

    case 13378002:                       // PATTERN_setIPI_float
      templateIPI = waitOnFloat(templateIPI);
      break;

    case 13378003:                       // PATTERN_setITI_float
      templateITI = waitOnFloat(templateITI);
      break;

    case 13378004:                       // PATTERN_setRep_int
      templateRep = waitOnInt(templateRep);
      break;

    case 13378010: {                     // PATTERN_setTemplate1_intlist
      int n = readByteListFrame(stateTemplate1, MAX_TEMPLATE_LEN);
      if (n > 0) numMemberTemplate1 = n;
      break;
    }

    case 13378011: {                     // PATTERN_setTemplate2_intlist
      int n = readByteListFrame(stateTemplate2, MAX_TEMPLATE_LEN);
      if (n > 0) numMemberTemplate2 = n;
      break;
    }
  }
}

// ── follow-number readers (20 s timeout, PARROTard convention) ─────

float waitOnFloat(float defaultFloat) {
  bool   waitOnSerial = true;
  float  readFloat    = 0.0;
  double startClock   = clockVar;

  while (waitOnSerial) {
    if (Serial.available() != 0) {
      readFloat    = Serial.parseFloat();
      waitOnSerial = false;
    }
    if (clockVar - startClock > 20.0) {
      waitOnSerial = false;
      readFloat    = defaultFloat;
    }
  }
  return readFloat;
}

int waitOnInt(int defaultInt) {
  bool   waitOnSerial = true;
  int    readInt      = 0;
  double startClock   = clockVar;

  while (waitOnSerial) {
    if (Serial.available() != 0) {
      readInt      = Serial.parseInt();
      waitOnSerial = false;
    }
    if (clockVar - startClock > 20.0) {
      waitOnSerial = false;
      readInt      = defaultInt;
    }
  }
  return readInt;
}

// ── frame readers ──────────────────────────────────────────────────
//   Frames look like:   >>v0,v1,v2,...,vN<<\n
//   Up to 20 s to receive; trailing '\n' is the delimiter.

int readByteListFrame(byte* dst, int maxLen) {
  double startClock = clockVar;
  while (Serial.available() == 0) {
    if (clockVar - startClock > 20.0) return 0;
  }

  String content = Serial.readStringUntil('\n');
  int open  = content.indexOf(">>");
  int close = content.indexOf("<<");
  if (open < 0 || close < 0 || close <= open + 2) return 0;
  content = content.substring(open + 2, close);

  int count = 0;
  while (count < maxLen && content.length() > 0) {
    int comma = content.indexOf(",");
    String tok = (comma >= 0) ? content.substring(0, comma) : content;
    tok.trim();
    if (tok.length() == 0) break;
    dst[count++] = byte(tok.toInt() & 0x7F);    // clamp to 0..127
    if (comma < 0) break;
    content = content.substring(comma + 1);
  }
  return count;
}

// 13376000 — upload a (time, trigger, voltage) timetable as one frame.
// Frame: >>t0,trig0,V0,t1,trig1,V1,...<<\n  (up to timeArrayLen triples).
// Slots beyond what the host sends are tail-filled with the last value
// so the natural end-of-experiment criterion is always legal.
void uploadTimeTable() {
  double startClock = clockVar;
  while (Serial.available() == 0) {
    if (clockVar - startClock > 20.0) {
      Serial.println(">>err,timetable_timeout<<");
      return;
    }
  }

  String content = Serial.readStringUntil('\n');
  int open  = content.indexOf(">>");
  int close = content.indexOf("<<");
  if (open < 0 || close < 0 || close <= open + 2) {
    Serial.println(">>err,timetable_frame<<");
    return;
  }
  content = content.substring(open + 2, close);

  int filled = 0;
  for (int i = 0; i < timeArrayLen; i++) {
    if (content.length() == 0) break;

    // ── t ──
    int comma = content.indexOf(",");
    String tok = (comma >= 0) ? content.substring(0, comma) : content;
    timeArray[i] = tok.toFloat();
    content = (comma >= 0) ? content.substring(comma + 1) : "";

    // ── trig ──
    comma = content.indexOf(",");
    tok = (comma >= 0) ? content.substring(0, comma) : content;
    triggerArray[i] = bool(tok.toInt());
    content = (comma >= 0) ? content.substring(comma + 1) : "";

    // ── V ──
    comma = content.indexOf(",");
    tok = (comma >= 0) ? content.substring(0, comma) : content;
    stateArray[i] = volt2state(tok.toFloat());
    content = (comma >= 0) ? content.substring(comma + 1) : "";

    filled = i + 1;
  }

  // Tail-fill so timeArray[timeArrayLen-1] is always a legal end criterion.
  for (int i = filled; i < timeArrayLen; i++) {
    if (filled == 0) {
      timeArray[i]    = 0.0;
      triggerArray[i] = LOW;
      stateArray[i]   = 127;
    } else {
      timeArray[i]    = timeArray[filled - 1];
      triggerArray[i] = triggerArray[filled - 1];
      stateArray[i]   = stateArray[filled - 1];
    }
  }

  Serial.print(">>ok,timetable,");
  Serial.print(filled);
  Serial.println("<<");
}
