// ────────────────────────────────────────────────────────────────────
//  writeOutFunc.ino  « single PARROTard-style telemetry frame »
// ────────────────────────────────────────────────────────────────────
//  Fires on every FlexiTimer2 tick when writeOutFlag is true.
//  One line per call, leading '>' and trailing '<' so the host can
//  distinguish telemetry from command responses ('50 1337', '>>...<<',
//  'end').
//
//  Fields, in order:
//    clockVar           seconds since last reset
//    pinState           current dampening byte (0..127)
//    pinTrigger digital level (0/1)
//    sysMod             0 free  1 stim-train  2 calib  3 pattern
//    phase              index into the loaded time/state/trigger arrays
//    experimentRunning  0/1

void writeOutFunc() {
  if (!writeOutFlag) return;

  Serial.print(">");
  Serial.print(clockVar);
  Serial.print(",");
  Serial.print(pinState);
  Serial.print(",");
  Serial.print(digitalRead(pinTrigger));
  Serial.print(",");
  Serial.print(sysMod);
  Serial.print(",");
  Serial.print(phase);
  Serial.print(",");
  Serial.print(experimentRunning ? 1 : 0);
  Serial.println("<");
}
