# Wavtoy

WAV rendering synth sequencer with percussive and additive synthesis tones. 

## Running
```
$ make all
$ // computer stuff...
$ ./wavtoy scores/twinkle.txt
```

## Scores
Scores are written in .txt files. Some example input files are shown in [scores](scores/). The layout looks a bit like a tracker sequencer.

### Header Information
The header contains sone information including:

 - Title
 - BPM
 - Synthesis and Effect Modes
 - Song meta-structure

```
Wavtoy Theme
120

# a comment looks like this

# line 0 is always song title, line 1 is always BPM. 

# Synthesis modes include:
#       - KICK, a sweeping pitch drop sin wave.
#               * Attack: pitch drop duration
#               * Octave Count: starting pitch above fundamental
#       - NOISE, a noise + fundamental tone that can sound like a snare, hat, cymbal
#       - SYNTH, a simple additive synth
# Arguments in order:
#   - Attack as a % of Duration
#   - Decay as a % of Duration
#   - Amplitude as a % of Max Render Amplitude
#   - Duration in seconds
#   - Fundamental Gain: Amplitude of Fundamental Frequency Sine Wave
#   - Harmonic Balance: Amplitude balance of odd vs even Harmonics
#   - Octave Count: Number of additional harmonics (octaves only)

# You can add multiple synths, with effects for each synth applied below each synth.
# Effects include:
#       - HPF, a simple high-pass filter
#               * 1 arg: value [0.0, 1.0] that sets filter frequency (TODO: math to set f in Hz)
#       - LPF, a simple low-pass filter
#               * 1 arg: value [0.0, 1.0] that sets filter frequency (TODO: math to set f in Hz)
#       - ECHO, a simple echo effect
#               * 2 args: decay & time (seconds)
#       - CRUSH, a map reduce / bitcrush type distortion
#               * 1 arg: int range to limit sample values to
#       - FOLD, a wave folding type distortion
#               * 1 arg: threshold at which to apply wavefold as % of maxamp[0.0, 1.0] 
#       - MULT, a formant-type sound created by multiplying samples by sample values in the future
#               * 1 arg: int distance between samples

KICK 0.05 0.9 0.5 0.5 0.5 0.2 5
FX HPF 0.02

NOISE 0.0 0.9 0.2 0.2 0.5 0.2 5
FX CRUSH 8
FX FOLD 0.6

NOISE 0.4 0.9 0.5 0.1 0.2 0.9 5
FX HPF 0.8

SYNTH 0.4 0.4 0.1 1.5 0.5 0.7 2

SYNTH 0.05 0.7 0.05 0.05 0.5 0.2 12
FX ECHO 0.3 0.5

# The STRUCTURE section allows definition of the song metastructure.
# SEC# keywords relate to sections or sequences defined in the body of the document
# SEC# on the same line will play in parallel
# SEC# on subsequent lines will play in series

STRUCTURE
SEC1
SEC1 SEC3 SEC4
SEC1 SEC4 SEC5
SEC1 SEC5
SEC2 SEC6
END STRUCTURE
```

### Body Notation
Writing music is done in sections `SEC#` which are combined into the song based on the `STRUCTURE`.
Note names are just standard western notation (TODO: make Frequency table more precise, not ints). 

Each instrument channel is seperated by a `|`, with instruments laid out left to right in the order they are defined in the header. Chords can be created by adding multiple notes to the same line in an instrument channel. Each line is a sequential count in the beat resolution defined by the most recent `DIV` line. The `REST` keyword allows all instruments to skip a beat. 

```
SEC1    # Each SEC# starts a new section
DIV 16  # DIV sets the beat resolution

# A section can include note events for multiple instruments. 

C1      |       |       | A3 C3 E3 G4   
REST
REST
REST
REST
REST
C1      # these are other instruments out here but they arent playing anything, so we don't need to write anything
REST
        | C2    
REST
C1
REST
REST
REST
C1
REST
# etc

SEC2
DIV 1
        |       |       | A3 C3 E3 G4
        |       |       | C3 E3 G3 B4
        |       |       | F3 A3 C3 E4
        |       |       | G3 B3 D3 F4

# etc

SEC4
DIV 6 # writing a DIV 6 melody on SEC1 would be impossible, but we can combine SEC1 and SEC4 to achieve the polyrhythm
REST
        |       |       |       | C5
        |       |       |       | E5
        |       |       |       | C5
REST
        |       |       |       | C5
        |       |       |       | E5
        |       |       |       | F5
REST
        |       |       |       | G5
        |       |       |       | F5
        |       |       |       | E5
        |       |       |       | C5
REST
REST
        |       |       |       | C5
        |       |       |       | E5
        |       |       |       | C5
        |       |       |       | C5
REST
REST
        |       |       |       | C5
        |       |       |       | E5
        |       |       |       | C5

# etc

```
## Editor
A slacker/no-lint curses editor is being built in parallel. It doesnt work enough yet.
