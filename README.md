# Wavtoy

WAV rendering synth sequencer with percussive and additive synthesis tones. 

## Scores
Some example input files are shown in `/scores`. The text files layout looks a bit like a tracker sequencer:

```
<SONG TITLE>
<SONG BPM>
# a comment

# Synth Definition starts with SYNTH:

SYNTH 0.05 0.9 0.2 0.5 0.5 0.2 5 3

# Attack, Decay, Amp, Duration, Fundamental Gain, Harmonic Balance, Harmonic Count, Mode
# Arguments in order:
#   - Attack as a % of Duration
#   - Decay as a % of Duration
#   - Amplitude as a % of Max Render Amplitude
#   - Duration in seconds
#   - Fundamental Gain: Amplitude of Fundamental Frequency Sine Wave
#   - Harmonic Balance: Amplitude balance of odd vs even Harmonics
#   - Harmonic Count: Number of additional harmonics (octaves only)
#   - Mode: 
#       1. Additive Synth
#        2. Noise Burst with Sine Fundamental for snare-type percussive sounds
#        3. Pitch Drop Sine Wave for kick-type percussive sounds
#           - Attack value changes drop to fundamental duration
#           - Harmonic Count changes octave aboce fundamental to start drop

# you can add multiple synths:

# KICK
SYNTH 0.05 0.9 0.2 0.5 0.5 0.2 5 3
# SNARE
SYNTH 0.0 0.9 0.2 0.2 0.5 0.2 5 2
# HIHAT
SYNTH 0.4 0.9 0.05 0.1 0.2 0.9 5 2
# PAD
SYNTH 0.4 0.4 0.03 1.5 0.5 0.7 2 1
# TING
SYNTH 0.05 0.7 0.02 0.05 0.5 0.2 12 1

# You can sequence multiple tracks, eg

TRACK 1

# each TRACK keyword sets the sequencer cursor to zero.
# It is sometimes it is easier to write the instrument parts seperate
# It is also the only way to write polyrhythms 

# the DIV keyword sets the beat resolution, for example:

DIV 4 # quarter notes
DIV 12 # triplets
DIV 1 # 1 measure
# etc...

# Tracks can be described as below, each column is an instrument as defined above. Use spaces between notes!
        | C2    |       | F5
        |       | D3    | C3 E3 G3 B4       # the 4th instrument is playing a chord here
        | C2                                # here the second two instruments aren't playing, so they dont need columns
REST                                        # REST 

TRACK 2         # a new sequence that playes with the above but in triplets

DIV 12

C1
D2
REST
E3
F4      | F2

```

Eventually it would be cool to make a curses style editor to write these files. #TODO #UPDATE - WIP in /scores/

## Running
```
$ make
$ // computer stuff...
$ ./wavtoy scores/twinkle.txt
```