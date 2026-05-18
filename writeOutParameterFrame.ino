// ────────────────────────────────────────────────────────────────────
//  writeOutParameterFrame.ino  « reply to EXPgetParameter_return »
// ────────────────────────────────────────────────────────────────────
//  Emits one >>...<< frame summarising the firmware's current state.
//  Parsed on the host side by cuewire.shinto.parse_exp_parameters.
//
//  Frame fields, in order:
//    preStimDur, pulseDur, IPI, ITI,
//    preTemplateDur, templateStepDur, templateIPI, templateITI,
//    templateRep, sysMod, experimentRunning

void writeOutParameterFrame() {
  bool oldWriteOutFlag = writeOutFlag;
  writeOutFlag = false;

  Serial.print(">>");
  Serial.print(preStimDur);       Serial.print(",");
  Serial.print(pulseDur);         Serial.print(",");
  Serial.print(IPI);              Serial.print(",");
  Serial.print(ITI);              Serial.print(",");
  Serial.print(preTemplateDur);   Serial.print(",");
  Serial.print(templateStepDur);  Serial.print(",");
  Serial.print(templateIPI);      Serial.print(",");
  Serial.print(templateITI);      Serial.print(",");
  Serial.print(templateRep);      Serial.print(",");
  Serial.print(sysMod);           Serial.print(",");
  Serial.print(experimentRunning ? 1 : 0);
  Serial.println("<<");

  writeOutFlag = oldWriteOutFlag;
}
