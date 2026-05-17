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

The host side is currently MATLAB, communicating over USB serial at
9600 baud.

---

## Hardware

| Item                        | Notes                                              |
|-----------------------------|----------------------------------------------------|
| Arduino Uno (ATmega328P)    | `FlexiTimer2` 10 ms ISR drives all timing          |
| ShockPowerSupply (lab-built)| 7-resistor R-network input, single trigger input   |
| Conditioning chamber        | T-maze or single-chamber tube with copper grid     |

**Pin map** (`shinto.ino`):

| Function          | Arduino pin           |
|-------------------|-----------------------|
| Resistor bit 0..6 | `13, 5, 10, 9, 8, 6, 12` |
| Shock trigger     | `4`                   |

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

Mode is set with the serial command `13X` (see protocol below).

| `sysMod` | Name             | Behaviour                                                                  |
|----------|------------------|-----------------------------------------------------------------------------|
| 0        | Free run         | Pins respond immediately to single-state / single-voltage commands.        |
| 1        | Stim-train       | Classic two-session aversive training using `statePattern1/2`.             |
| 2        | Calibration      | Cycle through all 128 states at 0.25 Hz (default 4 s/state).               |
| 3        | **Pattern**      | Peak-end-rule templates `stateTemplate1/2` repeated `templateRep` times.   |

## Pattern mode — peak-end-rule protocol

Two voltage templates are defined in `shinto.ino`:

```cpp
int stateTemplate1[5] = {67, 54, 67, 80, 92};       // body only
int stateTemplate2[6] = {67, 54, 67, 80, 92, 127};  // body + end-spike (state 127)
```

`getPatternStates()`, `getPatternTiming()`, and `getPatternTriggers()`
expand each template into the global `stateArray` / `timeArray` /
`triggerArray` for `templateRep` repetitions (default 10), with
`templateStepDur` between members and `templateIPI` between
repetitions.  Templates share the same body of {67, 54, 67, 80, 92};
template 2 appends the maximum-intensity state (127) so that two
otherwise-identical episodes differ only in their ending.

`runTimedExperiment()` walks the resulting time table on every
`FlexiTimer2` tick.  Unused tail entries of the three arrays are
filled with the last user-defined value so the end criterion is
always legal regardless of template length.

## Stim-train mode — classic Tully aversive training

`statePattern1` and `statePattern2` define the voltage sequence for
the two training sessions; `preStimDur`, `pulseDur`, `IPI`, and `ITI`
set the timing.  `getStimTrainStates/Timing/Triggers()` build the
same global arrays as pattern mode, so both protocols share
`startTimedExperiment` / `runTimedExperiment` / `endTimedExperiment`.

## Serial protocol

All commands are integers, parsed by `SerialCommunication.ino`.

### Free-run state setting
| Cmd  | Action                                                            |
|------|-------------------------------------------------------------------|
| `211`| Trigger HIGH                                                      |
| `210`| Trigger LOW                                                       |
| `23 <int>`  | Set `pinState` directly (0..127) and update resistors      |
| `22 <float>`| Set output voltage and update resistors (via `volt2state`) |

### Timed-experiment control
| Cmd  | Action                                                            |
|------|-------------------------------------------------------------------|
| `5 >t1,trig1,V1,t2,trig2,V2,...<` | Upload an arbitrary time table from host |
| `50` | End current timed experiment                                      |
| `51` | End current timed experiment **and** allow next `singleStart`     |

### Write-out / logging
| Cmd  | Action                                                            |
|------|-------------------------------------------------------------------|
| `121`| Enable serial write-out                                           |
| `120`| Disable serial write-out                                          |
| `122`| Reduced mode: `>clock pinState trigger sysMod<`                   |
| `123`| Human-readable mode                                               |

### Mode switching
| Cmd  | Mode                                                              |
|------|-------------------------------------------------------------------|
| `130`| Free run                                                          |
| `131`| Timed stim-train                                                  |
| `132`| Calibration                                                       |
| `133`| Pattern (peak-end rule)                                           |
| `1337`| Communication ping (replies `50 1337`)                           |

## Dependencies

- [`FlexiTimer2`](https://github.com/PaulStoffregen/FlexiTimer2) for
  the 10 ms ISR.

Install via the Arduino IDE Library Manager or `arduino-cli lib install FlexiTimer2`.

## Build and upload

```bash
arduino-cli compile --fqbn arduino:avr:uno .
arduino-cli upload  --fqbn arduino:avr:uno --port /dev/ttyACM0 .
```

The folder name must match the main `.ino` (Arduino convention), so
clone into a directory called `shinto`.

## Project layout

```
shinto/
├── shinto.ino                 ← entry point, globals, setup() / loop()
├── SerialCommunication.ino    ← host-side command dispatcher
├── TimerISR.ino               ← 10 ms clock interrupt
├── byte2pinMap.ino            ← state byte → 7 bits → pin booleans
├── setResistors.ino           ← write pin booleans to digital outs
├── volt2state.ino             ← volts → state byte (linear calibration)
├── startTimedExperiment.ino   ← reset clock, latch first state
├── runTimedExperiment.ino     ← advance state on every tick
├── endTimedExperiment.ino     ← clean shutdown
├── getStimTrainStates.ino     ← Tully two-session state table
├── getStimTrainTiming.ino     ← Tully two-session time table
├── getStrimTrainTriggers.ino  ← Tully two-session trigger table
├── getPatternStates.ino       ← peak-end-rule state table
├── getPatternTiming.ino       ← peak-end-rule time table
├── getPatternTriggers.ino     ← peak-end-rule trigger table
├── SprintTimeTable.ino        ← dump (time, trigger, state) to serial
└── writeOutFunc.ino           ← reduced / human-readable telemetry
```

## Citation

If you use `shinto` in published work, please cite it via the
metadata in `CITATION.cff`.  A Zenodo DOI is minted on every tagged
release.

## Licence

MIT — see `LICENSE`.
