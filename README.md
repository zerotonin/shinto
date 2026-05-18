# shinto

*Arduino firmware for Drosophila aversive-conditioning experiments,
including a pattern-stimulus engine for testing the peak-end rule.*

`shinto` drives a 128-step programmable electric-shock source for
*Drosophila* aversive conditioning.  It is the successor to the
classic Tully-style stimulus-train trigger
([Tully & Quinn 1985](https://doi.org/10.1007/BF01350033)) and adds:

- a **pattern mode** that emits structured shock sequences built from
  voltage templates with optional terminal spikes — the protocol used
  to test whether *Drosophila* retrospective evaluation of an
  aversive episode follows the **peak-end rule**
  ([Kahneman et al. 1993](https://doi.org/10.1111/j.1467-9280.1993.tb00589.x));
- a **timed-experiment lifecycle** (`start` / `run` / `end`) shared
  by both the stim-train and the pattern protocol;
- a **calibration mode** that steps through all 128 dampening states
  so the output voltage can be measured with a multimeter and
  matched to the linear `volt2state` map.

Host-side control is via the **[cuewire](https://github.com/zerotonin/cuewire)**
Python package (`cuewire.shinto.ShintoRig`), which speaks the
PARROTard 7-digit integer-command protocol over USB serial at 9600
baud.  The previous MATLAB host is retired.

---

## Hardware

| Item                        | Notes                                              |
|-----------------------------|----------------------------------------------------|
| Arduino Uno (ATmega328P)    | `FlexiTimer2` 10 ms ISR drives all timing          |
| ShockPowerSupply (lab-built)| 7-resistor R-network input, single trigger input   |
| Conditioning chamber        | T-maze or single-chamber tube with copper grid     |

**Pin map** (`shinto.ino`):

| Function          | Arduino pin              |
|-------------------|--------------------------|
| Resistor bit 0..6 | `13, 5, 10, 9, 8, 6, 12` |
| Shock trigger     | `4`                      |

The 7-bit `pinState` (0..127) is split into individual bits by
`byte2pinMap()` and written to the resistor pins by `setResistors()`.
The resulting analogue voltage at the ShockPowerSupply follows the
linear calibration

```
V = 150.52 - 0.77805 * state          (volts; state in 0..127)
```

which is inverted in `volt2state()` so the host can request voltages
directly.

## System modes

Mode is set with the serial commands `13375000..13375003`.

| `sysMod` | Name             | Behaviour                                                                  |
|----------|------------------|-----------------------------------------------------------------------------|
| 0        | Free run         | Pins respond immediately to single-state / single-voltage commands.        |
| 1        | Stim-train       | Classic two-session aversive training using `statePattern1/2`.             |
| 2        | Calibration      | Cycle through all 128 states at 0.25 Hz (default 4 s/state).               |
| 3        | **Pattern**      | Peak-end-rule templates `stateTemplate1/2` repeated `templateRep` times.   |

## Pattern mode — peak-end-rule protocol

Two voltage templates are loaded into the firmware (defaults shown):

```cpp
int stateTemplate1[MAX_TEMPLATE_LEN] = {67, 54, 67, 80, 92};       // body only
int stateTemplate2[MAX_TEMPLATE_LEN] = {67, 54, 67, 80, 92, 127};  // body + end-spike
```

`getPatternStates()`, `getPatternTiming()`, and `getPatternTriggers()`
expand each template into the global `stateArray` / `timeArray` /
`triggerArray` for `templateRep` repetitions (default 10), with
`templateStepDur` between members and `templateIPI` between
repetitions.  Both templates share the same body of {67, 54, 67, 80,
92}; template 2 appends the maximum-intensity state (127) so two
otherwise-identical episodes differ only in their ending.

`runTimedExperiment()` walks the resulting time table on every
`FlexiTimer2` tick.  Tail entries of the three arrays are filled with
the last user-defined value so the end criterion is always legal
regardless of template length.

## Stim-train mode — classic Tully aversive training

`statePattern1` and `statePattern2` define the voltage sequence for
the two training sessions; `preStimDur`, `pulseDur`, `IPI`, and `ITI`
set the timing.  `getStimTrain{States,Timing,Triggers}()` build the
same global arrays as pattern mode, so both protocols share
`startTimedExperiment` / `runTimedExperiment` / `endTimedExperiment`.

---

## Serial protocol — PARROTard

All commands are 7-digit integers parsed by
`SerialCommunication.ino`.  Follow-numbers are pulled with a 20 s
timeout; frame uploads (`>>v0,v1,...<<`) are terminated by `\n`.

### Write-out & clock
| Command   | Action                                                       |
|-----------|--------------------------------------------------------------|
| `13370001`| `writeOutTrue` — enable continuous telemetry                 |
| `13370000`| `writeOutFalse` — silence telemetry                          |
| `13379999`| `resetClock` — `clockVar := 0`                               |
| `1337`    | `communicationTest` — replies `50 1337`                      |

### Shock subsystem
| Command   | Follow      | Action                                            |
|-----------|-------------|---------------------------------------------------|
| `13374000`| `int`       | `SHOCKsetState_int` — pinState in 0..127          |
| `13374001`| `float`     | `SHOCKsetVoltage_float` — host sends volts        |
| `13374010`| —           | `SHOCKtriggerOn` — `pinTrigger := HIGH`           |
| `13374011`| —           | `SHOCKtriggerOff` — `pinTrigger := LOW`           |
| `13374020`| `float`     | `SHOCKsetCalibDwell_float` — seconds per step     |

### Mode selection
| Command   | Mode                                                         |
|-----------|--------------------------------------------------------------|
| `13375000`| Free run                                                     |
| `13375001`| Stim-train                                                   |
| `13375002`| Calibration                                                  |
| `13375003`| Pattern (peak-end rule)                                      |

### Experiment flow
| Command   | Action                                                       |
|-----------|--------------------------------------------------------------|
| `13372001`| `EXPstart` — arm the next-loop build-and-start cycle         |
| `13372000`| `EXPabort` — call `endTimedExperiment()` immediately         |
| `13372999`| `EXPgetParameter_return` — emit `>>...<<` parameter frame    |

### Time-table upload
| Command   | Frame                                                                | Action                              |
|-----------|----------------------------------------------------------------------|-------------------------------------|
| `13376000`| `>>t0,trig0,V0,t1,trig1,V1,...<<\n`                                  | `TIMETABLE_upload` — see below     |

Up to `timeArrayLen=240` triples.  Slots beyond what the host sends
are tail-filled with the last value.  Firmware acknowledges with
`>>ok,timetable,<count>\n` or `>>err,timetable_{frame,timeout}<<\n`.

### Stim-train parameters
| Command   | Follow      | Action                                            |
|-----------|-------------|---------------------------------------------------|
| `13377000`| `float`     | `setPreDur_float`                                 |
| `13377001`| `float`     | `setPulseDur_float`                               |
| `13377002`| `float`     | `setIPI_float`                                    |
| `13377003`| `float`     | `setITI_float`                                    |
| `13377010`| `intlist`   | `setStatePattern1_intlist` (up to `MAX_PATTERN_LEN=16`) |
| `13377011`| `intlist`   | `setStatePattern2_intlist`                        |

### Pattern parameters
| Command   | Follow      | Action                                            |
|-----------|-------------|---------------------------------------------------|
| `13378000`| `float`     | `setPreDur_float`                                 |
| `13378001`| `float`     | `setStepDur_float`                                |
| `13378002`| `float`     | `setIPI_float`                                    |
| `13378003`| `float`     | `setITI_float`                                    |
| `13378004`| `int`       | `setRep_int`                                      |
| `13378010`| `intlist`   | `setTemplate1_intlist` (up to `MAX_TEMPLATE_LEN=16`) |
| `13378011`| `intlist`   | `setTemplate2_intlist`                            |

### Frame formats
- **Telemetry** (when `writeOutFlag == true`, ~100 Hz at 10 ms tick):
  `>clockVar,pinState,trigger,sysMod,phase,experimentRunning<`
- **Parameter readout** (reply to `13372999`):
  `>>preStimDur,pulseDur,IPI,ITI,preTemplateDur,templateStepDur,templateIPI,templateITI,templateRep,sysMod,experimentRunning<<`
- **End sentinel** (emitted once on natural experiment end):
  `end` (no trailing newline — PARROTard convention)

## Dependencies

- [`FlexiTimer2`](https://github.com/PaulStoffregen/FlexiTimer2) for
  the 10 ms ISR.

Install via the Arduino IDE Library Manager or
`arduino-cli lib install FlexiTimer2`.

## Build and upload

```bash
arduino-cli compile --fqbn arduino:avr:uno .
arduino-cli upload  --fqbn arduino:avr:uno --port /dev/ttyACM0 .
```

The folder name must match the main `.ino` (Arduino convention), so
clone into a directory called `shinto`.

## Quickstart with cuewire

```python
from cuewire.shinto import ShintoRig, ShintoMode

with ShintoRig("/dev/ttyACM0") as rig:
    rig.set_mode(ShintoMode.PATTERN)
    rig.set_pattern_params(
        pre=60.0, step=0.25, ipi=3.75, iti=60.0, rep=10,
        template1=[67, 54, 67, 80, 92],
        template2=[67, 54, 67, 80, 92, 127],
    )
    rig.start_experiment()
```

See the [cuewire README](https://github.com/zerotonin/cuewire) for
the full Python API and experiment-runner examples.

## Project layout

```
shinto/
├── shinto.ino                    ← entry point, globals, setup() / loop()
├── SerialCommunication.ino       ← PARROTard command dispatcher + frame readers
├── writeOutFunc.ino              ← telemetry frame
├── writeOutParameterFrame.ino    ← reply to EXPgetParameter_return
├── TimerISR.ino                  ← 10 ms FlexiTimer2 ISR
├── byte2pinMap.ino               ← state byte → 7 bits → pin booleans
├── setResistors.ino              ← write pin booleans to digital outs
├── volt2state.ino                ← volts → state byte (linear calibration)
├── startTimedExperiment.ino      ← reset clock, latch first state
├── runTimedExperiment.ino        ← advance state on every tick; emit "end"
├── endTimedExperiment.ino        ← clean shutdown
├── getStimTrainStates.ino        ← Tully two-session state table
├── getStimTrainTiming.ino        ← Tully two-session time table
├── getStimTrainTriggers.ino      ← Tully two-session trigger table
├── getPatternStates.ino          ← peak-end-rule state table
├── getPatternTiming.ino          ← peak-end-rule time table
└── getPatternTriggers.ino        ← peak-end-rule trigger table
```

## Smoke test after flashing

A new flash should pass these four checks before the rig is rolled
out for experiments:

1. **Ping**: `screen /dev/ttyACM0 9600` (or `picocom`), type `1337`,
   expect `50 1337`.
2. **Free-run voltage**: send `13370001` (writeOut on), then
   `13375000` (free run), then `13374001` followed by `45.0`.
   Multimeter at ShockPowerSupply should show ~45 V.  Send `13374010`
   (trigger on), confirm grid is live.
3. **Calibration sweep**: send `13375002`, watch telemetry roll
   through pinState 0..127 every 4 s, multimeter trace
   `V = 150.52 - 0.77805 * state`.
4. **Pattern dry-run**: send `13375003` then `13372001`.
   With no flies, watch the telemetry: every 0.25 s (`templateStepDur`)
   `pinState` should step through the template, `trigger` toggles
   per `getPatternTriggers`, and the firmware emits `end` after
   `templateRep` * (sum of template lengths + 2) * step durations.

## Citation

If you use `shinto` in published work, please cite it via the
metadata in `CITATION.cff`.  A Zenodo DOI is minted on every tagged
release.

## Licence

MIT — see `LICENSE`.
